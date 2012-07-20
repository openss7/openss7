/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#include <stdint.h>
#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>

#include <sys/mxi.h>
#include <sys/mxi_ioctl2.h>

struct key_val {
	char *key;
	unsigned long val;
};

static struct key_val x721AdministrativeState_names[] = {
	{"locked", X721_ADMINISTRATIVESTATE_LOCKED},
	{"unlocked", X721_ADMINISTRATIVESTATE_UNLOCKED},
	{"shutting-down", X721_ADMINISTRATIVESTATE_SHUTTINGDOWN}
};

static struct key_val x721AlarmStatus_bitnames[] = {
	{"under-repair", (1 << X721_ALARMSTATUS_UNDERREPAIR)},
	{"critical", (1 << X721_ALARMSTATUS_CRITICAL)},
	{"major", (1 << X721_ALARMSTATUS_MAJOR)},
	{"minor", (1 << X721_ALARMSTATUS_MINOR)},
	{"outstanding", (1 << X721_ALARMSTATUS_ALARMOUTSTANDING)}
};

static struct key_val m3100AlarmStatus_bitnames[] = {
	{"critical", (1 << M3100_ALARMSTATUS_CRITICAL)},
	{"major", (1 << M3100_ALARMSTATUS_MAJOR)},
	{"minor", (1 << M3100_ALARMSTATUS_MINOR)},
	{"indeterminate", (1 << M3100_ALARMSTATUS_INDETERMINATE)},
	{"warning", (1 << M3100_ALARMSTATUS_WARNING)},
	{"pending", (1 << M3100_ALARMSTATUS_PENDING)},
	{"cleared", (1 << M3100_ALARMSTATUS_CLEARED)}
};

static struct key_val x721AvailabilityStatus_bitnames[] = {
	{"intest", (1 << X721_AVAILABILITYSTATUS_INTEST)},
	{"failed", (1 << X721_AVAILABILITYSTATUS_FAILED)},
	{"poweroff", (1 << X721_AVAILABILITYSTATUS_POWEROFF)},
	{"offline", (1 << X721_AVAILABILITYSTATUS_OFFLINE)},
	{"offduty", (1 << X721_AVAILABILITYSTATUS_OFFDUTY)},
	{"dependency", (1 << X721_AVAILABILITYSTATUS_DEPENDENCY)},
	{"degraded", (1 << X721_AVAILABILITYSTATUS_DEGRADED)},
	{"vacant", (1 << X721_AVAILABILITYSTATUS_NOTINSTALLED)},
	{"logfull", (1 << X721_AVAILABILITYSTATUS_LOGFULL)}
};

static struct key_val x721ControlStatus_bitnames[] = {
	{"subject-to-test", (1 << X721_CONTROLSTATUS_SUBJECTTOTEST)},
	{"part-of-services-locked", (1 << X721_CONTROLSTATUS_PARTOFSERVICESLOCKED)},
	{"reserved-for-test", (1 << X721_CONTROLSTATUS_RESERVEDFORTEST)},
	{"suspended", (1 << X721_CONTROLSTATUS_SUSPENDED)}
};

static struct key_val x721OperationalState_names[] = {
	{"disabled", X721_OPERATIONALSTATE_DISABLED},
	{"endabled", X721_OPERATIONALSTATE_ENABLED}
};

static struct key_val x721UsageState_names[] = {
	{"idle", X721_USAGESTATE_IDLE},
	{"active", X721_USAGESTATE_ACTIVE},
	{"busy", X721_USAGESTATE_BUSY}
};

static struct key_val x721ProceduralStatus_bitnames[] = {
	{"intialization-required", (1 << X721_PROCEDURALSTATUS_INITIALIZATIONREQUIRED)},
	{"not-initialized", (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED)},
	{"initializing", (1 << X721_PROCEDURALSTATUS_INITIALIZING)},
	{"reporting", (1 << X721_PROCEDURALSTATUS_REPORTING)},
	{"terminating", (1 << X721_PROCEDURALSTATUS_TERMINATING)}
};

static struct key_val x721StandbyStatus_names[] = {
	{"hot", X721_STANDBYSTATUS_HOTSTANDBY},
	{"cold", X721_STANDBYSTATUS_COLDSTANDBY},
	{"live", X721_STANDBYSTATUS_PROVIDINGSERVICE}
};

static struct key_val x721UnknownStatus_names[] = {
	{"false", X721_UNKNOWNSTATUS_FALSE},
	{"true", X721_UNKNOWNSTATUS_TRUE}
};

static struct key_val x721Action_names[] = {
	{"adm-lock", X721_ADM_LOCKED},
	{"adm-unlock", X721_ADM_UNLOCKED},
	{"adm-shutdown", X721_ADM_SHUTTINGDOWN},
	{"set-under-repair", X721_ALM_SET_UNDERREPAIR},
	{"clr-under-repair", X721_ALM_CLR_UNDERREPAIR},
	{"set-critical", X721_ALM_SET_CRITICAL},
	{"clr-critical", X721_ALM_CLR_CRITICAL},
	{"set-major", X721_ALM_SET_MAJOR},
	{"clr-major", X721_ALM_CLR_MAJOR},
	{"set-minor", X721_ALM_SET_MINOR},
	{"clr-minor", X721_ALM_CLR_MINOR},
	{"set-outstanding", X721_ALM_SET_OUTSTANDING},
	{"clr-outstanding", X721_ALM_CLR_OUTSTANDING},
	{"set-subject-to-test", X721_CTL_SET_SUBJECTTOTEST},
	{"clr-subject-to-test", X721_CTL_CLR_SUBJECTTOTEST},
	{"set-part-of-services-locked", X721_CTL_SET_PARTOFSERVICESLOCKED},
	{"clr-part-of-services-locked", X721_CTL_CLR_PARTOFSERVICESLOCKED},
	{"set-reserved-for-test", X721_CTL_SET_RESERVEDFORTEST},
	{"clr-reserved-for-test", X721_CTL_CLR_RESERVEDFORTEST},
	{"set-suspended", X721_CTL_SET_SUSPENDED},
	{"clr-suspended", X721_CTL_CLR_SUSPENDED}
};

static struct key_val mxDrivLicense_names[] = {
	{"prop", MXDRIVLICENSE_PROPRIETARY},
	{"gpl", MXDRIVLICENSE_GPL},
	{"gplv2", MXDRIVLICENSE_GPLV2},
	{"gpl+", MXDRIVLICENSE_GPLPLUS},
	{"bsd-gpl", MXDRIVLICENSE_DUALBSDGPL},
	{"mit-gpl", MXDRIVLICENSE_DUALMITGPL},
	{"mpl-gpl", MXDRIVLICENSE_DUALMPLGPL}
};

static struct key_val mxCardType_names[] = {
	{"none", X400PCARDTYPE_NONE},
	{"plx9030", X400PCARDTYPE_PLX9030},
	{"plxdevbrd", X400PCARDTYPE_PLXDEVBRD},
	{"x400p", X400PCARDTYPE_X400P},
	{"e400p", X400PCARDTYPE_E400P},
	{"t400p", X400PCARDTYPE_T400P},
	{"x400p-ss7", X400PCARDTYPE_X400PSS7},
	{"e400p-ss7", X400PCARDTYPE_E400PSS7},
	{"t400p-ss7", X400PCARDTYPE_T400PSS7},
	{"v400p", X400PCARDTYPE_V400P},
	{"v400pe", X400PCARDTYPE_V400PE},
	{"v400pt", X400PCARDTYPE_V400PT},
	{"v401pe", X400PCARDTYPE_V401PE},
	{"v401pt", X400PCARDTYPE_V401PT},
	{"ae400p", X400PCARDTYPE_AE400P},
	{"at400p", X400PCARDTYPE_AT400P},
	{"a400p", X400PCARDTYPE_A400P},
	{"a400pe", X400PCARDTYPE_A400PE},
	{"a400pt", X400PCARDTYPE_A400PT}
};

static struct key_val mxCardChipType_names[] = {
	{"none", X400PCARDCHIPTYPE_NONE},
	{"ds2152", X400PCARDCHIPTYPE_DS2152},
	{"ds21352", X400PCARDCHIPTYPE_DS21352},
	{"ds21552", X400PCARDCHIPTYPE_DS21552},
	{"ds2154", X400PCARDCHIPTYPE_DS2154},
	{"ds21354", X400PCARDCHIPTYPE_DS21354},
	{"ds21554", X400PCARDCHIPTYPE_DS21554},
	{"ds2155", X400PCARDCHIPTYPE_DS2155},
	{"ds2156", X400PCARDCHIPTYPE_DS2156},
	{"ds21455", X400PCARDCHIPTYPE_DS21455},
	{"ds21458", X400PCARDCHIPTYPE_DS21458}
};

static struct key_val mxCardMode_bitnames[] = {
	{"rlb", (1 << MXCARDMODE_REMOTELOOPBACK)},
	{"llb", (1 << MXCARDMODE_LOCALLOOPBACK)}
};

static struct key_val mxCardSyncMaster_names[] = {
	{"slave", MXCARDSYNCMASTER_SLAVE},
	{"master", MXCARDSYNCMASTER_MASTER}
};

static struct key_val mxCardSyncSource_names[] = {
	{"self", MXCARDSYNCSOURCE_SYNCSELF},
	{"span1", MXCARDSYNCSOURCE_SYNC1},
	{"span2", MXCARDSYNCSOURCE_SYNC2},
	{"span3", MXCARDSYNCSOURCE_SYNC3},
	{"span4", MXCARDSYNCSOURCE_SYNC4},
	{"ext", MXCARDSYNCSOURCE_SYNCEXTERN},
	{"auto", MXCARDSYNCSOURCE_SYNCAUTO}
};

static struct key_val mxSpanMode_bitnames[] = {
	{"lcl", (1 << MXSPANMODE_LOCAL)},
	{"rem", (1 << MXSPANMODE_REMOTE)},
	{"frm", (1 << MXSPANMODE_FRAMER)},
	{"pay", (1 << MXSPANMODE_PAYLOAD)}
};

static struct key_val mxSpanClocking_names[] = {
	{"none", MXSPANCLOCKING_NONE},
	{"loop", MXSPANCLOCKING_LOOP},
	{"lcl", MXSPANCLOCKING_LOCAL},
	{"int", MXSPANCLOCKING_INTERNAL},
	{"ext", MXSPANCLOCKING_EXTERNAL},
	{"adp", MXSPANCLOCKING_ADAPTIVE}
};

static struct key_val mxSpanLineImpedance_names[] = {
	{"none", MXSPANLINEIMPEDANCE_NONE},
	{"75-Ohm", MXSPANLINEIMPEDANCE_UNBALANCED75OHM},
	{"100-Ohm", MXSPANLINEIMPEDANCE_BALANCED100OHM},
	{"120-Ohm", MXSPANLINEIMPEDANCE_BALANCED120OHM}
};

static struct key_val mxSpanLineMode_names[] = {
	{"none", MXSPANLINEMODE_NONE},
	{"dsu", MXSPANLINEMODE_DSU},
	{"csu", MXSPANLINEMODE_CSU},
	{"mon", MXSPANLINEMODE_MONITOR}
};

static struct key_val mxSpanLineLength_names[] = {
	{"none", MXSPANLINELENGTH_NONE},
	{"133ft", MXSPANLINELENGTH_DSX133FT},
	{"266ft", MXSPANLINELENGTH_DSX266FT},
	{"399ft", MXSPANLINELENGTH_DSX399FT},
	{"533ft", MXSPANLINELENGTH_DSX533FT},
	{"666ft", MXSPANLINELENGTH_DSX666FT}
};

static struct key_val mxSpanLineAttenuation_names[] = {
	{"none", MXSPANLINEATTENUATION_NONE},
	{"0dB", MXSPANLINEATTENUATION_CSU0DB},
	{"8dB", MXSPANLINEATTENUATION_CSU8DB},
	{"15dB", MXSPANLINEATTENUATION_CSU15DB},
	{"23dB", MXSPANLINEATTENUATION_CSU23DB}
};

static struct key_val mxSpanLineGain_names[] = {
	{"none", MXSPANLINEGAIN_NONE},
	{"0dB", MXSPANLINEGAIN_MON0DB},
	{"12dB", MXSPANLINEGAIN_MON12DB},
	{"20dB", MXSPANLINEGAIN_MON20DB},
	{"26dB", MXSPANLINEGAIN_MON26DB},
	{"30dB", MXSPANLINEGAIN_MON30DB},
	{"32dB", MXSPANLINEGAIN_MON32DB}
};

static struct key_val mxSpanTxLevel_names[] = {
	{"oc", MXSPANTXLEVEL_OPEN},
	{"off", MXSPANTXLEVEL_OFF},
	{"on", MXSPANTXLEVEL_ON}
};

static struct key_val mxSpanRxLevel_names[] = {
	{"oc", MXSPANRXLEVEL_OPEN},
	{"off", MXSPANRXLEVEL_OFF},
	{"on", MXSPANRXLEVEL_ON}
};

static struct key_val mxSpanLineCode_names[] = {
	{"none", MXSPANLINECODE_NOCODE},
	{"line", MXSPANLINECODE_LINECODE},
	{"pay", MXSPANLINECODE_PAYLOADCODE},
	{"reset", MXSPANLINECODE_RESETCODE},
	{"test", MXSPANLINECODE_TESTCODE},
	{"uao", MXSPANLINECODE_UNFRAMEDALLONES},
	{"uoz", MXSPANLINECODE_UNFRAMEDONEANDZERO}
};

static struct key_val mxChanType_names[] = {
	{"none", MXCHANTYPE_NONE},
	{"cas", MXCHANTYPE_CAS},
	{"ccs", MXCHANTYPE_CCS}
};

static struct key_val mxChanFormat_names[] = {
	{"none", MXCHANFORMAT_NONE},
	{"ds0a", MXCHANFORMAT_DS0A},
	{"ds0", MXCHANFORMAT_DS0},
	{"t1", MXCHANFORMAT_T1},
	{"j1", MXCHANFORMAT_J1},
	{"e1", MXCHANFORMAT_E1}
};

static struct key_val mxChanRate_names[] = {
	{"var", MXCHANRATE_VARIABLE},
	{"56kbps", MXCHANRATE_KBITS56},
	{"64kbps", MXCHANRATE_KBITS64},
	{"1536kbps", MXCHANRATE_KBITS1536},
	{"1984kbps", MXCHANRATE_KBITS1984}
};

static struct key_val mxChanMode_bitnames[] = {
	{"rem", (1 << MXCHANMODE_REMOTELOOPBACK)},
	{"lcl", (1 << MXCHANMODE_LOCALLOOPBACK)},
	{"tst", (1 << MXCHANMODE_TEST)},
	{"lbe", (1 << MXCHANMODE_LOOPBACKECHO)}
};

static struct key_val mxXconType_names[] = {
	{"svc", MXXCONTYPE_SWITCHED},
	{"semi", MXXCONTYPE_SEMIPERMANENT},
	{"pvc", MXXCONTYPE_PERMANENT}
};

static struct key_val mxBertMode_names[] = {
	{"none", MXBERTMODE_NONE},
	{"span", MXBERTMODE_SPAN},
	{"chan", MXBERTMODE_CHANNEL},
	{"fbit", MXBERTMODE_FBIT}
};

static struct key_val mxBertSelect_names[] = {
	{"none", MXBERTSELECT_NONE},
	{"2e7", MXBERTSELECT_PRE2E7MINUS1},
	{"2e9", MXBERTSELECT_PRE2E9MINUS1},
	{"2e11", MXBERTSELECT_PRE2E11MINUS1},
	{"2e15", MXBERTSELECT_PRE2E15MINUS1},
	{"qrss", MXBERTSELECT_QRSS},
	{"daly", MXBERTSELECT_DALY},
	{"rept", MXBERTSELECT_REPEATING},
	{"alt", MXBERTSELECT_ALTERNATING}
};

static struct key_val mxCardSpanType_names[] = {
	{"none", MXCARDSPANTYPE_NONE},
	{"t1", MXCARDSPANTYPE_T1},
	{"e1", MXCARDSPANTYPE_E1},
	{"j1", MXCARDSPANTYPE_J1}
};

static struct key_val mxSpanType_names[] = {
	{"none", MXSPANTYPE_NONE},
	{"t1", MXSPANTYPE_T1},
	{"e1", MXSPANTYPE_E1},
	{"j1", MXSPANTYPE_J1},
	{"e2", MXSPANTYPE_E2},
	{"e3", MXSPANTYPE_E3},
	{"t3", MXSPANTYPE_T3}
};

static struct key_val mxSpanCrc_names[] = {
	{"none", MXSPANCRC_NONE},
	{"crc4", MXSPANCRC_CRC4},
	{"crc5", MXSPANCRC_CRC5},
	{"crc6", MXSPANCRC_CRC6},
	{"crc6j", MXSPANCRC_CRC6J}
};

static struct key_val mxSpanCoding_names[] = {
	{"none", MXSPANCODING_NONE},
	{"ami", MXSPANCODING_AMI},
	{"b6zs", MXSPANCODING_B6ZS},
	{"b8zs", MXSPANCODING_B8ZS},
	{"hdb3", MXSPANCODING_HDB3},
	{"jbzs", MXSPANCODING_JBZS},
	{"zbtsi", MXSPANCODING_ZBTSI}
};

static struct key_val mxSpanFraming_names[] = {
	{"none", MXSPANFRAMING_NONE},
	{"ccs", MXSPANFRAMING_CCS},
	{"cas", MXSPANFRAMING_CAS},
	{"sf", MXSPANFRAMING_SF},
	{"d4", MXSPANFRAMING_D4},
	{"esf", MXSPANFRAMING_ESF}
};

static struct key_val mxSpanDataLink_names[] = {
	{"t1.403", MXSPANDATALINK_ANSIT1403},
	{"att54015", MXSPANDATALINK_ATT54016},
	{"other", MXSPANDATALINK_OTHER}
};

static struct key_val mxSpanLoopbackStatus_bitnames[] = {
	{"neinw", (1 << MXSPANLOOPBACKSTATUS_NEARENDINWARD)},
	{"nelin", (1 << MXSPANLOOPBACKSTATUS_NEARENDLINE)},
	{"nefrm", (1 << MXSPANLOOPBACKSTATUS_NEARENDFRAMER)},
	{"nepay", (1 << MXSPANLOOPBACKSTATUS_NEARENDPAYLOAD)},
	{"felin", (1 << MXSPANLOOPBACKSTATUS_FARENDLINE)},
	{"fepay", (1 << MXSPANLOOPBACKSTATUS_FARENDPAYLOAD)}
};

static struct key_val mxSpanLineStatus_bitnames[] = {
	{"none", (1 << MXSPANLINESTATUS_NONE)},
	{"rfelof", (1 << MXSPANLINESTATUS_RCVFARENDLOF)},
	{"xfelof", (1 << MXSPANLINESTATUS_XMTFARENDLOF)},
	{"rais", (1 << MXSPANLINESTATUS_RCVAIS)},
	{"xais", (1 << MXSPANLINESTATUS_XMTAIS)},
	{"lof", (1 << MXSPANLINESTATUS_LOSSOFFRAME)},
	{"los", (1 << MXSPANLINESTATUS_LOSSOFSIGNAL)},
	{"lbs", (1 << MXSPANLINESTATUS_LOOPBACKSTATE)},
	{"t16ais", (1 << MXSPANLINESTATUS_T16AIS)},
	{"rfelomf", (1 << MXSPANLINESTATUS_RCVFARENDLOMF)},
	{"xfelomf", (1 << MXSPANLINESTATUS_XMTFARENDLOMF)},
	{"rtc", (1 << MXSPANLINESTATUS_RCVTESTCODE)},
	{"oth", (1 << MXSPANLINESTATUS_OTHERFAILURE)},
	{"uss", (1 << MXSPANLINESTATUS_UAVAILSIGSTATE)},
	{"neo", (1 << MXSPANLINESTATUS_NETEQUIPOOS)},
	{"rpa", (1 << MXSPANLINESTATUS_RCVPAYLOADAIS)},
	{"pft", (1 << MXSPANLINESTATUS_PERFTHRESHOLD)}
};

static struct key_val mxSpanAlarms_bitnames[] = {
	{"yel", (1 << MXSPANALARMS_YELLOW)},
	{"red", (1 << MXSPANALARMS_RED)},
	{"blu", (1 << MXSPANALARMS_BLUE)},
	{"rec", (1 << MXSPANALARMS_RECOVERY)},
	{"dmf", (1 << MXSPANALARMS_DISTMF)}
};

static struct key_val mxSpanEvents_bitnames[] = {
	{"rlos", (1 << MXSPANEVENTS_RLOS)},
	{"frcl", (1 << MXSPANEVENTS_FRCL)},
	{"rual", (1 << MXSPANEVENTS_RUAL)},
	{"ryel", (1 << MXSPANEVENTS_RYEL)},
	{"rra", (1 << MXSPANEVENTS_RRA)},
	{"rdma", (1 << MXSPANEVENTS_RDMA)},
	{"v52lnk", (1 << MXSPANEVENTS_V52LNK)},
	{"lorc", (1 << MXSPANEVENTS_LORC)},
	{"lotc", (1 << MXSPANEVENTS_LOTC)},
	{"lup", (1 << MXSPANEVENTS_LUP)},
	{"ldn", (1 << MXSPANEVENTS_LDN)},
	{"lspare", (1 << MXSPANEVENTS_LSPARE)},
	{"tocd", (1 << MXSPANEVENTS_TOCD)},
	{"tcle", (1 << MXSPANEVENTS_TCLE)}
};

static struct key_val mxEvent_bitnames[] = {
	{"sync", MX_SYNCTRANSITION},
	{"lup", MX_LOOPUP},
	{"ldn", MX_LOOPDOWN},
	{"yel", MX_YELLOWALARM},
	{"~yel", MX_YELLOWALARM_CLEARED},
	{"red", MX_REDALARM},
	{"~red", MX_REDALARM_CLEARED},
	{"blu", MX_BLUEALARM},
	{"~blu", MX_BLUEALARM_CLEARED},
	{"abt", MX_TESTABORTED},
	{"end", MX_TESTCOMPLETE}
};

struct mx_attr_all {
	struct mx_attr_dflt dflt;
	struct mx_attr_sync sync;
	struct mx_attr_card card;
	struct mx_attr_span span;
	struct mx_attr_chan chan;
	struct mx_attr_frac frac;
	struct mx_attr_xcon xcon;
	struct mx_attr_bert bert;
};

struct mx_attr_all attr = {
	.dflt = {
		 .config = {},
		 .option = {},
		 .inform = {
			    .mxDrivIdnum = 0,
			    .mxDrivMajor = 0,
			    .mxDrivDescription = "",
			    .mxDrivRevision = "",
			    .mxDrivCopyright = "",
			    .mxDrivSupportedDevice = "",
			    .mxDrivContact = "",
			    .mxDrivLicense = 0,
			    .mxDrivDate = {0,},
			    },
		 .statem = {},
		 .status = {},
		 .stats = {},
		 .events = {
			    .events = 0,
			    },
		 },
	.sync = {
		 .config = {
			    .mxSyncGroup = 0,
			    .mxSpanId = {0,},
			    },
		 .option = {},
		 .inform = {},
		 .statem = {},
		 .status = {},
		 .stats = {},
		 .events = {
			    .events = 0,
			    },
		 },
	.card = {
		 .config = {
			    .mxCardIndex = 0,
			    .mxCardSpanType = MXCARDSPANTYPE_NONE,
			    .mxCardSyncGroup = 0,
			    },
		 .option = {
			    .mxCardMode = 0,
			    .mxCardSyncMaster = MXCARDSYNCMASTER_SLAVE,
			    .mxCardSyncSource = MXCARDSYNCSOURCE_SYNCSELF,
			    },
		 .inform = {
			    .mxCardType = X400PCARDTYPE_NONE,
			    .mxCardIdentifier = 0,
			    .mxCardRevision = 0,
			    .mxCardChipType = X400PCARDCHIPTYPE_NONE,
			    .mxCardChipRevision = 0,
			    .mxCardPciBus = 0,
			    .mxCardPciSlot = 0,
			    .mxCardPciIrq = 0,
			    .mxCardName = "",
			    },
		 .statem = {},
		 .status = {
			    .mxCardAdministrativeState = X721_ADMINISTRATIVESTATE_LOCKED,
			    .mxCardOperationalState = X721_OPERATIONALSTATE_DISABLED,
			    .mxCardUsageState = X721_USAGESTATE_IDLE,
			    .mxCardAlarmStatus = 0,
			    .mxCardProceduralStatus = (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED),
			    .mxCardAvailabilityStatus = 0,
			    .mxCardControlStatus = 0,
			    .mxCardUnknownStatus = X721_UNKNOWNSTATUS_TRUE,
			    .mxCardStandbyStatus = X721_STANDBYSTATUS_PROVIDINGSERVICE,
			    },
		 .stats = {
			   .mxCardSyncTransitions = 0,
			   },
		 .events = {
			    .events = 0,
			    },
		 },
	.span = {
		 .config = {
			    .mxCardIndex = 0,
			    .mxSpanIndex = 0,
			    .mxSpanName = "",
			    .mxSpanType = MXSPANTYPE_NONE,
			    .mxSpanCrc = MXSPANCRC_NONE,
			    .mxSpanCoding = MXSPANCODING_NONE,
			    .mxSpanFraming = MXSPANFRAMING_NONE,
			    .mxSpanDataLink = MXSPANDATALINK_ANSIT1403,
			    .mxSpanPriority = 0,
			    .mxSpanPrimary = 0,
			    },
		 .option = {
			    .mxSpanMode = 0,
			    .mxSpanClocking = MXSPANCLOCKING_NONE,
			    .mxSpanLineImpedance = MXSPANLINEIMPEDANCE_NONE,
			    .mxSpanLineMode = MXSPANLINEMODE_NONE,
			    .mxSpanLineLength = MXSPANLINELENGTH_NONE,
			    .mxSpanLineAttenuation = MXSPANLINEATTENUATION_NONE,
			    .mxSpanLineGain = MXSPANLINEGAIN_NONE,
			    .mxSpanLineDelay = 0,
			    .mxSpanTxLevel = MXSPANTXLEVEL_OPEN,
			    .mxSpanRxLevel = MXSPANRXLEVEL_OPEN,
			    .mxSpanAlarmSettleTime = 5000,
			    .mxSpanLineCodeTime = 5000,
			    .mxSpanLineCode = MXSPANLINECODE_NOCODE,
			    .mxSpanReceiveThreshold = 0,
			    },
		 .inform = {},
		 .statem = {},
		 .status = {
			    .mxSpanAdministrativeState = X721_ADMINISTRATIVESTATE_LOCKED,
			    .mxSpanOperationalState = X721_OPERATIONALSTATE_DISABLED,
			    .mxSpanUsageState = X721_USAGESTATE_IDLE,
			    .mxSpanAlarmStatus = 0,
			    .mxSpanProceduralStatus = (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED),
			    .mxSpanAvailabilityStatus = 0,
			    .mxSpanControlStatus = 0,
			    .mxSpanStandbyStatus = X721_STANDBYSTATUS_PROVIDINGSERVICE,
			    .mxSpanUnknownStatus = X721_UNKNOWNSTATUS_TRUE,
			    .mxSpanLoopbackStatus = 0,
			    .mxSpanLineStatus = MXSPANLINESTATUS_NONE,
			    .mxSpanAlarms = 0,
			    .mxSpanEvents = 0,
			    },
		 .stats = {
			   .mxNearEndESs = 0,
			   .mxNearEndSESs = 0,
			   .mxNearEndSEFSs = 0,
			   .mxNearEndUASs = 0,
			   .mxNearEndCSSs = 0,
			   .mxNearEndPCVs = 0,
			   .mxNearEndLESs = 0,
			   .mxNearEndBESs = 0,
			   .mxNearEndDMs = 0,
			   .mxNearEndLCVs = 0,
			   .mxNearEndValidData = 0,
			   .mxFarEndESs = 0,
			   .mxFarEndSESs = 0,
			   .mxFarEndSEFSs = 0,
			   .mxFarEndUASs = 0,
			   .mxFarEndCSSs = 0,
			   .mxFarEndPCVs = 0,
			   .mxFarEndLESs = 0,
			   .mxFarEndBESs = 0,
			   .mxFarEndDMs = 0,
			   .mxFarEndValidData = 0,
			   },
		 .events = {
			    .events = 0,
			    },
		 },
	.chan = {
		 .config = {
			    .mxCardIndex = 0,
			    .mxSpanIndex = 0,
			    .mxChanIndex = 0,
			    },
		 .option = {
			    .mxChanType = MXCHANTYPE_NONE,
			    .mxChanFormat = MXCHANFORMAT_NONE,
			    .mxChanRate = MXCHANRATE_VARIABLE,
			    .mxChanMode = 0,
			    },
		 .inform = {},
		 .statem = {},
		 .status = {
			    .mxChanAdministrativeState = X721_ADMINISTRATIVESTATE_LOCKED,
			    .mxChanOperationalState = X721_OPERATIONALSTATE_DISABLED,
			    .mxChanUsageState = X721_USAGESTATE_IDLE,
			    .mxChanAvailabilityStatus = 0,
			    .mxChanControlStatus = 0,
			    .mxChanProceduralStatus = (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED),
			    .mxChanAlarmStatus = 0,
			    .mxChanStandbyStatus = X721_STANDBYSTATUS_PROVIDINGSERVICE,
			    },
		 .stats = {},
		 .events = {
			    .events = 0,
			    },
		 },
	.frac = {
		 .config = {},
		 .option = {},
		 .inform = {},
		 .statem = {},
		 .status = {},
		 .stats = {},
		 .events = {
			    .events = 0,
			    },
		 },
	.xcon = {
		 .config = {
			    .mxCardIndex = 0,
			    .mxSpanIndex = 0,
			    .mxChanIndex = 0,
			    .mxXconCardIndex = 0,
			    .mxXconSpanIndex = 0,
			    .mxXconChanIndex = 0,
			    },
		 .option = {
			    .mxXconType = 0,
			    },
		 .inform = {},
		 .statem = {},
		 .status = {},
		 .stats = {},
		 .events = {
			    .events = 0,
			    },
		 },
	.bert = {
		 .config = {
			    .mxCardIndex = 0,
			    .mxSpanIndex = 0,
			    },
		 .option = {
			    .mxBertMode = MXBERTMODE_NONE,
			    .mxBertSelect = MXBERTSELECT_NONE,
			    .mxBertPatternLen = 0,
			    .mxBertPattern = {0,},
			    },
		 .inform = {},
		 .statem = {},
		 .status = {
			    .mxBertOperationalState = X721_OPERATIONALSTATE_DISABLED,
			    .mxBertProceduralStatus = (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED),
			    },
		 .stats = {
			   .mxBertBitCount = 0,
			   .mxBertErrorCount = 0,
			   },
		 .events = {
			    .events = 0,
			    },
		 },
};

struct optname_struct {
	struct key_val *name;
	size_t size;
	volatile mx_ulong *valp;
};

enum {
	CFG_ADMINSTATE,
	CFG_OPERASTATE,
	CFG_USAGESTATE,
	CFG_STDBYSTATUS,
	CFG_UNKWNSTATUS,
	CFG_ACTION,
	CFG_LICENSE,
	CFG_CARDTYPE,
	CFG_CHIPTYPE,
	CFG_SYNCMASTER,
	CFG_SYNCSOURCE,
	CFG_SPANCLOCK,
	CFG_SPANIMPEAD,
	CFG_LINEMODE,
	CFG_LINELEN,
	CFG_LINEATTEN,
	CFG_TXLEVEL,
	CFG_RXLEVEL,
	CFG_LINECODE,
	CFG_CHANTYPE,
	CFG_CHANFORMAT,
	CFG_CHANRATE,
	CFG_XCONTYPE,
	CFG_BERTMODE,
	CFG_BERTSELECT,
	CFG_CARDSPANTYPE,
	CFG_SPANTYPE,
	CFG_SPANCODING,
	CFG_SPANFRAMING,
	CFG_SPANDATALINK,
};

struct optname_struct optnames[] = {
};

enum {
	CFG_ALARMSTATUS,
	CFG_AVAILSTATUS,
	CFG_CNTRLSTATUS,
	CFG_PROCDSTATUS,
	CFG_CARDMODE,
	CFG_SPANMODE,
	CFG_CHANMODE,
	CFG_SPANLOOPSTATUS,
	CFG_SPANLINESTATUS,
	CFG_SPANALARMS,
	CFG_SPANEVENTS,
	CFG_EVENTS,
};

int card = 0;
int span = 0;
int slot = 1;
int syng = 0;

int my_argc = 0;
char **my_argv = NULL;

int output = 1;

char devname[256] = "/dev/streams/clone/x400p-sl";
char cfgfile[256] = "/etc/sysconfig/ss7/mx.config";

static void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
", ident);
}

static void
version(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2011, 2012  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision: 1.1.2.2 $ $Date: 2010-11-28 14:22:38 $");
}

static void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-g,--get} {-d,--device} [{-c,--card} card] [{-p,--span} span] [{-t,--slot} slot] [{-y,--sync} sync]\n\
    %1$s [options] {-s,--set} {-d,--device} [{-c,--card} card] [{-p,--span} span] [{-t,--slot} slot] [{-y,--sync} sync] [set-options]\n\
    %1$s [options] {-f,--file} config_file\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options] {-g,--get} {-d,--device} [{-c,--card} card] [{-p,--span} span] [{-t,--slot} slot] [{-y,--sync} sync]\n\
    %1$s [options] {-s,--set} {-d,--device} [{-c,--card} card] [{-p,--span} span] [{-t,--slot} slot] [{-y,--sync} sync] [set-options]\n\
    %1$s [options] {-f,--file} config_file\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
Arguments:\n\
    (none)\n\
General Options:\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -D, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: 0]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
Command Options:\n\
    -g, --get\n\
        get (display) device configuration\n\
    -s, --set\n\
        set (configure) device configuration\n\
    -f, --file file                     (default: %2$s)\n\
        read device configuration from file\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
Set/Get Options:\n\
    -d, --device devname                (default: %3$s)\n\
        device name\n\
    -c, --card card                     (default: %4$d)\n\
        card in host (numbered from 0)\n\
    -p, --span span                     (default: %5$d)\n\
        span on card (numbered from 0)\n\
    -t, --slot slot                     (default: %6$d)\n\
        timeslot (numbered from 1, 0 for entire span)\n\
    -y, --sync sync                     (default: %7$d)\n\
        synchronization group (numbered from 0)\n\
", argv[0], cfgfile, devname, card, span, slot, syng);
}

#define BUFSIZE 512

struct mx_attr_buf {
	struct mx_attr attr;
	union mx_attr_obj vals;
};

struct mx_list_buf {
	struct mx_config conf;
	uint idxs[128];
};

void
mxconf_get_dflt(void)
{
	struct mx_attr_buf abuf = { };
	struct mx_list_buf lbuf = { };
	struct strioctl ctl = { };
	int fd, ret, i;
	char sep[2] = { 0, 0 };

	if (output > 1)
		printf("I: opening %s\n", devname);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	lbuf.conf.type = MX_OBJ_TYPE_DFLT;
	lbuf.conf.id = 0;
	lbuf.conf.cmd = MX_GET;
	ctl.ic_cmd = MX_IOCLCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lbuf);
	if (output > 1)
		printf("I: issuing MX_IOCLCONFIG input-output control\n");
	if ((ret = ioctl(fd, I_STR, &ctl)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output > 1)
		printf("I: got %d MX_OBJ_TYPE_DFLT objects\n", ret);
	for (i = 0; i < ret; i++) {
		abuf.attr.type = MX_OBJ_TYPE_DFLT;
		abuf.attr.id = lbuf.idxs[i];
		abuf.attr.cmd = MX_GET;
		ctl.ic_cmd = MX_IOCGATTR;
		ctl.ic_timout = 0;
		ctl.ic_len = sizeof(abuf);
		ctl.ic_dp = (char *) &abuf;
		if (output > 1)
			printf("I: issuing MX_IOCGATTR input-output control, PPA = 0x%04x\n", lbuf.idxs[i]);
		if (ioctl(fd, I_STR, &ctl) < 0) {
			perror(__FUNCTION__);
			exit(1);
		}
		printf("Defaults: 0x%04x\n", lbuf.idxs[i]);
		printf("  Config:\n");
		printf("  Option:\n");
		printf("  Inform:\n");
		printf("    mxDrivIdnum:.............. %u\n", abuf.vals.dflt.inform.mxDrivIdnum);
		printf("    mxDrivMajor:.............. %u\n", abuf.vals.dflt.inform.mxDrivMajor);
		printf("    mxDrivDescription:........ %s\n",
		       abuf.vals.dflt.inform.mxDrivDescription);
		printf("    mxDrivRevision:........... %s\n", abuf.vals.dflt.inform.mxDrivRevision);
		printf("    mxDrivCopyright:.......... %s\n",
		       abuf.vals.dflt.inform.mxDrivCopyright);
		printf("    mxDrivSupportedDevice:.... %s\n",
		       abuf.vals.dflt.inform.mxDrivSupportedDevice);
		printf("    mxDrivContact:............ %s\n", abuf.vals.dflt.inform.mxDrivContact);
		printf("    mxDrivLicense:............ ");
		printf("\n");
		printf("    mxDrivDate:............... ");
		printf("\n");
		printf("  Statem:\n");
		printf("  Status:\n");
		printf("  Stats:\n");
		printf("  Events:\n");
		printf("    events:................... <");
		sep[0] = '\0';
		if (abuf.vals.dflt.events.events & MX_SYNCTRANSITION) {
			printf("%s%s", sep, "sync");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_LOOPUP) {
			printf("%s%s", sep, "loop-up");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_LOOPDOWN) {
			printf("%s%s", sep, "loop-down");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_YELLOWALARM) {
			printf("%s%s", sep, "yel");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_YELLOWALARM_CLEARED) {
			printf("%s%s", sep, "~yel");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_REDALARM) {
			printf("%s%s", sep, "red");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_REDALARM_CLEARED) {
			printf("%s%s", sep, "~red");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_BLUEALARM) {
			printf("%s%s", sep, "blu");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_BLUEALARM_CLEARED) {
			printf("%s%s", sep, "~blu");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_TESTABORTED) {
			printf("%s%s", sep, "test-aborted");
			sep[0] = ',';
		}
		if (abuf.vals.dflt.events.events & MX_TESTCOMPLETE) {
			printf("%s%s", sep, "test-complete");
			sep[0] = ',';
		}
		printf(">\n");
	}
	if (output > 1)
		printf("I: closing %s\n", devname);
	close(fd);
}

void
mxconf_get_sync(void)
{
}

void
mxconf_get_card(void)
{
	struct mx_attr_buf abuf = { };
	struct mx_list_buf lbuf = { };
	struct strioctl ctl = { };
	int fd, ret, i;
	char sep[2] = { 0, 0 };

	if (output > 1)
		printf("I: opening %s\n", devname);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	lbuf.conf.type = MX_OBJ_TYPE_CARD;
	lbuf.conf.id = 0;
	lbuf.conf.cmd = MX_GET;
	ctl.ic_cmd = MX_IOCLCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lbuf);
	if (output > 1)
		printf("I: issuing MX_IOCLCONFIG input-output control\n");
	if ((ret = ioctl(fd, I_STR, &ctl)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output > 1)
		printf("I: got %d MX_OBJ_TYPE_CARD objects\n", ret);
	for (i = 0; i < ret; i++) {
		abuf.attr.type = MX_OBJ_TYPE_CARD;
		abuf.attr.id = lbuf.idxs[i];
		abuf.attr.cmd = MX_GET;
		ctl.ic_cmd = MX_IOCGATTR;
		ctl.ic_timout = 0;
		ctl.ic_len = sizeof(abuf);
		ctl.ic_dp = (char *) &abuf;
		if (output > 1)
			printf("I: issuing MX_IOCGATTR input-output control, PPA = 0x%04x\n", lbuf.idxs[i]);
		if (ioctl(fd, I_STR, &ctl) < 0) {
			perror(__FUNCTION__);
			exit(1);
		}
		printf("Card: 0x%04x\n", lbuf.idxs[i]);
		printf("  Config:\n");
		printf("    mxCardIndex:.............. %u\n", abuf.vals.card.config.mxCardIndex);
		printf("    mxCardSpanType:........... ");
		switch (abuf.vals.card.config.mxCardSpanType) {
		case MXCARDSPANTYPE_NONE:
			printf("none");
			break;
		case MXCARDSPANTYPE_T1:
			printf("t1");
			break;
		case MXCARDSPANTYPE_E1:
			printf("e1");
			break;
		case MXCARDSPANTYPE_J1:
			printf("j1");
			break;
		default:
			printf("0x%04x", abuf.vals.card.config.mxCardSpanType);
			break;
		}
		printf("    mxCardSyncGroup:.......... %u\n",
		       abuf.vals.card.config.mxCardSyncGroup);
		printf("  Option:\n");
		printf("    mxCardMode:............... <");
		sep[0] = '\0';
		if (abuf.vals.card.option.mxCardMode & (1 << MXCARDMODE_REMOTELOOPBACK)) {
			printf("%s%s", sep, "rlb");
			sep[0] = ',';
		}
		if (abuf.vals.card.option.mxCardMode & (1 << MXCARDMODE_LOCALLOOPBACK)) {
			printf("%s%s", sep, "llb");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxCardSyncMaster:......... ");
		switch (abuf.vals.card.option.mxCardSyncMaster) {
		case MXCARDSYNCMASTER_SLAVE:
			printf("slave");
			break;
		case MXCARDSYNCMASTER_MASTER:
			printf("master");
			break;
		default:
			printf("0x%04x", abuf.vals.card.option.mxCardSyncMaster);
			break;
		}
		printf("\n");
		printf("    mxCardSyncSource:......... ");
		switch (abuf.vals.card.option.mxCardSyncSource) {
		case MXCARDSYNCSOURCE_SYNCSELF:
			printf("self");
			break;
		case MXCARDSYNCSOURCE_SYNC1:
			printf("span1");
			break;
		case MXCARDSYNCSOURCE_SYNC2:
			printf("span2");
			break;
		case MXCARDSYNCSOURCE_SYNC3:
			printf("span3");
			break;
		case MXCARDSYNCSOURCE_SYNC4:
			printf("span4");
			break;
		case MXCARDSYNCSOURCE_SYNCEXTERN:
			printf("extern");
			break;
		case MXCARDSYNCSOURCE_SYNCAUTO:
			printf("auto");
			break;
		default:
			printf("0x%04x", abuf.vals.card.option.mxCardSyncSource);
			break;
		}
		printf("\n");
		printf("  Inform:\n");
		printf("    mxCardType:............... ");
		switch (abuf.vals.card.inform.mxCardType) {
		case X400PCARDTYPE_NONE:
			printf("none");
			break;
		case X400PCARDTYPE_PLX9030:
			printf("plx9030");
			break;
		case X400PCARDTYPE_PLXDEVBRD:
			printf("plxdevbrd");
			break;
		case X400PCARDTYPE_X400P:
			printf("x400p");
			break;
		case X400PCARDTYPE_E400P:
			printf("e400p");
			break;
		case X400PCARDTYPE_T400P:
			printf("t400p");
			break;
		case X400PCARDTYPE_X400PSS7:
			printf("x400p-ss7");
			break;
		case X400PCARDTYPE_E400PSS7:
			printf("e400p-ss7");
			break;
		case X400PCARDTYPE_T400PSS7:
			printf("t400p-ss7");
			break;
		case X400PCARDTYPE_V400P:
			printf("v400p");
			break;
		case X400PCARDTYPE_V400PE:
			printf("v400pe");
			break;
		case X400PCARDTYPE_V400PT:
			printf("v400pt");
			break;
		case X400PCARDTYPE_V401PE:
			printf("v401pe");
			break;
		case X400PCARDTYPE_V401PT:
			printf("v401pt");
			break;
		case X400PCARDTYPE_AE400P:
			printf("ae400p");
			break;
		case X400PCARDTYPE_AT400P:
			printf("at400p");
			break;
		case X400PCARDTYPE_A400P:
			printf("a400p");
			break;
		case X400PCARDTYPE_A400PE:
			printf("a400pe");
			break;
		case X400PCARDTYPE_A400PT:
			printf("a400pt");
			break;
		default:
			printf("0x%04x", abuf.vals.card.inform.mxCardType);
			break;
		}
		printf("\n");
		printf("    mxCardIdentifier:......... %u\n",
		       abuf.vals.card.inform.mxCardIdentifier);
		printf("    mxCardRevision:........... %u\n", abuf.vals.card.inform.mxCardRevision);
		printf("    mxCardChipType:........... %u\n", abuf.vals.card.inform.mxCardChipType);
		switch (abuf.vals.card.inform.mxCardChipType) {
		case X400PCARDCHIPTYPE_NONE:
			printf("none");
			break;
		case X400PCARDCHIPTYPE_DS2152:
			printf("ds2152");
			break;
		case X400PCARDCHIPTYPE_DS21352:
			printf("ds21352");
			break;
		case X400PCARDCHIPTYPE_DS21552:
			printf("ds21552");
			break;
		case X400PCARDCHIPTYPE_DS2154:
			printf("ds2154");
			break;
		case X400PCARDCHIPTYPE_DS21354:
			printf("ds21354");
			break;
		case X400PCARDCHIPTYPE_DS21554:
			printf("ds21554");
			break;
		case X400PCARDCHIPTYPE_DS2155:
			printf("ds2155");
			break;
		case X400PCARDCHIPTYPE_DS2156:
			printf("ds2156");
			break;
		case X400PCARDCHIPTYPE_DS21455:
			printf("ds21455");
			break;
		case X400PCARDCHIPTYPE_DS21458:
			printf("ds21458");
			break;
		default:
			printf("0x%04x", abuf.vals.card.inform.mxCardChipType);
			break;
		}
		printf("\n");
		printf("    mxCardChipRevision:....... %u\n",
		       abuf.vals.card.inform.mxCardChipRevision);
		printf("    mxCardPciBus:............. %u\n", abuf.vals.card.inform.mxCardPciBus);
		printf("    mxCardPciSlot:............ %u\n", abuf.vals.card.inform.mxCardPciSlot);
		printf("    mxCardPciIrq:............. %u\n", abuf.vals.card.inform.mxCardPciIrq);
		printf("    mxCardName:............... %s\n", abuf.vals.card.inform.mxCardName);
		printf("  Statem:\n");
		printf("  Status:\n");
		printf("    mxCardAdministrativeState: ");
		switch (abuf.vals.card.status.mxCardAdministrativeState) {
		case X721_ADMINISTRATIVESTATE_LOCKED:
			printf("locked");
			break;
		case X721_ADMINISTRATIVESTATE_UNLOCKED:
			printf("unlocked");
			break;
		case X721_ADMINISTRATIVESTATE_SHUTTINGDOWN:
			printf("shutting-down");
			break;
		default:
			printf("0x%04x", abuf.vals.card.status.mxCardAdministrativeState);
			break;
		}
		printf("\n");
		printf("    mxCardOperationalState:... ");
		switch (abuf.vals.card.status.mxCardOperationalState) {
		case X721_OPERATIONALSTATE_DISABLED:
			printf("disabled");
			break;
		case X721_OPERATIONALSTATE_ENABLED:
			printf("enabled");
			break;
		default:
			printf("0x%04x", abuf.vals.card.status.mxCardOperationalState);
			break;
		}
		printf("\n");
		printf("    mxCardUsageState:......... ");
		switch (abuf.vals.card.status.mxCardUsageState) {
		case X721_USAGESTATE_IDLE:
			printf("idle");
			break;
		case X721_USAGESTATE_ACTIVE:
			printf("active");
			break;
		case X721_USAGESTATE_BUSY:
			printf("busy");
			break;
		default:
			printf("0x%04x", abuf.vals.card.status.mxCardUsageState);
			break;
		}
		printf("\n");
		printf("    mxCardAlarmStatus:........ <");
		sep[0] = '\0';
		if (abuf.vals.card.status.mxCardAlarmStatus & (1 << X721_ALARMSTATUS_UNDERREPAIR)) {
			printf("%s%s", sep, "under-repair");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAlarmStatus & (1 << X721_ALARMSTATUS_CRITICAL)) {
			printf("%s%s", sep, "critical");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAlarmStatus & (1 << X721_ALARMSTATUS_MAJOR)) {
			printf("%s%s", sep, "major");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAlarmStatus & (1 << X721_ALARMSTATUS_MINOR)) {
			printf("%s%s", sep, "minor");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAlarmStatus
		    & (1 << X721_ALARMSTATUS_ALARMOUTSTANDING)) {
			printf("%s%s", sep, "outstanding");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxCardProceduralStatus:... <");
		sep[0] = '\0';
		if (abuf.vals.card.status.mxCardProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZATIONREQUIRED)) {
			printf("%s%s", sep, "initialization-required");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED)) {
			printf("%s%s", sep, "not-initialized");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZING)) {
			printf("%s%s", sep, "initializing");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_REPORTING)) {
			printf("%s%s", sep, "reporting");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_TERMINATING)) {
			printf("%s%s", sep, "terminating");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxCardAvailabilityStatus:. <");
		sep[0] = '\0';
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_INTEST)) {
			printf("%s%s", sep, "in-test");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_FAILED)) {
			printf("%s%s", sep, "failed");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_POWEROFF)) {
			printf("%s%s", sep, "power-off");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_OFFLINE)) {
			printf("%s%s", sep, "off-line");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_OFFDUTY)) {
			printf("%s%s", sep, "off-duty");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_DEPENDENCY)) {
			printf("%s%s", sep, "dependency");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_DEGRADED)) {
			printf("%s%s", sep, "degraded");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_NOTINSTALLED)) {
			printf("%s%s", sep, "not-installed");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_LOGFULL)) {
			printf("%s%s", sep, "log-full");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxCardControlStatus:...... <");
		sep[0] = '\0';
		if (abuf.vals.card.status.mxCardControlStatus
		    & (1 << X721_CONTROLSTATUS_SUBJECTTOTEST)) {
			printf("%s%s", sep, "subject-to-test");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardControlStatus
		    & (1 << X721_CONTROLSTATUS_PARTOFSERVICESLOCKED)) {
			printf("%s%s", sep, "part-of-services-locked");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardControlStatus
		    & (1 << X721_CONTROLSTATUS_RESERVEDFORTEST)) {
			printf("%s%s", sep, "reserved-for-test");
			sep[0] = ',';
		}
		if (abuf.vals.card.status.mxCardControlStatus & (1 << X721_CONTROLSTATUS_SUSPENDED)) {
			printf("%s%s", sep, "suspended");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxCardUnknownStatus:...... ");
		switch (abuf.vals.card.status.mxCardUnknownStatus) {
		case X721_UNKNOWNSTATUS_FALSE:
			printf("false");
			break;
		case X721_UNKNOWNSTATUS_TRUE:
			printf("true");
			break;
		default:
			printf("0x%04x", abuf.vals.card.status.mxCardUnknownStatus);
			break;
		}
		printf("\n");
		printf("    mxCardStandbyStatus:...... ");
		switch (abuf.vals.card.status.mxCardStandbyStatus) {
		case X721_STANDBYSTATUS_HOTSTANDBY:
			printf("hot-standby");
			break;
		case X721_STANDBYSTATUS_COLDSTANDBY:
			printf("cold-standby");
			break;
		case X721_STANDBYSTATUS_PROVIDINGSERVICE:
			printf("providing-service");
			break;
		default:
			printf("0x%04x", abuf.vals.card.status.mxCardStandbyStatus);
			break;
		}
		printf("\n");
		printf("  Stats:\n");
		printf("    mxCardSyncTransitions:.... %u\n",
		       abuf.vals.card.stats.mxCardSyncTransitions);
		printf("  Events:\n");
		printf("    events:................... <");
		sep[0] = '\0';
		if (abuf.vals.card.events.events & MX_SYNCTRANSITION) {
			printf("%s%s", sep, "sync");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_LOOPUP) {
			printf("%s%s", sep, "loop-up");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_LOOPDOWN) {
			printf("%s%s", sep, "loop-down");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_YELLOWALARM) {
			printf("%s%s", sep, "yel");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_YELLOWALARM_CLEARED) {
			printf("%s%s", sep, "~yel");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_REDALARM) {
			printf("%s%s", sep, "red");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_REDALARM_CLEARED) {
			printf("%s%s", sep, "~red");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_BLUEALARM) {
			printf("%s%s", sep, "blu");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_BLUEALARM_CLEARED) {
			printf("%s%s", sep, "~blu");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_TESTABORTED) {
			printf("%s%s", sep, "test-aborted");
			sep[0] = ',';
		}
		if (abuf.vals.card.events.events & MX_TESTCOMPLETE) {
			printf("%s%s", sep, "test-complete");
			sep[0] = ',';
		}
		printf(">\n");
	}
	if (output > 1)
		printf("I: closing %s\n", devname);
	close(fd);
}

void
mxconf_get_span(void)
{
	struct mx_attr_buf abuf = { };
	struct mx_list_buf lbuf = { };
	struct strioctl ctl = { };
	int fd, ret, i;
	char sep[2] = { 0, 0 };

	if (output > 1)
		printf("I: opening %s\n", devname);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	lbuf.conf.type = MX_OBJ_TYPE_SPAN;
	lbuf.conf.id = 0;
	lbuf.conf.cmd = MX_GET;
	ctl.ic_cmd = MX_IOCLCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lbuf);
	if (output > 1)
		printf("I: issuing MX_IOCLCONFIG input-output control\n");
	if ((ret = ioctl(fd, I_STR, &ctl)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output > 1)
		printf("I: got %d MX_OBJ_TYPE_SPAN objects\n", ret);
	for (i = 0; i < ret; i++) {
		abuf.attr.type = MX_OBJ_TYPE_SPAN;
		abuf.attr.id = lbuf.idxs[i];
		abuf.attr.cmd = MX_GET;
		ctl.ic_cmd = MX_IOCGATTR;
		ctl.ic_timout = 0;
		ctl.ic_len = sizeof(abuf);
		ctl.ic_dp = (char *) &abuf;
		if (output > 1)
			printf("I: issuing MX_IOCGATTR input-output control, PPA = 0x%04x\n", lbuf.idxs[i]);
		if (ioctl(fd, I_STR, &ctl) < 0) {
			perror(__FUNCTION__);
			exit(1);
		}
		printf("Span: 0x%04x\n", lbuf.idxs[i]);
		printf("  Config:\n");
		printf("    mxCardIndex:.............. %u\n", abuf.vals.span.config.mxCardIndex);
		printf("    mxSpanIndex:.............. %u\n", abuf.vals.span.config.mxSpanIndex);
		printf("    mxSpanName:............... %s\n", abuf.vals.span.config.mxSpanName);
		printf("    mxSpanType:............... ");
		switch (abuf.vals.span.config.mxSpanType) {
		case MXSPANTYPE_NONE:
			printf("none");
			break;
		case MXSPANTYPE_T1:
			printf("t1");
			break;
		case MXSPANTYPE_E1:
			printf("e1");
			break;
		case MXSPANTYPE_J1:
			printf("j1");
			break;
		case MXSPANTYPE_E2:
			printf("e2");
			break;
		case MXSPANTYPE_E3:
			printf("e3");
			break;
		case MXSPANTYPE_T3:
			printf("t3");
			break;
		default:
			printf("0x%04x", abuf.vals.span.config.mxSpanType);
			break;
		}
		printf("\n");
		printf("    mxSpanCrc:................ ");
		switch (abuf.vals.span.config.mxSpanCrc) {
		case MXSPANCRC_NONE:
			printf("none");
			break;
		case MXSPANCRC_CRC4:
			printf("crc4");
			break;
		case MXSPANCRC_CRC5:
			printf("crc5");
			break;
		case MXSPANCRC_CRC6:
			printf("crc6");
			break;
		case MXSPANCRC_CRC6J:
			printf("crc6j");
			break;
		default:
			printf("0x%04x", abuf.vals.span.config.mxSpanCrc);
			break;
		}
		printf("\n");
		printf("    mxSpanCoding:............. ");
		switch (abuf.vals.span.config.mxSpanCoding) {
		case MXSPANCODING_NONE:
			printf("none");
			break;
		case MXSPANCODING_AMI:
			printf("ami");
			break;
		case MXSPANCODING_B6ZS:
			printf("b6zs");
			break;
		case MXSPANCODING_B8ZS:
			printf("b8zs");
			break;
		case MXSPANCODING_HDB3:
			printf("hdb3");
			break;
		case MXSPANCODING_JBZS:
			printf("jbzs");
			break;
		case MXSPANCODING_ZBTSI:
			printf("zbtsi");
			break;
		default:
			printf("0x%04x", abuf.vals.span.config.mxSpanCoding);
			break;
		}
		printf("\n");
		printf("    mxSpanFraming:............ ");
		switch (abuf.vals.span.config.mxSpanFraming) {
		case MXSPANFRAMING_NONE:
			printf("none");
			break;
		case MXSPANFRAMING_CCS:
			printf("ccs");
			break;
		case MXSPANFRAMING_CAS:
			printf("cas");
			break;
		case MXSPANFRAMING_SF:
			printf("sf");
			break;
		case MXSPANFRAMING_D4:
			printf("d4");
			break;
		case MXSPANFRAMING_ESF:
			printf("esf");
			break;
		default:
			printf("0x%04x", abuf.vals.span.config.mxSpanFraming);
			break;
		}
		printf("\n");
		printf("    mxSpanDataLink:........... ");
		switch (abuf.vals.span.config.mxSpanDataLink) {
		case MXSPANDATALINK_ANSIT1403:
			printf("ansi-t1.403");
			break;
		case MXSPANDATALINK_ATT54016:
			printf("att-54016");
			break;
		case MXSPANDATALINK_OTHER:
			printf("other");
			break;
		default:
			printf("0x%04x", abuf.vals.span.config.mxSpanDataLink);
			break;
		}
		printf("\n");
		printf("    mxSpanPriority:........... %u\n", abuf.vals.span.config.mxSpanPriority);
		printf("    mxSpanPrimary:............ %u\n", abuf.vals.span.config.mxSpanPrimary);
		printf("  Option:\n");
		printf("    mxSpanMode:............... <");
		sep[0] = '\0';
		if (abuf.vals.span.option.mxSpanMode & (1 << MXSPANMODE_LOCAL)) {
			printf("%s%s", sep, "local-loopback");
			sep[0] = ',';
		}
		if (abuf.vals.span.option.mxSpanMode & (1 << MXSPANMODE_REMOTE)) {
			printf("%s%s", sep, "remote-loopback");
			sep[0] = ',';
		}
		if (abuf.vals.span.option.mxSpanMode & (1 << MXSPANMODE_FRAMER)) {
			printf("%s%s", sep, "framer-loopback");
			sep[0] = ',';
		}
		if (abuf.vals.span.option.mxSpanMode & (1 << MXSPANMODE_PAYLOAD)) {
			printf("%s%s", sep, "payload-loopback");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxSpanClocking:........... ");
		switch (abuf.vals.span.option.mxSpanClocking) {
		case MXSPANCLOCKING_NONE:
			printf("none");
			break;
		case MXSPANCLOCKING_LOOP:
			printf("loop");
			break;
		case MXSPANCLOCKING_LOCAL:
			printf("local");
			break;
		case MXSPANCLOCKING_INTERNAL:
			printf("internal");
			break;
		case MXSPANCLOCKING_EXTERNAL:
			printf("external");
			break;
		case MXSPANCLOCKING_ADAPTIVE:
			printf("adaptive");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanClocking);
			break;
		}
		printf("\n");
		printf("    mxSpanLineImpedance:...... ");
		switch (abuf.vals.span.option.mxSpanLineImpedance) {
		case MXSPANLINEIMPEDANCE_NONE:
			printf("none");
			break;
		case MXSPANLINEIMPEDANCE_UNBALANCED75OHM:
			printf("75-Ohm");
			break;
		case MXSPANLINEIMPEDANCE_BALANCED100OHM:
			printf("100-Ohm");
			break;
		case MXSPANLINEIMPEDANCE_BALANCED120OHM:
			printf("120-Ohm");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanLineImpedance);
			break;
		}
		printf("\n");
		printf("    mxSpanLineMode:........... ");
		switch (abuf.vals.span.option.mxSpanLineMode) {
		case MXSPANLINEMODE_NONE:
			printf("none");
			break;
		case MXSPANLINEMODE_DSU:
			printf("dsu");
			break;
		case MXSPANLINEMODE_CSU:
			printf("csu");
			break;
		case MXSPANLINEMODE_MONITOR:
			printf("monitor");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanLineMode);
			break;
		}
		printf("\n");
		printf("    mxSpanLineLength:......... ");
		switch (abuf.vals.span.option.mxSpanLineLength) {
		case MXSPANLINELENGTH_NONE:
			printf("none");
			break;
		case MXSPANLINELENGTH_DSX133FT:
			printf("dsx-133-ft");
			break;
		case MXSPANLINELENGTH_DSX266FT:
			printf("dsx-266-ft");
			break;
		case MXSPANLINELENGTH_DSX399FT:
			printf("dsx-399-ft");
			break;
		case MXSPANLINELENGTH_DSX533FT:
			printf("dsx-533-ft");
			break;
		case MXSPANLINELENGTH_DSX666FT:
			printf("dsx-666-ft");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanLineLength);
			break;
		}
		printf("\n");
		printf("    mxSpanLineAttenuation:.... ");
		switch (abuf.vals.span.option.mxSpanLineAttenuation) {
		case MXSPANLINEATTENUATION_NONE:
			printf("none");
			break;
		case MXSPANLINEATTENUATION_CSU0DB:
			printf("csu-0dB");
			break;
		case MXSPANLINEATTENUATION_CSU8DB:
			printf("csu-8dB");
			break;
		case MXSPANLINEATTENUATION_CSU15DB:
			printf("csu-15dB");
			break;
		case MXSPANLINEATTENUATION_CSU23DB:
			printf("csu-23dB");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanLineAttenuation);
			break;
		}
		printf("\n");
		printf("    mxSpanLineGain:........... ");
		switch (abuf.vals.span.option.mxSpanLineGain) {
		case MXSPANLINEGAIN_NONE:
			printf("none");
			break;
		case MXSPANLINEGAIN_MON0DB:
			printf("monitor-0dB");
			break;
		case MXSPANLINEGAIN_MON12DB:
			printf("monitor-12dB");
			break;
		case MXSPANLINEGAIN_MON20DB:
			printf("monitor-20dB");
			break;
		case MXSPANLINEGAIN_MON26DB:
			printf("monitor-26dB");
			break;
		case MXSPANLINEGAIN_MON30DB:
			printf("monitor-30dB");
			break;
		case MXSPANLINEGAIN_MON32DB:
			printf("monitor-32dB");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanLineGain);
			break;
		}
		printf("\n");
		printf("    mxSpanLineDelay:.......... %u milliseconds\n",
		       abuf.vals.span.option.mxSpanLineDelay);
		printf("    mxSpanTxLevel:............ ");
		switch (abuf.vals.span.option.mxSpanTxLevel) {
		case MXSPANTXLEVEL_OPEN:
			printf("open");
			break;
		case MXSPANTXLEVEL_OFF:
			printf("off");
			break;
		case MXSPANTXLEVEL_ON:
			printf("on");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanTxLevel);
			break;
		}
		printf("\n");
		printf("    mxSpanRxLevel:............ ");
		switch (abuf.vals.span.option.mxSpanRxLevel) {
		case MXSPANRXLEVEL_OPEN:
			printf("open");
			break;
		case MXSPANRXLEVEL_OFF:
			printf("off");
			break;
		case MXSPANRXLEVEL_ON:
			printf("on");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanRxLevel);
			break;
		}
		printf("\n");
		printf("    mxSpanAlarmSettleTime:.... %u milliseconds\n",
		       abuf.vals.span.option.mxSpanAlarmSettleTime);
		printf("    mxSpanLineCodeTime:....... %u milliseconds\n",
		       abuf.vals.span.option.mxSpanLineCodeTime);
		printf("    mxSpanLineCode:........... ");
		switch (abuf.vals.span.option.mxSpanLineCode) {
		case MXSPANLINECODE_NOCODE:
			printf("no-code");
			break;
		case MXSPANLINECODE_LINECODE:
			printf("line-code");
			break;
		case MXSPANLINECODE_PAYLOADCODE:
			printf("payload-code");
			break;
		case MXSPANLINECODE_RESETCODE:
			printf("reset-code");
			break;
		case MXSPANLINECODE_TESTCODE:
			printf("test-code");
			break;
		case MXSPANLINECODE_UNFRAMEDALLONES:
			printf("unframed-all-ones");
			break;
		case MXSPANLINECODE_UNFRAMEDONEANDZERO:
			printf("unframed-one-and-zero");
			break;
		default:
			printf("0x%04x", abuf.vals.span.option.mxSpanLineCode);
			break;
		}
		printf("\n");
		printf("    mxSpanReceiveThreshold:... %u\n",
		       abuf.vals.span.option.mxSpanReceiveThreshold);
		printf("  Inform:\n");
		printf("  Statem:\n");
		printf("  Status:\n");
		printf("    mxSpanAdministrativeState: ");
		switch (abuf.vals.span.status.mxSpanAdministrativeState) {
		case X721_ADMINISTRATIVESTATE_LOCKED:
			printf("locked");
			break;
		case X721_ADMINISTRATIVESTATE_UNLOCKED:
			printf("unlocked");
			break;
		case X721_ADMINISTRATIVESTATE_SHUTTINGDOWN:
			printf("shutting-down");
			break;
		default:
			printf("0x%04x", abuf.vals.span.status.mxSpanAdministrativeState);
			break;
		}
		printf("\n");
		printf("    mxSpanOperationalState:... ");
		switch (abuf.vals.span.status.mxSpanOperationalState) {
		case X721_OPERATIONALSTATE_DISABLED:
			printf("disabled");
			break;
		case X721_OPERATIONALSTATE_ENABLED:
			printf("enabled");
			break;
		default:
			printf("0x%04x", abuf.vals.span.status.mxSpanOperationalState);
			break;
		}
		printf("\n");
		printf("    mxSpanUsageState:......... ");
		switch (abuf.vals.span.status.mxSpanUsageState) {
		case X721_USAGESTATE_IDLE:
			printf("idle");
			break;
		case X721_USAGESTATE_ACTIVE:
			printf("active");
			break;
		case X721_USAGESTATE_BUSY:
			printf("busy");
			break;
		default:
			printf("0x%04x", abuf.vals.span.status.mxSpanUsageState);
			break;
		}
		printf("\n");
		printf("    mxSpanAlarmStatus:........ <");
		sep[0] = '\0';
		if (abuf.vals.span.status.mxSpanAlarmStatus & (1 << X721_ALARMSTATUS_UNDERREPAIR)) {
			printf("%s%s", sep, "under-repair");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarmStatus & (1 << X721_ALARMSTATUS_CRITICAL)) {
			printf("%s%s", sep, "critical");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarmStatus & (1 << X721_ALARMSTATUS_MAJOR)) {
			printf("%s%s", sep, "major");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarmStatus & (1 << X721_ALARMSTATUS_MINOR)) {
			printf("%s%s", sep, "minor");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarmStatus
		    & (1 << X721_ALARMSTATUS_ALARMOUTSTANDING)) {
			printf("%s%s", sep, "outstanding");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxSpanProceduralStatus:... <");
		sep[0] = '\0';
		if (abuf.vals.span.status.mxSpanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZATIONREQUIRED)) {
			printf("%s%s", sep, "initialization-required");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED)) {
			printf("%s%s", sep, "not-initialized");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZING)) {
			printf("%s%s", sep, "initializing");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_REPORTING)) {
			printf("%s%s", sep, "reporting");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_TERMINATING)) {
			printf("%s%s", sep, "terminating");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxSpanAvailabilityStatus:. <");
		sep[0] = '\0';
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_INTEST)) {
			printf("%s%s", sep, "in-test");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_FAILED)) {
			printf("%s%s", sep, "failed");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_POWEROFF)) {
			printf("%s%s", sep, "power-off");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_OFFLINE)) {
			printf("%s%s", sep, "off-line");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_OFFDUTY)) {
			printf("%s%s", sep, "off-duty");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_DEPENDENCY)) {
			printf("%s%s", sep, "dependency");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_DEGRADED)) {
			printf("%s%s", sep, "degraded");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_NOTINSTALLED)) {
			printf("%s%s", sep, "not-installed");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_LOGFULL)) {
			printf("%s%s", sep, "log-full");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxSpanControlStatus:...... <");
		sep[0] = '\0';
		if (abuf.vals.span.status.mxSpanControlStatus
		    & (1 << X721_CONTROLSTATUS_SUBJECTTOTEST)) {
			printf("%s%s", sep, "subject-to-test");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanControlStatus
		    & (1 << X721_CONTROLSTATUS_PARTOFSERVICESLOCKED)) {
			printf("%s%s", sep, "part-of-services-locked");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanControlStatus
		    & (1 << X721_CONTROLSTATUS_RESERVEDFORTEST)) {
			printf("%s%s", sep, "reserved-for-test");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanControlStatus & (1 << X721_CONTROLSTATUS_SUSPENDED)) {
			printf("%s%s", sep, "suspended");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxSpanStandbyStatus:...... ");
		switch (abuf.vals.span.status.mxSpanStandbyStatus) {
		case X721_STANDBYSTATUS_HOTSTANDBY:
			printf("hot-standby");
			break;
		case X721_STANDBYSTATUS_COLDSTANDBY:
			printf("cold-standby");
			break;
		case X721_STANDBYSTATUS_PROVIDINGSERVICE:
			printf("providing-service");
			break;
		default:
			printf("0x%04x", abuf.vals.span.status.mxSpanStandbyStatus);
			break;
		}
		printf("\n");
		printf("    mxSpanUnknownStatus:...... ");
		switch (abuf.vals.span.status.mxSpanUnknownStatus) {
		case X721_UNKNOWNSTATUS_FALSE:
			printf("false");
			break;
		case X721_UNKNOWNSTATUS_TRUE:
			printf("true");
			break;
		default:
			printf("0x%04x", abuf.vals.span.status.mxSpanUnknownStatus);
			break;
		}
		printf("\n");
		printf("    mxSpanLoopbackStatus:..... ");
		switch (abuf.vals.span.status.mxSpanLoopbackStatus) {
		case MXSPANLOOPBACKSTATUS_NEARENDINWARD:
			printf("near-end-inward");
			break;
		case MXSPANLOOPBACKSTATUS_NEARENDLINE:
			printf("near-end-line");
			break;
		case MXSPANLOOPBACKSTATUS_NEARENDFRAMER:
			printf("near-end-framer");
			break;
		case MXSPANLOOPBACKSTATUS_NEARENDPAYLOAD:
			printf("near-end-payload");
			break;
		case MXSPANLOOPBACKSTATUS_FARENDLINE:
			printf("far-end-line");
			break;
		case MXSPANLOOPBACKSTATUS_FARENDPAYLOAD:
			printf("far-end-payload");
			break;
		default:
			printf("0x%04x", abuf.vals.span.status.mxSpanLoopbackStatus);
			break;
		}
		printf("\n");
		printf("    mxSpanLineStatus:......... <");
		sep[0] = '\0';
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_NONE)) {
			printf("%s%s", sep, "none");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_RCVFARENDLOF)) {
			printf("%s%s", sep, "rcv-far-end-lof");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_XMTFARENDLOF)) {
			printf("%s%s", sep, "xmt-far-end-lof");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_RCVAIS)) {
			printf("%s%s", sep, "rcv-ais");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_XMTAIS)) {
			printf("%s%s", sep, "xmt-ais");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_LOSSOFFRAME)) {
			printf("%s%s", sep, "loss-of-frame");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_LOSSOFSIGNAL)) {
			printf("%s%s", sep, "loss-of-signal");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_LOOPBACKSTATE)) {
			printf("%s%s", sep, "loop-back-state");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_T16AIS)) {
			printf("%s%s", sep, "t16-ais");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_RCVFARENDLOMF)) {
			printf("%s%s", sep, "rcv-far-end-lomf");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_XMTFARENDLOMF)) {
			printf("%s%s", sep, "xmt-far-end-lomf");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_RCVTESTCODE)) {
			printf("%s%s", sep, "rcv-test-code");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_OTHERFAILURE)) {
			printf("%s%s", sep, "other-failure");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_UAVAILSIGSTATE)) {
			printf("%s%s", sep, "uavail-sig-state");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_NETEQUIPOOS)) {
			printf("%s%s", sep, "net-equip-oos");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_RCVPAYLOADAIS)) {
			printf("%s%s", sep, "rcv-payload-ais");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanLineStatus & (1 << MXSPANLINESTATUS_PERFTHRESHOLD)) {
			printf("%s%s", sep, "perf-threshold");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxSpanAlarms:............. <");
		sep[0] = '\0';
		if (abuf.vals.span.status.mxSpanAlarms & (1 << MXSPANALARMS_YELLOW)) {
			printf("%s%s", sep, "yel");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarms & (1 << MXSPANALARMS_RED)) {
			printf("%s%s", sep, "red");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarms & (1 << MXSPANALARMS_BLUE)) {
			printf("%s%s", sep, "blu");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarms & (1 << MXSPANALARMS_RECOVERY)) {
			printf("%s%s", sep, "rec");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanAlarms & (1 << MXSPANALARMS_DISTMF)) {
			printf("%s%s", sep, "dmf");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxSpanEvents:............. <");
		sep[0] = '\0';
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_RLOS)) {
			printf("%s%s", sep, "rlos");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_FRCL)) {
			printf("%s%s", sep, "frcl");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_RUAL)) {
			printf("%s%s", sep, "rual");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_RYEL)) {
			printf("%s%s", sep, "ryel");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_RRA)) {
			printf("%s%s", sep, "rra");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_RDMA)) {
			printf("%s%s", sep, "rdma");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_V52LNK)) {
			printf("%s%s", sep, "v52lnk");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_LORC)) {
			printf("%s%s", sep, "lorc");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_LOTC)) {
			printf("%s%s", sep, "lotc");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_LUP)) {
			printf("%s%s", sep, "lup");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_LDN)) {
			printf("%s%s", sep, "ldn");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_LSPARE)) {
			printf("%s%s", sep, "lspare");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_TOCD)) {
			printf("%s%s", sep, "tocd");
			sep[0] = ',';
		}
		if (abuf.vals.span.status.mxSpanEvents & (1 << MXSPANEVENTS_TCLE)) {
			printf("%s%s", sep, "tcle");
			sep[0] = ',';
		}
		printf(">\n");
		printf("  Stats:\n");
		printf("    mxNearEndESs:............. %u\n", abuf.vals.span.stats.mxNearEndESs);
		printf("    mxNearEndSESs:............ %u\n", abuf.vals.span.stats.mxNearEndSESs);
		printf("    mxNearEndSEFSs:........... %u\n", abuf.vals.span.stats.mxNearEndSEFSs);
		printf("    mxNearEndUASs:............ %u\n", abuf.vals.span.stats.mxNearEndUASs);
		printf("    mxNearEndCSSs:............ %u\n", abuf.vals.span.stats.mxNearEndCSSs);
		printf("    mxNearEndPCVs:............ %u\n", abuf.vals.span.stats.mxNearEndPCVs);
		printf("    mxNearEndLESs:............ %u\n", abuf.vals.span.stats.mxNearEndLESs);
		printf("    mxNearEndBESs:............ %u\n", abuf.vals.span.stats.mxNearEndBESs);
		printf("    mxNearEndDMs:............. %u\n", abuf.vals.span.stats.mxNearEndDMs);
		printf("    mxNearEndLCVs:............ %u\n", abuf.vals.span.stats.mxNearEndLCVs);
		printf("    mxNearEndValidData:....... ");
		switch (abuf.vals.span.stats.mxNearEndValidData) {
		case 1:	/* false(1) */
			printf("false");
			break;
		case 2:	/* true(2) */
			printf("true");
			break;
		default:
			printf("0x%04x", abuf.vals.span.stats.mxNearEndValidData);
			break;
		}
		printf("\n");
		printf("    mxFarEndESs:.............. %u\n", abuf.vals.span.stats.mxFarEndESs);
		printf("    mxFarEndSESs:............. %u\n", abuf.vals.span.stats.mxFarEndSESs);
		printf("    mxFarEndSEFSs:............ %u\n", abuf.vals.span.stats.mxFarEndSEFSs);
		printf("    mxFarEndUASs:............. %u\n", abuf.vals.span.stats.mxFarEndUASs);
		printf("    mxFarEndCSSs:............. %u\n", abuf.vals.span.stats.mxFarEndCSSs);
		printf("    mxFarEndPCVs:............. %u\n", abuf.vals.span.stats.mxFarEndPCVs);
		printf("    mxFarEndLESs:............. %u\n", abuf.vals.span.stats.mxFarEndLESs);
		printf("    mxFarEndBESs:............. %u\n", abuf.vals.span.stats.mxFarEndBESs);
		printf("    mxFarEndDMs:.............. %u\n", abuf.vals.span.stats.mxFarEndDMs);
		printf("    mxFarEndValidData:........ ");
		switch (abuf.vals.span.stats.mxFarEndValidData) {
		case 1:	/* false(1) */
			printf("false");
			break;
		case 2:	/* true(2) */
			printf("true");
			break;
		default:
			printf("0x%04x", abuf.vals.span.stats.mxFarEndValidData);
			break;
		}
		printf("\n");
		printf("  Events:\n");
		printf("    events:................... <");
		sep[0] = '\0';
		if (abuf.vals.span.events.events & MX_SYNCTRANSITION) {
			printf("%s%s", sep, "sync");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_LOOPUP) {
			printf("%s%s", sep, "loop-up");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_LOOPDOWN) {
			printf("%s%s", sep, "loop-down");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_YELLOWALARM) {
			printf("%s%s", sep, "yel");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_YELLOWALARM_CLEARED) {
			printf("%s%s", sep, "~yel");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_REDALARM) {
			printf("%s%s", sep, "red");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_REDALARM_CLEARED) {
			printf("%s%s", sep, "~red");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_BLUEALARM) {
			printf("%s%s", sep, "blu");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_BLUEALARM_CLEARED) {
			printf("%s%s", sep, "~blu");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_TESTABORTED) {
			printf("%s%s", sep, "test-aborted");
			sep[0] = ',';
		}
		if (abuf.vals.span.events.events & MX_TESTCOMPLETE) {
			printf("%s%s", sep, "test-complete");
			sep[0] = ',';
		}
		printf(">\n");
	}
	if (output > 1)
		printf("I: closing %s\n", devname);
	close(fd);
}

void
mxconf_get_chan(void)
{
	struct mx_attr_buf abuf = { };
	struct mx_list_buf lbuf = { };
	struct strioctl ctl = { };
	int fd, ret, i;
	char sep[2] = { 0, 0 };

	if (output > 1)
		printf("I: opening %s\n", devname);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	lbuf.conf.type = MX_OBJ_TYPE_CHAN;
	lbuf.conf.id = 0;
	lbuf.conf.cmd = MX_GET;
	ctl.ic_cmd = MX_IOCLCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lbuf);
	if (output > 1)
		printf("I: issuing MX_IOCLCONFIG input-output control\n");
	if ((ret = ioctl(fd, I_STR, &ctl)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output > 1)
		printf("I: got %d MX_OBJ_TYPE_CHAN objects\n", ret);
	for (i = 0; i < ret; i++) {
		abuf.attr.type = MX_OBJ_TYPE_CHAN;
		abuf.attr.id = lbuf.idxs[i];
		abuf.attr.cmd = MX_GET;
		ctl.ic_cmd = MX_IOCGATTR;
		ctl.ic_timout = 0;
		ctl.ic_len = sizeof(abuf);
		ctl.ic_dp = (char *) &abuf;
		if (output > 1)
			printf("I: issuing MX_IOCGATTR input-output control, PPA = 0x%04x\n", lbuf.idxs[i]);
		if (ioctl(fd, I_STR, &ctl) < 0) {
			perror(__FUNCTION__);
			exit(1);
		}
		printf("Chan: 0x%04x\n", lbuf.idxs[i]);
		printf("  Config:\n");
		printf("    mxCardIndex:.............. %u\n", abuf.vals.chan.config.mxCardIndex);
		printf("    mxSpanIndex:.............. %u\n", abuf.vals.chan.config.mxSpanIndex);
		printf("    mxChanIndex:.............. %u\n", abuf.vals.chan.config.mxChanIndex);
		printf("  Option:\n");
		printf("    mxChanType:............... ");
		switch (abuf.vals.chan.option.mxChanType) {
		case MXCHANTYPE_NONE:
			printf("none");
			break;
		case MXCHANTYPE_CAS:
			printf("cas");
			break;
		case MXCHANTYPE_CCS:
			printf("ccs");
			break;
		default:
			printf("0x%04x", abuf.vals.chan.option.mxChanType);
			break;
		}
		printf("\n");
		printf("    mxChanFormat:............. ");
		switch (abuf.vals.chan.option.mxChanFormat) {
		case MXCHANFORMAT_NONE:
			printf("none");
			break;
		case MXCHANFORMAT_DS0A:
			printf("ds0a");
			break;
		case MXCHANFORMAT_DS0:
			printf("ds0");
			break;
		case MXCHANFORMAT_T1:
			printf("t1");
			break;
		case MXCHANFORMAT_J1:
			printf("j1");
			break;
		case MXCHANFORMAT_E1:
			printf("e1");
			break;
		default:
			printf("0x%04x", abuf.vals.chan.option.mxChanFormat);
			break;
		}
		printf("\n");
		printf("    mxChanRate:............... ");
		switch (abuf.vals.chan.option.mxChanRate) {
		case MXCHANRATE_VARIABLE:
			printf("variable");
			break;
		case MXCHANRATE_KBITS56:
			printf("56-kbps");
			break;
		case MXCHANRATE_KBITS64:
			printf("64-kbps");
			break;
		case MXCHANRATE_KBITS1536:
			printf("1536-kbps");
			break;
		case MXCHANRATE_KBITS1984:
			printf("1984-kbps");
			break;
		default:
			printf("%u-kbps", abuf.vals.chan.option.mxChanRate);
			break;
		}
		printf("\n");
		printf("    mxChanMode:............... <");
		sep[0] = '\0';
		if (abuf.vals.chan.option.mxChanMode & (1 << MXCHANMODE_REMOTELOOPBACK)) {
			printf("%s%s", sep, "remote-loopback");
			sep[0] = ',';
		}
		if (abuf.vals.chan.option.mxChanMode & (1 << MXCHANMODE_LOCALLOOPBACK)) {
			printf("%s%s", sep, "local-loopback");
			sep[0] = ',';
		}
		if (abuf.vals.chan.option.mxChanMode & (1 << MXCHANMODE_TEST)) {
			printf("%s%s", sep, "test");
			sep[0] = ',';
		}
		if (abuf.vals.chan.option.mxChanMode & (1 << MXCHANMODE_LOOPBACKECHO)) {
			printf("%s%s", sep, "loopback-echo");
			sep[0] = ',';
		}
		printf(">\n");
		printf("  Inform:\n");
		printf("  Statem:\n");
		printf("  Status:\n");
		printf("    mxChanAdministrativeState: ");
		switch (abuf.vals.chan.status.mxChanAdministrativeState) {
		case X721_ADMINISTRATIVESTATE_LOCKED:
			printf("locked");
			break;
		case X721_ADMINISTRATIVESTATE_UNLOCKED:
			printf("unlocked");
			break;
		case X721_ADMINISTRATIVESTATE_SHUTTINGDOWN:
			printf("shutting-down");
			break;
		default:
			printf("0x%04x", abuf.vals.chan.status.mxChanAdministrativeState);
			break;
		}
		printf("\n");
		printf("    mxChanOperationalState:... ");
		switch (abuf.vals.chan.status.mxChanOperationalState) {
		case X721_OPERATIONALSTATE_DISABLED:
			printf("disabled");
			break;
		case X721_OPERATIONALSTATE_ENABLED:
			printf("enabled");
			break;
		default:
			printf("0x%04x", abuf.vals.chan.status.mxChanOperationalState);
			break;
		}
		printf("\n");
		printf("    mxChanUsageState:......... ");
		switch (abuf.vals.chan.status.mxChanUsageState) {
		case X721_USAGESTATE_IDLE:
			printf("idle");
			break;
		case X721_USAGESTATE_ACTIVE:
			printf("active");
			break;
		case X721_USAGESTATE_BUSY:
			printf("busy");
			break;
		default:
			printf("0x%04x", abuf.vals.chan.status.mxChanUsageState);
			break;
		}
		printf("\n");
		printf("    mxChanAvailabilityStatus:. <");
		sep[0] = '\0';
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_INTEST)) {
			printf("%s%s", sep, "in-test");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_FAILED)) {
			printf("%s%s", sep, "failed");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_POWEROFF)) {
			printf("%s%s", sep, "power-off");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_OFFLINE)) {
			printf("%s%s", sep, "off-line");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_OFFDUTY)) {
			printf("%s%s", sep, "off-duty");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_DEPENDENCY)) {
			printf("%s%s", sep, "dependency");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_DEGRADED)) {
			printf("%s%s", sep, "degraded");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_NOTINSTALLED)) {
			printf("%s%s", sep, "not-installed");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAvailabilityStatus
		    & (1 << X721_AVAILABILITYSTATUS_LOGFULL)) {
			printf("%s%s", sep, "log-full");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxChanControlStatus:...... <");
		sep[0] = '\0';
		if (abuf.vals.chan.status.mxChanControlStatus
		    & (1 << X721_CONTROLSTATUS_SUBJECTTOTEST)) {
			printf("%s%s", sep, "subject-to-test");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanControlStatus
		    & (1 << X721_CONTROLSTATUS_PARTOFSERVICESLOCKED)) {
			printf("%s%s", sep, "part-of-services-locked");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanControlStatus
		    & (1 << X721_CONTROLSTATUS_RESERVEDFORTEST)) {
			printf("%s%s", sep, "reserved-for-test");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanControlStatus & (1 << X721_CONTROLSTATUS_SUSPENDED)) {
			printf("%s%s", sep, "suspended");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxChanProceduralStatus:... <");
		sep[0] = '\0';
		if (abuf.vals.chan.status.mxChanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZATIONREQUIRED)) {
			printf("%s%s", sep, "initialization-required");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED)) {
			printf("%s%s", sep, "not-initialized");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZING)) {
			printf("%s%s", sep, "initializing");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_REPORTING)) {
			printf("%s%s", sep, "reporting");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_TERMINATING)) {
			printf("%s%s", sep, "terminating");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxChanAlarmStatus:........ <");
		sep[0] = '\0';
		if (abuf.vals.chan.status.mxChanAlarmStatus & (1 << X721_ALARMSTATUS_UNDERREPAIR)) {
			printf("%s%s", sep, "under-repair");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAlarmStatus & (1 << X721_ALARMSTATUS_CRITICAL)) {
			printf("%s%s", sep, "critical");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAlarmStatus & (1 << X721_ALARMSTATUS_MAJOR)) {
			printf("%s%s", sep, "major");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAlarmStatus & (1 << X721_ALARMSTATUS_MINOR)) {
			printf("%s%s", sep, "minor");
			sep[0] = ',';
		}
		if (abuf.vals.chan.status.mxChanAlarmStatus
		    & (1 << X721_ALARMSTATUS_ALARMOUTSTANDING)) {
			printf("%s%s", sep, "outstanding");
			sep[0] = ',';
		}
		printf(">\n");
		printf("    mxChanStandbyStatus:...... ");
		switch (abuf.vals.chan.status.mxChanStandbyStatus) {
		case X721_STANDBYSTATUS_HOTSTANDBY:
			printf("hot-standby");
			break;
		case X721_STANDBYSTATUS_COLDSTANDBY:
			printf("cold-standby");
			break;
		case X721_STANDBYSTATUS_PROVIDINGSERVICE:
			printf("providing-service");
			break;
		default:
			printf("0x%04x", abuf.vals.chan.status.mxChanStandbyStatus);
			break;
		}
		printf("\n");
		printf("  Stats:\n");
		printf("  Events:\n");
		printf("    events.................... <");
		sep[0] = '\0';
		if (abuf.vals.chan.events.events & MX_SYNCTRANSITION) {
			printf("%s%s", sep, "sync");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_LOOPUP) {
			printf("%s%s", sep, "loop-up");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_LOOPDOWN) {
			printf("%s%s", sep, "loop-down");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_YELLOWALARM) {
			printf("%s%s", sep, "yel");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_YELLOWALARM_CLEARED) {
			printf("%s%s", sep, "~yel");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_REDALARM) {
			printf("%s%s", sep, "red");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_REDALARM_CLEARED) {
			printf("%s%s", sep, "~red");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_BLUEALARM) {
			printf("%s%s", sep, "blu");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_BLUEALARM_CLEARED) {
			printf("%s%s", sep, "~blu");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_TESTABORTED) {
			printf("%s%s", sep, "test-aborted");
			sep[0] = ',';
		}
		if (abuf.vals.chan.events.events & MX_TESTCOMPLETE) {
			printf("%s%s", sep, "test-complete");
			sep[0] = ',';
		}
		printf(">\n");
	}
	if (output > 1)
		printf("I: closing %s\n", devname);
	close(fd);
}

void
mxconf_get_frac(void)
{
	struct mx_attr_buf abuf = { };
	struct mx_list_buf lbuf = { };
	struct strioctl ctl = { };
	int fd, ret, i;
	char sep[2] = { 0, 0 };

	if (output > 1)
		printf("I: opening %s\n", devname);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	lbuf.conf.type = MX_OBJ_TYPE_FRAC;
	lbuf.conf.id = 0;
	lbuf.conf.cmd = MX_GET;
	ctl.ic_cmd = MX_IOCLCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lbuf);
	if (output > 1)
		printf("I: issuing MX_IOCLCONFIG input-output control\n");
	if ((ret = ioctl(fd, I_STR, &ctl)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output > 1)
		printf("I: got %d MX_OBJ_TYPE_FRAC objects\n", ret);
	for (i = 0; i < ret; i++) {
		abuf.attr.type = MX_OBJ_TYPE_FRAC;
		abuf.attr.id = lbuf.idxs[i];
		abuf.attr.cmd = MX_GET;
		ctl.ic_cmd = MX_IOCGATTR;
		ctl.ic_timout = 0;
		ctl.ic_len = sizeof(abuf);
		ctl.ic_dp = (char *) &abuf;
		if (output > 1)
			printf("I: issuing MX_IOCGATTR input-output control, PPA = 0x%04x\n", lbuf.idxs[i]);
		if (ioctl(fd, I_STR, &ctl) < 0) {
			perror(__FUNCTION__);
			exit(1);
		}
		printf("Frac: 0x%04x\n", lbuf.idxs[i]);
		printf("  Config:\n");
		printf("  Option:\n");
		printf("  Inform:\n");
		printf("  Statem:\n");
		printf("  Status:\n");
		printf("  Stats:\n");
		printf("  Events:\n");
	}
	if (output > 1)
		printf("I: closing %s\n", devname);
	close(fd);
}

void
mxconf_get_xcon(void)
{
	struct mx_attr_buf abuf = { };
	struct mx_list_buf lbuf = { };
	struct strioctl ctl = { };
	int fd, ret, i;
	char sep[2] = { 0, 0 };

	if (output > 1)
		printf("I: opening %s\n", devname);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	lbuf.conf.type = MX_OBJ_TYPE_XCON;
	lbuf.conf.id = 0;
	lbuf.conf.cmd = MX_GET;
	ctl.ic_cmd = MX_IOCLCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lbuf);
	if (output > 1)
		printf("I: issuing MX_IOCLCONFIG input-output control\n");
	if ((ret = ioctl(fd, I_STR, &ctl)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output > 1)
		printf("I: got %d MX_OBJ_TYPE_XCON objects\n", ret);
	for (i = 0; i < ret; i++) {
		abuf.attr.type = MX_OBJ_TYPE_XCON;
		abuf.attr.id = lbuf.idxs[i];
		abuf.attr.cmd = MX_GET;
		ctl.ic_cmd = MX_IOCGATTR;
		ctl.ic_timout = 0;
		ctl.ic_len = sizeof(abuf);
		ctl.ic_dp = (char *) &abuf;
		if (output > 1)
			printf("I: issuing MX_IOCGATTR input-output control, PPA = 0x%04x\n", lbuf.idxs[i]);
		if (ioctl(fd, I_STR, &ctl) < 0) {
			perror(__FUNCTION__);
			exit(1);
		}
		printf("Xcon: 0x%04x\n", lbuf.idxs[i]);
		printf("  Config:\n");
		printf("    mxCardIndex:.............. %u\n", abuf.vals.xcon.config.mxCardIndex);
		printf("    mxSpanIndex:.............. %u\n", abuf.vals.xcon.config.mxSpanIndex);
		printf("    mxChanIndex:.............. %u\n", abuf.vals.xcon.config.mxChanIndex);
		printf("    mxXconCardIndex:.......... %u\n",
		       abuf.vals.xcon.config.mxXconCardIndex);
		printf("    mxXconSpanIndex:.......... %u\n",
		       abuf.vals.xcon.config.mxXconSpanIndex);
		printf("    mxXconChanIndex:.......... %u\n",
		       abuf.vals.xcon.config.mxXconChanIndex);
		printf("  Option:\n");
		printf("    mxXconType:............... ");
		switch (abuf.vals.xcon.option.mxXconType) {
		case 0:
			printf("none");
			break;
		case MXXCONTYPE_SWITCHED:
			printf("switched");
			break;
		case MXXCONTYPE_SEMIPERMANENT:
			printf("semi-permanent");
			break;
		case MXXCONTYPE_PERMANENT:
			printf("permanent");
			break;
		default:
			printf("0x%04x", abuf.vals.xcon.option.mxXconType);
			break;
		}
		printf("\n");
		printf("  Inform:\n");
		printf("  Statem:\n");
		printf("  Status:\n");
		printf("  Stats:\n");
		printf("  Events:\n");
		printf("    events.................... <");
		sep[0] = '\0';
		if (abuf.vals.xcon.events.events & MX_SYNCTRANSITION) {
			printf("%s%s", sep, "sync");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_LOOPUP) {
			printf("%s%s", sep, "loop-up");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_LOOPDOWN) {
			printf("%s%s", sep, "loop-down");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_YELLOWALARM) {
			printf("%s%s", sep, "yel");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_YELLOWALARM_CLEARED) {
			printf("%s%s", sep, "~yel");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_REDALARM) {
			printf("%s%s", sep, "red");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_REDALARM_CLEARED) {
			printf("%s%s", sep, "~red");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_BLUEALARM) {
			printf("%s%s", sep, "blu");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_BLUEALARM_CLEARED) {
			printf("%s%s", sep, "~blu");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_TESTABORTED) {
			printf("%s%s", sep, "test-aborted");
			sep[0] = ',';
		}
		if (abuf.vals.xcon.events.events & MX_TESTCOMPLETE) {
			printf("%s%s", sep, "test-complete");
			sep[0] = ',';
		}
		printf(">\n");
	}
	if (output > 1)
		printf("I: closing %s\n", devname);
	close(fd);
}

void
mxconf_get_bert(void)
{
	struct mx_attr_buf abuf = { };
	struct mx_list_buf lbuf = { };
	struct strioctl ctl = { };
	int fd, ret, i;
	char sep[2] = { 0, 0 };

	if (output > 1)
		printf("I: opening %s\n", devname);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	lbuf.conf.type = MX_OBJ_TYPE_BERT;
	lbuf.conf.id = 0;
	lbuf.conf.cmd = MX_GET;
	ctl.ic_cmd = MX_IOCLCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lbuf);
	if (output > 1)
		printf("I: issuing MX_IOCLCONFIG input-output control\n");
	if ((ret = ioctl(fd, I_STR, &ctl)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output > 1)
		printf("I: got %d MX_OBJ_TYPE_BERT objects\n", ret);
	for (i = 0; i < ret; i++) {
		abuf.attr.type = MX_OBJ_TYPE_BERT;
		abuf.attr.id = lbuf.idxs[i];
		abuf.attr.cmd = MX_GET;
		ctl.ic_cmd = MX_IOCGATTR;
		ctl.ic_timout = 0;
		ctl.ic_len = sizeof(abuf);
		ctl.ic_dp = (char *) &abuf;
		if (output > 1)
			printf("I: issuing MX_IOCGATTR input-output control, PPA = 0x%04x\n", lbuf.idxs[i]);
		if (ioctl(fd, I_STR, &ctl) < 0) {
			perror(__FUNCTION__);
			exit(1);
		}
		printf("Bert: 0x%04x\n", lbuf.idxs[i]);
		printf("  Config:\n");
		printf("    mxCardIndex:.............. %u\n", abuf.vals.bert.config.mxCardIndex);
		printf("    mxSpanIndex:.............. %u\n", abuf.vals.bert.config.mxSpanIndex);
		printf("  Option:\n");
		printf("    mxBertMode:............... ");
		switch (abuf.vals.bert.option.mxBertMode) {
		case MXBERTMODE_NONE:
			printf("none");
			break;
		case MXBERTMODE_SPAN:
			printf("span");
			break;
		case MXBERTMODE_CHANNEL:
			printf("channel");
			break;
		case MXBERTMODE_FBIT:
			printf("f-bit");
			break;
		default:
			printf("0x%04x", abuf.vals.bert.option.mxBertMode);
			break;
		}
		printf("\n");
		printf("    mxBertSelect:............. ");
		switch (abuf.vals.bert.option.mxBertSelect) {
		case MXBERTSELECT_NONE:
			printf("none");
			break;
		case MXBERTSELECT_PRE2E7MINUS1:
			printf("pre-2E7-1");
			break;
		case MXBERTSELECT_PRE2E9MINUS1:
			printf("pre-2E9-1");
			break;
		case MXBERTSELECT_PRE2E11MINUS1:
			printf("pre-2E11-1");
			break;
		case MXBERTSELECT_PRE2E15MINUS1:
			printf("pre-2E15-1");
			break;
		case MXBERTSELECT_QRSS:
			printf("qrss");
			break;
		case MXBERTSELECT_DALY:
			printf("daly");
			break;
		case MXBERTSELECT_REPEATING:
			printf("repeating");
			break;
		case MXBERTSELECT_ALTERNATING:
			printf("alternating");
			break;
		default:
			printf("0x%04x", abuf.vals.bert.option.mxBertSelect);
			break;
		}
		printf("\n");
		printf("    mxBertPatternLen:......... %u\n",
		       abuf.vals.bert.option.mxBertPatternLen);
		printf("    mxBertPattern:............ ");
		{
			int j;

			/* FIXME FIXME: this is basically wrong wrong wrong: see OPENSS7-MX-MIB */
			sep[0] = '\0';
			for (j = 0; j < abuf.vals.bert.option.mxBertPatternLen && j < 32; j++) {
				printf("%s%02X", sep, abuf.vals.bert.option.mxBertPattern[j]);
				sep[0] = ' ';
			}
		}
		printf("\n");
		printf("  Inform:\n");
		printf("  Statem:\n");
		printf("  Status:\n");
		printf("    mxBertOperationalState:... ");
		switch (abuf.vals.bert.status.mxBertOperationalState) {
		case X721_OPERATIONALSTATE_DISABLED:
			printf("disabled");
			break;
		case X721_OPERATIONALSTATE_ENABLED:
			printf("enabled");
			break;
		default:
			printf("0x%04x", abuf.vals.bert.status.mxBertOperationalState);
			break;
		}
		printf("\n");
		printf("    mxBertProceduralStatus:... <");
		sep[0] = '\0';
		if (abuf.vals.bert.status.mxBertProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZATIONREQUIRED)) {
			printf("%s%s", sep, "initialization-required");
			sep[0] = ',';
		}
		if (abuf.vals.bert.status.mxBertProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_NOTINITIALIZED)) {
			printf("%s%s", sep, "not-initialized");
			sep[0] = ',';
		}
		if (abuf.vals.bert.status.mxBertProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_INITIALIZING)) {
			printf("%s%s", sep, "initializing");
			sep[0] = ',';
		}
		if (abuf.vals.bert.status.mxBertProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_REPORTING)) {
			printf("%s%s", sep, "reporting");
			sep[0] = ',';
		}
		if (abuf.vals.bert.status.mxBertProceduralStatus
		    & (1 << X721_PROCEDURALSTATUS_TERMINATING)) {
			printf("%s%s", sep, "terminating");
			sep[0] = ',';
		}
		printf(">\n");
		printf("  Stats:\n");
		printf("    mxBertBitCount:........... %u\n", abuf.vals.bert.stats.mxBertBitCount);
		printf("    mxBertErrorCount:......... %u\n",
		       abuf.vals.bert.stats.mxBertErrorCount);
		printf("  Events:\n");
		printf("    events.................... <");
		printf(">\n");
	}
	if (output > 1)
		printf("I: closing %s\n", devname);
	close(fd);
}

void
mxconf_get(int what)
{
	switch (what) {
	case MX_OBJ_TYPE_DFLT:
		return mxconf_get_dflt();
	case MX_OBJ_TYPE_SYNC:
		return mxconf_get_sync();
	case MX_OBJ_TYPE_CARD:
		return mxconf_get_card();
	case MX_OBJ_TYPE_SPAN:
		return mxconf_get_span();
	case MX_OBJ_TYPE_CHAN:
		return mxconf_get_chan();
	case MX_OBJ_TYPE_FRAC:
		return mxconf_get_frac();
	case MX_OBJ_TYPE_XCON:
		return mxconf_get_xcon();
	case MX_OBJ_TYPE_BERT:
		return mxconf_get_bert();
	}
}

void
mxconf_set(void)
{
}

int
main(int argc, char **argv)
{
	int c;
	int val;
	enum {
		CMD_NONE = 0,
		CMD_GET,	/* get device */
		CMD_SET,	/* set device */
	} cmd = CMD_NONE;
	int what = MX_OBJ_TYPE_DFLT;

	my_argc = argc;
	my_argv = argv;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"get",		no_argument,		NULL,	'g'},
			{"set",		no_argument,		NULL,	's'},
			{"device",	optional_argument,	NULL,	'd'},
			{"card",	optional_argument,	NULL,	'c'},
			{"span",	optional_argument,	NULL,	'p'},
			{"slot",	optional_argument,	NULL,	't'},
			{"sync",	optional_argument,	NULL,	'y'},
			{"quiet",	no_argument,		NULL,	'q'},
			{"verbose",	optional_argument,	NULL,	'v'},
			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copying",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'h'},
			{NULL,		0,			NULL,	 0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "gsd::c::p::t::y::qvhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'g':	/* -g, --get */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_GET;
			break;
		case 's':	/* -s, --set */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_SET;
			break;
		case 'd':	/* -d, --device */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			if (optarg != NULL)
				strncpy(devname, optarg, sizeof(devname) - 1);
			break;
		case 'c':	/* -c, --card */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			if (optarg != NULL)
				card = atoi(optarg);
			if (what < MX_OBJ_TYPE_CARD)
				what = MX_OBJ_TYPE_CARD;
			break;
		case 'p':	/* -p, --span */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			if (optarg != NULL)
				span = atoi(optarg);
			if (what < MX_OBJ_TYPE_SPAN)
				what = MX_OBJ_TYPE_SPAN;
			break;
		case 't':	/* -t, --slot */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			if (what != MX_OBJ_TYPE_DFLT)
				goto bad_option;
			if (optarg != NULL)
				slot = atoi(optarg);
			if (what < MX_OBJ_TYPE_CHAN)
				what = MX_OBJ_TYPE_CHAN;
			break;
		case 'y':	/* -y, --sync */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			if (what != MX_OBJ_TYPE_DFLT)
				goto bad_option;
			if (optarg != NULL)
				syng = atoi(optarg);
			what = MX_OBJ_TYPE_SYNC;
			break;
		case 'q':	/* -q, --quiet */
			output -= output > 0 ? 1 : output;
			break;
		case 'v':	/* -v, --verbose */
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
			if (cmd != CMD_NONE)
				goto bad_option;
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (cmd != CMD_NONE)
				goto bad_option;
			version(argc, argv);
			exit(0);
		case 'C':
			if (cmd != CMD_NONE)
				goto bad_option;
			copying(argc, argv);
			exit(0);
		case ':':
			fprintf(stderr, "%s: missing parm -- %s\n", argv[0], argv[optind - 1]);
			usage(argc, argv);
			exit(2);
		case '?':
		default:
		      bad_option:
			optind--;
			// syntax_error:
			if (optind < argc) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	switch (cmd) {
	default:
	case CMD_NONE:
		help(argc, argv);
		exit(0);
	case CMD_GET:
		mxconf_get(what);
		exit(0);
	case CMD_SET:
		mxconf_set();
		exit(0);
	}
	exit(4);
}
