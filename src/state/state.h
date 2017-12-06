#ifndef __STATE_H__
#define __STATE_H__

#include "types.h"
#include "led.h"
#include "msg.h"

// Forward Declarations
typedef struct msg_data_t msg_data_t;
typedef struct message_t message_t;
typedef struct nbrs_info_t nbrs_info_t;
typedef struct fifo_t fifo_t;
typedef struct state_t state_t;

// Function type definitions
typedef void setup_func_t(state_t*);
typedef void state_func_t(state_t*);
typedef void led_func_t(state_t*);

/*! state_t
 *
 * Represents entire robot state
 */
typedef struct state_t
{
    /*! s
     *
     * Current state (STATE MACHINE)
     */
    uint8_t s;

    /*! r
     *
     * Current role
     */
    uint8_t r;

#define SA_STOP      0x0
#define SA_FORWARD   0x1
#define SA_RIGHT     0x2
#define SA_LEFT      0x3
#define SA_FWD_RIGHT 0x4
#define SA_FWD_LEFT  0x5

    /*! a
     *
     * Current action
     */
    uint8_t a;

#define SF_REPEAT_LAST      (0x1 << 0)
#define SF_NEW_STATE        (0x1 << 1)
#define SF_NEW_ROLE         (0x1 << 2)
#define SF_MSG_SENT         (0x1 << 3)

    /*! flags
     *
     * Some flags
     */
    uint8_t flags;

    /*! ticks
     *
     * Current time of the simulation/clock
     */
    kb_time_t ticks;

    /*! msg
     *
     * Actual message to send
     */
    message_t *msg;

    /*! msg_q
     *
     * Queue of message data to be sent
     */
    fifo_t *msg_q;

    /*! md_list_sz
     *
     * Maximum number of message data that can be allocated at once
     */
    uint32_t md_list_sz;

    /*! md_list
     *
     * Pointer to static allocator space for msg data objects
     */
    msg_data_t *md_list;

    /*! nbi
     *
     * Neighbors information
     */
    nbrs_info_t *nbi;

    /*! s_state
     *
     * State-specific state information
     */
    void *ss;

    //
    // Function Pointers
    //
    
    /*! setup
     *
     * Pointer to current setup function
     */
    setup_func_t *setup;

    /*! loop
     *
     * Pointer to current main state loop function
     */
    state_func_t *loop;

    /*! led
     *
     * Pointer to the current function for the LED
     */
    led_func_t *led;

    /*! msg_rx
     *
     * Pointer to current function for message handling
     */
    msg_rx_handler_func_t *msg_rx;
} state_t;

/*! *_NAME
 *
 * Name functions to get the appropriate compiler
 * name from a given state token
 */
#define STATE_NAME(NAME)  S_ ## NAME
#define SETUP_NAME(NAME)  setup_ ## NAME
#define LOOP_NAME(NAME)   state_ ## NAME
#define LED_NAME(NAME)    led_ ## NAME
#define MSG_RX_NAME(NAME) state_ ## NAME ## _msg_rx_handler
#define ROLE_NAME(NAME)   R_ ## NAME

/*! DECLARE_STATE
 *
 * Helper macro to define constant and declare functions for a state
 */
#define DECLARE_STATE(NAME) \
    extern const uint8_t STATE_NAME(NAME); \
    setup_func_t SETUP_NAME(NAME); \
    state_func_t LOOP_NAME(NAME); \
    led_func_t LED_NAME(NAME); \
    msg_rx_handler_func_t MSG_RX_NAME(NAME)

/*! DEFINE_STATE
 *
 * Actually define the state. Should be invoked at the top of state.c
 */
#define DEFINE_STATE(NAME, ID) \
    const uint8_t STATE_NAME(NAME) = (ID)

/*! DECLARE_ROLE
 *
 * Helper macros to define constant and led function for a state
 */
#define DECLARE_ROLE(NAME) \
    extern const uint8_t ROLE_NAME(NAME); \
    led_func_t LED_NAME(NAME)

/*! DEFINE_ROLE
 *
 * Defines the role constant. Should be invoked at the top of state.c
 */
#define DEFINE_ROLE(NAME, ID) \
    const uint8_t ROLE_NAME(NAME) = (ID)

/*! STATE_SET
 *
 * Helper macro to change state. Only changes LED state if our role is
 * NONE.
 */
#define STATE_SET(st, NAME) \
    do { \
        uint8_t s = STATE_NAME(NAME); \
        if (s != st->s) { \
            state_set_flag(st, SF_NEW_STATE); \
            st->s = s; \
            st->setup = SETUP_NAME(NAME); \
            st->loop = LOOP_NAME(NAME); \
            st->msg_rx = MSG_RX_NAME(NAME); \
            if (st->r == ROLE_NAME(NONE)) { \
                st->led = LED_NAME(NAME); \
            } \
        } \
    } while(0)

/*! ROLE_SET
 *
 * Helper macro to change role
 */
#define STATE_SET_ROLE(st, NAME) \
    do { \
        uint8_t r = ROLE_NAME(NAME); \
        if (r != st->r) { \
            state_set_flag(st, SF_NEW_ROLE); \
            st->r = r; \
            st->led = LED_NAME(NAME); \
        } \
    } while(0)

// States (possible values for s)
DECLARE_STATE(ERR);
DECLARE_STATE(IDLE);
DECLARE_STATE(WAIT);
DECLARE_STATE(G_OHN);
DECLARE_STATE(LCV);
DECLARE_STATE(ELECT);
DECLARE_STATE(TWIDDLE);
DECLARE_STATE(COALESCE);
DECLARE_STATE(FINALIZE);
DECLARE_STATE(SEARCH);
DECLARE_STATE(AVOID);
DECLARE_STATE(DONE);

// Roles
DECLARE_ROLE(NONE);
DECLARE_ROLE(BORDER);
DECLARE_ROLE(SEED);

// Constructor/Destructor
void state_init(state_t *st);
void state_reset(state_t *st);

// Accessors/Mutators
void state_set(state_t *st, uint8_t s);
void state_set_role(state_t *st, uint8_t r);

bool state_flag_is_set(state_t *st, uint8_t flag);
void state_set_flag(state_t *st, uint8_t flag);
void state_clr_flag(state_t *st, uint8_t flag);

// Message related functions
message_t *state_get_next_msg(state_t *st);
void state_push_msg(state_t *st, msg_data_t *data);

// Debug
void state_print(state_t *st);
char *state_getstr(uint8_t s);
char *role_getstr(uint8_t r);
char *action_getstr(uint8_t a);

// Functions affecting hardware
void state_set_motion(state_t *st, uint8_t a);

#endif
