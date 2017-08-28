#if !defined(_frame_H)
# define _frame_H (1)

#include "buffer.h"

typedef enum {
  AUDIO_FRAME = 1,
  VIDEO_FRAME = 2
} frame_type_t;

typedef struct {
  int channels;
  int rate;
  int fmt;
} audio_frame_t;

typedef struct {
  int width;
  int height;
  int rate;
  bool keyframe;
} video_frame_t;

typedef struct {
  buffer_t buf;
  int64_t pts;
  int64_t dts;
  int type;
  union {
    audio_frame_t audio;
    video_frame_t video;
  };
} frame_t;

#endif
