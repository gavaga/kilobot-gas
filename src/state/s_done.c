#include "state.h"

#include <kilolib.h>

#include "led.h"

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(DONE)(state_t *st)
{
}

/*! led_done
 *
 * Led function for current state
 */
void
LED_NAME(DONE)(state_t *st)
{
    LED_GREEN;
}

/*! state_done
 *
 * Handler function for S_DONE main loop functionality
 */
void
LOOP_NAME(DONE)(state_t *st)
{
}

/*! state_done_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_DONE
 */
void
MSG_RX_NAME(DONE)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
