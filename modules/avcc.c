#include "util.h"
#include "avcc.h"

struct avcc_transmuxer {
  avcc_transmux_cb *cb;
  buffer_t *header;
  buffer_t *sps;
  buffer_t *pps;
  buffer_t *pkt;
  void *user;
};

avcc_transmuxer_t *avcc_transmuxer_create(avcc_transmux_cb *cb, void *user) {
  avcc_transmuxer_t *t;
  if (!cb) return NULL;
  t = calloc(1, sizeof(*t));
  t->cb = cb;
  t->user = user;
  t->header = buffer_alloc(4096);
  t->sps = buffer_alloc(128);
  t->pps = buffer_alloc(128);
  t->pkt = buffer_alloc(65536 * 4);
  return t;
}

int avcc_transmuxer_destroy(avcc_transmuxer_t *t) {
  if (!t) return -1;
  buffer_free(t->header);
  buffer_free(t->sps);
  buffer_free(t->pps);
  buffer_free(t->pkt);
  free(t);
  return 0;
}

int avcc_transmux(avcc_transmuxer_t *t, nal_frame_t *nalu) {
  avcc_pkt_t pkt;
  memset(&pkt, 0, sizeof(pkt));

  if (nalu->type == NAL_SPS) {
    DEBUG("avcc: sps");
    buffer_copy(t->sps, &nalu->buffer, 0, 0, nalu->buffer.length);
    t->sps->length = nalu->buffer.length;
  }

  if (nalu->type == NAL_PPS) {
    DEBUG("avcc: pps");
    buffer_copy(t->pps, &nalu->buffer, 0, 0, nalu->buffer.length);
    t->pps->length = nalu->buffer.length;
  }

  if (nalu->type == NAL_SPS && t->sps->length && t->pps->length) {
    int pos = 0;
    t->header->data[pos++] = 0x01;
    t->header->data[pos++] = t->sps->data[1];
    t->header->data[pos++] = t->sps->data[2];
    t->header->data[pos++] = t->sps->data[3];
    t->header->data[pos++] = 0xff;
    t->header->data[pos++] = 0xe1;
    t->header->data[pos++] = t->sps->length >> 8;
    t->header->data[pos++] = t->sps->length & 0xFF;
    buffer_copy(t->header, t->sps, pos, 0, t->sps->length);
    pos += t->sps->length;
    t->header->data[pos++] = 0x01;
    t->header->data[pos++] = t->pps->length >> 8;
    t->header->data[pos++] = t->pps->length & 0xFF;
    buffer_copy(t->header, t->pps, pos, 0, t->pps->length);
    pos += t->pps->length;
    t->header->length = pos;
    pkt.buffer = *t->header;
    pkt.keyframe = true;
    pkt.header = true;
    pkt.buffer.pts = nalu->buffer.pts;
    return t->cb(&pkt, t->user);
  }

  if (nalu->type != NAL_PPS && nalu->type != NAL_SPS) {
    t->pkt->data[0] = (nalu->buffer.length >> 24) & 0xff;
    t->pkt->data[1] = (nalu->buffer.length >> 16) & 0xff;
    t->pkt->data[2] = (nalu->buffer.length >> 8) & 0xff;
    t->pkt->data[3] = nalu->buffer.length & 0xff;
    buffer_copy(t->pkt, &nalu->buffer, 4, 0, nalu->buffer.length);
    t->pkt->length = 4 + nalu->buffer.length;
    pkt.buffer = *t->pkt;
    pkt.keyframe = nalu->type == NAL_IDR;
    pkt.buffer.pts = nalu->buffer.pts;
    return t->cb(&pkt, t->user);
  }

  return 0;
}

int avcc_get_header(avcc_transmuxer_t *t, buffer_t *hdr) {
  return buffer_copy(hdr, t->header, 0, 0, t->header->length);
}
