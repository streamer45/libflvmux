#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lib.h"
#include "util.h"

int mux_cb(buffer_t *buf, buffer_t *tag, void *user) {
  //DEBUG("%zu", pkt->length);
  write(1, buf->data, buf->length);
  return 0;
}

int test() {
  int ret;
  int vfd;
  int afd;
  buffer_t *buf;
  flv_muxer_t *muxer;

  buf = buffer_alloc(4096);
  if (!buf) return -1;

  vfd = open("../tsmux/samples/in.h264", O_RDONLY);
  if (vfd < 0) return -3;

  afd = open("../tsmux/samples/in.aac", O_RDONLY);
  if (afd < 0) return -4;

  muxer = flv_muxer_create(mux_cb, NULL);
  if (!muxer) return -5;

  bool vend = false;
  bool aend = false;

  for (;;) {
    if (vend && aend) break;
    if (!vend) {
      ret = read(vfd, buf->data, buf->size);
      if (ret == 0) vend = true;
      if (ret < 0) return -6;
      buf->length = ret;
      ret = flv_muxer_mux(muxer, buf, FRAME_VIDEO);
      if (ret != 0) return -7;
    }
    if (!aend) {
      ret = read(afd, buf->data, buf->size);
      if (ret == 0) aend = true;
      if (ret < 0) return -8;
      buf->length = ret;
      ret = flv_muxer_mux(muxer, buf, FRAME_AUDIO);
      if (ret != 0) return -9;
    }
  }

  ret = flv_muxer_destroy(muxer);
  if (ret != 0) return -10;

  ret = buffer_free(buf);
  if (ret != 0) return -11;

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
