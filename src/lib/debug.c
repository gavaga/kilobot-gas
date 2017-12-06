#include <kilolib.h>
#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "constants.h"
#include "types.h"
#include "neighbor.h"

void
print_neighbor_array(
    neighbor_info_t *n)
{
    printf("NEIGHBORS uid: %d; time: %d\n", kilo_uid, kilo_ticks);
    for (uint32_t i = 0; i < MAX_NEIGHBORS; ++i) {
        printf("\tid: %x, last_time: %d, flags: 0x%0x, loc: (%0.2f, %0.2f)\n",
            n[i].id,
            n[i].last_time,
            n[i].flags,
            n[i].loc.x, n[i].loc.y);
    }
}

void
print_pairwise_dist_array(
    kilo_dist_t *pd)
{
    printf("PWD_ARR uid: %d; time: %d\n", kilo_uid, kilo_ticks);
    for (uint32_t i = 0; i < MAX_NEIGHBORS; ++i) {
        printf("\t");
        for (uint32_t j = 0; j < i; ++j) {
            printf("%4.1f\t", (double)get_dist_idx(pd, i, j));
        }
        printf("\n");
    }
}

void
print_neighbor_bit_array(
    uint8_t *flags)
{
    printf("NEIGHBOR_BITS uid: %d; time: %d\n", kilo_uid, kilo_ticks);
    for (uint32_t i = 0; i < MAX_NEIGHBORS; ++i) {
        printf("\t");
        for (uint32_t j = 0; j <= i; ++j) {
            printf("%c", are_neighbors(flags, i, j)? 'O': 'X');
        }
        printf("\n");
    }
}

void
print_message(
    ohn_msg_t *msg)
{
    printf("MSG uid: %d; time: %d\n", kilo_uid, kilo_ticks);
    printf("\tsender: %x\n", msg->sender_id);
    printf("\ttype: %x\n", msg->msg_type);
    printf("\tohn_id: %x\n", msg->ohn_id);
    printf("\tohn_dist: %0.2f\n", (double)msg->ohn_dist);
}
