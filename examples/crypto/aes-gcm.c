/*
 * Galois/Counter Mode (GCM) and GMAC with AES
 *
 * Copyright (c) 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "os.h"
#include "aes.h"

static void inc32(u8 *block)
{
	u32 val;
    val = GET_BE32(block + AES_BLOCK_SIZE - 4);
	val++;
    PUT_BE32(block + AES_BLOCK_SIZE - 4, val);
}


static void xor_block(u8 *dst, const u8 *src)
{
	u32 *d = (u32 *) dst;
	u32 *s = (u32 *) src;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
}


static void shift_right_block(u8 *v)
{
	u32 val;

    val = GET_BE32(v + 12);
	val >>= 1;
	if (v[11] & 0x01)
		val |= 0x80000000;
    PUT_BE32(v + 12, val);

    val = GET_BE32(v + 8);
	val >>= 1;
	if (v[7] & 0x01)
		val |= 0x80000000;
    PUT_BE32(v + 8, val);

    val = GET_BE32(v + 4);
	val >>= 1;
	if (v[3] & 0x01)
		val |= 0x80000000;
    PUT_BE32(v + 4, val);

    val = GET_BE32(v);
	val >>= 1;
    PUT_BE32(v, val);
}


/* Multiplication in GF(2^128) */
static void gf_mult(const u8 *x, const u8 *y, u8 *z)
{
	u8 v[16];
	int i, j;

	os_memset(z, 0, 16); /* Z_0 = 0^128 */
	os_memcpy(v, y, 16); /* V_0 = Y */

	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			if (x[i] & BIT(7 - j)) {
				/* Z_(i + 1) = Z_i XOR V_i */
				xor_block(z, v);
			} else {
				/* Z_(i + 1) = Z_i */
			}

			if (v[15] & 0x01) {
				/* V_(i + 1) = (V_i >> 1) XOR R */
				shift_right_block(v);
				/* R = 11100001 || 0^120 */
				v[0] ^= 0xe1;
			} else {
				/* V_(i + 1) = V_i >> 1 */
				shift_right_block(v);
			}
		}
	}
}


static void ghash_start(u8 *y)
{
	/* Y_0 = 0^128 */
	os_memset(y, 0, 16);
}


static void ghash(const u8 *h, const u8 *x, size_t xlen, u8 *y)
{
	size_t m, i;
	const u8 *xpos = x;
	u8 tmp[16];

	m = xlen / 16;

	for (i = 0; i < m; i++) {
		/* Y_i = (Y^(i-1) XOR X_i) dot H */
		xor_block(y, xpos);
		xpos += 16;

		/* dot operation:
		 * multiplication operation for binary Galois (finite) field of
		 * 2^128 elements */
		gf_mult(y, h, tmp);
		os_memcpy(y, tmp, 16);
	}

	if (x + xlen > xpos) {
		/* Add zero padded last block */
		size_t last = x + xlen - xpos;
		os_memcpy(tmp, xpos, last);
		os_memset(tmp + last, 0, sizeof(tmp) - last);

		/* Y_i = (Y^(i-1) XOR X_i) dot H */
		xor_block(y, tmp);

		/* dot operation:
		 * multiplication operation for binary Galois (finite) field of
		 * 2^128 elements */
		gf_mult(y, h, tmp);
		os_memcpy(y, tmp, 16);
	}

	/* Return Y_m */
}


static void aes_gctr(void *ctx, const u8 *icb, const u8 *x, size_t xlen, u8 *y)
{
	size_t i, n, last;
	u8 cb[AES_BLOCK_SIZE], tmp[AES_BLOCK_SIZE];
	const u8 *xpos = x;
	u8 *ypos = y;

	if (xlen == 0)
		return;

	n = xlen / 16;

	os_memcpy(cb, icb, AES_BLOCK_SIZE);
	/* Full blocks */
	for (i = 0; i < n; i++) {
        aes_encrypt(ctx, cb, ypos);
		xor_block(ypos, xpos);
		xpos += AES_BLOCK_SIZE;
		ypos += AES_BLOCK_SIZE;
		inc32(cb);
	}

	last = x + xlen - xpos;
	if (last) {
		/* Last, partial block */
        aes_encrypt(ctx, cb, tmp);
		for (i = 0; i < last; i++)
			*ypos++ = *xpos++ ^ tmp[i];
	}
}


static int aes_gcm_init_hash_subkey(u8 *ctx, const u8 *key, size_t key_len, u8 *H)
{
    int res = aes_encrypt_init(ctx, key, key_len);
    if (res == -1)
        return res;

	/* Generate hash subkey H = AES_K(0^128) */
	os_memset(H, 0, AES_BLOCK_SIZE);
    aes_encrypt(ctx, H, H);
	wpa_hexdump_key(MSG_EXCESSIVE, "Hash subkey H for GHASH",
			H, AES_BLOCK_SIZE);
    return 0;
}


static void aes_gcm_prepare_j0(const u8 *iv, size_t iv_len, const u8 *H, u8 *J0)
{
	u8 len_buf[16];

	if (iv_len == 12) {
		/* Prepare block J_0 = IV || 0^31 || 1 [len(IV) = 96] */
		os_memcpy(J0, iv, iv_len);
		os_memset(J0 + iv_len, 0, AES_BLOCK_SIZE - iv_len);
		J0[AES_BLOCK_SIZE - 1] = 0x01;
	} else {
		/*
		 * s = 128 * ceil(len(IV)/128) - len(IV)
		 * J_0 = GHASH_H(IV || 0^(s+64) || [len(IV)]_64)
		 */
		ghash_start(J0);
		ghash(H, iv, iv_len, J0);
		WPA_PUT_BE64(len_buf, 0);
		WPA_PUT_BE64(len_buf + 8, iv_len * 8);
		ghash(H, len_buf, sizeof(len_buf), J0);
	}
}


static void aes_gcm_gctr(void *ctx, const u8 *J0, const u8 *in, size_t len,
			 u8 *out)
{
	u8 J0inc[AES_BLOCK_SIZE];

	if (len == 0)
		return;

	os_memcpy(J0inc, J0, AES_BLOCK_SIZE);
	inc32(J0inc);
    aes_gctr(ctx, J0inc, in, len, out);
}


static void aes_gcm_ghash(const u8 *H, const u8 *aad, size_t aad_len,
			  const u8 *crypt, size_t crypt_len, u8 *S)
{
	u8 len_buf[16];

	/*
	 * u = 128 * ceil[len(C)/128] - len(C)
	 * v = 128 * ceil[len(A)/128] - len(A)
	 * S = GHASH_H(A || 0^v || C || 0^u || [len(A)]64 || [len(C)]64)
	 * (i.e., zero padded to block size A || C and lengths of each in bits)
	 */
	ghash_start(S);
	ghash(H, aad, aad_len, S);
	ghash(H, crypt, crypt_len, S);
	WPA_PUT_BE64(len_buf, aad_len * 8);
	WPA_PUT_BE64(len_buf + 8, crypt_len * 8);
	ghash(H, len_buf, sizeof(len_buf), S);

	wpa_hexdump_key(MSG_EXCESSIVE, "S = GHASH_H(...)", S, 16);
}


/**
 * aes_gcm_ae - GCM-AE_K(IV, P, A)
 */
int aes_gcm_ae(void *ctx, const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
           const u8 *plain, size_t plain_len,
           const u8 *aad, size_t aad_len, u8 *crypt, u8 *tag)
{
	u8 H[AES_BLOCK_SIZE];
	u8 J0[AES_BLOCK_SIZE];
	u8 S[16];

    if (aes_gcm_init_hash_subkey(ctx, key, key_len, H) == -1)
		return -1;

	aes_gcm_prepare_j0(iv, iv_len, H, J0);

	/* C = GCTR_K(inc_32(J_0), P) */
    aes_gcm_gctr(ctx, J0, plain, plain_len, crypt);

	aes_gcm_ghash(H, aad, aad_len, crypt, plain_len, S);

	/* T = MSB_t(GCTR_K(J_0, S)) */
    aes_gctr(ctx, J0, S, sizeof(S), tag);

	/* Return (C, T) */
    aes_encrypt_deinit(ctx);

	return 0;
}


int aes_gcm_init(aes_gcm_ctx *gcm_ctx, const u8 *key, size_t key_len, const u8 *iv, size_t iv_len)
{
    int res = aes_gcm_init_hash_subkey(gcm_ctx->aes_ctx, key, key_len, gcm_ctx->H);

    aes_gcm_prepare_j0(iv, iv_len, gcm_ctx->H, gcm_ctx->J0);

    ghash_start(gcm_ctx->S);

    return res;
}

/**
 * @brief Expects input to be a multiple of 16 bytes! Only the last call before aes_gcm_finish()
 *        can be less than 16 bytes.
 * @param gcm_ctx
 * @param plain
 * @param plain_len
 * @param crypt
 * @param aad
 * @param aad_len
 * @return
 */
int aes_gcm_update(aes_gcm_ctx *gcm_ctx, const u8 *plain, size_t plain_len, u8 *crypt, const u8 *aad, size_t aad_len)
{

    /* C = GCTR_K(inc_32(J_0), P) */
    aes_gcm_gctr(gcm_ctx->aes_ctx, gcm_ctx->J0, plain, plain_len, crypt);

    ghash(gcm_ctx->H, aad, aad_len, gcm_ctx->S);
    ghash(gcm_ctx->H, crypt, plain_len, gcm_ctx->S);


    return 0;
}

int aes_gcm_finish(aes_gcm_ctx *gcm_ctx, u8 *tag, size_t plain_len, size_t aad_len)
{
    u8 len_buf[16];
    WPA_PUT_BE64(len_buf, aad_len * 8);
    WPA_PUT_BE64(len_buf + 8, plain_len * 8);
    ghash(gcm_ctx->H, len_buf, sizeof(len_buf), gcm_ctx->S);

    /* T = MSB_t(GCTR_K(J_0, S)) */
    aes_gctr(gcm_ctx->aes_ctx, gcm_ctx->J0, gcm_ctx->S, sizeof(gcm_ctx->S), tag);
    /* Return (C, T) */
    aes_encrypt_deinit(gcm_ctx->aes_ctx);

    return 0;
}



/**
 * aes_gcm_ad - GCM-AD_K(IV, C, A, T)
 */
int aes_gcm_ad(void *ctx, const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
           const u8 *crypt, size_t crypt_len,
           const u8 *aad, size_t aad_len, const u8 *tag, u8 *plain)
{
	u8 H[AES_BLOCK_SIZE];
	u8 J0[AES_BLOCK_SIZE];
	u8 S[16], T[16];

    if (aes_gcm_init_hash_subkey(ctx, key, key_len, H) == -1)
        return -1;

	aes_gcm_prepare_j0(iv, iv_len, H, J0);

	/* P = GCTR_K(inc_32(J_0), C) */
    aes_gcm_gctr(ctx, J0, crypt, crypt_len, plain);

	aes_gcm_ghash(H, aad, aad_len, crypt, crypt_len, S);

	/* T' = MSB_t(GCTR_K(J_0, S)) */
    aes_gctr(ctx, J0, S, sizeof(S), T);

    aes_encrypt_deinit(ctx);

	if (os_memcmp_const(tag, T, 16) != 0) {
		wpa_printf(MSG_EXCESSIVE, "GCM: Tag mismatch");
		return -1;
	}

	return 0;
}


int aes_gmac(void *ctx, const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
         const u8 *aad, size_t aad_len, u8 *tag)
{
    return aes_gcm_ae(ctx, key, key_len, iv, iv_len, NULL, 0, aad, aad_len, NULL,
			  tag);
}
