/*
 * Copyright (C) 2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef HARDWARE_DEPS_H
#define HARDWARE_DEPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "defined_error.h"

#define MAXLINE 1024
#define IMSI_LEN 15
#define AES_BLOCK_SIZE 16

retcode_t get_key(uint8_t *key);
retcode_t get_hardware_identifier(char *device_id);

#ifdef __cplusplus
}
#endif

#endif  // HARDWARE_DEPS_H
