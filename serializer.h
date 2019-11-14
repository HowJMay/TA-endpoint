/*
 * Copyright (C) 2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int serialize_msg(uint8_t *ciphertext, uint32_t ciphertext_len, uint8_t *iv,
                  char *out_msg);
int deserialize_msg(char *msg, uint8_t *ciphertext, uint32_t *ciphertext_len,
                    uint8_t *iv);

#ifdef __cplusplus
}
#endif

#endif  // SERIALIZER_H