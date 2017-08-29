#if !defined(_frame_H)
# define _frame_H (1)

#include "buffer.h"

typedef enum {
  FRAME_AUDIO = 1,
  FRAME_VIDEO
} frame_type_t;

typedef enum {
  FRAME_AAC = 1,
  FRAME_MP3
} frame_audio_format_t;

typedef enum {
  FRAME_H264 = 1
} frame_video_format_t;

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
  int fmt;
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
