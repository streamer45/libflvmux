#include "buffer.h"

buffer_t *buffer_alloc(size_t size) {
  buffer_t *buf;
  buf = calloc(1, sizeof(*buf));
  if (!buf) return NULL;
  buf->size = size;
  buf->data = calloc(1, buf->size);
  buf->length = 0;
  return buf;
}

int buffer_free(buffer_t *buf) {
  if (!buf) return -1;
  free(buf->data);
  free(buf);
  return 0;
}

int buffer_write(buffer_t *buf, size_t off, uint8_t *src, size_t len) {
  int ret;
  ret = 0;
  if (!buf || !buf->data) ret = -1;
  if (buf->size < buf->length) ret = -2;
  if (buf->size < (off + len)) ret = -3;
  if (ret < 0) {
    ERROR("buffer_write error %d", ret);
    return ret;
  }
  memcpy(buf->data + off, src, len);
  return ret;
}

int buffer_read(buffer_t *buf, size_t off, uint8_t *dst, size_t len) {
  int ret;
  if (!buf || !buf->data) ret = -1;
  if (buf->size < buf->length) ret = -2;
  if (buf->length < (off + len)) ret = -3;
  if (ret < 0) {
    ERROR("buffer_read error %d", ret);
    return ret;
  }
  memcpy(dst, buf->data + off, len);
  return 0;
}

int buffer_copy(buffer_t *dst, buffer_t *src, size_t dst_off, size_t src_off, size_t len) {
  int ret;
  ret = 0;
  if (!dst || !src) ret = -1;
  if (!src->data || !dst->data) ret = -2;
  if (dst->size < dst->length) ret = -3;
  if (src->size < src->length) ret = -4;
  if (dst->size < (dst_off + len)) ret = -5;
  if (src->size < (src_off + len)) ret = -6;
  if (src->length < (src_off + len)) ret = -7;
  if (ret < 0) {
    ERROR("buffer_copy error %d", ret);
    return ret;
  }
  memcpy(dst->data + dst_off, src->data + src_off, len);
  return ret;
}
