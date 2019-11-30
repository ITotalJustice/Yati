#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "network.h"
#include "util.h"


// globals
static CURL *curl;
static const char *global_url;


// start easy init.
bool ntwrk_start(const char *url)
{
    if ((global_url = url) == NULL)
    {
        print_message_loop_lock("no url entered\n");
        return false;
    }

    if (curl_global_init(CURL_GLOBAL_ALL))
    {
        print_message_loop_lock("failed to global init curl\npress b to exit \n");
        return false;
    }

    curl = curl_easy_init();
    if (!curl)
    {
        print_message_loop_lock("failed to start to curl\n");
        curl_global_cleanup();
        return false;
    }
    return true;
}

// exit curl and cleanup.
void ntwrk_exit()
{
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

CURLcode ntwrk_resume_large(curl_off_t resume_pos)
{
    CURLcode res = curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, resume_pos);
    if ((res))
        print_message_loop_lock("failed to set resume from large %s\n", curl_easy_strerror(res));
    return res;
}

CURLcode ntwrk_set_file_size(size_t file_size)
{
    CURLcode res = curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size);
    if ((res))
        print_message_loop_lock("failed to set file size %s\n", curl_easy_strerror(res));
    return res;
}

size_t ntwrk_write_function(void *contents, size_t size, size_t num_files, void *userp)
{
    ntwrk_struct_t *data_struct = (ntwrk_struct_t *)userp;
    size_t realsize = size * num_files;

    if (!data_struct->chunk_mode && realsize + data_struct->offset >= data_struct->data_size)
    {
        fwrite(data_struct->data, data_struct->offset, 1, data_struct->out);
        data_struct->offset = 0;
    }

    memcpy(&data_struct->data[data_struct->offset], contents, realsize);
    data_struct->offset += realsize;
    data_struct->data[data_struct->offset] = 0;
    return realsize;
}

// connect to the url host.
CURLcode ntwrk_setup_connect(const char *url)
{
    CURLcode res = 0;

    // set url
    if ((res = curl_easy_setopt(curl, CURLOPT_URL, url)))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    // set useragent.
    if ((res = curl_easy_setopt(curl, CURLOPT_USERAGENT, "ITotalJustice")))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    // set curl to follow url.
    if ((res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true)))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    // verify peer.
    if ((res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false)))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    // run the curl process.
    if ((res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false)))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    return res;
}

void ntwrk_setup_port(long port)
{
    curl_easy_setopt(curl, CURLOPT_PORT, port);
}

void ntwrk_setup_auth(const char *usr, const char *psswd)
{
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_USERNAME, usr);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, psswd);

    /*curl_easy_setopt(curl, CURLOPT_TLSAUTH_TYPE, "SRP");
    curl_easy_setopt(curl, CURLOPT_TLSAUTH_USERNAME, usr);
    curl_easy_setopt(curl, CURLOPT_TLSAUTH_PASSWORD, psswd);*/
}

CURLcode ntwrk_setup_write(void *data_struct)
{
    CURLcode res = 0;

    if ((res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ntwrk_write_function)))
    {
        print_message_loop_lock("failed to set write function %s\n", curl_easy_strerror(res));
        return res;
    }

    if ((res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_struct)))
    {
        print_message_loop_lock("failed to setup writedata %s\n", curl_easy_strerror(res));
        return res;
    }

    return res;
}

CURLcode ntwrk_setup_range(u_int64_t start, u_int64_t end)
{
    char range[128];
    snprintf(range, 128, "%lu-%lu", start, end);

    CURLcode res = curl_easy_setopt(curl, CURLOPT_RANGE, range);
    if ((res))
        print_message_loop_lock("failed to set range %s\n", curl_easy_strerror(res));
    return res;
}

CURLcode ntwrk_start_download(ntwrk_struct_t *data_struct, u_int64_t offset_start, u_int64_t offset_end)
{
    CURLcode res = 0;

    if ((res = ntwrk_setup_connect(global_url)))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    if ((res = ntwrk_setup_write(data_struct)))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    if (data_struct->chunk_mode)
    {
        if ((res = ntwrk_setup_range(offset_start, offset_end - 1))) // setup offset.
        {
            print_message_loop_lock("%s\n", curl_easy_strerror(res));
            return res;
        }
    }

    if ((res = curl_easy_perform(curl)))
    {
        print_message_loop_lock("%s\n", curl_easy_strerror(res));
        return res;
    }

    return res;
}

void ntwrk_setup_download(void *data, size_t data_size, u_int64_t start_offset)
{
    ntwrk_struct_t data_struct = { data, data_size, 0, true, NULL };
    ntwrk_start_download(&data_struct, start_offset, start_offset + data_size);
}

bool ntwrk_quick_download(const char *url, size_t buf_size, const char *file_out)
{
    // init curl.
    if (!ntwrk_start(url)) return false;

    // setup data struct.
    ntwrk_struct_t data_struct = { malloc(buf_size), buf_size, 0, false, open_file(file_out, "wb") };

    // start download.
    if (ntwrk_start_download(&data_struct, 0, 0))
    {
        free(data_struct.data);
        return false;
    }

    // write remaning data to file before free.
    fwrite(data_struct.data, data_struct.offset, 1, data_struct.out);
    free(data_struct.data);

    // exit.
    ntwrk_exit();
    fclose(data_struct.out);
    return true;
}

void ntwrk_encode_url(const char *url, char *out_url)
{
    char *temp_url = curl_easy_escape(curl, url, 0);
    if (!temp_url)
    {
        print_message_loop_lock("failed to encode url\n");
        return;
    }
    printf("new encode %s\n", temp_url);

    strcpy(out_url, temp_url);
    curl_free(temp_url);
}

void ntwrk_decode_url(const char *url, char *out_url)
{
    int new_str_len = 0; 
    char *temp_url = curl_easy_unescape(curl, url, 0, &new_str_len);
    if (!temp_url)
    {
        print_message_loop_lock("failed to decode url\n");
        return;
    }
    strcpy(out_url, temp_url);
    curl_free(temp_url);
}