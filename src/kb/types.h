#ifndef TYPES_H
#define TYPES_H

// for fixed_size integers
#include <stdint.h>
// for bool
#include <stdbool.h>

typedef uint16_t kb_id_t;
typedef uint16_t kb_dist_t;
typedef float    kb_pos_t;
typedef uint32_t kb_time_t;
typedef int32_t status_t;

typedef struct point_t {
    kb_pos_t x, y;
} point_t;

#endif
