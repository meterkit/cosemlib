/*
 * AES functions
 *
 * - AES Key Wrap Algorithm (RFC3394)
 * - One-Key CBC MAC (OMAC1) hash with AES-128 and AES-256
 * - AES-128 CTR mode encryption
 * - AES-128 EAX mode encryption/decryption
 * - AES-128 CBC
 * - AES-GCM
 * - AES-CCM
 *
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef AES_H
#define AES_H

#include "os.h"
#include "aes_i.h"

#define AES_BLOCK_SIZE 16

typedef struct
{
    u8 H[AES_BLOCK_SIZE];
    u8 J0[AES_BLOCK_SIZE];
    u8 S[16];
    u8 aes_ctx[AES_PRIV_SIZE];
} aes_gcm_ctx;

// ----------------------------- AES -----------------------------
int aes_encrypt_init(void *ctx, const u8 *key, size_t len);
void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void aes_encrypt_deinit(void *ctx);
int aes_decrypt_init(void *ctx, const u8 *key, size_t len);
void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void aes_decrypt_deinit(void *ctx);


// ----------------------------- AES-Wrap functions -----------------------------
int  aes_wrap(void *ctx, const u8 *kek, size_t kek_len, int n, const u8 *plain,
              u8 *cipher);
int  aes_unwrap(void *ctx, const u8 *kek, size_t kek_len, int n,
                const u8 *cipher, u8 *plain);

// ----------------------------- AES-GCM functions -----------------------------
int  aes_gcm_ae(void *ctx, const u8 *key, size_t key_len,
                const u8 *iv, size_t iv_len,
                const u8 *plain, size_t plain_len,
                const u8 *aad, size_t aad_len,
                u8 *crypt, u8 *tag);
int  aes_gcm_ad(void *ctx, const u8 *key, size_t key_len,
                const u8 *iv, size_t iv_len,
                const u8 *crypt, size_t crypt_len,
                const u8 *aad, size_t aad_len, const u8 *tag,
                u8 *plain);
int  aes_gmac(void *ctx, const u8 *key, size_t key_len,
              const u8 *iv, size_t iv_len,
              const u8 *aad, size_t aad_len, u8 *tag);


// Streamed version of GCM
int aes_gcm_init(aes_gcm_ctx *gcm_ctx, const u8 *key, size_t key_len, const u8 *iv, size_t iv_len);
int aes_gcm_update(aes_gcm_ctx *gcm_ctx, const u8 *plain, size_t plain_len, u8 *crypt, const u8 *aad, size_t aad_len);
int aes_gcm_finish(aes_gcm_ctx *gcm_ctx, u8 *tag, size_t plain_len, size_t aad_len);


#endif /* AES_H */
