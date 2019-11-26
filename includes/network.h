#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <curl/curl.h>


/*typedef enum
{
    HTTP = 0,
    HTTPS = 1,
    FTP = 2,
} ntwrk_protocol;*/

typedef struct
{
    u_int8_t *data;
    size_t data_size;
    u_int64_t offset;
    bool chunk_mode;
    FILE *out;
} ntwrk_struct_t;


//
bool ntwrk_start(const char *url);

//
void ntwrk_exit(void);

//
CURLcode ntwrk_resume_large(curl_off_t resume_pos);

//
CURLcode ntwrk_set_file_size(size_t file_size);

//
size_t ntwrk_write_function(void *contents, size_t size, size_t num_files, void *userp);

//
CURLcode ntwrk_setup_connect(const char *url);

//
void ntwrk_setup_port(long port);

//
void ntwrk_setup_auth(const char *usr, const char *psswd);

//
CURLcode ntwrk_setup_write(void *data_struct);

//
CURLcode ntwrk_setup_range(u_int64_t start, u_int64_t end);

//
void ntwrk_setup_download(void *data, size_t data_size, u_int64_t start_offset);

//
void ntwrk_setup_download(void *data, size_t data_size, u_int64_t start_offset);

// quickly download to file_out.
// url: the url to download from.
// buf_size: size of the buffer (should be larger than 16kb). this will be the max size of the chunks written to file from mem.
// file_out the file that the data will be written to.
bool ntwrk_quick_download(const char *url, size_t buf_size, const char *file_out);

//
void ntwrk_encode_url(const char *url, char *out_url);

//
void ntwrk_decode_url(const char *url, char *out_url);

#endif