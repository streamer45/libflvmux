#if !defined(_buffer_H)
# define _buffer_H (1)

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "util.h"

typedef struct {
  uint8_t *data;
  size_t size;
  size_t length;
} buffer_t;

buffer_t *buffer_alloc(size_t size);
int buffer_free(buffer_t *buf);
int buffer_write(buffer_t *buf, size_t off, uint8_t *src, size_t len);
int buffer_read(buffer_t *buf, size_t off, uint8_t *dst, size_t len);
int buffer_copy(buffer_t *dst, buffer_t *src, size_t dst_off, size_t src_off, size_t len);

#endif
