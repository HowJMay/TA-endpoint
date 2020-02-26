/*
 * Copyright (C) 2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef DEVICE_H
#define DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "defined_error.h"

#ifdef SIERRA_WP7702
#include "sierra_wireless/wp7702/hardware_deps.h"
#endif

typedef struct device_s {
  retcode_t (*get_key)(uint8_t *key);
  retcode_t (*get_hardware_identifier)(char *device_id);
} device_t;

device_t *hardware_deps_init(void);
retcode_t release_device(device_t *);

#ifdef __cplusplus
}
#endif

#endif  // DEVICE_H
