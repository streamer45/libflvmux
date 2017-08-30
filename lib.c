#include <limits.h>
#include <time.h>
#include "lib.h"

uint8_t hdr_data[] = {
  0x46, 0x4C, 0x56, 0x01, 0x05, 0x00, 0x00, 0x00, 0x09
};

buffer_t hdr = {
  .data = hdr_data,
  .length = 9,
  .size = 9
};

struct flv_muxer {
  flv_mux_cb *cb;
  void *user;
};

flv_muxer_t *flv_muxer_create(flv_mux_cb *cb, void *user) {
  flv_muxer_t *mux;
  if (!cb) return NULL;
  mux = calloc(1, sizeof(*mux));
  mux->cb = cb;
  mux->user = user;
  return mux;
}

int flv_muxer_destroy(flv_muxer_t *muxer) {
  if (!muxer) return -1;
  free(muxer);
  return 0;
}

int flv_muxer_mux(flv_muxer_t *muxer, frame_t *frame) {
  int ret = 0;
  if (!muxer || !frame) return -1;
  return ret;
}

buffer_t *flv_muxer_get_header(flv_muxer_t *muxer) {
  return &hdr;
}
