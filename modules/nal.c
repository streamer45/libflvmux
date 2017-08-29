#include "util.h"
#include "nal.h"

struct nal_parser {
  buffer_t *bufs[2];
  buffer_t *curr;
  size_t nframes;
  nal_parser_cb *cb;
  void *user;
};

static int checkNALStart(uint8_t *buf) {
  if (buf[0] == 0x00 && buf[1] == 0x00) {
    if (buf[2] == 0x01) return 3;
    else if (buf[2] == 0x00 && buf[3] == 0x01) return 4;
  }
  return 0;
}

static int checkNALEnd(uint8_t *buf) {
  if (buf[0] == 0x00 && buf[1] == 0x00) {
    if (buf[2] == 0x01 || buf[2] == 0x00) return 3;
  }
  return 0;
}

static int parse(nal_parser_t *np, buffer_t *in) {
  int res;
  bool inNAL = false;
  size_t pos = 0;
  size_t nalStart = 0;
  size_t nalStartHdrSz = 0;
  size_t nalEnd = 0;
  nal_frame_t frame;
  uint8_t *data;
  buffer_t *buf;

  buf = np->curr;
  res = buffer_copy(buf, in, buf->length, 0, in->length);

  //if (buf->pts == -1) buf->pts = in->pts;

  if (res != 0) {
    ERROR("%d", res);
    return -2;
  }

  buf->length += in->length;
  data = buf->data;

  for (;;) {
    if (!inNAL) {
      if (buf->length < (pos + 4)) break;
      res = checkNALStart(&data[pos]);
      if (res > 0) {
        nalStartHdrSz = res;
        pos += res;
        nalStart = pos;
        inNAL = true;
        continue;
      }
    } else {
      if (buf->length < (pos + 3)) break;
      res = checkNALEnd(&data[pos]);
      if (res > 0) {
        nalEnd = pos;
        inNAL = false;
        ++np->nframes;
        memset(&frame, 0, sizeof(frame));
        frame.type = data[nalStart] & 0x1f;
        frame.buffer.data = &data[nalStart - nalStartHdrSz];
        frame.buffer.size = buf->size - (nalStart - nalStartHdrSz);
        frame.buffer.length = nalEnd - nalStart + nalStartHdrSz;
        //frame.buffer.pts = buf->pts;
        np->cb(&frame, np->user);
        //buf->pts = -1;
        continue;
      }
    }
    ++pos;
  }

  if (buf->length > nalEnd) {
    buffer_t *new;
    if (buf == np->bufs[0]) new = np->bufs[1];
    else new = np->bufs[0];
    new->length = buf->length - nalEnd;
    res = buffer_copy(new, buf, 0, nalEnd, new->length);
    if (res != 0) {
      ERROR("%d", res);
      return -2;
    }
    np->curr = new;
  }

  return 0;
}

nal_parser_t *nal_parser_create(nal_parser_cb *cb, void *user) {
  nal_parser_t *np;
  if (!cb || !user) return NULL;
  np = calloc(1, sizeof(*np));
  if (!np) return NULL;
  np->bufs[0] = buffer_alloc(65536 * 4);
  if (!np->bufs[0]) {
    nal_parser_destroy(np);
    return NULL;
  }
  //np->bufs[0]->pts = -1;
  np->bufs[1] = buffer_alloc(65536 * 4);
  if (!np->bufs[1]) {
    nal_parser_destroy(np);
    return NULL;
  }
  //np->bufs[1]->pts = -1;
  np->curr = np->bufs[0];
  np->cb = cb;
  np->user = user;
  return np;
}

int nal_parser_destroy(nal_parser_t *np) {
  if (!np) return -1;
  if (np->bufs[0]) buffer_free(np->bufs[0]);
  if (np->bufs[1]) buffer_free(np->bufs[1]);
  free(np);
  return 0;
}

int nal_parser_parse(nal_parser_t *np, buffer_t *buf) {
  if (!np || !buf) return -1;
  return parse(np, buf);
}
