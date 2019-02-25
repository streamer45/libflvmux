#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libflvmux.h"
#include "adts.h"
#include "nal.h"
#include "avcc.h"
#include "util.h"

typedef struct {
  uint64_t aframes;
  uint64_t vframes;
  flv_muxer_t *muxer;
  avcc_transmuxer_t *at;
  nal_parser_t *np;
  adts_parser_t *ap;
  bool header_sent;
} state_t;

int mux_cb(buffer_t *buf, void *user) {
  state_t *st;
  buffer_t *header;
  st = (state_t *)user;
  if (!st->header_sent) {
    header = flv_muxer_get_header(st->muxer);
    write(1, header->data, header->length);
    st->header_sent = true;
    //memset(buf->data, 0, 4);
    write(1, buf->data, buf->length);
  } else {
    write(1, buf->data, buf->length);
  }
  return 0;
}

int avcc_cb(avcc_pkt_t *pkt, void *user) {
  int ret;
  frame_t frame;
  state_t *st;
  st = (state_t *)user;
  memset(&frame, 0, sizeof(frame));
  frame.type = FRAME_VIDEO;
  frame.buf = pkt->buffer;

  //DEBUG("%zu", frame.pts);
  frame.video.keyframe = pkt->keyframe;
  frame.header = pkt->header;
  if (!frame.header) frame.dts = frame.pts = (st->vframes++ * (1000)) / 25;
  else frame.dts = frame.pts = (st->vframes * (1000)) / 25;
  //write(1, pkt->buffer.data, pkt->buffer.length);
  //DEBUG("got avcc pkt %zu", pkt->buffer.length);
  ret = flv_muxer_mux(st->muxer, &frame);
  if (ret != 0) {
    ERROR("flv muxer failed %d", ret);
    return -1;
  }
  return 0;
}

int nalu_cb(nal_frame_t *nalu, void *user) {
  int ret;
  state_t *st;
  st = (state_t *)user;
  ret = avcc_transmux(st->at, nalu);
  if (ret != 0) {
    ERROR("avcc transmux failed");
    return -1;
  }
  return 0;
}

int adts_cb(adts_frame_t *aframe, void *user) {
  int ret;
  frame_t frame;
  state_t *st;
  st = (state_t *)user;
  memset(&frame, 0, sizeof(frame));
  frame.type = FRAME_AUDIO;
  frame.buf = aframe->buffer;
  frame.header = aframe->header;
  if (!frame.header) frame.pts = frame.dts = (st->aframes++ * 1024 * 1000) / 48000;
  else frame.pts = frame.dts = (st->aframes * 1024 * 1000) / 48000;
  DEBUG("%zu", frame.pts);
  ret = flv_muxer_mux(st->muxer, &frame);
  if (ret != 0) {
    ERROR("flv muxer failed %d", ret);
    return -1;
  }
  return 0;
}

int test() {
  int ret;
  int vfd;
  int afd;
  buffer_t *buf;

  state_t st;

  memset(&st, 0, sizeof(st));

  buf = buffer_alloc(4096);
  if (!buf) return -1;

  vfd = open("../tsmux/samples/in.h264", O_RDONLY);
  if (vfd < 0) return -3;

  afd = open("../tsmux/samples/in.aac", O_RDONLY);
  if (afd < 0) return -4;

  st.muxer = flv_muxer_create(mux_cb, &st);
  if (!st.muxer) return -5;

  st.np = nal_parser_create(nalu_cb, &st);
  if (!st.np) return -6;

  st.ap = adts_parser_create(adts_cb, &st);
  if (!st.ap) return -7;

  st.at = avcc_transmuxer_create(avcc_cb, &st);
  if (!st.at) return -8;

  bool vend = false;
  bool aend = false;

  for (;;) {
    if (vend && aend) break;
    if (!vend) {
      ret = read(vfd, buf->data, buf->size);
      if (ret == 0) vend = true;
      if (ret < 0) return -9;
      buf->length = ret;
      ret = nal_parser_parse(st.np, buf);
      if (ret != 0) return -10;
    }
    if (!aend) {
      ret = read(afd, buf->data, buf->size);
      if (ret == 0) aend = true;
      if (ret < 0) return -11;
      buf->length = ret;
      ret = adts_parser_parse(st.ap, buf);
      if (ret != 0) return -12;
    }
  }

  ret = avcc_transmuxer_destroy(st.at);
  if (ret != 0) return -13;

  ret = adts_parser_destroy(st.ap);
  if (ret != 0) return -14;

  ret = nal_parser_destroy(st.np);
  if (ret != 0) return -15;

  ret = flv_muxer_destroy(st.muxer);
  if (ret != 0) return -16;

  ret = buffer_free(buf);
  if (ret != 0) return -17;

  close(afd);
  close(vfd);

  return 0;
}

int main() {
  int ret;
  ret = test();
  if (ret != 0) {
    ERROR("Failure! %d", ret);
    return ret;
  }
  return 0;
}
