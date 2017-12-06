#include "state.h"

#include <kilolib.h>

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(TWIDDLE)(state_t *st)
{
}

/*! led_twiddle
 *
 * Led function for current state
 */
void
LED_NAME(TWIDDLE)(state_t *st)
{
}

/*! state_twiddle
 *
 * Handler function for S_TWIDDLE main loop functionality
 */
void
LOOP_NAME(TWIDDLE)(state_t *st)
{
}

/*! state_twiddle_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_TWIDDLE
 */
void
MSG_RX_NAME(TWIDDLE)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
