/*****************************************************************************

 @(#) src/include/ss7/x400p_ioctl.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 *****************************************************************************/

#ifndef __X400P_IOCTL_H__
#define __X400P_IOCTL_H__

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>
#include <sys/smi_ioctl.h>

#define MX_IOC_MAGIC	'c'

#define MX_OBJ_TYPE_DFLT	0	/* defaults */
#define MX_OBJ_TYPE_SYNC	1	/* synchronization group */
#define MX_OBJ_TYPE_CARD	2	/* card */
#define MX_OBJ_TYPE_SPAN	3	/* span */
#define MX_OBJ_TYPE_CHAN	4	/* channel */
#define MX_OBJ_TYPE_FRAC	5	/* fractional */
#define MX_OBJ_TYPE_XCON	6	/* cross connect */
#define MX_OBJ_TYPE_BERT	7	/* bit error rate test */

#define X400PCARDTYPE_NONE				 0
#define X400PCARDTYPE_PLX9030				 1
#define X400PCARDTYPE_PLXDEVBRD				 2
#define X400PCARDTYPE_X400P				 3
#define X400PCARDTYPE_E400P				 4
#define X400PCARDTYPE_T400P				 5
#define X400PCARDTYPE_X400PSS7				 6
#define X400PCARDTYPE_E400PSS7				 7
#define X400PCARDTYPE_T400PSS7				 8
#define X400PCARDTYPE_V400P				 9
#define X400PCARDTYPE_V400PE				10
#define X400PCARDTYPE_V400PT				11
#define X400PCARDTYPE_V401PE				12
#define X400PCARDTYPE_V401PT				13

#define X400PCARDCHIPTYPE_NONE				 0
#define X400PCARDCHIPTYPE_DS2152			 1
#define X400PCARDCHIPTYPE_DS21352			 2
#define X400PCARDCHIPTYPE_DS21552			 3
#define X400PCARDCHIPTYPE_DS2154			 4
#define X400PCARDCHIPTYPE_DS21354			 5
#define X400PCARDCHIPTYPE_DS21554			 6
#define X400PCARDCHIPTYPE_DS2155			 7
#define X400PCARDCHIPTYPE_DS2156			 8

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static information.
 */
struct mx_info_dflt {
	uint mxDrivIdnum;
	uint mxDrivMajor;
	char mxDrivDescription[256];
	char mxDrivRevision[256];
	char mxDrivCopyright[256];
	char mxDrivSupportedDevice[256];
	char mxDrivContact[256];
	uint mxDrivLicense;
#define MXDRIVLICENSE_PROPRIETARY	0
#define MXDRIVLICENSE_GPL		1
#define MXDRIVLICENSE_GPLV2		2
#define MXDRIVLICENSE_GPLPLUS		3
#define MXDRIVLICENSE_DUALBSDGPL	4
#define MXDRIVLICENSE_DUALMITGPL	5
#define MXDRIVLICENSE_DUALMPLGPL	6
	uint8_t mxDrivDate[12];
};
struct mx_info_sync {
};
struct mx_info_card {
	uint mxCardType;
	uint mxCardIdentifier;
	uint mxCardRevision;
	uint mxCardChipType;
	uint mxCardChipRevision;
	uint mxCardPciBus;
	uint mxCardPciSlot;
	uint mxCardPciIrq;
	char mxCardName[32];
};
struct mx_info_span {
};
struct mx_info_chan {
};
struct mx_info_frac {
};
struct mx_info_xcon {
};
struct mx_info_bert {
};
union mx_info_obj {
	struct mx_info_dflt dflt;
	struct mx_info_sync sync;
	struct mx_info_card card;
	struct mx_info_span span;
	struct mx_info_chan chan;
	struct mx_info_frac frac;
	struct mx_info_xcon xcon;
	struct mx_info_bert bert;
};
typedef struct mx_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mx_info_obj info[0];
} mx_info_t;

#define MX_IOCGINFO	_IOWR(MX_IOC_MAGIC,	 0, mx_info_t)	/* get */

/*
 * OPTIONS
 *
 * Provides options not necessary for configuration but that can be changed
 * while the object exists without reconfiguration of the object (that is, a
 * change to these items does not change the relationship between the object
 * changed and other objects).
 *
 * Note that one of these options structures can optionally be appended to a
 * configuration structure when an object is created or moved.
 */
struct mx_opt_conf_dflt {
};
struct mx_opt_conf_sync {
};
struct mx_opt_conf_card {
	uint mxCardMode;
#define MXCARDMODE_REMOTELOOPBACK	0
#define MXCARDMODE_LOCALLOOPBACK	1
	uint mxCardSyncMaster;
#define MXCARDSYNCMASTER_SLAVE		0
#define MXCARDSYNCMASTER_MASTER		1
	uint mxCardSyncSource;
#define MXCARDSYNCSOURCE_SYNCSELF	0
#define MXCARDSYNCSOURCE_SYNC1		1
#define MXCARDSYNCSOURCE_SYNC2		2
#define MXCARDSYNCSOURCE_SYNC3		3
#define MXCARDSYNCSOURCE_SYNC4		4
#define MXCARDSYNCSOURCE_SYNCEXTERN	5
#define MXCARDSYNCSOURCE_SYNCAUTO	6
};
struct mx_opt_conf_span {
	uint mxSpanMode;
#define MXSPANMODE_LOCAL		0
#define MXSPANMODE_REMOTE		1
#define MXSPANMODE_FRAMER		2
#define MXSPANMODE_PAYLOAD		3
	uint mxSpanClocking;
#define MXSPANCLOCKING_NONE		0
#define MXSPANCLOCKING_LOOP		1
#define MXSPANCLOCKING_LOCAL		2
#define MXSPANCLOCKING_INTERNAL		3
#define MXSPANCLOCKING_EXTERNAL		4
#define MXSPANCLOCKING_ADAPTIVE		5
	uint mxSpanLineImpedance;
#define MXSPANLINEIMPEDANCE_NONE		0
#define MXSPANLINEIMPEDANCE_UNBALANCED75OHM	1
#define MXSPANLINEIMPEDANCE_BALANCED100OHM	2
#define MXSPANLINEIMPEDANCE_BALANCED120OHM	3
	uint mxSpanLineMode;
#define MXSPANLINEMODE_NONE		0
#define MXSPANLINEMODE_DSU		1
#define MXSPANLINEMODE_CSU		2
#define MXSPANLINEMODE_MONITOR		3
	uint mxSpanLineLength;
#define MXSPANLINELENGTH_NONE		0
#define MXSPANLINELENGTH_DSX133FT	1
#define MXSPANLINELENGTH_DSX266FT	2
#define MXSPANLINELENGTH_DSX399FT	3
#define MXSPANLINELENGTH_DSX533FT	4
#define MXSPANLINELENGTH_DSX666FT	5
	uint mxSpanLineAttenuation;
#define MXSPANLINEATTENUATION_NONE	0
#define MXSPANLINEATTENUATION_CSU0DB	1
#define MXSPANLINEATTENUATION_CSU8DB	2
#define MXSPANLINEATTENUATION_CSU15DB	3
#define MXSPANLINEATTENUATION_CSU23DB	4
	uint mxSpanLineGain;
#define MXSPANLINEGAIN_NONE		0
#define MXSPANLINEGAIN_MON0DB		1
#define MXSPANLINEGAIN_MON12DB		2
#define MXSPANLINEGAIN_MON20DB		3
#define MXSPANLINEGAIN_MON26DB		4
#define MXSPANLINEGAIN_MON30DB		5
#define MXSPANLINEGAIN_MON32DB		6
	uint mxSpanLineDelay;		/* milliseconds */
	uint mxSpanTxLevel;
#define MXSPANTXLEVEL_OPEN		0
#define MXSPANTXLEVEL_OFF		1
#define MXSPANTXLEVEL_ON		2
	uint mxSpanRxLevel;
#define MXSPANRXLEVEL_OPEN		0
#define MXSPANRXLEVEL_OFF		1
#define MXSPANRXLEVEL_ON		2
	uint mxSpanAlarmSettleTime;	/* milliseconds */
	uint mxSpanLineCodeTime;	/* milliseconds */
	uint mxSpanLineCode;
#define MXSPANLINECODE_NOCODE		1
#define MXSPANLINECODE_LINECODE		2
#define MXSPANLINECODE_PAYLOADCODE	3
#define MXSPANLINECODE_RESETCODE	4
#define MXSPANLINECODE_TESTCODE		5
#define MXSPANLINECODE_UNFRAMEDALLONES	6
#define MXSPANLINECODE_UNFRAMEDONEANDZERO 7
	uint mxSpanReceiveThreshold;
};
struct mx_opt_conf_chan {
	uint mxChanType;
#define MXCHANTYPE_NONE			0
#define MXCHANTYPE_CAS			1
#define MXCHANTYPE_CCS			2
	uint mxChanFormat;
#define MXCHANFORMAT_NONE		1
#define MXCHANFORMAT_DS0A		2
#define MXCHANFORMAT_DS0		3
#define MXCHANFORMAT_T1			4
#define MXCHANFORMAT_J1			5
#define MXCHANFORMAT_E1			6
	uint mxChanRate;
#define MXCHANRATE_VARIABLE		   0
#define MXCHANRATE_KBITS56		  56
#define MXCHANRATE_KBITS64		  64
#define MXCHANRATE_KBITS1536		1536
#define MXCHANRATE_KBITS1984		1984
	uint mxChanMode;
#define MXCHANMODE_REMOTELOOPBACK	0
#define MXCHANMODE_LOCALLOOPBACK	1
#define MXCHANMODE_TEST			2
#define MXCHANMODE_LOOPBACKECHO		3
};
struct mx_opt_conf_frac {
};
struct mx_opt_conf_xcon {
	uint mxXconType;
#define MXXCONTYPE_SWITCHED		1
#define MXXCONTYPE_SEMIPERMANENT	2
#define MXXCONTYPE_PERMANENT		3
};
struct mx_opt_conf_bert {
	uint mxBertMode;
#define MXBERTMODE_NONE			0
#define MXBERTMODE_SPAN			1
#define MXBERTMODE_CHANNEL		2
#define MXBERTMODE_FBIT			3
	uint mxBertSelect;
#define MXBERTSELECT_NONE		0
#define MXBERTSELECT_PRE2E7MINUS1	1
#define MXBERTSELECT_PRE2E9MINUS1	2
#define MXBERTSELECT_PRE2E11MINUS1	3
#define MXBERTSELECT_PRE2E15MINUS1	4
#define MXBERTSELECT_QRSS		5
#define MXBERTSELECT_DALY		6
#define MXBERTSELECT_REPEATING		7
#define MXBERTSELECT_ALTERNATING	8
	uint mxBertPatternLen;
	uint8_t mxBertPattern[32];
};
union mx_option_obj {
	struct mx_opt_conf_dflt dflt;
	struct mx_opt_conf_sync sync;
	struct mx_opt_conf_card card;
	struct mx_opt_conf_span span;
	struct mx_opt_conf_chan chan;
	struct mx_opt_conf_frac frac;
	struct mx_opt_conf_xcon xcon;
	struct mx_opt_conf_bert bert;
};
typedef struct mx_option {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union mx_option_obj option[0];
} mx_option_t;

#define MX_IOCGOPTION	_IOWR(MX_IOC_MAGIC,	 1, mx_option_t)	/* get */
#define MX_IOCSOPTION	_IOWR(MX_IOC_MAGIC,	 2, mx_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
struct mx_conf_dflt {
};
struct mx_conf_sync {
	uint mxSyncGroup;
	uint mxSpanId[8];
};
struct mx_conf_card {
	uint mxCardIndex;
	uint mxCardSpanType;		/* affects configuration of spans */
#define MXCARDSPANTYPE_NONE		1
#define MXCARDSPANTYPE_T1		2
#define MXCARDSPANTYPE_E1		3
#define MXCARDSPANTYPE_J1		4
	uint mxCardSyncGroup;
};
struct mx_conf_span {
	uint mxCardIndex;
	uint mxSpanIndex;
	char mxSpanName[32];
	uint mxSpanType;		/* affects configuration of channels */
#define MXSPANTYPE_NONE			0
#define MXSPANTYPE_T1			1
#define MXSPANTYPE_E1			2
#define MXSPANTYPE_J1			3
#define MXSPANTYPE_E2			4
#define MXSPANTYPE_E3			5
#define MXSPANTYPE_T3			6
	uint mxSpanCrc;
#define MXSPANCRC_NONE			0
#define MXSPANCRC_CRC4			1
#define MXSPANCRC_CRC5			2
#define MXSPANCRC_CRC6			3
#define MXSPANCRC_CRC6J			4
	uint mxSpanCoding;
#define MXSPANCODING_NONE		0
#define MXSPANCODING_AMI		1
#define MXSPANCODING_B6ZS		2
#define MXSPANCODING_B8ZS		3
#define MXSPANCODING_HDB3		4
#define MXSPANCODING_JBZS		5
#define MXSPANCODING_ZBTSI		6
	uint mxSpanFraming;
#define MXSPANFRAMING_NONE		0
#define MXSPANFRAMING_CCS		1
#define MXSPANFRAMING_CAS		2
#define MXSPANFRAMING_SF		3
#define MXSPANFRAMING_D4		4
#define MXSPANFRAMING_ESF		5
	uint mxSpanDataLink;
#define MXSPANDATALINK_ANSIT1403	0
#define MXSPANDATALINK_ATT54016		1
#define MXSPANDATALINK_OTHER		2
	uint mxSpanPriority;
	uint mxSpanPrimary;
};
struct mx_conf_chan {
	uint mxCardIndex;
	uint mxSpanIndex;
	uint mxChanIndex;
};
struct mx_conf_frac {
};
struct mx_conf_xcon {
	uint mxCardIndex;
	uint mxSpanIndex;
	uint mxChanIndex;
	uint mxXconCardIndex;
	uint mxXconSpanIndex;
	uint mxXconChanIndex;
};
struct mx_conf_bert {
	uint mxCardIndex;
	uint mxSpanIndex;
};
union mx_config_obj {
	struct mx_conf_dflt dflt;
	struct mx_conf_sync sync;
	struct mx_conf_card card;
	struct mx_conf_span span;
	struct mx_conf_chan chan;
	struct mx_conf_frac frac;
	struct mx_conf_xcon xcon;
	struct mx_conf_bert bert;
};

#define MX_GET		0	/* get options or configuration */
#define MX_ADD		1	/* add configuration */
#define MX_CHA		2	/* set options or change configuration */
#define MX_DEL		3	/* delete configuraiton */
#define MX_TST		4	/* test options or configuration */
#define MX_COM		5	/* commit options or configuration */
typedef struct mx_config {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mx_config_obj config[0];
} mx_config_t;

#define MX_IOCLCONFIG	_IOWR(MX_IOC_MAGIC,	 3, mx_config_t)	/* list */
#define MX_IOCGCONFIG	_IOWR(MX_IOC_MAGIC,	 4, mx_config_t)	/* get */
#define MX_IOCSCONFIG	_IOWR(MX_IOC_MAGIC,	 5, mx_config_t)	/* set */
#define MX_IOCTCONFIG	_IOWR(MX_IOC_MAGIC,	 6, mx_config_t)	/* test */
#define MX_IOCCCONFIG	_IOWR(MX_IOC_MAGIC,	 7, mx_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
struct mx_statem_dflt {
};
struct mx_statem_sync {
};
struct mx_statem_card {
};
struct mx_statem_span {
};
struct mx_statem_chan {
};
struct mx_statem_frac {
};
struct mx_statem_xcon {
};
struct mx_statem_bert {
};
union mx_statem_obj {
	struct mx_statem_dflt dflt;
	struct mx_statem_sync sync;
	struct mx_statem_card card;
	struct mx_statem_span span;
	struct mx_statem_chan chan;
	struct mx_statem_frac frac;
	struct mx_statem_xcon xcon;
	struct mx_statem_bert bert;
};
typedef struct mx_statem {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mx_statem_obj statem[0];
} mx_statem_t;

#define MX_IOCGSTATEM	_IOWR(MX_IOC_MAGIC,	 8, mx_statem_t)	/* get */
#define MX_IOCCMRESET	_IOWR(MX_IOC_MAGIC,	 9, mx_statem_t)	/* master reset */

/*
 * STATUS
 *
 * Provides status on the object.  Many of these are X.721 status fields or
 * object-specific status information.  Only read-write status fields may be
 * altered with a set or clear command.
 */
struct mx_status_dflt {
};
struct mx_status_sync {
};
struct mx_status_card {
	uint mxCardAdministrativeState;	/* X.721 */
	uint mxCardOperationalState;	/* X.721 */
	uint mxCardUsageState;		/* X.721 */
	uint mxCardAlarmStatus;		/* X.721 */
	uint mxCardProceduralStatus;	/* X.721 */
	uint mxCardAvailabilityStatus;	/* X.721 */
	uint mxCardControlStatus;	/* X.721 */
	uint mxCardUnknownStatus;	/* X.721 */
	uint mxCardStandbyStatus;	/* X.721 */
};
struct mx_status_span {
	uint mxSpanAdministrativeState;	/* X.721 */
	uint mxSpanOperationalState;	/* X.721 */
	uint mxSpanUsageState;		/* X.721 */
	uint mxSpanAlarmStatus;		/* X.721 */
	uint mxSpanProceduralStatus;	/* X.721 */
	uint mxSpanAvailabilityStatus;	/* X.721 */
	uint mxSpanControlStatus;	/* X.721 */
	uint mxSpanStandbyStatus;	/* X.721 */
	uint mxSpanUnknownStatus;	/* X.721 */
	uint mxSpanLoopbackStatus;
#define MXSPANLOOPBACKSTATUS_NEARENDINWARD	 0
#define MXSPANLOOPBACKSTATUS_NEARENDLINE	 1
#define MXSPANLOOPBACKSTATUS_NEARENDFRAMER	 2
#define MXSPANLOOPBACKSTATUS_NEARENDPAYLOAD	 3
#define MXSPANLOOPBACKSTATUS_FARENDLINE		 4
#define MXSPANLOOPBACKSTATUS_FARENDPAYLOAD	 5
	uint mxSpanLineStatus;
#define MXSPANLINESTATUS_NONE			 0
#define MXSPANLINESTATUS_RCVFARENDLOF		 1
#define MXSPANLINESTATUS_XMTFARENDLOF		 2
#define MXSPANLINESTATUS_RCVAIS			 3
#define MXSPANLINESTATUS_XMTAIS			 4
#define MXSPANLINESTATUS_LOSSOFFRAME		 5
#define MXSPANLINESTATUS_LOSSOFSIGNAL		 6
#define MXSPANLINESTATUS_LOOPBACKSTATE		 7
#define MXSPANLINESTATUS_T16AIS			 8
#define MXSPANLINESTATUS_RCVFARENDLOMF		 9
#define MXSPANLINESTATUS_XMTFARENDLOMF		10
#define MXSPANLINESTATUS_RCVTESTCODE		11
#define MXSPANLINESTATUS_OTHERFAILURE		12
#define MXSPANLINESTATUS_UAVAILSIGSTATE		13
#define MXSPANLINESTATUS_NETEQUIPOOS		14
#define MXSPANLINESTATUS_RCVPAYLOADAIS		15
#define MXSPANLINESTATUS_PERFTHRESHOLD		16
	uint mxSpanAlarms;
#define MXSPANALARMS_YELLOW			 0
#define MXSPANALARMS_RED			 1
#define MXSPANALARMS_BLUE			 2
#define MXSPANALARMS_RECOVERY			 3
	uint mxSpanEvents;
#define MXSPANEVENTS_RLOS			 0
#define MXSPANEVENTS_FRCL			 1
#define MXSPANEVENTS_RUAL			 2
#define MXSPANEVENTS_RYEL			 3
#define MXSPANEVENTS_RRA			 4
#define MXSPANEVENTS_RDMA			 5
#define MXSPANEVENTS_V52LNK			 6
#define MXSPANEVENTS_LORC			 7
#define MXSPANEVENTS_LOTC			 8
#define MXSPANEVENTS_LUP			 9
#define MXSPANEVENTS_LDN			10
#define MXSPANEVENTS_LSPARE			11
#define MXSPANEVENTS_TOCD			12
#define MXSPANEVENTS_TCLE			13
};
struct mx_status_chan {
	uint mxChanAdministrativeState;	/* X.721 */
	uint mxChanOperationalState;	/* X.721 */
	uint mxChanUsageState;		/* X.721 */
	uint mxChanAvailabilityStatus;	/* X.721 */
	uint mxChanControlStatus;	/* X.721 */
	uint mxChanProceduralStatus;	/* X.721 */
	uint mxChanAlarmStatus;		/* X.721 */
	uint mxChanStandbyStatus;	/* X.721 */
};
struct mx_status_frac {
};
struct mx_status_xcon {
};
struct mx_status_bert {
	uint mxBertOperationalState;	/* X.721 */
	uint mxBertProceduralStatus;	/* X.721 */
};
union mx_status_obj {
	struct mx_status_dflt dflt;
	struct mx_status_sync sync;
	struct mx_status_card card;
	struct mx_status_span span;
	struct mx_status_chan chan;
	struct mx_status_frac frac;
	struct mx_status_xcon xcon;
	struct mx_status_bert bert;
};
typedef struct mx_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union mx_status_obj status[0];
} mx_status_t;

#define MX_IOCGSTATUS	_IOWR(MX_IOC_MAGIC,	10, mx_status_t)	/* get */
#define MX_IOCSSTATUS	_IOWR(MX_IOC_MAGIC,	11, mx_status_t)	/* set */
#define MX_IOCCSTATUS	_IOWR(MX_IOC_MAGIC,	12, mx_status_t)	/* clear */

/*
 * STATISTICS
 */
struct mx_stats_dflt {
};
struct mx_stats_sync {
};
struct mx_stats_card {
	uint mxCardSyncTransitions;
};
struct mx_stats_span {
	uint mxNearEndESs;
	uint mxNearEndSESs;
	uint mxNearEndSEFSs;
	uint mxNearEndUASs;
	uint mxNearEndCSSs;
	uint mxNearEndPCVs;
	uint mxNearEndLESs;
	uint mxNearEndBESs;
	uint mxNearEndDMs;
	uint mxNearEndLCVs;
	uint mxNearEndValidData;
	uint mxFarEndESs;
	uint mxFarEndSESs;
	uint mxFarEndSEFSs;
	uint mxFarEndUASs;
	uint mxFarEndCSSs;
	uint mxFarEndPCVs;
	uint mxFarEndLESs;
	uint mxFarEndBESs;
	uint mxFarEndDMs;
	uint mxFarEndValidData;
};
struct mx_stats_chan {
};
struct mx_stats_frac {
};
struct mx_stats_xcon {
};
struct mx_stats_bert {
	uint mxBertBitCount;
	uint mxBertErrorCount;
};
union mx_stats_obj {
	struct mx_stats_dflt dflt;
	struct mx_stats_sync sync;
	struct mx_stats_card card;
	struct mx_stats_span span;
	struct mx_stats_chan chan;
	struct mx_stats_frac frac;
	struct mx_stats_xcon xcon;
	struct mx_stats_bert bert;
};
typedef struct mx_stats {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint interval;			/* time interval */
	uint time;			/* time period */
	uint stamp;			/* time stamp */
	/* followed by object-specific structure */
	union mx_stats_obj stats[0];
} mx_stats_t;

#define MX_IOCGSTATSP	_IOWR(MX_IOC_MAGIC,	13, mx_stats_t)	/* get period */
#define MX_IOCSSTATSP	_IOWR(MX_IOC_MAGIC,	14, mx_stats_t)	/* set period */
#define MX_IOCGSTATS	_IOWR(MX_IOC_MAGIC,	15, mx_stats_t)	/* get */
#define MX_IOCCSTATS	_IOWR(MX_IOC_MAGIC,	16, mx_stats_t)	/* get and clear */

/*
 * EVENTS
 *
 * Provides a mechanism for requesting event notifications.  The flags field in
 * the header is used to requesting X.721 events (e.g. object creation
 * notification).  Events can be requested by object class, and a mechanism
 * exists for requesting events by specific object; however, the driver is not
 * required to support per-object notification and will return EOPNOTSUPP.  To
 * request notification for an object class, specify zero (0) for the
 * object identifier.
 *
 * Event requests are per requesting Stream.  Non-management streams should only
 * be notified of events for objects associated with the Stream and not X.721
 * events unless also specified as a specific event (e.g. communications alarm
 * events).  Management Streams can be notified of any object.  Notifications
 * are provided using the LMI_EVENT_IND (event indication) for X.721 events, and
 * the MX_EVENT_IND (for other events).
 */
struct mx_notify_dflt {
	uint events;
};
struct mx_notify_sync {
	uint events;
};
struct mx_notify_card {
	uint events;
};
struct mx_notify_span {
	uint events;
};
struct mx_notify_chan {
	uint events;
};
struct mx_notify_frac {
	uint events;
};
struct mx_notify_xcon {
	uint events;
};
struct mx_notify_bert {
	uint events;
};
union mx_notify_obj {
	struct mx_notify_dflt dflt;
	struct mx_notify_sync sync;
	struct mx_notify_card card;
	struct mx_notify_span span;
	struct mx_notify_chan chan;
	struct mx_notify_frac frac;
	struct mx_notify_xcon xcon;
	struct mx_notify_bert bert;
};

#define MX_SYNCTRANSITION		(1<< 0)
#define MX_LOOPUP			(1<< 1)
#define MX_LOOPDOWN			(1<< 2)
#define MX_YELLOWALARM			(1<< 3)
#define MX_YELLOWALARM_CLEARED		(1<< 4)
#define MX_REDALARM			(1<< 5)
#define MX_REDALARM_CLEARED		(1<< 6)
#define MX_BLUEALARM			(1<< 7)
#define MX_BLUEALARM_CLEARED		(1<< 8)
#define MX_TESTABORTED			(1<< 9)
#define MX_TESTCOMPLETE			(1<<10)
typedef struct mx_notify {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint flags;			/* X.721 flags */
	/* followed by object-specific structure */
	union mx_notify_obj events[0];
} mx_notify_t;

#define MX_IOCGNOTIFY	_IOWR(MX_IOC_MAGIC,	17, mx_notify_t)	/* get */
#define MX_IOCSNOTIFY	_IOWR(MX_IOC_MAGIC,	18, mx_notify_t)	/* set */
#define MX_IOCCNOTIFY	_IOWR(MX_IOC_MAGIC,	19, mx_notify_t)	/* clear */

/*
 * ATTRIBUTES
 *
 * Provides a structure that contains all of the other information structures
 * pertaining to an object.  This is the total view of the object.  This 
 * structure can only be read and cannot be used to reconfigure or change
 * options associated with an object.  This structure is primarily for the
 * convenience of management agents so that it is possible to atomically read
 * all of the attributes associated with an object.
 */
struct mx_attr_dflt {
	struct mx_conf_dflt config;
	struct mx_opt_conf_dflt option;
	struct mx_info_dflt inform;
	struct mx_statem_dflt statem;
	struct mx_status_dflt status;
	struct mx_stats_dflt stats;
	struct mx_notify_dflt events;
};
struct mx_attr_sync {
	struct mx_conf_sync config;
	struct mx_opt_conf_sync option;
	struct mx_info_sync inform;
	struct mx_statem_sync statem;
	struct mx_status_sync status;
	struct mx_stats_sync stats;
	struct mx_notify_sync events;
};
struct mx_attr_card {
	struct mx_conf_card config;
	struct mx_opt_conf_card option;
	struct mx_info_card inform;
	struct mx_statem_card statem;
	struct mx_status_card status;
	struct mx_stats_card stats;
	struct mx_notify_card events;
};
struct mx_attr_span {
	struct mx_conf_span config;
	struct mx_opt_conf_span option;
	struct mx_info_span inform;
	struct mx_statem_span statem;
	struct mx_status_span status;
	struct mx_stats_span stats;
	struct mx_notify_span events;
};
struct mx_attr_chan {
	struct mx_conf_chan config;
	struct mx_opt_conf_chan option;
	struct mx_info_chan inform;
	struct mx_statem_chan statem;
	struct mx_status_chan status;
	struct mx_stats_chan stats;
	struct mx_notify_chan events;
};
struct mx_attr_frac {
	struct mx_conf_frac config;
	struct mx_opt_conf_frac option;
	struct mx_info_frac inform;
	struct mx_statem_frac statem;
	struct mx_status_frac status;
	struct mx_stats_frac stats;
	struct mx_notify_frac events;
};
struct mx_attr_xcon {
	struct mx_conf_xcon config;
	struct mx_opt_conf_xcon option;
	struct mx_info_xcon inform;
	struct mx_statem_xcon statem;
	struct mx_status_xcon status;
	struct mx_stats_xcon stats;
	struct mx_notify_xcon events;
};
struct mx_attr_bert {
	struct mx_conf_bert config;
	struct mx_opt_conf_bert option;
	struct mx_info_bert inform;
	struct mx_statem_bert statem;
	struct mx_status_bert status;
	struct mx_stats_bert stats;
	struct mx_notify_bert events;
};
union mx_attr_obj {
	struct mx_attr_dflt dflt;
	struct mx_attr_sync sync;
	struct mx_attr_card card;
	struct mx_attr_span span;
	struct mx_attr_chan chan;
	struct mx_attr_frac frac;
	struct mx_attr_xcon xcon;
	struct mx_attr_bert bert;
};
typedef struct mx_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mx_attr_obj attrs[0];
} mx_attr_t;

#define	MX_IOCGATTR	_IOWR(MX_IOC_MAGIC,	20, mx_attr_t)	/* get attributes */

/*
 * MANAGEMENT
 *
 * Provides specific and defined management actions that can be affected on
 * objects.
 */
struct mx_action_dflt {
};
struct mx_action_sync {
};
struct mx_action_card {
};
struct mx_action_span {
};
struct mx_action_chan {
};
struct mx_action_frac {
};
struct mx_action_xcon {
};
struct mx_action_bert {
};
union mx_action_obj {
	struct mx_action_dflt dflt;
	struct mx_action_sync sync;
	struct mx_action_card card;
	struct mx_action_span span;
	struct mx_action_chan chan;
	struct mx_action_frac frac;
	struct mx_action_xcon xcon;
	struct mx_action_bert bert;
};

#define MX_TST_STOP		 1
#define MX_TST_FBIT		 2
#define MX_TST_SPAN		 3
#define MX_TST_CHANNELS		 4
typedef struct mx_mgmt {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mx_action_obj action[0];
} mx_mgmt_t;

#define MX_IOCCMGMT	_IOWR(MX_IOC_MAGIC,	21, mx_mgmt_t)	/* command */

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct mx_pass {
	int muxid;			/* mux index of lower stream */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of ctrl part */
	uint dat_length;		/* length of data part */
	/* followed by ctrl and data part of message to pass */
} mx_pass_t;

#define MX_IOCCPASS	_IOWR(MX_IOC_MAGIC,	22, mx_pass_t)	/* pass */

#define MX_IOC_FIRST	0
#define MX_IOC_LAST	22
#define MX_IOC_PRIVATE	32

#endif				/* __X400P_IOCTL_H__ */
