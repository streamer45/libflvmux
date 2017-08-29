#include "adts.h"
#include "util.h"

#define ADTS_HEADER_SIZE 7
#define ADTS_HEADER_SIZE_PROTECTED 9

struct adts_parser {
  buffer_t *bufs[2];
  buffer_t *curr;
  size_t nframes;
  adts_parser_cb *cb;
  void *user;
};

static int parse(adts_parser_t *ap, buffer_t *in) {
  int res;
  adts_frame_t frame;
  uint8_t *data;
  buffer_t *buf;
  size_t pos = 0;

  buf = ap->curr;

  //if (buf->length == 0) buf->pts = in->pts;

  res = buffer_copy(buf, in, buf->length, 0, in->length);


  if (res != 0) {
    ERROR("%d", res);
    return -1;
  }

  buf->length += in->length;
  data = buf->data;

  for (;;) {

    //DEBUG("%zu %zu", pos, buf->length);
    if (buf->length < (pos + ADTS_HEADER_SIZE)) break;

    if (data[pos] == 0xff && ((data[pos + 1] >> 4) == 0x0f)) {
    //   //const mpegVersion = (data[pos + 1] & 0x0f) >> 3;
    //   //const layer = (data[pos + 1] & 0x07) >> 1;
    //   //const protection = !(data[pos + 1] & 0x01);
    //   //const profile = data[pos + 2] >> 6;
    //   //const freqIndex = (data[pos + 2] & 0x3f) >> 2;
    //   //const channelConfig = ((data[pos + 2] & 0x01) << 2) | (data[pos + 3] >> 6);
      size_t frame_length = ((data[pos + 3] & 0x03) << 11) | (data[pos + 4] << 3) | (data[pos + 5] >> 5);
    //   //const nframes = (data[pos + 6] & 0x03) + 1;

      if ((buf->length < (pos + frame_length))) break;

      ++ap->nframes;

      memset(&frame, 0, sizeof(frame));
      frame.buffer.size = buf->size - pos;
      frame.buffer.data = &data[pos];
      frame.buffer.length = frame_length;
      //frame.buffer.pts = buf->pts;
      ap->cb(&frame, ap->user);
      pos += frame_length;
      continue;
    }
    ++pos;
  }

  if (pos < buf->length) {
    buffer_t *new;
    if (buf == ap->bufs[0]) new = ap->bufs[1];
    else new = ap->bufs[0];
    new->length = buf->length - pos;
    res = buffer_copy(new, buf, 0, pos, new->length);
    if (res != 0) {
      ERROR("%d", res);
      return -2;
    }
    ap->curr = new;
  } else {
    buf->length = 0;
  }

  return 0;
}

int adts_parser_parse(adts_parser_t *ap, buffer_t *buf) {
  if (!ap || !buf) return -1;
  return parse(ap, buf);
}

adts_parser_t *adts_parser_create(adts_parser_cb *cb, void *user) {
  adts_parser_t *ap;
  if (!cb || !user) return NULL;
  ap = calloc(1, sizeof(*ap));
  if (!ap) return NULL;
  ap->bufs[0] = buffer_alloc(65536);
  if (!ap->bufs[0]) {
    adts_parser_destroy(ap);
    return NULL;
  }
  ap->bufs[1] = buffer_alloc(65536);
  if (!ap->bufs[1]) {
    adts_parser_destroy(ap);
    return NULL;
  }
  ap->curr = ap->bufs[0];
  ap->cb = cb;
  ap->user = user;
  return ap;
}

int adts_parser_destroy(adts_parser_t *ap) {
  if (!ap) return -1;
  if (ap->bufs[0]) buffer_free(ap->bufs[0]);
  if (ap->bufs[1]) buffer_free(ap->bufs[1]);
  free(ap);
  return 0;
}
