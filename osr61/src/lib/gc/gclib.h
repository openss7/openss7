/*****************************************************************************

 @(#) $Id: gclib.h,v 0.9.2.2 2007/08/14 09:06:52 brian Exp $

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

 Last Modified $Date: 2007/08/14 09:06:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: gclib.h,v $
 Revision 0.9.2.2  2007/08/14 09:06:52  brian
 - GPLv3 header update

 Revision 0.9.2.1  2007/02/25 12:45:29  brian
 - added new files for release package

 *****************************************************************************/

#ifndef __GCLIB_H__
#define __GCLIB_H__

#ident "@(#) $RCSfile: gclib.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

struct alarm_field {
	long alarm_number;
	union gc_parm alarm_data;
	union gc_parm rfu;
};

struct alarm_list {
	int n_alarms;
	struct alarm_field alarm_fields[MAX_NUMBER_OF_ALARMS];
};

struct alarm_parm_field {
	union gc_parm alarm_parm_number;
	union gc_parm alarm_parm_data;
	union gc_parm rfu;
};

struct alarm_parm_list {
	int n_parms;
	struct alarm_parm_field alarm_parm_fields[MAX_NUMBER_OF_ALARM_PARMS];
};

struct alarm_source_object_field {
	union gc_parm aso_data;
	int rfu;
};

struct alarm_source_object_list {
	int n_asos;
	struct alarm_source_object_field aso_fields[MAX_ASOS];
};

struct cclib_start_struct {
	char *cclib_name;
	void *cclib_data;
};

struct ct_devinfo {
	unsigned long ct_prodid;
	unsigned long ct_devfamily;
	unsigned long ct_devmode;
	unsigned long ct_nettype;
	unsigned long ct_busmode;
	unsigned long ct_busencoding;
	union {
		unsigned char ct_RFU[7];	/* reserved */
		struct {
			unsigned char ct_prottype;
		} ct_net_devinfo;
	} ct_ext_devinfo;
};

/* ct_devfamily */
#define CT_DFDM3	1	/* DM3 device */
#define CT_FCHMPDMS	2	/* HMP device (Host Media Processing) */
#define CT_NETSTRUCTIP	3	/* IPT series board */
#define CT_DFD41D	4	/* D/41D board family */
#define CT_DFD41E	5	/* analog or void channeld of a D/xx or VFX/xx board such as
				   D/41ESC or VFX/40ESC */
#define CT_DFSPAN	6	/* analog channel such as of a D/160SC-LS board; a voice channel
				   such as of a D/240SC, D/320SC, D/240SC-T1, D/300SC-E1, or
				   D/160SC-LS board; or a digital channel such as a D/240SC-T1 or
				   D/300SC-E1 board */
#define CT_DFMSI	7	/* a station on an MSI board */
#define CT_DFSCX	8	/* SCX160 SCxbus adapter family */

/* ct_devmode */
#define CT_DMRESOURCE	1	/* DM3 void device in flexible routing configuration, analog
				   channel not in use */
#define CT_DMNETWORK	2	/* DMS network device or DM3 voice device in fixed routing, analog
				   channel available to process calls from the telephone network */

/* ct_nettype */
#define CT_NTNONE		0	/* D/xx or VFX/xx board configured as a resource device;
					   voice channels are available for call processing; analog 
					   channels are disabled */
#define CT_IPT			1	/* IP connectivity */
#define CT_NTANALOG		2	/* analog interface.  Analog and voice devices on board are 
					   handling call processing */
#define CT_NTT1			3	/* T1 digital network interface */
#define CT_NTE1			4	/* E1 digital network interface */
#define CT_NTMSI		5	/* MSI/SC station interface */
#define CT_NTHIZ		0x1000	/* high impeadance (HiZ) interace.  This valid is bitwise
					   ORed with the type of network interface. */
#define CT_NTTXZSWITCHABLE	0x2000	/* The network interface can be switched to the transmit
					   impeadance state.  This valid is bitwise ORed with the
					   type of network interface. */
#define CT_NTANALOG		6	/* Used to transmit the record notification beep tone. */

/* ct_busmode */
#define CT_BMSCBUS		1	/* TDM bus architecture */
#define CT_H100			2	/* H.100 bus */
#define CT_H110			3	/* H.110 bus */

/* ct_busencoding */
#define CT_BEULAW	1	/* mu-law encoding */
#define CT_BEALAW	2	/* A-law encoding */
#define CT_BELLAW	3	/* linear encoding */
#define CT_BEBYPASS	4	/* encoding is being bypassed */

/* ct_prottype */
#define CT_CAS		1	/* channel associated signalling */
#define CT_CLEAR	2	/* clear channel signalling */
#define CT_ISDN		3	/* ISDN */
#define CT_R2MF		4	/* R2MF */

struct dx_cap {
	unsigned short ca_nbrdna;
	unsigned short ca_stdely;
	unsigned short ca_cnosig;
	unsigned short ca_lcdly;
	unsigned short ca_lcdly1;
	unsigned short ca_hedge;
	unsigned short ca_lo1tola;
	unsigned short ca_lo1tolb;
	unsigned short ca_lo2tola;
	unsigned short ca_lo2tolb;
	unsigned short ca_hi1tola;
	unsigned short ca_hi1tolb;
	unsigned short ca_lo1bmax;
	unsigned short ca_lo2bmax;
	unsigned short ca_hi1nmax;
	unsigned short ca_nsbusy;
	unsigned short ca_logltch;
	unsigned short ca_higltch;
	unsigned short ca_lo1rmax;
	unsigned short ca_lo2rmax;
	unsigned short ca_intflg;
	unsigned short ca_intfltr;
	unsigned short ca_rfu1;
	unsigned short ca_rfu2;
	unsigned short ca_rfu3;
	unsigned short ca_rfu4;
	unsigned short ca_hisiz;
	unsigned short ca_alowmax;
	unsigned short ca_blowmax;
	unsigned short ca_nbrbeg;
	unsigned short ca_hi1ceil;
	unsigned short ca_lo1ceil;
	unsigned short ca_lowerfrq;
	unsigned short ca_upperfrq;
	unsigned short ca_timefrq;
	unsigned short ca_rejctfrq;
	unsigned short ca_maxansr;
	unsigned short ca_mxtimefrq;
	unsigned short ca_lower2frq;
	unsigned short ca_upper2frq;
	unsigned short ca_timer2frq;
	unsigned short ca_mxtime2frq;
	unsigned short ca_lower3frq;
	unsigned short ca_upper3frq;
	unsigned short ca_time3frq;
	unsigned short ca_mxtime3frq;
	unsigned short ca_dtn_pres;
	unsigned short ca_dtn_npres;
	unsigned short ca_dtn_deboff;
	unsigned short ca_pamd_failtime;
	unsigned short ca_pamd_minring;
	byte ca_pamd_spdval;
	btye ca_pamd_qtemp;
	unsigned short ca_noanswer;
	unsigned short ca_maxintering;
};

struct extensionevtblk {
	unsigned char ext_id;
	struct gc_parm_blk parmblk;
};

struct gc_callack_blk {
	unsigned long type;
	long rfu;
	union {
		struct {
			int accept;
		} dnis;
		struct {
			int acceptance;
			int linedev;
		} isdn;
		struct {
			int info_len;
			int info_type;
		} info;
		struct {
			long gc_private[4];
		} gc_private;
	} service;
};

struct gc_cclib_state {
	char name[GC_MAX_CCLIBNAME_LEN];
	int state;
};

#define GC_ALL_LIB	"GC_ALL_LIB"
#define GC_CUSTOM1_LIB	"GC_CUSTOM1_LIB"
#define GC_CUSTOM2_LIB	"GC_CUSTOM2_LIB"
#define GC_DM3CC_LIB	"GC_DM3CC_LIB"
#define GC_H3R_LIB	"GC_H3R_LIB"
#define GC_ICAPI_LIB	"GC_ICAPI_LIB"
#define GC_IPM_LIB	"GC_IPM_LIB"
#define GC_ISDN_LIB	"GC_ISDN_LIB"
#define GC_PDKRT_LIB	"GC_PDKRT_LIB"
#define GC_SS7_LIB	"GC_SS7_LIB"

#define GC_CCLIB_CONFIGURED	0
#define GC_CCLIB_AVAILABLE	1
#define GC_CCLIB_FAILED		-1

struct gc_cclib_status {
	int num_avllibraries;
	int num_configuredlibraries;
	int num_failedlibraries;
	int num_stublibraries;
	char **avllibraries;
	char **configuredlibraries;
	char **failedlibraries;
	char **stublibraries;
};

struct gc_cclib_statusall {
	struct gc_cclib_state cclib_status[MAX_CCLIBS];
};

struct gc_customlib_struct {
	char *file_name;
	char *start_proc_name;
};

struct gc_ie_blk {
	struct gclib_ie_blk *gclib;
	void *cclib;
};

struct gc_info {
	int gcValue;
	char *gcMsg;
	int ccLibId;
	char *ccLibName;
	long ccValue;
	char *ccMsg;
	char *additionalInfo;
};

struct gc_l2_blk {
	struct gclib_l2_blk *gclib;
	void *cclib;
};

struct gc_makecall_blk {
	struct gclib_makecall_blk *gclib;
	void *cclib;
};

union gc_parm {
	short shortvalue;
	unsigned long ulongvalue;
	long longvalue;
	int intvalue;
	unsigned int uintvalue;
	char charvalue;
	char *paddress;
	void *pstruct;
};

struct gc_parm_blk {
	unsigned short parm_data_size;
	unsigned char parm_data_buf[1];
};

struct gc_parm_data {
	unsigned short set_ID;
	unsigned short parm_ID;
	unsigned char value_size;
	unsigned char value_buf[1];
};

struct gc_parm_data_ext {
	unsigned long version;
	void *pInternal;
	unsigned long set_ID;
	unsigned long parm_ID;
	unsigned long data_size;
	void *pData;
};

struct gc_parm_id {
	unsigned short set_ID;
	unsigned short parm_ID;
	unsigned char value_type;
	unsigned char update_perm;
};

union gc_rate_u {
	struct {
		long cents;
	} ATT, *ATT_PTR;
};

struct gc_rerouting_info {
	char *rerouting_num;
	struct gclib_address_blk rerouting_addr;
	struct gc_parm_blk *parm_blkp;
};

struct gc_rtcm_evtdata {
	long request_ID;
	int gc_result;
	int cclib_result;
	int cclib_ID;
	char additional_msg[MAX_ADDITIONAL_MSG];
	struct gc_parm_blk *retrieved_parmblkp;
};

struct gc_start_struct {
	int num_cclibs;
	struct cclib_start_struct *cclib_list;
};

struct gclib_address_blk {
	char address[MAX_ADDRES_LEN];
	unsigned char address_type;
	unsigned char address_plan;
	char sub_address[MAX_ADDRESS_LEN];
	unsigned char sub_address_type;
	unsigned char sub_address_plan;
};

/* address_type */
#define GCADDRTYPE_TRANSPARENT		1	/* number type is tranparent */
#define GCADDRTYPE_NAT			2	/* national number */
#define GCADDRTYPE_INTL			3	/* international number */
#define GCADDRTYPE_LOC			4	/* local number */
#define GCADDRTYPE_IP			5	/* internet protocol address */
#define GCADDRTYPE_URL			6	/* URL address */
#define GCADDRTYPE_DOMAIN		7	/* domain address */
#define GCADDRTYPE_EMAIL		8	/* e-mail address */

/* address plan */
#define GCADDRPLAN_UNKNOWN		0	/* unknown numbering plan */
#define GCADDRPLAN_ISDN			1	/* ISDN/Telephony numbering plan E.163/E.164 */
#define GCADDRPLAN_TELEPHONY		2	/* telephony numbering plan E.164 */
#define GCADDRPLAN_PRIVATE		3	/* private numbering plan */

/* subaddress type */
#define GCSUBADDR_UNKNOWN		0	/* subaddress type unknown */
#define GCSUBADDR_OSI			1	/* NSAP - X.213/ISO 8348 AD2 */
#define GCSUBADDR_USER			2	/* user specified */
#define GCSUBADDR_IA5			3	/* IA5 digit format */

struct gclib_call_blk {
	unsigned char category;
	unsigned char address_info;
};

#define GCCAT_SUB_NOPRIOR	0	/* subscriber without prioirty */
#define GCCAT_SUB_PRIOR		1	/* subscriber with prioirty */
#define GCCAT_MAIN_EQUIP	2	/* maintenance equipment */
#define GCCAT_COIN_BOX		3	/* coinbox or subscribed with charge metering */
#define GCCAT_OPERATOR		4	/* operator */
#define GCCAT_DATA		5	/* data transmission */
#define GCCAT_CPTP		6	/* C.P.T.P */
#define GCCAT_SPECIAL		7	/* special line */
#define GCCAT_MOBILE		8	/* mobile users */
#define GCCAT_VPN		9	/* virtual private network line */

#define GCADDRINFO_ENBLOC	0	/* address is complete */
#define GCADDRINFO_OVERLAP	1	/* address is not complete */

struct gclib_chan_blk {
	unsigned char medium_id;
	unsigned char medium_sel;
};

#define GCMEDSEL_MEDIUM_PREF	0
#define GCMEDSEL_MEDIUM_EXCL	1

struct gclib_makecall_blk {
	struct gclib_address_blk destination;
	struct gclib_address_blk origination;
	struct gclib_chan_blk chan_info;
	struct gclib_call_blk call_info;
	struct gc_parm_blk *ext_datap;
};

struct metaevent {
	long magicno;
	unsigned long flags;
	void *evtdatap;
	long evtlen;
	long evtdev;
	long evttype;
	struct linedev linedev;
	struct crn crn;
	void *extevtdatap;
	void *usrattr;
	int cclibid;
	int rfu1;
};

struct sc_tsinfo {
	unsigned long sc_numts;
	long *sc_tsarrayp;
};

typedef struct alarm_field ALARM_FIELD;
typedef struct alarm_list ALARM_LIST;
typedef struct alarm_parm_field ALARM_PARM_FIELD;
typedef struct alarm_parm_list ALARM_PARM_LIST;
typedef struct alarm_source_object_field ALARM_SOURCE_OBJECT_FIELD;
typedef struct alarm_source_object_list ALARM_SOURCE_OBJECT_LIST;
typedef struct cclib_start_struct CCLIB_START_STRUCT, *CCLIB_START_STRUCTP;
typedef struct ct_devinfo CT_DEVINFO;
typedef struct dx_cap DX_CAP;
typedef struct extensionevtblk EXTENSIONEVTBLK;
typedef struct gc_callack_blk GC_CALLACK_BLK, *GC_CALLACK_BLK_PTR;
typedef struct gc_cclib_state GC_CCLIB_STATE, *GC_CCLIB_STATEP;
typedef struct gc_cclib_status GC_CCLIB_STATUS, *GC_CCLIB_STATUSP;
typedef struct gc_cclib_statusall GC_CCLIB_STATUSALL, *GC_CCLIB_STATUSALLP;
typedef struct gc_customlib_struct GC_CUSTOMLIB_STRUCT, *GC_CUSTOMLIB_STRUCTP;
typedef struct gc_ie_blk GC_IE_BLK, *GC_IE_BLKP;
typedef struct gc_info GC_INFO;
typedef struct gc_l2_blk GC_L2_BLK, *GC_L2_BLKP;
typedef struct gc_makecall_blk GC_MAKECALL_BLK, *GC_MAKECALL_BLKP;
typedef union gc_parm GC_PARM;
typedef struct gc_parm_blk GC_PARM_BLK, *GC_PARM_BLKP;
typedef struct gc_parm_data GC_PARM_DATA, *GC_PARM_DATAP;
typedef struct gc_parm_data_ext GC_PARM_DATA_EXT, *GC_PARM_DATA_EXTP;
typedef struct gc_parm_id GC_PARM_ID, *GC_PARM_IDP;
typedef union gc_rate_u GC_RATE_U, *GC_RATE_U_PTR;
typedef struct gc_rerouting_info GC_REROUTING_INFO, *GC_REROUTING_INFOP;
typedef struct gc_rtcm_evtdata GC_RTCM_EVTDATA, *GC_RTCM_EVTDATAP;
typedef struct gc_start_struct GC_START_STRUCT, *GC_START_STRUCTP;
typedef struct gclib_address_blk GCLIB_ADDRESS_BLK;
typedef struct gclib_call_blk GCLIB_CALL_BLK;
typedef struct gclib_chan_blk GCLIB_CHAN_BLK;
typedef struct gclib_makecall_blk GCLIB_MAKECALL_BLK, *GLIB_MAKECALL_BLKP;
typedef struct metaevent METAEVENT, *METAEVENTP;
typedef struct sc_tsinfo SC_TSINFO;

typedef int LINEDEV;			/* ??? */
typedef int CRN;			/* ??? */

int gc_AcceptCall(int crn, int rings, unsigned long mode);
int gc_AcceptInit_Xfer(int crn, struct gc_rerouting_info *reroutinginfop, unsigned long mode);
int gc_AcceptModifyCall(int crn, struct gc_parm_blk *parmblkp, unsigned long mode);
int gc_AcceptXfer(int crn, struct gc_parm_blk *parmblkp, unsigned long mode);
int gc_AnswerCall(int crn, int rings, unsigned long mode);
int gc_BlindTransfer(int activecall, char *numberstr, struct gc_makecall_blk *makecallp,
		     int timeout, unsigned long mode);
int gc_CallAck(int crn, struct gc_callack_blk *callack_blkp, unsigned long mode);
int gc_CallProgress(int crn, int indicator);
int gc_CompleteTransfer(int callonhold, int *consultationcall, unsigned long mode);
int gc_DropCall(int crn, int cause, unsigned long mode);
int gc_getANI(int crn, char *ani_buf);
int gc_GetBilling(int crn, char *billing_buf);
int gc_GetCallInfo(int crn, int info_id, char *valuep);
int gc_GetCallState(int crn, int *state_ptr);
int gc_GetCRN(int *crn_ptr, struct metaevent *metaeventp);
int gc_GetDNIS(int crn, char *dnis);
int gc_GetNetCRV(int crn, int *netcrvp);
int gc_HoldACK(int crn);
int gc_HoldCall(int crn, unsigned long mode);
int gc_HoldRej(int crn, int cause);
int gc_InitXfer(int crn, struct gc_parm_blk *parmblkp,
		struct gc_rerouting_info **ret_rerouting_infopp, unsigned long mode);
int gc_InvokeXfer(int crn, int extracrn, char *numberstr, struct gc_makecall_blk *makecallp,
		  int timeout, unsigned long mode);
int gc_RejectInitXfer(int crn, unsigned long reason, struct gc_parm_blk *parmblkp,
		      unsigned long mode);
int gc_RejectModifyCall(int crn, unsigned long reason, unsigned long mode);
int gc_RejectXfer(int crn, unsigned long reason, struct gc_parm_blk *parmblkp, unsigned long mode);
int gc_ReleaseCall(int crn);
int gc_ReleaseCallEx(int crn, unsigned long mode);
int gc_ReqANI(int crn, char *anibuf, int req_type, unsigned long mode);
int gc_ReqModifyCall(int crn, struct gc_parm_blk *parmblkp, unsigned long mode);
int gc_ReqMoreInfo(int crn, int info_id, int info_len, int timeout, unsigned long mode);
int gc_RetrieveAck(int crn);
int gc_RetrieveCall(int crn, unsigned long mode);
int gc_RetrieveRej(int crn, int cause);
int gc_SendMoreInfo(int crn, int info_id, union gc_parm *info_ptr, unsigned long mode);
int gc_SetBilling(int crn, int rate_type, union gc_rate_u *ratep, unsigned long mode);
int gc_SetupTransfer(int calltohold, int *consultationcall, unsigned long mode);
int gc_SwapHold(int activecall, int callonhold, unsigned long mode);

int gc_Attach(int linedev, int voiceh, unsigned long mode);
int gc_AttachResource(int linedev, int resourceh, struct gc_parm_blk *resourseattrp,
		      struct gc_parm_blk **retblkpp, int resourcetype, unsigned long mode);
int gc_Close(int linedev);
int gc_CRN2LineDev(int *lindevp);
int gc_Detach(int linedev, int voiceh, unsigned long mode);
int gc_GetAlarmConfiguration(int linedev, unsigned long aso_id, struct alarm_list *alarm_list,
			     int alarm_config_type);
int gc_GetAlarmParm(int linedev, unsigned long aso_id, int ParmSetID,
		    struct alarm_parm_list *alarm_parm_list, unsigned long mode);
int gc_GetAlarmSourceObjectList(int linedev, int ByNameOrById,
				struct alarm_souce_object_list *aso_list);
int gc_getAlarmSourceObjectNetworkID(int linedev, unsigned long *aso_networkID);
int gc_GetCallProgressParm(int linedev, struct dx_cap *parmp);
int gc_GetCTInfo(int linedev, CT_DEVINFO * ct_devinfo);
int gc_GetFrame(int linedev, struct gc_l2_blk *l2_blkp);
int gc_GetInfoElem(int linedev, struct gc_ie_blk *iep);
int gc_GetLineDev(int *linedevp, struct metaevent *metaeventp);
int gc_GetLinedevState(int linedev, int type, int *state_buf);
int gc_GetNetworkH(int linedev, int *networkhp);
int gc_GetParm(int linedev, int parm_id, union gc_parm *valuep);
int gc_GetResourceH(int linedev, int *resourcehp, int resourcetype);
int gc_GetSigInfo(int linedev, char *valuep, int info_id, struct metaevent *metaeventp);
int gc_GetUsrAttr(int linedev, void **usr_attrp);
int gc_GetVer(int linedev, unsigned int *releasenump, unsigned int *intnump, long component);
int gc_GetVoiceH(int linedev, int *voicehp);
int gc_GetXmitSlot(int linedev, struct sc_tsinfo *sctsinfop);
int gc_LinedevToCCLIBID(int linedev, int *cclibid);
int gc_Listen(int linedev, struct sc_tsinfo *sctsinfop, unsigned long mode);
int gc_LoadDxParm(int linedev, char *pathp, char *errmsgp, int err_length);
int gc_MakeCall(int linedev, int *crnp, char *numberstr, struct gc_makecall_blk *makecallp,
		int timeout, unsigned long mode);
int gc_Open(int linedev, char *devicename, int rfu);
int gc_OpenEx(int *linedevp, char *devicename, int mode, void *usrattr);
int gc_ResetLineDev(int linedev, unsigned long mode);
int gc_SetAlarmConfiguration(int linedev, unsigned long aso_id, struct alarm_list *alarm_list,
			     int alarm_config_type);
int gc_SetAlarmNotifyAll(int linedev, unsigned long aso_id, int value);
int gc_SetAlarmParm(int linedev, unsigned long aso_id, int ParmSetID,
		    struct alarm_parm_list *alarm_parm_list, unsigned long mode);
int gc_SetCallingNum(int linedev, char *calling_num);
int gc_SetCallProgressParm(int linedev, struct dx_cap *parmp);
int gc_SetChanState(int linedev, int chanstate, unsigned long mode);
int gc_SetEvtMsk(int linedev, unsigned long mask, int action);
int gc_SetInfoElem(int linedev, struct gc_ie_blk *iep);
int gc_SetParm(int linedev, int parm_id, union gc_parm value);
int gc_SetUsrAttr(int linedev, void *userattr);
int gc_SndFrame(int linedev, struct gc_l2_blk *l2_blkp);
int gc_SndMsg(int linedev, int crn, int msg_type, struct gc_ie_blk *sndmsgptr);
int gc_StartTrace(int linedev, char *tracefilename);
int gc_StopTrace(int linedev);
int gc_StopTransmitAlarms(int linedev, unsigned long aso_id, struct alarm_list *alarm_list,
			  unsigned long mode);
int gc_TransmitAlarms(int linedev, unsigned long aso_id, struct alarm_list *alarm_list,
		      unsigned long mode);
int gc_UnListen(int linedev, unsigned long mode);
int gc_WaitCall(int linedev, int *crnp, GC_WAITCALL_BLK * waitcallp, int timeout,
		unsigned long mode);

int gc_AlarmName(struct metaevent *metaeventp, char **alarm_name);
int gc_AlarmNumber(struct metaevent *metaeventp, long *alarm_number);
int gc_AlarmSourceObjectID(struct metaevent *metaeventp, unsigned long *aso_id);
int gc_AlarmSourceObjectName(struct metaevent *metaeventp, char **aso_name);
int gc_GetMetaEvent(struct metaevent *metaeventp);
int gc_GetMetaEventExt(struct metaevent *metaeventp, unsigned long evt_handle);
int gc_ResultInfo(struct metaevent *metaeventp, struct gc_info *infop);
int gc_ResultValue(struct metaevent *metaeventp, int *gc_resultp, int *cclibidp,
		   long *cclib_resultp);

int gc_AlarmNumberToName(unsigned long aso_id, long alarm_number, char **name);
int gc_AlarmSourceObjectIDToName(unsigned long aso_id, char **aso_name);
int gc_AlarmSourceObjectNameToID(char *aso_name, unsigned long *aso_id);
int gc_GetAlarmFlow(unsigned long aso_id, int *flow);
int gc_SetAlarmFlow(unsigned long aso_id, int flow);

int gc_Extension(int target_type, long target_id, unsigned char ext_id,
		 struct gc_parm_blk *parmblkp, struct gc_parm_blk **retblkp, unsigned long mode);
int gc_GetConfig_data(int target_type, long target_id, struct gc_parm_blk *target_datap,
		      int time_out, long *request_idp, unsigned long mode);
int gc_GetUserInfo(int target_type, long target_id, struct gc_parm_blk *infoparmblkp);
int gc_QueryConfigData(int target_type, long target_id, union gc_parm *source_datap, long query_id,
		       union gc_parm *response_datap);
int gc_ReqService(int target_type, long target_ID, unsigned long *pserviceID,
		  struct gc_parm_blk *reqdatap, struct gc_parm_blk **respdatapp,
		  unsigned long mode);
int gc_RespService(int target_type, long target_ID, struct gc_parm_blk *datap, unsigned long mode);
int gc_SetAuthenticationInfo(int target_type, long target_id, struct gc_parm_blk *infoparmblkp);
int gc_SetConfigData(int target_type, long target_id, struct gc_parm_blk *target_datap,
		     int time_out, int update_cond, long *request_idp, unsigned long mode);
int gc_SetUserInfo(int target_type, long target_id, struct gc_parm_blk *infoparmblkp, int duration);

int gc_CCLibIDToName(int cclibid, char **lib_name);
int gc_CCLibNameToID(char *lib_name, int *cclibidp);
int gc_CCLibStatus(char *cclib_name, int *cclib_infop);
int gc_CCLibStatusAll(struct gc_cclib_status *cclib_status);
int gc_CCLibStatusEx(char *cclib_name, void *cclib_infop);

int gc_ErrorInfo(struct gc_info *a_Infop);
int gc_ErrorValue(int *gc_errorp, int *cclibidp, long *cclib_errorp);

int gc_ResultMsg(int cclibid, long result_code, char **msg);

int gc_Start(struct gc_start_struct *startp);
int gc_Stop(void);

int gc_util_copy_parm_blk(struct gc_parm_blk **parm_blkpp, struct gc_parm_blk *parm_blkp);
int gc_util_delete_parm_blk(struct gc_parm_blk *parm_blk);
int gc_util_find_parm(struct gc_parm_blk *parm_blk, unsigned short setID, unsigned short parmID);
int gc_util_find_parm_ex(struct gc_parm_blk *parm_blk, unsigned long setID, unsigned long parmID,
			 struct gc_parm_data_ext *parm);
int gc_util_insert_parm_ref(struct gc_parm_blk ***parm_blkpp, unsigned short setID,
			    unsigned short ParmID, unsigned char data_size, void *datap);
int gc_util_insert_parm_ref_ex(struct gc_parm_blk **parm_blkpp, unsigned long setID,
			       unsigned long parmID, unsigned long data_size, void *datap);
int gc_util_insert_parm_val(struct gc_parm_blk ***parm_blkpp, unsigned short setID,
			    unsigned short parmID, unsigned char data_size, unsigned long data);
int gc_util_next_parm(struct gc_parm_blk **parm_blk, struct gc_parm_data **cur_parm);
int gc_util_next_parm_ex(struct gc_parm_blk *parm_blk, struct gc_parm_data_ext *parm);

#endif				/* __GCLIB_H__ */
