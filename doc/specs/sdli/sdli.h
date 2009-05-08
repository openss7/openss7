/*
 *  The purpose of the SDL interface is to provide separation between the
 *  SDTI (Signalling Data Terminal Interface) which provides SS7 Signalling
 *  Data Terminal (SDT) state machine services including DAEDR, DAEDT, AERM,
 *  SUERM and EIM, and the underlying driver which provides access to the
 *  line (L1).
 */

typedef lmi_long sdl_long;
typedef lmi_ulong sdl_ulong;
typedef lmi_ushort sdl_ushort;
typedef lmi_uchar sdl_uchar;

#define SDL_PROTO_BASE                   32L

#define SDL_DSTR_FIRST                  ( 1L + SDL_PROTO_BASE)
#define SDL_BITS_FOR_TRANSMISSION_REQ   ( 1L + SDL_PROTO_BASE)
#define SDL_CONNECT_REQ                 ( 2L + SDL_PROTO_BASE)
#define SDL_DISCONNECT_REQ              ( 3L + SDL_PROTO_BASE)
#define SDL_DSTR_LAST                   ( 3L + SDL_PROTO_BASE)

#define SDL_USTR_LAST                   (-1L - SDL_PROTO_BASE)
#define SDL_RECEIVED_BITS_IND           (-1L - SDL_PROTO_BASE)
#define SDL_DISCONNECT_IND              (-2L - SDL_PROTO_BASE)
#define SDL_USTR_FIRST                  (-2L - SDL_PROTO_BASE)

#define SDL_DISCONNECTED    0
#define SDL_CONNECTED       1

/*
 *  SDLI PROTOCOL PRIMITIVES
 */

/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ, M_PROTO w/ M_DATA or M_DATA
 *  -------------------------------------------------------------------------
 *  Used by the SDT to send bits to the SDL.
 */
typedef struct {
    sdl_long sdl_primitive;     /* SDL_BITS_FOR_TRANSMISSION_REQ */
} sdl_bits_for_transmission_req_t;

/*
 *  SDL_CONNECT_REQ, M_PROTO or M_PCPROTO
 *  -------------------------------------------------------------------------
 *  Used by the SDT to request that it be connected to the line.  Connection
 *  to the line might require some switching or other mecahnism.
 */
typedef struct {
    sdl_long sdl_primitive;     /* SDL_CONNECT_REQ */
    sdl_ulong sdl_flags;        /* direction flags */
} sdl_connect_req_t;

#define SDL_RX_DIRECTION        0x01
#define SDL_TX_DIRECTION        0x02

/*
 *  SDL_DISCONNECT_REQ, M_PROTO or M_PCPROTO
 *  -------------------------------------------------------------------------
 *  Used by the SDT to request that it be disconnected from the line.
 *  Disconnection from the line might require some switching or other
 *  mecahnism.
 */
typedef struct {
    sdl_long sdl_primitive;     /* SDL_DISCONNECT_REQ */
    sdl_ulong sdl_flags;        /* direction flags */
} sdl_disconnect_req_t;

/*
 *  SDL_RECEIVED_BITS_IND, M_PROTO w/ M_DATA or M_DATA
 *  -------------------------------------------------------------------------
 *  Used by the SDL to send received bits to the SDT.
 */
typedef struct {
    sdl_long sdl_primitive;     /* SDL_RECEIVED_BITS_IND */
} sdl_received_bits_ind_t;

/*
 *  SDL_DISCONNECT_IND, M_PROTO or M_PCPROTO
 *  -------------------------------------------------------------------------
 *  Used by the SDL to indicated to the SDT that it has been disconnected from
 *  the line.
 */
typedef struct {
    sdl_long sdl_primitive;     /* SDL_DISCONNECT_IND */
} sdl_disconnect_ind_t;

union SDL_primitives {
    sdl_long sdl_primitive;
    sdl_bits_for_transmission_req_t bits_for_transmission_req;
    sdl_connect_req_t connect_req;
    sdl_disconnect_req_t disconnect_req;
    sdl_received_bits_ind_t received_bits_ind;
    sdl_disconnect_ind_t disconnect_ind;
};

#define SDL_BITS_FOR_TRANSMISSION_REQ_SIZE      sizeof(sdl_bits_for_transmission_req_t)
#define SDL_CONNECT_REQ_SIZE                    sizeof(sdl_connect_req_t)
#define SDL_DISCONNECT_REQ_SIZE                 sizeof(sdl_disconnect_req_t)
#define SDL_RECEIVED_BITS_IND_SIZE              sizeof(sdl_received_bits_ind_t)
#define SDL_DISCONNECT_IND_SIZE                 sizeof(sdl_disconnect_ind_t)
