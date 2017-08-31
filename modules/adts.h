#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

typedef struct adts_parser adts_parser_t;

typedef struct {
  buffer_t frame;
  buffer_t buffer;
  bool header;
} adts_frame_t;

typedef int (adts_parser_cb)(adts_frame_t *, void *user);

adts_parser_t *adts_parser_create(adts_parser_cb *cb, void *user);
int adts_parser_destroy(adts_parser_t *ap);
int adts_parser_parse(adts_parser_t *ap, buffer_t *buf);
