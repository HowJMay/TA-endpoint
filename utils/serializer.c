/*
 * Copyright (C) 2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#include "serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IV_LEN 16
#define UINT32_LEN 10

int serialize_msg(uint8_t *iv, uint32_t ciphertext_len, uint8_t *ciphertext,
                  char *out_msg, uint32_t *out_msg_len) {
  char str_ciphertext_len[UINT32_LEN + 1] = {};
  char *ptr = out_msg;

  sprintf(str_ciphertext_len, "%010d", ciphertext_len);
  if (iv) {
    memcpy(ptr, iv, IV_LEN);
  } else {
    memset(ptr, 0, IV_LEN);
  }
  ptr += IV_LEN;

  memcpy(ptr, str_ciphertext_len, UINT32_LEN);
  ptr += UINT32_LEN;

  memcpy(ptr, ciphertext, ciphertext_len);

  *out_msg_len = IV_LEN + UINT32_LEN + ciphertext_len;

  return 0;
}

int deserialize_msg(char *msg, uint8_t *iv, uint32_t *ciphertext_len,
                    uint8_t *ciphertext) {
  char str_ciphertext_len[UINT32_LEN + 1] = {};
  char *ptr = msg;
  uint32_t ciphertext_len_tmp;

  memcpy(iv, ptr, IV_LEN);
  ptr += IV_LEN;

  memcpy(str_ciphertext_len, ptr, UINT32_LEN);
  ciphertext_len_tmp = atoi(str_ciphertext_len);
  ptr += UINT32_LEN;

  memcpy(ciphertext, ptr, ciphertext_len_tmp);
  *ciphertext_len = ciphertext_len_tmp;

  return 0;
}