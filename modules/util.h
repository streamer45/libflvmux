#include <stdio.h>
#include <stdint.h>

#define ERROR(fmt, ...) \
  { \
    fprintf(stderr, "E: %s@%s:%d ", __func__, __FILE__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
  }

#define DEBUG(fmt, ...) \
  { \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
  }

#define true 1
#define false 0

typedef uint8_t bool;
