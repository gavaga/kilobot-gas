#include "state.h"

#include <kilolib.h>

#include "constants.h"
#include "kb_math.h"

#include "localize.h"
#include "msg.h"

typedef struct ss_avoid_t {
} ss_avoid_t;

// static singleton object
ss_avoid_t _avoid_data;

/*! setup_avoid
 *
 * Function called once when this state is entered
 */
void
SETUP_NAME(AVOID)(state_t *st)
{
    st->ss = &_avoid_data;
}

/*! led_avoid
 *
 * Led function for current state
 */
void
LED_NAME(AVOID)(state_t *st)
{
}

/*! state_avoid
 *
 * Handler function for S_AVOID main loop functionality
 */
void
LOOP_NAME(AVOID)(state_t *st)
{
    state_set_motion(st, SA_FORWARD);
}

/*! state_avoid_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_AVOID
 *
 * Continue to gossip one-hop neighbors when
 * information changes (within threshold)
 */
void
MSG_RX_NAME(AVOID)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
    if (msg->type == MSG_NAME(OHN)) {
        MSG_DEFAULT_RX_NAME(OHN)(st, msg, dist);
    } else if (msg->type == MSG_NAME(ID)) {
        MSG_DEFAULT_RX_NAME(ID)(st, msg, dist);
    } else {
        MSG_DEFAULT_RX_NAME(NONE)(st, msg, dist);
    }
}
