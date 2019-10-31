#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "https.h"

int main(int argc, char *argv[]) {
  char *host = "https://tangle-accel.biilabs.io/";
  char req_body[1024], response[4096];
  char api[] = "transaction/";
  int ret, size;

  HTTP_INFO http_info;

  // Init http session. verify: check the server CA cert.
  http_init(&http_info, false);
  size_t url_len = (strlen(host) + strlen(api) + 1);
  char *url;
  url = (char *)malloc(url_len * sizeof(char));
  snprintf(url, url_len, "%s%s", host, api);
  if (http_open(&http_info, url) < 0) {
    http_strerror(req_body, 1024);
    printf("socket error: %s \n", req_body);

    goto error;
  }

  http_info.request.close = false;
  http_info.request.chunked = false;
  snprintf(http_info.request.method, 8, "POST");
  snprintf(http_info.request.content_type, 256, "application/json");
  size = sprintf(
      req_body,
      "{\"value\": 0, \"tag\": \"POWEREDBYTANGLEACCELERATOR9\"}\r\n\r\n");

  http_info.request.content_length = size;

  if (http_write_header(&http_info) < 0) {
    http_strerror(req_body, 1024);
    printf("socket error: %s \n", req_body);

    goto error;
  }

  if (http_write(&http_info, req_body, size) != size) {
    http_strerror(req_body, 1024);
    printf("socket error: %s \n", req_body);

    goto error;
  }

  // Write end-chunked
  if (http_write_end(&http_info) < 0) {
    http_strerror(req_body, 1024);
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
