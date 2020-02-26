#include "hardware_deps.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Get AES key with hashchain in legato originated app form.
retcode_t get_key(uint8_t *key) {
  char hash_chain_res[MAXLINE];
  char cmd[] = "cm sim info";  // TODO Use the right command
  FILE *fp;

  fp = popen(cmd, "r");

  if (NULL == fp) {
    perror("popen open error");
    return RET_HARDWARE_DEPS;
  }

  if (fgets(hash_chain_res, sizeof(hash_chain_res), fp) != NULL) {
    hash_chain_res[strlen(hash_chain_res) - 2] = '\0';
  }

  strncpy(key, hash_chain_res, AES_BLOCK_SIZE);

  if (pclose(fp) == -1) {
    perror("close FILE pointer");
    return RET_HARDWARE_DEPS;
  }

  return RET_OK;
}

retcode_t get_hardware_identifier(char *device_id) {
  char result_buf[MAXLINE], *imsi;
  char cmd[] = "cm sim info";
  FILE *fp;

  fp = popen(cmd, "r");
  if (NULL == fp) {
    perror("popen open error");
    return RET_HARDWARE_DEPS;
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
    return RET_OK;
  }

  return 0;
}
