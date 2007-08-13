/*****************************************************************************

 @(#) sli.h,v 0.9.2.1 2007/08/13 19:55:44 brian Exp

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified 2007/08/13 19:55:44 by brian

 -----------------------------------------------------------------------------

 sli.h,v
 Revision 0.9.2.1  2007/08/13 19:55:44  brian
 - added spec headers

 Revision 0.9.2.7  2007/08/12 16:19:53  brian
 - new PPA handling

 Revision 0.9.2.6  2007/08/03 13:35:01  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.5  2007/06/17 01:56:02  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SS7_SLI_H__
#define __SS7_SLI_H__

#ident "@(#) sli.h,v (0.9.2.1) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

typedef lmi_long sl_long;
typedef lmi_ulong sl_ulong;
typedef lmi_ushort sl_ushort;
typedef lmi_uchar sl_uchar;

#define SL_PROTO_BASE                             64

#define SL_DSTR_FIRST                           (  1 + SL_PROTO_BASE)
#define SL_PDU_REQ                              (  1 + SL_PROTO_BASE)
#define SL_EMERGENCY_REQ                        (  2 + SL_PROTO_BASE)
#define SL_EMERGENCY_CEASES_REQ                 (  3 + SL_PROTO_BASE)
#define SL_START_REQ                            (  4 + SL_PROTO_BASE)
#define SL_STOP_REQ                             (  5 + SL_PROTO_BASE)
#define SL_RETRIEVE_BSNT_REQ                    (  6 + SL_PROTO_BASE)
#define SL_RETRIEVAL_REQUEST_AND_FSNC_REQ       (  7 + SL_PROTO_BASE)
#define SL_CLEAR_BUFFERS_REQ                    (  8 + SL_PROTO_BASE)
#define SL_CLEAR_RTB_REQ                        (  9 + SL_PROTO_BASE)
#define SL_CONTINUE_REQ                         ( 10 + SL_PROTO_BASE)
#define SL_LOCAL_PROCESSOR_OUTAGE_REQ           ( 11 + SL_PROTO_BASE)
#define SL_RESUME_REQ                           ( 12 + SL_PROTO_BASE)
#define SL_CONGESTION_DISCARD_REQ               ( 13 + SL_PROTO_BASE)
#define SL_CONGESTION_ACCEPT_REQ                ( 14 + SL_PROTO_BASE)
#define SL_NO_CONGESTION_REQ                    ( 15 + SL_PROTO_BASE)
#define SL_POWER_ON_REQ                         ( 16 + SL_PROTO_BASE)
#define SL_OPTMGMT_REQ                          ( 17 + SL_PROTO_BASE)
#define SL_NOTIFY_REQ                           ( 18 + SL_PROTO_BASE)
#define SL_DSTR_LAST                            ( 18 + SL_PROTO_BASE)

#define SL_USTR_LAST                            ( -1 - SL_PROTO_BASE)
#define SL_PDU_IND                              ( -1 - SL_PROTO_BASE)
#define SL_LINK_CONGESTED_IND                   ( -2 - SL_PROTO_BASE)
#define SL_LINK_CONGESTION_CEASED_IND           ( -3 - SL_PROTO_BASE)
#define SL_RETRIEVED_MESSAGE_IND                ( -4 - SL_PROTO_BASE)
#define SL_RETRIEVAL_COMPLETE_IND               ( -5 - SL_PROTO_BASE)
#define SL_RB_CLEARED_IND                       ( -6 - SL_PROTO_BASE)
#define SL_BSNT_IND                             ( -7 - SL_PROTO_BASE)
#define SL_IN_SERVICE_IND                       ( -8 - SL_PROTO_BASE)
#define SL_OUT_OF_SERVICE_IND                   ( -9 - SL_PROTO_BASE)
#define SL_REMOTE_PROCESSOR_OUTAGE_IND          (-10 - SL_PROTO_BASE)
#define SL_REMOTE_PROCESSOR_RECOVERED_IND       (-11 - SL_PROTO_BASE)
#define SL_RTB_CLEARED_IND                      (-12 - SL_PROTO_BASE)
#define SL_RETRIEVAL_NOT_POSSIBLE_IND           (-13 - SL_PROTO_BASE)
#define SL_BSNT_NOT_RETRIEVABLE_IND             (-14 - SL_PROTO_BASE)
#define SL_OPTMGMT_ACK                          (-15 - SL_PROTO_BASE)
#define SL_NOTIFY_IND                           (-16 - SL_PROTO_BASE)
#define SL_LOCAL_PROCESSOR_OUTAGE_IND           (-17 - SL_PROTO_BASE)
#define SL_LOCAL_PROCESSOR_RECOVERED_IND        (-18 - SL_PROTO_BASE)
#define SL_USTR_FIRST                           (-18 - SL_PROTO_BASE)

/*
 *  SLI PROVIDER STATE
 */
#define SLS_POWER_OFF                    0
#define SLS_OUT_OF_SERVICE               1
#define SLS_NOT_ALIGNED                  2
#define SLS_INITIAL_ALIGNMENT            3
#define SLS_PROVING                      4
#define SLS_ALIGNED_READY                5
#define SLS_ALIGNED_NOT_READY            6
#define SLS_IN_SERVICE                   7
#define SLS_PROCESSOR_OUTAGE             8

/*
 *  SLI PROVIDER FLAGS
 */
#define SLF_LOC_PROC_OUT        (1<< 0)
#define SLF_REM_PROC_OUT        (1<< 1)
#define SLF_LOC_IN_SERV         (1<< 2)
#define SLF_REM_IN_SERV         (1<< 3)
#define SLF_LOC_BUSY            (1<< 4)
#define SLF_REM_BUSY            (1<< 5)
#define SLF_LOC_EMERG           (1<< 6)
#define SLF_EMERGENCY           SLF_LOC_EMERG
#define SLF_REM_EMERG           (1<< 7)
#define SLF_RECV_MSU            (1<< 8)
#define SLF_SEND_MSU            (1<< 9)
#define SLF_CONG_ACCEPT         (1<<10)
#define SLF_CONG_DISCARD        (1<<11)
#define SLF_RTB_FULL            (1<<12)
#define SLF_L3_CONG_DETECT      (1<<13)
#define SLF_L2_CONG_DETECT      (1<<14)
#define SLF_LINK_CONGESTED      SLF_L2_CONG_DETECT
#define SLF_CONTINUE            (1<<15)
#define SLF_LEVEL_3_IND         SLF_CONTINUE
#define SLF_CLEAR_RTB           (1<<16)
#define SLF_NEED_FLUSH          (1<<17)
#define SLF_WAIT_SYNC           (1<<18)
#define SLF_REM_ALIGN           (1<<19)

/*
 *  SLI PROTOCOL PRIMITIVES
 */

/*
 *  SL_PDU_REQ, optional M_PROTO type, with M_DATA block(s)
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_mp;
} sl_pdu_req_t;

/*
 *  SL_PDU_IND, optional M_PROTO type, with M_DATA block(s)
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_mp;
} sl_pdu_ind_t;

/*
 *  PROTOCOL CONTROL PRIMITIVES
 */

/*
 *  SL_EMERGENCY_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_emergency_req_t;

/*
 *  SL_EMERGENCY_CEASES_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_emergency_ceases_req_t;

/*
 *  SL_START_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_start_req_t;

/*
 *  SL_STOP_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_stop_req_t;

/*
 *  SL_RETRIEVE_BSNT_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_retrieve_bsnt_req_t;

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_fsnc;
} sl_retrieval_req_and_fsnc_t;

/*
 *  SL_CLEAR_BUFFERS_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_clear_buffers_req_t;

/*
 *  SL_CLEAR_RTB_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_clear_rtb_req_t;

/*
 *  SL_CONTINUE_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_continue_req_t;

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_local_proc_outage_req_t;

/*
 *  SL_RESUME_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_resume_req_t;

/*
 *  SL_CONGESTION_DISCARD_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_cong_discard_req_t;

/*
 *  SL_CONGESTION_ACCEPT_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_cong_accept_req_t;

/*
 *  SL_NO_CONGESTION_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_no_cong_req_t;

/*
 * SL_POWER_ON_REQ, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_power_on_req_t;

/*
 *  SL_LINK_CONGESTED_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_timestamp;
        sl_ulong sl_cong_status;        /* congestion status */
        sl_ulong sl_disc_status;        /* discard status */
} sl_link_cong_ind_t;

/*
 *  SL_LINK_CONGESTION_CEASED_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_timestamp;
        sl_ulong sl_cong_status;        /* congestion status */
        sl_ulong sl_disc_status;        /* discard status */
} sl_link_cong_ceased_ind_t;

/*
 *  SL_RETRIEVED_MESSAGE_IND, M_PROTO or M_PCPROTO type with M_DATA block(s)
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_mp;
} sl_retrieved_msg_ind_t;

/*
 *  SL_RETRIEVAL_COMPLETE_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_mp;
} sl_retrieval_comp_ind_t;

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_retrieval_not_poss_ind_t;

/*
 *  SL_RB_CLEARED_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_rb_cleared_ind_t;

/*
 *  SL_BSNT_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_bsnt;
} sl_bsnt_ind_t;

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_bsnt;
} sl_bsnt_not_retr_ind_t;

/*
 *  SL_IN_SERVICE_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_in_service_ind_t;

/*
 *  SL_OUT_OF_SERVICE_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_timestamp;
        sl_ulong sl_reason;
} sl_out_of_service_ind_t;

/*
 *  These reasons for failure as so that upstream module can
 *  collect statistics per link per ITU-T Q.752 Table 1
 *  requirements.
 */
#define SL_FAIL_UNSPECIFIED             0x0001
#define SL_FAIL_CONG_TIMEOUT            0x0002
#define SL_FAIL_ACK_TIMEOUT             0x0004
#define SL_FAIL_ABNORMAL_BSNR           0x0008
#define SL_FAIL_ABNORMAL_FIBR           0x0010
#define SL_FAIL_SUERM_EIM               0x0020
#define SL_FAIL_ALIGNMENT_NOT_POSSIBLE  0x0040
#define SL_FAIL_RECEIVED_SIO            0x0080
#define SL_FAIL_RECEIVED_SIN            0x0100
#define SL_FAIL_RECEIVED_SIE            0x0200
#define SL_FAIL_RECEIVED_SIOS           0x0400
#define SL_FAIL_T1_TIMEOUT              0x0800

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_timestamp;
} sl_rem_proc_out_ind_t;

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_timestamp;
} sl_rem_proc_recovered_ind_t;

/*
 *  SL_RTB_CLEARED_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
} sl_rtb_cleared_ind_t;

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_timestamp;
} sl_loc_proc_out_ind_t;

/*
 *  SL_LOCAL_PROCESSOR_RECOVERED_IND, M_PROTO or M_PCPROTO type
 */
typedef struct {
        sl_long sl_primitive;
        sl_ulong sl_timestamp;
} sl_loc_proc_recovered_ind_t;

/*
 *  Generic single argument type
 */
typedef struct {
        sl_ulong sl_cmd;
        sl_ulong sl_arg;
} sl_cmd_arg_t;

/*
 *  Generic double argument type
 */
typedef struct {
        sl_ulong sl_cmd;
        sl_ulong sl_arg1;
        sl_ulong sl_arg2;
} sl_cmd_2arg_t;

/*
 *  Generic triple argument type
 */
typedef struct {
        sl_ulong sl_cmd;
        sl_ulong sl_arg1;
        sl_ulong sl_arg2;
        sl_ulong sl_arg3;
} sl_cmd_3arg_t;

union SL_primitives {
        sl_long sl_primitive;
        sl_cmd_arg_t cmd_arg;
        sl_cmd_2arg_t cmd_2arg;
        sl_cmd_3arg_t cmd_3arg;
        sl_pdu_req_t pdu_req;
        sl_pdu_ind_t pdu_ind;
        sl_emergency_req_t emergency_req;
        sl_emergency_ceases_req_t emergency_ceases_req;
        sl_start_req_t start_req;
        sl_stop_req_t stop_req;
        sl_retrieve_bsnt_req_t retrieve_bsnt_req;
        sl_retrieval_req_and_fsnc_t retrieval_req_and_fsnc;
        sl_resume_req_t resume_req;
        sl_continue_req_t continue_req;
        sl_clear_buffers_req_t clear_buffers_req;
        sl_clear_rtb_req_t clear_rtb_req;
        sl_local_proc_outage_req_t local_proc_outage_req;
        sl_cong_discard_req_t cong_discard_req;
        sl_cong_accept_req_t cong_accept_req;
        sl_no_cong_req_t no_cong_req;
        sl_power_on_req_t power_on_req;
        sl_link_cong_ind_t link_cong_ind;
        sl_link_cong_ceased_ind_t link_cong_ceased_ind;
        sl_retrieved_msg_ind_t retrieved_msg_ind;
        sl_retrieval_comp_ind_t retrieval_comp_ind;
        sl_retrieval_not_poss_ind_t retrieval_not_poss_ind;
        sl_rb_cleared_ind_t rb_cleared_ind;
        sl_bsnt_ind_t bsnt_ind;
        sl_bsnt_not_retr_ind_t bsnt_not_retr_ind;
        sl_in_service_ind_t in_service_ind;
        sl_out_of_service_ind_t out_of_service_ind;
        sl_rem_proc_out_ind_t rem_proc_out_ind;
        sl_rem_proc_recovered_ind_t rem_proc_recovered_ind;
        sl_rtb_cleared_ind_t rtb_cleared_ind;
        sl_loc_proc_out_ind_t loc_proc_out_ind;
        sl_loc_proc_recovered_ind_t loc_proc_recovered_ind;
};

typedef union SL_primitives sl_prim_t;

#define SL_CMD_ARG_SIZE                 sizeof(sl_cmd_arg_t)
#define SL_CMD_2ARG_SIZE                sizeof(sl_cmd_2arg_t)
#define SL_CMD_3ARG_SIZE                sizeof(sl_cmd_3arg_t)
#define SL_PDU_REQ_SIZE                 sizeof(sl_pdu_req_t)
#define SL_PDU_IND_SIZE                 sizeof(sl_pdu_ind_t)
#define SL_EMERGENCY_REQ_SIZE           sizeof(sl_emergency_req_t)
#define SL_EMERGENCY_CEASES_REQ_SIZE    sizeof(sl_emergency_ceases_req_t)
#define SL_START_REQ_SIZE               sizeof(sl_start_req_t)
#define SL_STOP_REQ_SIZE                sizeof(sl_stop_req_t)
#define SL_RETRIEVE_BSNT_REQ_SIZE       sizeof(sl_retrieve_bsnt_req_t)
#define SL_RETRIEVAL_REQ_AND_FSNC_SIZE  sizeof(sl_retrieval_req_and_fsnc_t)
#define SL_RESUME_REQ_SIZE              sizeof(sl_resume_req_t)
#define SL_CONTINUE_REQ_SIZE            sizeof(sl_continue_req_t)
#define SL_CLEAR_BUFFERS_REQ_SIZE       sizeof(sl_clear_buffers_req_t)
#define SL_CLEAR_RTB_REQ_SIZE           sizeof(sl_clear_rtb_req_t)
#define SL_LOCAL_PROC_OUTAGE_REQ_SIZE   sizeof(sl_local_proc_outage_req_t)
#define SL_CONG_DISCARD_REQ_SIZE        sizeof(sl_cong_discard_req_t)
#define SL_CONG_ACCEPT_REQ_SIZE         sizeof(sl_cong_accept_req_t)
#define SL_NO_CONG_REQ_SIZE             sizeof(sl_no_cong_req_t)
#define SL_POWER_ON_REQ_SIZE            sizeof(sl_power_on_req_t)
#define SL_LINK_CONG_IND_SIZE           sizeof(sl_link_cong_ind_t)
#define SL_LINK_CONG_CEASED_IND_SIZE    sizeof(sl_link_cong_ceased_ind_t)
#define SL_RETRIEVED_MSG_IND_SIZE       sizeof(sl_retrieved_msg_ind_t)
#define SL_RETRIEVAL_COMP_IND_SIZE      sizeof(sl_retrieval_comp_ind_t)
#define SL_RETRIEVAL_NOT_POSS_IND_SIZE  sizeof(sl_retrieval_not_poss_ind_t)
#define SL_RB_CLEARED_IND_SIZE          sizeof(sl_rb_cleared_ind_t)
#define SL_BSNT_IND_SIZE                sizeof(sl_bsnt_ind_t)
#define SL_BSNT_NOT_RETR_IND_SIZE       sizeof(sl_bsnt_not_retr_ind_t)
#define SL_IN_SERVICE_IND_SIZE          sizeof(sl_in_service_ind_t)
#define SL_OUT_OF_SERVICE_SIZE          sizeof(sl_out_of_service_ind_t)
#define SL_REM_PROC_OUT_IND_SIZE        sizeof(sl_rem_proc_out_ind_t)
#define SL_REM_PROC_RECOVERED_IND_SIZE  sizeof(sl_rem_proc_recovered_ind_t)
#define SL_RTB_CLEARED_IND_SIZE         sizeof(sl_rtb_cleared_ind_t)
#define SL_LOC_PROC_OUT_IND_SIZE        sizeof(sl_loc_proc_out_ind_t)
#define SL_LOC_PROC_RECOVERED_IND_SIZE  sizeof(sl_loc_proc_recovered_ind_t)

#define SL_OPT_PROTOCOL         LMI_OPT_PROTOCOL
#define SL_OPT_STATISTICS       LMI_OPT_STATISTICS
#define SL_OPT_CONFIG           3       /* use struct sl_config */
#define SL_OPT_STATEM           4       /* use struct sl_statem */
#define SL_OPT_STATS            5       /* use struct sl_stats */

#endif                          /* __SS7_SLI_H__ */
