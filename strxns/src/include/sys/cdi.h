#ifndef _SYS_CDI_H
#define _SYS_CDI_H

#ident "@(#) cdi.h 1.20 11/17/03"

/*
 * cdi.h header for Communications Device Interface
 *
 * Copyright (c) 1989 NCR Comten
 *
 * This file distributed by Gcom, Inc with permission of NCR Comten
 */

/*
 * Primitives for Local Management Services
 */
#define CD_INFO_REQ     	0x00    /* Information request */
#define CD_INFO_ACK     	0x01    /* Information acknowledgement */
#define CD_ATTACH_REQ   	0x02    /* Attach a PPA */
#define CD_DETACH_REQ   	0x03    /* Detach a PPA */
#define CD_ENABLE_REQ   	0x04    /* Prepare a device */
#define CD_DISABLE_REQ  	0x05    /* Disable a device */
#define CD_OK_ACK       	0x06    /* Success acknowledgement */
#define CD_ERROR_ACK    	0x07    /* Error acknowledgement */
#define CD_ENABLE_CON   	0x08    /* Enable confirmation */
#define CD_DISABLE_CON  	0x09    /* Disable confirmation */
#define CD_ERROR_IND    	0x0a    /* Error indication */

/*
 * Primitives used for Data Transfer
 */
#define CD_ALLOW_INPUT_REQ      0x0b    /* Allow input */
#define CD_READ_REQ     	0x0c    /* Wait-for-input request */
#define CD_UNITDATA_REQ 	0x0d    /* Data send request */
#define CD_WRITE_READ_REQ       0x0e    /* Write/read request */
#define CD_UNITDATA_ACK 	0x0f    /* Data send acknowledgement */
#define CD_UNITDATA_IND 	0x10    /* Data receive indication */
#define CD_HALT_INPUT_REQ       0x11    /* Halt input */
#define CD_ABORT_OUTPUT_REQ     0x12    /* Abort output */
#define CD_MUX_NAME_REQ		0x13	/* get mux name (Gcom) */
#define	CD_BAD_FRAME_IND	0x14	/* frame w/error (Gcom extension) */
#define	CD_MODEM_SIG_REQ	0x15	/* Assert modem signals (Gcom) */
#define	CD_MODEM_SIG_IND	0x16	/* Report modem signal state (Gcom) */
#define CD_MODEM_SIG_POLL	0x17	/* requests a CD_MODEM_SIG_IND (Gcom) */


/*
 * CDI device states
 */
#define CD_UNATTACHED   	0x00    /* No PPA attached */
#define CD_UNUSABLE     	0x01    /* PPA cannot be used */
#define CD_DISABLED     	0x02    /* PPA attached */
#define CD_ENABLE_PENDING       0x03    /* Waiting ack of enable req */
#define CD_ENABLED      	0x04    /* Awaiting use */
#define CD_READ_ACTIVE  	0x05    /* Input section enabled; */
					/* disabled after data arrives */
#define CD_INPUT_ALLOWED        0x06    /* Input section permanently enabled */
#define CD_DISABLE_PENDING      0x07    /* Waiting ack of disable req */
#define CD_OUTPUT_ACTIVE        0x08    /* Output section active only */
#define	CD_XRAY			0x09	/* Xray-ing another ppa */
#define CD_NOT_AUTH		0x0A	/* Not authorized, unusable */

/*
 * CD_ERROR_ACK and CD_ERROR_IND error return values
 */
#define CD_BADADDRESS   0x01    /* Address was invalid */
#define CD_BADADDRTYPE  0x02    /* Invalid address type */
#define CD_BADDIAL      0x03    /* Dial information was invalid */
#define CD_BADDIALTYPE  0x04    /* Invalid dial information type */
#define CD_BADDISPOSAL  0x05    /* Invalid disposal parameter */
#define CD_BADFRAME     0x06    /* Defective SDU received */
#define CD_BADPPA       0x07    /* Invalid PPA identifier */
#define CD_BADPRIM      0x08    /* Unrecognized primitive */
#define CD_DISC 	0x09    /* Disconnected */
#define CD_EVENT        0x0a    /* Protocol-specific event occurred */
#define CD_FATALERR     0x0b    /* Device has become unusable */
#define CD_INITFAILED   0x0c    /* Line initialization failed */
#define CD_NOTSUPP      0x0d    /* Primitive not supported by this device */
#define CD_OUTSTATE     0x0e    /* Primitive was issued from an invalid state */
#define CD_PROTOSHORT   0x0f    /* M_PROTO block too short */
#define CD_READTIMEOUT  0x10    /* Read request timed out before data arrived */
#define CD_SYSERR       0x11    /* UNIX system error */
#define CD_WRITEFAIL    0x12    /* Unitdata request failed */

/*
 * Error explanations
 */
#define CD_CRCERR       	0x01    /* CRC or FCS error */
#define CD_DLE_EOT      	0x02    /* DLE EOT detected */
#define CD_FORMAT       	0x03    /* Format error detected */
#define CD_HDLC_ABORT   	0x04    /* Aborted frame detected */
#define CD_OVERRUN      	0x05    /* Input overrun */
#define CD_TOOSHORT     	0x06    /* Frame too short */
#define CD_INCOMPLETE   	0x07    /* Partial frame received */
#define CD_BUSY 		0x08    /* Telephone was busy */
#define CD_NOANSWER     	0x09    /* Connection went unanswered */
#define CD_CALLREJECT   	0x0a    /* Connection rejected */
#define CD_HDLC_IDLE    	0x0b    /* HDLC line went idle */
#define CD_HDLC_NOTIDLE 	0x0c    /* HDLC line no longer idle */
#define CD_QUIESCENT    	0x0d    /* Line being reassigned */
#define CD_RESUMED      	0x0e    /* Line has been reassigned */
#define CD_DSRTIMEOUT   	0x0f    /* Did not see DSR in time */
#define CD_LAN_COLLISIONS       0x10    /* LAN excessive collisions */
#define CD_LAN_REFUSED  	0x11    /* LAN message refused */
#define CD_LAN_NOSTATION        0x12    /* LAN no such station */
#define CD_LOSTCTS      	0x13    /* Lost Clear to Send signal */
#define CD_DEVERR       	0x100   /* Start of device-specific codes */

/*
 * CDI device classes
 */
#define CD_HDLC 	0x00    /* Bit-synchronous */
#define CD_BISYNC       0x01    /* Character-synchronous */
#define CD_LAN          0x02    /* ISO 8802-3,4,5 local-area network MAC */
#define	CD_NODEV	0x03	/* no device, ppa used for X-ray */

/*
 * CDI duplex types
 */
#define CD_FULLDUPLEX   0x00    /* Full duplex; allow input supported */
#define CD_HALFDUPLEX   0x01    /* Half duplex; read and write/read supported 
*/

/*
 * CDI output styles
 */
#define CD_UNACKEDOUTPUT 0x00    /* No unitdata acknowledgements */
#define CD_ACKEDOUTPUT   0x01    /* Unitdata acknowledgements */
#define CD_PACEDOUTPUT   0x02    /* Unitdata acks as output timing hints */

/*
 * CDI optional features
 */
#define CD_CANREAD      0x01    /* Read request supported on full duplex */
#define CD_CANDIAL      0x02    /* Dial information supported */
#define CD_AUTOALLOW    0x04    /* CD_INPUT_ALLOWED as soon as enabled */
#define CD_KEEPALIVE	0x08	/* Gcom: Don't send off at CD_DISABLE_REQ */

/*
 * CDI provider style.
 * The CDI provider style which determines whether a provider
 * requires a CD_ATTACH_REQ to inform the provider which PPA
 * user messages should be sent/received on.
 */
#define CD_STYLE1       0x00    /* PPA is implicitly bound by open(2) */
#define CD_STYLE2       0x01    /* PPA must be explicitly bound via 
CD_ATTACH_REQ */
#define	CD_STYLE_1	CD_STYLE1	/* Gcom -- to match document */
#define	CD_STYLE_2	CD_STYLE2	/* Gcom -- to match document */

/*
 * Symbolic value for "no dialing information"
 */
#define CD_NODIAL       0x00

/*
 * Actions to take with undelivered data in a CD_DISABLE_REQ or
CD_HALT_INPUT_REQ
 */
#define CD_FLUSH        0x00    /* Discard undelivered data */
#define CD_WAIT 	0x01    /* Attempt to deliver unsent data */
#define CD_DELIVER      0x02

/*
 * Address types
 */
#define CD_SPECIFIC     0x00    /* Specific address follows */
#define CD_BROADCAST    0x01    /* Broadcast; no address follows */
#define CD_IMPLICIT     0x02    /* No address or embedded address */

/*
 * Error types for CD_BAD_FRAME_IND
 */

#define	CD_FRMTOOLONG	0xFFFF		/* frame overflowed rcv bfr */
#define	CD_FRMNONOCTET	0xFFFE		/* frame not octet-aligned */
#define	CD_EMPTY_BFR	0xFFFD		/* empty rcv buffer (not used) */
#define	CD_BAD_CRC	0xFFFC		/* CRC error */
#define	CD_FRM_ABORTED	0xFFFB		/* frame aborted */
#define	CD_RCV_OVERRUN	0xFFFA		/* receive overrun */


/*
 * Modem signal bits for modem signal related requests and indications
 */
#define CD_DTR		0x01
#define CD_RTS		0x02
#define CD_DSR		0x04
#define CD_DCD		0x08
#define CD_CTS		0x10
#define CD_RI		0x20


/*
 * CDI interface primitive definitions.
 *
 * Each primitive is sent as a Stream message.  It is possible that
 * the messages may be viewed as a sequence of bytes that have the
 * following form without any padding. The structure definition
 * of the following messages may have to change depending on the
 * underlying hardware architecture and crossing of a hardware
 * boundary with a different hardware architecture.
 *
 * Each message has the name defined followed by the
 * Stream message type (M_PROTO, M_PCPROTO, M_DATA)
 */
#if !defined(_LINUX_TYPES_H) && !defined(_INTTYPES_H)
# if defined(__sparcv9)			/* 64-bit SPARC compile */
    typedef unsigned int	cd_ulong ;
    typedef unsigned short	cd_ushort ;
# else
    typedef unsigned long	cd_ulong ;
    typedef unsigned short	cd_ushort ;
# endif
#else
    typedef uint32_t		cd_ulong ;
    typedef uint16_t		cd_ushort ;
#endif


/*
 *      LOCAL MANAGEMENT PRIMITIVES
 */

/*
 * CD_INFO_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
} cd_info_req_t;

/*
 * CD_INFO_ACK, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
        cd_ulong   cd_max_sdu;
        cd_ulong   cd_min_sdu;
        cd_ulong   cd_class;
        cd_ulong   cd_duplex;
        cd_ulong   cd_output_style;
        cd_ulong   cd_features;
        cd_ulong   cd_addr_length;
        cd_ulong   cd_ppa_style;
} cd_info_ack_t;

/*
 * CD_ATTACH_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_ppa;
} cd_attach_req_t;

/*
 * CD_DETACH_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
} cd_detach_req_t;

/*
 * CD_ENABLE_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_dial_type;
        cd_ulong   cd_dial_length;
        cd_ulong   cd_dial_offset;
} cd_enable_req_t;

/*
 * CD_DISABLE_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_disposal;
} cd_disable_req_t;

/*
 * CD_OK_ACK, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
        cd_ulong   cd_correct_primitive;
} cd_ok_ack_t;

/*
 * CD_ERROR_ACK, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
        cd_ulong   cd_error_primitive;
        cd_ulong   cd_errno;
        cd_ulong   cd_explanation;
} cd_error_ack_t;

/*
 * CD_ENABLE_CON, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
} cd_enable_con_t;

/*
 * CD_DISABLE_CON, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
} cd_disable_con_t;

/*
 * CD_ERROR_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
        cd_ulong   cd_errno;
        cd_ulong   cd_explanation;
} cd_error_ind_t;

/*
 *      DATA TRANSFER PRIMITIVES
 */

/*
 * CD_ALLOW_INPUT_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
} cd_allow_input_req_t;

/*
 * CD_READ_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_msec;
} cd_read_req_t;

/*
 * CD_UNITDATA_REQ, optional M_PROTO type, with M_DATA block(s)
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ushort  cd_addr_type;
        cd_ushort  cd_priority;
        cd_ulong   cd_dest_addr_length;
        cd_ulong   cd_dest_addr_offset;
} cd_unitdata_req_t;

/*
 * CD_WRITE_READ_REQ, M_PROTO type
 */
typedef struct {
        cd_ulong           cd_primitive;
        cd_unitdata_req_t       cd_unitdata_req;
        cd_read_req_t           cd_read_req;
} cd_write_read_req_t;

/*
 * CD_UNITDATA_ACK, M_PROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
} cd_unitdata_ack_t;

/*
 * CD_UNITDATA_IND, optional M_PROTO type, with M_DATA block(s)
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
        cd_ulong   cd_src_addr_length;
        cd_ulong   cd_src_addr_offset;
        cd_ushort  cd_addr_type;
        cd_ushort  cd_priority;
        cd_ulong   cd_dest_addr_length;
        cd_ulong   cd_dest_addr_offset;
} cd_unitdata_ind_t;

/*
 * CD_BAD_FRAME_IND, M_PROTO type, with M_DATA block(s)
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_state;
	cd_ulong   cd_error ;		/* what is wrong with the frame */

} cd_bad_frame_ind_t;

/*
 * CD_MODEM_SIG_REQ, M_PROTO type
 *
 * Assert the modem signals with '1' bits in the cd_sigs mask and
 * drop those signals with '0' bits.  Sensed modem signals such
 * as DCD or CTS are ignored.
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_sigs;

} cd_modem_sig_req_t;

/*
 * CD_MODEM_SIG_IND, M_PROTO type
 *
 * The cd_sigs field reports the current state of the modem signals.
 * This message is sent when modem signals change at the hardware
 * interface.  Only changes in signals selected by the cd_modem_sig_enb_req_t
 * cd_sigs mask will be evaluated for purposes of change detection.
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_sigs;

} cd_modem_sig_ind_t;

typedef struct
{
        cd_ulong   cd_primitive;

} cd_modem_sig_poll_t;


/*
 * CD_HALT_INPUT_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
        cd_ulong   cd_disposal;
} cd_halt_input_req_t;

/*
 * CD_ABORT_OUTPUT_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        cd_ulong   cd_primitive;
} cd_abort_output_req_t;

union CD_primitives {
        cd_ulong           cd_primitive;
        cd_info_req_t           info_req;
        cd_info_ack_t           info_ack;
        cd_attach_req_t attach_req;
        cd_detach_req_t detach_req;
        cd_enable_req_t enable_req;
        cd_disable_req_t        disable_req;
        cd_ok_ack_t             ok_ack;
        cd_error_ack_t          error_ack;
        cd_enable_con_t enable_con;
        cd_disable_con_t        disable_con;
        cd_error_ind_t          error_ind;
        cd_allow_input_req_t    allow_input_req;
        cd_read_req_t           read_req;
        cd_unitdata_req_t       unitdata_req;
        cd_write_read_req_t     write_read_req;
        cd_unitdata_ack_t       unitdata_ack;
        cd_unitdata_ind_t       unitdata_ind;
	cd_halt_input_req_t     halt_input_req;
};

#define CD_INFO_REQ_SIZE                sizeof(cd_info_req_t)
#define CD_INFO_ACK_SIZE                sizeof(cd_info_ack_t)
#define CD_ATTACH_REQ_SIZE              sizeof(cd_attach_req_t)
#define CD_DETACH_REQ_SIZE              sizeof(cd_detach_req_t)
#define CD_ENABLE_REQ_SIZE              sizeof(cd_enable_req_t)
#define CD_DISABLE_REQ_SIZE             sizeof(cd_disable_req_t)
#define CD_OK_ACK_SIZE          	sizeof(cd_ok_ack_t)
#define CD_ERROR_ACK_SIZE               sizeof(cd_error_ack_t)
#define CD_ALLOW_INPUT_REQ_SIZE 	sizeof(cd_allow_input_req_t)
#define CD_READ_REQ_SIZE                sizeof(cd_read_req_t)
#define CD_UNITDATA_REQ_SIZE            sizeof(cd_unitdata_req_t)
#define CD_WRITE_READ_REQ_SIZE  	sizeof(cd_write_read_req_t)
#define CD_UNITDATA_ACK_SIZE            sizeof(cd_unitdata_ack_t)
#define CD_UNITDATA_IND_SIZE            sizeof(cd_unitdata_ind_t)
#define CD_HALT_INPUT_REQ_SIZE          sizeof(cd_halt_input_req_t)
#define CD_ABORT_OUTPUT_REQ_SIZE        sizeof(cd_abort_output_req_t)
#define CD_ERROR_IND_SIZE               sizeof(cd_error_ind_t)
#define CD_ENABLE_CON_SIZE              sizeof(cd_enable_con_t)
#define CD_DISABLE_CON_SIZE             sizeof(cd_disable_con_t)
#define CD_BAD_FRAME_IND_SIZE		sizeof(cd_bad_frame_ind_t)
#define CD_MODEM_SIG_REQ_SIZE		sizeof(cd_modem_sig_req_t)
#define CD_MODEM_SIG_IND_SIZE		sizeof(cd_modem_sig_ind_t)
#define CD_MODEM_SIG_POLL_SIZE		sizeof(cd_modem_sig_poll_t)

#endif /* _SYS_CDI_H */

