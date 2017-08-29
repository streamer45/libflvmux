#include <limits.h>
#include <time.h>
#include "adts.h"
#include "nal.h"
#include "lib.h"

struct flv_muxer {
  adts_parser_t *ap;
  nal_parser_t *np;
  flv_mux_cb *cb;
  void *user;
};

flv_muxer_t *flv_muxer_create(flv_mux_cb *cb, void *user) {
  return NULL;
}

int flv_muxer_destroy(flv_muxer_t *muxer) {
  return 0;
}

int flv_muxer_mux(flv_muxer_t *muxer, buffer_t *buf, int type) {
  return 0;
}

buffer_t *flv_muxer_get_header(flv_muxer_t *muxer) {
  return NULL;
}
