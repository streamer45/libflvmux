#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nal.h"
#include "buffer.h"

typedef struct avcc_transmuxer avcc_transmuxer_t;

typedef int (avcc_transmux_cb)(buffer_t *buf, void *user);

avcc_transmuxer_t *avcc_transmuxer_create(avcc_transmux_cb *cb, void *user);
int avcc_transmuxer_destroy(avcc_transmuxer_t *t);
int avcc_transmux(avcc_transmuxer_t *t, nal_frame_t *nalu);
int avcc_get_header(avcc_transmuxer_t *t, buffer_t *hdr);
