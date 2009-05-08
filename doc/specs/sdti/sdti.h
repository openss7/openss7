 /* 
  * The purpose of the SDT interface is to provide a separation between
  * the SL (Signalling Link) interface which provides SS7 Level 2 (LINK)
  * state machine services and the underlying driver which provides
  * essentially HDLC capablities.  In SS7 the entity providing HDLC
  * services is called the Signalling Data Terminal (SDT).  An SDTI
  * implements the AERM/SUERM/EIM and DAEDR/DAEDT capabilities and
  * communicates upstream to the Signalling Link using the primitives
  * provided here.
  *
  * The SDT interface also recognizes Local Management Interface (LMI)
  * primitives defined elsewhere <sys/ss7/lmi.h>.
  */

typedef lmi_long sdt_long;
typedef lmi_ulong sdt_ulong;
typedef lmi_ushort sdt_ushort;
typedef lmi_uchar sdt_uchar;

#define SDT_PROTO_BASE                           48L

#define SDT_DSTR_FIRST                          ( 1L + SDT_PROTO_BASE)
#define SDT_DAEDT_TRANSMISSION_REQ              ( 1L + SDT_PROTO_BASE)
#define SDT_DAEDT_START_REQ                     ( 2L + SDT_PROTO_BASE)
#define SDT_DAEDR_START_REQ                     ( 3L + SDT_PROTO_BASE)
#define SDT_AERM_START_REQ                      ( 4L + SDT_PROTO_BASE)
#define SDT_AERM_STOP_REQ                       ( 5L + SDT_PROTO_BASE)
#define SDT_AERM_SET_TI_TO_TIN_REQ              ( 6L + SDT_PROTO_BASE)
#define SDT_AERM_SET_TI_TO_TIE_REQ              ( 7L + SDT_PROTO_BASE)
#define SDT_SUERM_START_REQ                     ( 8L + SDT_PROTO_BASE)
#define SDT_SUERM_STOP_REQ                      ( 9L + SDT_PROTO_BASE)
#define SDT_DSTR_LAST                           ( 9L + SDT_PROTO_BASE)

#define SDT_USTR_LAST                           (-1L - SDT_PROTO_BASE)
#define SDT_RC_SIGNAL_UNIT_IND                  (-1L - SDT_PROTO_BASE)
#define SDT_RC_CONGESTION_ACCEPT_IND            (-2L - SDT_PROTO_BASE)
#define SDT_RC_CONGESTION_DISCARD_IND           (-3L - SDT_PROTO_BASE)
#define SDT_RC_NO_CONGESTION_IND                (-4L - SDT_PROTO_BASE)
#define SDT_IAC_CORRECT_SU_IND                  (-5L - SDT_PROTO_BASE)
#define SDT_IAC_ABORT_PROVING_IND               (-6L - SDT_PROTO_BASE)
#define SDT_LSC_LINK_FAILURE_IND                (-7L - SDT_PROTO_BASE)
#define SDT_TXC_TRANSMISSION_REQUEST_IND        (-8L - SDT_PROTO_BASE)
#define SDT_USTR_FIRST                          (-8L - SDT_PROTO_BASE)

/*
 *  STATE
 */
#define SDTS_POWER_OFF                  0
#define SDTS_IDLE                       1
#define SDTS_ABORTED_PROVING            2
#define SDTS_NORMAL_PROVING             3
#define SDTS_EMERGENCY_PROVING          4
#define SDTS_MONITORING_ERRORS          5
#define SDTS_MONITORING                 6

/*
 *  FLAGS
 */
#define SDTF_DAEDT_ACTIVE               (1<<0)
#define SDTF_DAEDR_ACTIVE               (1<<1)
#define SDTF_AERM_ACTIVE                (1<<2)
#define SDTF_SUERM_ACTIVE               (1<<3)

/*
 *  SDT_RC_SIGNAL_UNIT_IND, M_DATA or M_PROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_RC_SIGNAL_UNIT_IND */
    sdt_ulong sdt_count;
} sdt_rc_signal_unit_ind_t;

/*
 *  SDT_DAEDT_TRANSMISSION_REQ, M_DATA or M_PROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_DAEDT_TRANSMISSION_REQ */
} sdt_daedt_transmission_req_t;

/*
 *  SDT_DAEDT_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_DAEDT_START_REQ */
} sdt_daedt_start_req_t;

/*
 *  SDT_DAEDR_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_DAEDR_START_REQ */
} sdt_daedr_start_req_t;

/*
 *  SDT_IAC_CORRECT_SU_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_IAC_CORRECT_SU_IND */
} sdt_iac_correct_su_ind_t;

/*
 *  SDT_AERM_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_AERM_START_REQ */
} sdt_aerm_start_req_t;

/*
 *  SDT_AERM_STOP_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_AERM_STOP_REQ */
} sdt_aerm_stop_req_t;

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_AERM_SET_TI_TO_TIN_REQ */
} sdt_aerm_set_ti_to_tin_req_t;

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_AERM_SET_TI_TO_TIE_REQ */
} sdt_aerm_set_ti_to_tie_req_t;

/*
 *  SDT_IAC_ABORT_PROVING_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_IAC_ABORT_PROVING_IND */
} sdt_iac_abort_proving_ind_t;

/*
 *  SDT_SUERM_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_SUERM_START_REQ */
} sdt_suerm_start_req_t;

/*
 *  SDT_SUERM_STOP_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_SUERM_STOP_REQ */
} sdt_suerm_stop_req_t;

/*
 *  SDT_LSC_LINK_FAILURE_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_LSC_LINK_FAILURE_IND */
} sdt_lsc_link_failure_ind_t;

/*
 *  SDT_RC_CONGESTION_ACCEPT_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_RC_CONGESTION_ACCEPT_IND */
} sdt_rc_congestion_accept_ind_t;

/*
 *  SDT_RC_CONGESTION_DISCARD_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_RC_CONGESTION_DISCARD_IND */
} sdt_rc_congestion_discard_ind_t;

/*
 *  SDT_RC_NO_CONGESTION_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_RC_NO_CONGESTION_IND */
} sdt_rc_no_congestion_ind_t;

/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
    sdt_long sdt_primitive;     /* SDT_TXC_TRANSMISSION_REQUEST_IND */
} sdt_txc_transmission_request_ind_t;

union SDT_primitives {
    sdt_long sdt_primitive;
    sdt_daedt_transmission_req_t daedt_transmission_req;
    sdt_daedt_start_req_t daedt_start_req;
    sdt_daedr_start_req_t daedr_start_req;
    sdt_aerm_start_req_t aerm_start_req;
    sdt_aerm_stop_req_t aerm_stop_req;
    sdt_aerm_set_ti_to_tin_req_t aerm_set_ti_to_tin_req;
    sdt_aerm_set_ti_to_tie_req_t aerm_set_ti_to_tie_req;
    sdt_suerm_start_req_t suerm_start_req;
    sdt_suerm_stop_req_t suerm_stop_req;
    sdt_rc_signal_unit_ind_t rc_signal_unit_ind;
    sdt_rc_congestion_accept_ind_t rc_congestion_accept_ind;
    sdt_rc_congestion_discard_ind_t rc_congestion_discard_ind;
    sdt_rc_no_congestion_ind_t rc_no_congestion_ind;
    sdt_iac_correct_su_ind_t iac_correct_su_ind;
    sdt_iac_abort_proving_ind_t iac_abort_proving_ind;
    sdt_lsc_link_failure_ind_t lsc_link_failure_ind;
    sdt_txc_transmission_request_ind_t txc_transmission_request_ind;
};

#define SDT_DAEDT_TRANSMISSION_REQ_SIZE         sizeof(sdt_daedt_transmission_req_t)
#define SDT_DAEDR_START_REQ_SIZE                sizeof(sdt_daedr_start_req_t)
#define SDT_DAEDT_START_REQ_SIZE                sizeof(sdt_daedt_start_req_t)
#define SDT_AERM_START_REQ_SIZE                 sizeof(sdt_aerm_start_req_t)
#define SDT_AERM_STOP_REQ_SIZE                  sizeof(sdt_aerm_stop_req_t)
#define SDT_AERM_SET_TI_TO_TIN_REQ_SIZE         sizeof(sdt_aerm_set_ti_to_tin_req_t)
#define SDT_AERM_SET_TI_TO_TIE_REQ_SIZE         sizeof(sdt_aerm_set_ti_to_tie_req_t)
#define SDT_SUERM_START_REQ_SIZE                sizeof(sdt_suerm_start_req_t)
#define SDT_SUERM_STOP_REQ_SIZE                 sizeof(sdt_suerm_stop_req_t)
#define SDT_RC_SIGNAL_UNIT_IND_SIZE             sizeof(sdt_rc_signal_unit_ind_t)
#define SDT_RC_CONGESTION_ACCEPT_IND_SIZE       sizeof(sdt_rc_congestion_accept_ind_t)
#define SDT_RC_CONGESTION_DISCARD_IND_SIZE      sizeof(sdt_rc_congestion_discard_ind_t)
#define SDT_RC_NO_CONGESTION_IND_SIZE           sizeof(sdt_rc_no_congestion_ind_t)
#define SDT_IAC_CORRECT_SU_IND_SIZE             sizeof(sdt_iac_correct_su_ind_t)
#define SDT_IAC_ABORT_PROVING_IND_SIZE          sizeof(sdt_iac_abort_proving_ind_t)
#define SDT_LSC_LINK_FAILURE_IND_SIZE           sizeof(sdt_lsc_link_failure_ind_t)
#define SDT_TXC_TRANSMISSION_REQUEST_IND_SIZE   sizeof(sdt_txc_transmission_request_ind_t)
