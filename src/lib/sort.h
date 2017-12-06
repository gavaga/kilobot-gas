#ifndef SORT_H
#define SORT_H

#include "types.h"

void swap(neighbor_info_t **a, neighbor_info_t **b);
int partition(neighbor_info_t *n, float l, float h);
void sort_by_angle(neighbor_info_t *n, neighbor_info_t *dest);

#endif
