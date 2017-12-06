/*! file: kb/message.h
 *
 * author: Gavin McDowell
 * created: 2017-11-28
 *
 * Holds all the message structures, and declarations for all
 * the corresponding handler functions.
 *
 * changelog:
 */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "types.h"

// Forward Declarations
typedef struct state_t state_t;
typedef struct message_t message_t;
typedef struct msg_data_t msg_data_t;

// Function types
typedef void msg_rx_handler_func_t(state_t*, msg_data_t*, kb_dist_t);

/*! *_NAME
 *
 * Helper naming macros
 */
#define MSG_NAME(NAME) M_ ## NAME
#define MSG_DEFAULT_RX_NAME(NAME) msg_rx_handler_ ## NAME

/*! DECLARE_MSG
 *
 * Helper macro to make a new message type
 */
#define DECLARE_MSG(NAME) \
    extern const uint8_t MSG_NAME(NAME); \
    msg_rx_handler_func_t MSG_DEFAULT_RX_NAME(NAME)

/*! DEFINE_MSG
 *
 * Defines the message type--should be invoked at the top of msg.c
 */
#define DEFINE_MSG(NAME, ID) \
    const uint8_t MSG_NAME(NAME) = (ID)

// Message types
DECLARE_MSG(NONE);
DECLARE_MSG(ID);
DECLARE_MSG(OHN);

/*! msg_data_t
 *
 * Type for message data.
 */
typedef struct __attribute__((__packed__)) msg_data_t {
    /*! type
     *
     * Type of the message. Must be the first element
     * in all msg data structs
     */
    uint8_t type;

    /*! rsvd
     *
     * Explicit padding. Used for other things in other
     * message types.
     */
    uint8_t rsvd[8];
} msg_data_t;

/*! msg_data_id_t
 *
 * Type for an ID message
 */
typedef struct __attribute__((__packed__)) msg_data_id_t {
    /*! type
     *
     * Type of the message. Must be the first element
     * in all msg data structs
     */
    uint8_t  type;

    /*! sender_id
     *
     * uid of the sender
     */
    uint16_t sender_id;

    /*! rsvd
     *
     * Explicit padding
     */
    uint8_t rsvd[6];
} msg_data_id_t;

/*! msg_data_ohn_t
 *
 * Type for a one-hop neighbor message
 */
typedef struct __attribute__((__packed__)) msg_data_ohn_t {
    /*! type
     *
     * Type of the message. Must be the first element
     * in all msg data structs
     */
    uint8_t  type;

    /*! sender_id
     *
     * uid of the sender
     */
    uint16_t sender_id;

    /*! ohn_id
     *
     * id of the one-hop neighbor to gossip about
     */
    uint16_t ohn_id;

    /*! ohn_dist
     *
     * Estimated distance to the ohn
     */
    uint32_t ohn_dist;
} msg_data_ohn_t;

// Raw message functions
void message_init(message_t *m);
void message_update_data(message_t *m, msg_data_t *data);

// Generic constructor/destructor
msg_data_t *msg_data_create(state_t *st, uint8_t type);
void msg_data_init(msg_data_t *msg, uint8_t type);
void msg_data_delete(msg_data_t *msg);

// Debug
char *msg_data_getstr(uint8_t type);
void msg_data_print(void *msg, char *pref);
void message_print(message_t *m, char *pref);

#endif
