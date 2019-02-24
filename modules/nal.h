#if !defined(_nal_H)
# define _nal_H (1)

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

typedef struct nal_parser nal_parser_t;

typedef enum {
  NAL_NON_IDR = 1,
  NAL_IDR = 5,
  NAL_SEI = 6,
  NAL_SPS = 7,
  NAL_PPS = 8,
  NAL_AUD = 9
} nal_frame_type_t;

typedef struct {
  buffer_t nalu;
  buffer_t buffer;
  int long_sc;
  int type;
} nal_frame_t;

typedef int (nal_parse_cb)(nal_frame_t *, void *user);

nal_parser_t *nal_parser_create(nal_parse_cb *cb, void *user);
int nal_parser_destroy(nal_parser_t *np);
int nal_parser_parse(nal_parser_t *np, buffer_t *buf);
int nal_parser_flush(nal_parser_t *np);

#endif
