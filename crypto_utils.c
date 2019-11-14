/*
 * Copyright (C) 2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#include "crypto_utils.h"
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "third_party/hashchain/hashchain.c"

#define OPENSSL_SUCCESS 1
#define FAILED 1
#define AES_BLOCK_SIZE 16
#define CIPHER_LEN_PKCS5(plain_text) \
  (strlen(plain_text) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE

#define MAXLINE 1024
#define IMSI_LEN 15
#define MAX_TIMESTAMP_LEN 20
#define SHA_TYPE "sha256"

static void handleErrors(void) {
  ERR_print_errors_fp(stderr);
  abort();
}

// The device ID we are used here is IMSI. We could use other physical ID in the
// future.
int get_device_id(char *device_id) {
#if DEBUG
  strncpy(device_id, "470010171566423", 15);
  return 0;
#endif
  char result_buf[MAXLINE], *imsi;
  char cmd[] = "cm sim info";
  FILE *fp;

  fp = popen(cmd, "r");
  if (NULL == fp) {
    perror("popen open error");
    return FAILED;
  }

  while (fgets(result_buf, sizeof(result_buf), fp) != NULL) {
    if (strstr(result_buf, "IMSI")) {
      result_buf[strlen(result_buf) - 1] = '\0';
      imsi = strtok(result_buf + 5, " ");
    }
  }

  strncpy(device_id, imsi, IMSI_LEN);

  if (pclose(fp) == -1) {
    perror("close FILE pointer");
    return FAILED;
  }

  return 0;
}

// Get AES key with hashchain in legato originated app form.
int get_aes_key(uint8_t *key) {
#if DEBUG
  uint8_t key_init[32] = {82,  142, 184, 64,  74,  105, 126, 65,  154, 116, 14,
                          193, 208, 41,  8,   115, 158, 252, 228, 160, 79,  5,
                          167, 185, 13,  159, 135, 113, 49,  209, 58,  68};
  memcpy(key, &key_init, 32);
  return 0;
#endif
  char hash_chain_res[MAXLINE];
  char cmd[] = "cm sim info";  // TODO Use the right command
  FILE *fp;

  fp = popen(cmd, "r");

  if (NULL == fp) {
    perror("popen open error");
    return FAILED;
  }

  if (fgets(hash_chain_res, sizeof(hash_chain_res), fp) != NULL) {
    hash_chain_res[strlen(hash_chain_res) - 2] = '\0';
  }

  char *delim = ":", *token;
  uint8_t counter = 0;
  token = strtok(hash_chain_res, delim);
  while (token != NULL) {
    key[counter++] = (uint8_t)atoi(token);
    token = strtok(NULL, delim);
  }

  if (pclose(fp) == -1) {
    perror("close FILE pointer");
    return FAILED;
  }

  return 0;
}

int aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext) {
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    handleErrors();
  }

  /*
   * Initialise the encryption operation.
   */
  if (OPENSSL_SUCCESS !=
      EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
    handleErrors();
  }

  /*
   * Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if (OPENSSL_SUCCESS !=
      EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
    handleErrors();
  }
  ciphertext_len = len;

  /*
   * Finalise the encryption.
   */
  if (OPENSSL_SUCCESS != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
    handleErrors();
  }
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *plaintext) {
  EVP_CIPHER_CTX *ctx;

  int len;
  int plaintext_len;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    handleErrors();
  }

  /*
   * Initialise the decryption operation.
   * TODO - ensure using a key and IV size appropriate for cipher
   * For AES key is 256 bit and IV is 128 bit
   * For CBC mode, IV is not repeatable.
   */
  if (OPENSSL_SUCCESS !=
      EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
    handleErrors();
  }

  /*
   * Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary.
   */
  if (OPENSSL_SUCCESS !=
      EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
    handleErrors();
  }
  plaintext_len = len;

  /*
   * Finalise the decryption.
   */
  if (OPENSSL_SUCCESS != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
    handleErrors();
  }
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

#if DEBUG
static uint8_t iv_global[16] = {};
#endif

int encrypt(unsigned char *plaintext, int plaintext_len,
            unsigned char *ciphertext, uint32_t *ciphertext_len, uint8_t *iv) {
  char nonce[IMSI_LEN + MAX_TIMESTAMP_LEN + 1] = {}, device_id[IMSI_LEN + 1];
  uint8_t key[AES_BLOCK_SIZE * 2] = {};
  // step 1 fetch Device_ID (IMSI, len <= 15)
  get_device_id(device_id);

  // step 2 fetch timestamp
  uint64_t timestamp = time(NULL);
  // step 3 concatenate (Device_ID, timestamp)
  snprintf(nonce, IMSI_LEN + MAX_TIMESTAMP_LEN, "%s-%ld", device_id, timestamp);
  // TODO step 4 hash above string with sha128 and used it as IV
  OpenSSL_add_all_digests();
  const EVP_MD *hash = EVP_get_digestbyname(SHA_TYPE);
  const struct hash_chain iv_hash = hash_chain_create(
      nonce, IMSI_LEN + MAX_TIMESTAMP_LEN, EVP_get_digestbyname(SHA_TYPE), 1);

  // TODO step 5 request the hash of current order from hashchain and use it as
  // AES key hashchain would be another leagato original application
  unsigned char *buffer = NULL;
  get_aes_key((uint8_t *)key);

  for (int i = 0; i < 16; i++) {
    iv[i] = iv_hash.data[i] ^ iv_hash.data[i + 16];
  }
  *ciphertext_len = aes_encrypt(plaintext, plaintext_len, key, iv, ciphertext);

  free(iv_hash.data);

  EVP_cleanup();
  return 0;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, uint8_t *iv,
            unsigned char *plaintext) {
  char nonce[IMSI_LEN + MAX_TIMESTAMP_LEN + 1] = {};
  uint8_t key[AES_BLOCK_SIZE * 2] = {};

  OpenSSL_add_all_digests();
  // TODO step 5 request the hash of current order from hashchain and use it as
  // AES key hashchain would be another leagato original application
  get_aes_key(key);
  aes_decrypt(ciphertext, ciphertext_len, key, iv, plaintext);
  EVP_cleanup();
  return 0;
}