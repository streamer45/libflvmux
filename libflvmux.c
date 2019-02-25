#include <limits.h>
#include <time.h>
#include "libflvmux.h"

uint8_t hdr_data[] = {
  0x46, 0x4C, 0x56, 0x01, 0x05, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00 // audio + video
  //0x46, 0x4C, 0x56, 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00 // video only
};

buffer_t hdr = {
  .data = hdr_data,
  .length = 13,
  .size = 13
};

struct flv_muxer {
  flv_mux_cb *cb;
  void *user;
  buffer_t *buf;
};

flv_muxer_t *flv_muxer_create(flv_mux_cb *cb, void *user) {
  flv_muxer_t *mux;
  if (!cb) return NULL;
  mux = calloc(1, sizeof(*mux));
  mux->cb = cb;
  mux->user = user;
  mux->buf = buffer_alloc(65536 * 4);
  return mux;
}

int flv_muxer_destroy(flv_muxer_t *muxer) {
  if (!muxer) return -1;
  buffer_free(muxer->buf);
  free(muxer);
  return 0;
}

int flv_muxer_mux(flv_muxer_t *muxer, frame_t *frame) {
  uint32_t len;
  if (!muxer || !frame) return -1;

  len = 0;

  //muxer->buf->length = 4 + 11 + frame->buf.length;

  if (frame->type == FRAME_VIDEO) {
    muxer->buf->data[0] = 0x09;
    len = 5 + frame->buf.length;
    if (frame->video.keyframe) muxer->buf->data[11] = 0x17;
    else muxer->buf->data[11] = 0x27;
    if (frame->header) muxer->buf->data[12] = 0x00;
    else muxer->buf->data[12] = 0x01;
    int ct = abs(frame->pts - frame->dts);
    muxer->buf->data[13] = (ct >> 16) & 0xff;
    muxer->buf->data[14] = (ct >> 8) & 0xff;
    muxer->buf->data[15] = ct & 0xff;
  }

  if (frame->type == FRAME_AUDIO) {
    muxer->buf->data[0] = 0x08;
    muxer->buf->data[11] = 0xaf;
    if (frame->header) muxer->buf->data[12] = 0x00;
    else muxer->buf->data[12] = 0x01;
    len = 2 + frame->buf.length;
  }

  muxer->buf->length = 11 + len + 4;

  //fprintf(stderr, "len %d\n", len);

  muxer->buf->data[1] = (len >> 16) & 0xff;
  muxer->buf->data[2] = (len >> 8) & 0xff;
  muxer->buf->data[3] = len & 0xff;


  //fprintf(stderr, "dts: %lld\n", frame->dts);

  muxer->buf->data[4] = (frame->dts >> 16) & 0xff;
  muxer->buf->data[5] = (frame->dts >> 8) & 0xff;
  muxer->buf->data[6] = (frame->dts) & 0xff;
  muxer->buf->data[7] = (frame->dts >> 24) & 0xff;

  buffer_copy(muxer->buf, &frame->buf, 11 + len - frame->buf.length, 0, frame->buf.length);

  int pos = 11 + len;

  int tag_len = muxer->buf->length - 4;

  muxer->buf->data[pos++] = (tag_len >> 24) & 0xff;
  muxer->buf->data[pos++] = (tag_len >> 16) & 0xff;
  muxer->buf->data[pos++] = (tag_len >> 8) & 0xff;
  muxer->buf->data[pos++] = tag_len & 0xff;

  return muxer->cb(muxer->buf, muxer->user);
}

buffer_t *flv_muxer_get_header(flv_muxer_t *muxer) {
  return &hdr;
}
