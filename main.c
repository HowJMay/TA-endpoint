/*
 * Copyright (C) 2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto_utils.h"
#include "https.h"
#include "serializer.h"
#include "tryte_byte_conv.h"

#define MAX_REQ_LEN 1024
#define MAX_RES_LEN 4096

#define HOST "https://tangle-accel.biilabs.io/"
#define API "transaction/"
#define REQ_BODY                                                           \
  "{\"value\": 0, \"tag\": \"POWEREDBYTANGLEACCELERATOR9\", \"message\": " \
  "\"%s\"}\r\n\r\n"
#define MSG "THISISMSG9THISISMSG9THISISMSG"

int main(int argc, char *argv[]) {
  char req_body[MAX_REQ_LEN] = {}, response[MAX_RES_LEN] = {},
       tryte_msg[MAX_REQ_LEN] = {}, msg[MAX_REQ_LEN] = {};
  char url[] = HOST API;
  uint8_t ciphertext[MAX_REQ_LEN] = {}, iv[16] = {};
  uint32_t ciphertext_len = 0;
  int ret, size;
  HTTP_INFO http_info;

  encrypt(MSG, strlen(MSG), ciphertext, &ciphertext_len, iv);
  serialize_msg(ciphertext, ciphertext_len, iv, msg);
  ascii_to_trytes(msg, tryte_msg);
#if DEBUG
  char msg_de[MAX_REQ_LEN] = {}, plain[MAX_REQ_LEN] = {};
  printf("msg len = %d, tryte_msg = %d\n", strlen(msg), strlen(tryte_msg));
  trytes_to_ascii(tryte_msg, msg_de);
  if (memcmp(msg, msg_de, ciphertext_len)) {
    printf("\nNOT EQUAL \n");
  }
  uint32_t ciphertext_len_de;
  printf("msg = %s \n", msg);
  deserialize_msg(msg_de, ciphertext, &ciphertext_len_de, iv);
  printf("ciphertext_len_de = ");
  printf("%d \n", ciphertext_len_de);
  decrypt(ciphertext, ciphertext_len, iv, plain);
  printf("plain = %s \n", plain);
#endif

  // Init http session. verify: check the server CA cert.
  https_init(&http_info, true, false);

  if (http_open(&http_info, url) < 0) {
    http_strerror(req_body, MAX_REQ_LEN);
    printf("socket error: %s \n", req_body);

    goto error;
  }

  sprintf(req_body, REQ_BODY, tryte_msg);
  http_info.request.close = false;
  http_info.request.chunked = false;
  snprintf(http_info.request.method, 8, "POST");
  snprintf(http_info.request.content_type, 256, "application/json");
  http_info.request.content_length = strlen(req_body);
  size = http_info.request.content_length;

  if (http_write_header(&http_info) < 0) {
    http_strerror(req_body, MAX_REQ_LEN);
    printf("socket error: %s \n", req_body);

    goto error;
  }

  if (http_write(&http_info, req_body, size) != size) {
    http_strerror(req_body, MAX_REQ_LEN);
    printf("socket error: %s \n", req_body);

    goto error;
  }

  // Write end-chunked
  if (http_write_end(&http_info) < 0) {
    http_strerror(req_body, MAX_REQ_LEN);
    printf("socket error: %s \n", req_body);

    goto error;
  }

  ret = http_read_chunked(&http_info, response, sizeof(response));

  printf("return code: %d \n", ret);
  printf("return body: %s \n", response);

error:
  http_close(&http_info);

  return 0;
}
