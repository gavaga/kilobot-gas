#include <stdio.h>

#include "state.h"
#include "nbi.h"
#include "bitarray.h"
#include "led.h"
#include "debug.h"

#include <kilolib.h>

/*! state
 *
 * Global state for the program
 */
state_t state;

/*! setup
 *
 * Function called to setup everything before
 * the main loop starts running
 */
void setup()
{
    // seed the software rng
    rand_seed(rand_hard());
    // initialize the state
    state_init(&state);
}

/*! loop
 *
 * Main function loop
 */
void loop()
{
    // update the current time
    state.ticks = kilo_ticks;

    // print the state before each loop
    /*state_print(&state);*/

    // if the state is new, run the setup function
    if (state_flag_is_set(&state, SF_NEW_STATE)) {
        state.setup(&state);
        state_clr_flag(&state, SF_NEW_STATE);
    }

    // run led function
    state.led(&state);

    // run loop function
    state.loop(&state);

    // if the msg_sent flag wasn't handled/cleared by the
    // loop or led functions, clear it anyway so it's valid
    // each loop
    state_clr_flag(&state, SF_MSG_SENT);
}

/*! message_rx
 *
 * Handler for message receipt
 */
void message_rx(message_t *m, distance_measurement_t *d)
{
    msg_data_t *msg = (msg_data_t*)m->data;
    kb_dist_t dist = estimate_distance(d);

    // Invoke state-specific message handlers
    state.msg_rx(&state, msg, dist);
}

/*! message_tx
 *
 * Retrieve a message to send when we finished sending the previous
 * one
 */
message_t *message_tx()
{
    return state_get_next_msg(&state);
}

/*! message_tx_success
 *
 * Called on successful message send.
 */
void message_tx_success()
{
    state_set_flag(&state, SF_MSG_SENT);
}

/*! main
 *
 * Entrypoint for kilobot behavior
 */
int main()
{
    // Initialize the hardware
    kilo_init();

    // setup callbacks
    kilo_message_rx         = message_rx;
    kilo_message_tx         = message_tx;
    kilo_message_tx_success = message_tx_success;

    // Register the program entrypoints and start running
    kilo_start(setup, loop);
    
    return 0;
}
