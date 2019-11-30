#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <zstd.h>

#include "ncz.h"
#include "nca.h"
#include "ncm.h"
#include "util.h"


#define NCZ_SECTION_MAGIC 0x4E544345535A434E
#define NCZ_HEADER_OFFSET 0x4000


void ncz_get_header(ncz_structs_t *ptr)
{
    memset(&ptr->header, 0, sizeof(ncz_header_t));
    read_data_from_protocal(ptr->nca.mode, &ptr->header, sizeof(ncz_header_t), ptr->nca.offset, ptr->nca.nca_file, NULL);
}

bool ncz_check_valid_magic(ncz_structs_t *ptr)
{
    if (ptr->header.magic != NCZ_SECTION_MAGIC)
    {
        print_message_loop_lock("\ngot wrong magic %016lx\n", ptr->header.magic);
        return false;
    }
    return true;
}

void ncz_populate_sizes_offsets(ncz_structs_t *ptr)
{
    ptr->ncz_section_offset = ptr->nca.offset + sizeof(ncz_header_t);
    ptr->ncz_section_size = ptr->header.total_sections * sizeof(ncz_section_t);
    ptr->ncz_data_offset = ptr->ncz_section_offset + ptr->ncz_section_size;
}

void ncz_populate_sections(ncz_structs_t *ptr)
{
    ptr->sections = malloc(ptr->ncz_section_size);
    read_data_from_protocal(ptr->nca.mode, ptr->sections, ptr->ncz_section_size, ptr->ncz_section_offset, ptr->nca.nca_file, NULL);
}

int ncz_get_section(ncz_structs_t *ptr, u64 offset)
{
    for (int i = 0; i < ptr->header.total_sections; i++)
        if (offset < ptr->sections[i].offset + ptr->sections[i].decompressed_size && offset >= ptr->sections[i].offset)
            return i;
    print_message_loop_lock("error couldnt find section\n");
    return -1;
}

void ncz_encryption_setup(ncz_structs_t *ptr, u_int8_t *buf, size_t buf_size, u64 written_offset)
{
    size_t back_up_size = buf_size;
    for (size_t buf_start_offset = 0; buf_start_offset < back_up_size;)
    {
        int loc = ncz_get_section(ptr, written_offset);
        size_t total_size = ptr->sections[loc].offset + ptr->sections[loc].decompressed_size;
        u_int64_t chunk = buf_size;

        if (written_offset + buf_size > total_size)
            chunk = total_size - written_offset;

        if (ptr->sections[loc].crypto_type == 3)
            nca_encrypt_ctr(&buf[buf_start_offset], &buf[buf_start_offset], ptr->sections[loc].crypto_key, ptr->sections[loc].crypto_counter, chunk, written_offset);
        
        written_offset += chunk;
        buf_start_offset += chunk;
        buf_size -= chunk;
    }

    print_message_display("\twriting file %ldMB - %ldMB\r", ptr->nca.data_written / 0x100000, ptr->nca.nca_size / 0x100000);
    ncm_write_placeholder(&ptr->nca.ncm.storage, &ptr->nca.ncm.placeholder_id, &ptr->nca.data_written, buf, back_up_size);
}

void ncz_temp(ncz_structs_t *ptr)
{
    ZSTD_DStream *zds = ZSTD_createDStream();

    for (size_t buf_size = 0x1000000, offset = 0; offset < ptr->ncz_size; offset += buf_size)
    {
        if (offset + buf_size > ptr->ncz_size)
            buf_size = ptr->ncz_size - offset;

        // alloc buffer then read data into buffer.
        void *buf_in = malloc(buf_size);
        read_data_from_protocal(ptr->nca.mode, buf_in, buf_size, ptr->ncz_data_offset + offset, ptr->nca.nca_file, NULL);

        // set up input struct.
        ZSTD_inBuffer input = { buf_in, buf_size, 0 };

        // TODO: make this into a data struct.
        size_t output_chunk_size = ZSTD_DStreamOutSize();
        size_t data_size = buf_size * 2;
        u_int8_t *data_buf = malloc(data_size);
        u_int64_t data_stored = 0;

        // loop until all data is decompressed.
        while (input.pos < input.size)
        {
            ZSTD_outBuffer output = { malloc(output_chunk_size), output_chunk_size, 0 };
            size_t res = ZSTD_decompressStream(zds, &output, &input);

            if (ZSTD_isError(res))
            {
                print_message_loop_lock("ncz error: %s\n", ZSTD_getErrorName(res));
                free(output.dst);
                break;
            }

            // if the buffer is filled, encrypt and write data to file.
            if (output.pos + data_stored >= data_size)
            {
                ncz_encryption_setup(ptr, data_buf, data_stored, ptr->nca.data_written);
                data_stored = 0;
            }

            // copy decompressed data to buffer, then free temp decompressed data.
            memcpy(&data_buf[data_stored], output.dst, output.pos);
            free(output.dst);
            data_stored += output.pos;
        }

        // write remaining data to file then free the buffers.
        ncz_encryption_setup(ptr, data_buf, data_stored, ptr->nca.data_written);
        free(data_buf);
        free(buf_in);
    }
}

void ncz_free_structs(ncz_structs_t *ptr)
{
    if (ptr->sections != NULL) free(ptr->sections);
}

void ncz_start_install(const char *name, size_t size, u64 offset, NcmStorageId storage_id, InstallProtocal mode, FILE *f)
{
    ncz_structs_t ptr;
    ncz_first_0x4000 *data = malloc(sizeof(ncz_first_0x4000));

    read_data_from_protocal(mode, data, NCZ_HEADER_OFFSET, offset, f, NULL);
    nca_encrypt_decrypt_xts(data, data, 0, NCZ_HEADER_OFFSET, NCA_DECRYPT);
    data->header.distribution_type = 0; // for xci installs.

    ptr.nca.nca_file        = f;
    ptr.nca.mode            = mode;
    ptr.nca.offset          = offset + NCZ_HEADER_OFFSET;
    ptr.nca.nca_size        = data->header.nca_size;
    ptr.nca.data_written    = 0;

    // now that we have the nca size, we can setup the placeholder and write the header to it
    nca_setup_placeholder(&ptr.nca.ncm, name, data->header.nca_size, storage_id);
    nca_encrypt_decrypt_xts(data, data, 0, NCZ_HEADER_OFFSET, NCA_ENCRYPT);
    ncm_write_placeholder(&ptr.nca.ncm.storage, &ptr.nca.ncm.placeholder_id, &ptr.nca.data_written, data, NCZ_HEADER_OFFSET);
    free(data);

    ncz_get_header(&ptr);
    ncz_populate_sizes_offsets(&ptr);
    ncz_populate_sections(&ptr);

    ptr.ncz_size = size - ptr.ncz_section_size - NCZ_HEADER_OFFSET - sizeof(ncz_header_t);

    ncz_temp(&ptr);

    nca_register_placeholder(&ptr.nca.ncm);
    ncm_close_storage(&ptr.nca.ncm.storage);
    ncz_free_structs(&ptr);
}