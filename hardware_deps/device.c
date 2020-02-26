#include "device.h"
#include <stdio.h>
#include <stdlib.h>

device_t *hardware_deps_init(void) {
  device_t *dv = (device_t *)malloc(sizeof(device_t));
#ifdef SIERRA_WP7702
  dv->get_key = get_key;
  dv->get_hardware_identifier = get_hardware_identifier;
  return dv;
#else
  fprintf(stderr, "Hardware not implement\n");
  free(dv);
  return NULL;
#endif
}

retcode_t release_device(device_t *d) {
  free(d);
  return RET_OK;
}
