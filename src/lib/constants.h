#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PI                          3.14159265359f
#define TWO_PI                      6.28318530718f

#define MAX_NEIGHBORS               16
#define PAIRWISE_DIST_ARR_SIZE      136
//PAIRWISE_DIST_ARR_SIZE/8 rounded up
#define NEIGHBOR_FLAGS_BIT_ARR_SIZE 17
#define NEIGHBOR_TIMEOUT_TICKS      256
#define COORD_UPDATE_INTERVAL       128
#define UNKNOWN_DIST                0xffff
#define INVALID_INDEX               0xdead
#define INVALID_SIZE                INVALID_INDEX
#define KB_DIST_INVALID             ((kb_dist_t)-1)
#define KB_ID_INVALID               0xffff

//TODO: find the real number
#define COMM_RANGE                  100.0f

#define HULL_COLOR                  RGB(2, 0, 2) // magenta
#define NON_HULL_COLOR              RGB(1, 1, 1) // white
#define HULL_THRESHOLD              0.015 // 1.5% tolerance for hull detection

// Static allocation stuff
#define STATIC_ALLOC
#define STATIC_SIZE_MSG_Q_RAW_LIST 128
#define STATIC_SIZE_MD_LIST        STATIC_SIZE_MSG_Q_RAW_LIST
#define STATIC_SIZE_NBI_NBRS       MAX_NEIGHBORS
#define STATIC_SIZE_NBI_PD_DATA    MAX_NEIGHBORS*(MAX_NEIGHBORS+1)/2
#define STATIC_SIZE_NBI_ADJ_DATA   (MAX_NEIGHBORS*MAX_NEIGHBORS + sizeof(uint8_t) - 1)/sizeof(uint8_t)

#endif
