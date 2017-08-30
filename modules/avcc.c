#include "util.h"
#include "avcc.h"

struct avcc_transmuxer {
  avcc_transmux_cb *cb;
  void *user;
};

avcc_transmuxer_t *avcc_transmuxer_create(avcc_transmux_cb *cb, void *user) {
  return NULL;
}

int avcc_transmuxer_destroy(avcc_transmuxer_t *t) {
  return 0;
}

int avcc_transmux(avcc_transmuxer_t *t, nal_frame_t *nalu) {
  return 0;
}

int avcc_get_header(avcc_transmuxer_t *t, buffer_t *hdr) {
  return 0;
}
