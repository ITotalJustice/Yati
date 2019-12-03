#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <switch.h>


typedef enum
{
    EncryptMode_Decrypt = 0x0,
    EncryptMode_Encrypt = 0x1
} EncryptMode;


//
void *crypto_encrypt_ctr(const void *in, void *out, u8 *key, u8 *counter, size_t size, u64 offset);

//
void *crypto_encrypt_decrypt_ctr_cbc(const void *in, void *out, const u8 *key, size_t size, void *iv, EncryptMode mode);

//
void *crypto_encrypt_decrypt_xts(const void *in, void *out, void *key0, void *key1, u64 sector, size_t data_size, EncryptMode mode);

#endif