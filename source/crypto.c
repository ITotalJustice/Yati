#include <stdint.h>
#include <string.h>
#include <switch/crypto/aes.h>

#include "crypto.h"


void *crypto_encrypt_decrypt_aes(const void *in, void *out, uint8_t *key, EncryptMode mode)
{
    Aes128Context ctx;
    aes128ContextCreate(&ctx, key, mode);

    switch (mode)
    {
        case EncryptMode_Decrypt:
        {
            aes128EncryptBlock(&ctx, out, in);
            break;
        }
        case EncryptMode_Encrypt:
        {
            aes128DecryptBlock(&ctx, out, in);
            break;
        }
    }

    return out;
}

void *crypto_encrypt_aes_ctr(const void *in, void *out, uint8_t *key, uint8_t *counter, size_t size, uint64_t offset)
{
    Aes128CtrContext ctx;
    aes128CtrContextCreate(&ctx, key, counter);

    uint64_t swp = __bswap64(offset >> 4);
    memcpy(&counter[0x8], &swp, 0x8);
    aes128CtrContextResetCtr(&ctx, counter);
    aes128CtrCrypt(&ctx, out, in, size);

    return out;
}

void *crypto_encrypt_decrypt_aes_cbc(const void *in, void *out, const uint8_t *key, size_t size, void *iv, EncryptMode mode)
{
    Aes128CbcContext ctx;
    aes128CbcContextCreate(&ctx, key, iv, mode);
    aes128CbcContextResetIv(&ctx, iv);

    switch (mode)
    {
        case EncryptMode_Decrypt:
        {
            aes128CbcDecrypt(&ctx, out, in, size);
            break;
        }
        case EncryptMode_Encrypt:
        {
            aes128CbcEncrypt(&ctx, out, in, size);
            break;
        }
    }

    return out;
}

void *crypto_encrypt_decrypt_aes_xts(const void *in, void *out, void *key0, void *key1, uint64_t sector, size_t sector_size, size_t data_size, EncryptMode mode)
{
    uint8_t NCA_HEADER_KEY_LOWER[] = { 0xAE, 0xAA, 0xB1, 0xCA, 0x08, 0xAD, 0xF9, 0xBE, 0xF1, 0x29, 0x91, 0xF3, 0x69, 0xE3, 0xC5, 0x67 };
    uint8_t NCA_HEADER_KEY_UPPER[] = { 0xD6, 0x88, 0x1E, 0x4E, 0x4A, 0x6A, 0x47, 0xA5, 0x1F, 0x6E, 0x48, 0x77, 0x06, 0x2D, 0x54, 0x2D };

    Aes128XtsContext ctx;
    aes128XtsContextCreate(&ctx, NCA_HEADER_KEY_LOWER, NCA_HEADER_KEY_UPPER, mode);

    for (uint64_t pos = 0; pos < data_size; pos += sector_size)
    {
        aes128XtsContextResetSector(&ctx, sector++, true);
        switch (mode)
        {
            case EncryptMode_Decrypt:
            {
                aes128XtsDecrypt(&ctx, (uint8_t *)out + pos, (const uint8_t *)in + pos, sector_size);
                break;
            }
            case EncryptMode_Encrypt:
            {
                aes128XtsEncrypt(&ctx, (uint8_t *)out + pos, (const uint8_t *)in + pos, sector_size);
                break;
            }
        }
    }

    return out;
}