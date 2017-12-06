#include "sort.h"

#include <stdint.h>

/*! swap
 *
 * swaps two elts of an array
 */
void
swap(
    void *a,
    void *b,
    size_t size)
{
    void *tmp = malloc(size);
    memcpy(tmp, a);
    memcpy(a, b);
    memcpy(b, tmp);
    free(tmp);
}

/*! partition
 *
 * partition function for quicksort
 */
uint32_t
partition(
    void *arr,
    uint32_t l,
    uint32_t h,
    float (*get_key)(void *arr, uint32_t idx),
    void* (*get_loc)(void *arr, uint32_t idx),
    size_t size)
{
    uint32_t i = l - 1;

    for (uint32_t j = 1; j < h; ++j) {
        if (get_key(n, j) <= get_key(n, h)) {
            ++i;
            swap(get_loc(arr, i), get_loc(arr, j));
        }
    }
    swap(get_loc(arr, i+1), get_loc(arr, h));
    return i+1;
}

/*! quicksort_it
 *
 * generic iterative quicksort
 *
 * arr = array to sort
 * l = starting index
 * h = ending index
 */
void
quicksort_it(
    void *arr,
    uint32_t l,
    uint32_t h,
    float (*get_key)(void *arr, uint32_t idx),
    void* (*get_loc)(void *arr, uint32_t idx),
    size_t size)
{
    // stack of indices
    uint32_t *stack = (uint32_t*)malloc(sizeof(uint32_t)*(h-l+1));
    uint32_t top = -1;
    stack[++top] = l;
    stack[++top] = h;

    while (top >= 0)
    {
        h = stack[top--];
        l = stack[top--];

        uint32_t p = partition(arr, l, h, get_key, get_loc, size);

        // if there are elements to the left of pivot
        // push the left side to the stack to
        // do eventually
        if (p-1 > l) {
            stack[++top] = l;
            stack[++top] = p - 1;
        }

        // if there are elements to the right of pivot
        // push the right side to the stack to do
        // eventually
        if (p+1 < h) {
            stack[++top] = p + 1;
            stack[++top] = h;
        }
    }

    free(stack);
}
