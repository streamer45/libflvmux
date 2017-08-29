#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "frame.h"

typedef struct flv_muxer flv_muxer_t;

typedef int (flv_mux_cb)(buffer_t *buf, buffer_t *tag, void *user);

flv_muxer_t *flv_muxer_create(flv_mux_cb *cb, void *user);
int flv_muxer_destroy(flv_muxer_t *muxer);
int flv_muxer_mux(flv_muxer_t *muxer, buffer_t *buf, int type);
buffer_t *flv_muxer_get_header(flv_muxer_t *muxer);
