/*****************************************************************************

 @(#) $RCSfile: usage.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-30 14:05:06 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-04-30 14:05:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: usage.c,v $
 Revision 0.9.2.5  2008-04-30 14:05:06  brian
 - updates to pass make check target

 Revision 0.9.2.4  2008-04-29 07:11:33  brian
 - updating headers for release

 Revision 0.9.2.3  2008/01/15 17:53:34  brian
 - add _GNU_SOURCE for independent compile

 Revision 0.9.2.2  2008/01/14 18:04:54  brian
 - update tests and MIBS

 Revision 0.9.2.1  2008/01/13 00:10:21  brian
 - example program for SNMP usage metering

 *****************************************************************************/

#ident "@(#) $RCSfile: usage.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-30 14:05:06 $"

static char const ident[] = "$RCSfile: usage.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-30 14:05:06 $";

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define MSG_LEN 32

#define HOST_BUF_LEN 256

static int debug = 0;
static int verbose = 1;

/* This is a simple example of a trap agent for collecting billing
 * information. */

#define ASN_BOOLEAN		0x01
#define ASN_INTEGER		0x02
#define ASN_BIT_STR		0x03
#define ASN_OCTET_STR		0x04
#define ASN_NULL		0x05
#define ASN_OBJECT_ID		0x06

#define ASN_SEQUENCE		(ASN_CONSTRUCTOR|0x10)
#define ASN_SET			(ASN_CONSTRUCTOR|0x11)

#define ASN_UNIVERSAL		0x00
#define ASN_APPLICATION		0x40
#define ASN_CONTEXT		0x80
#define ASN_PRIVATE		0xc0

#define ASN_PRIMITIVE		0x00
#define ASN_CONSTRUCTOR		0x20

#define ASN_EXTENSION_ID	0x1f

#define ASN_IPADDRESS		(ASN_APPLICATION|0x0)
#define ASN_COUNTER		(ASN_APPLICATION|0x1)
#define ASN_GAUGE		(ASN_APPLICATION|0x2)
#define ASN_UNSIGNED		(ASN_APPLICATION|0x2)
#define ASN_TIMETICKS		(ASN_APPLICATION|0x3)
#define ASN_OPAQUE		(ASN_APPLICATION|0x4)
#define ASN_NSAP		(ASN_APPLICATION|0x5)
#define ASN_COUNTER64		(ASN_APPLICATION|0x6)
#define ASN_UINTEGER		(ASN_APPLICATION|0x7)
#define ASN_FLOAT		(ASN_APPLICATION|0x8)
#define ASN_DOUBLE		(ASN_APPLICATION|0x9)
#define ASN_INTEGER64		(ASN_APPLICATION|0xa)
#define ASN_UNSIGNED64		(ASN_APPLICATION|0xb)

#define SNMP_MSG_GET		(ASN_CONTEXT|ASN_CONSTRUCTOR|0x0)
#define SNMP_MSG_GETNEXT	(ASN_CONTEXT|ASN_CONSTRUCTOR|0x1)
#define SNMP_MSG_RESPONSE	(ASN_CONTEXT|ASN_CONSTRUCTOR|0x2)
#define SNMP_MSG_SET		(ASN_CONTEXT|ASN_CONSTRUCTOR|0x3)
#define SNMP_MSG_TRAP		(ASN_CONTEXT|ASN_CONSTRUCTOR|0x4)
#define SNMP_MSG_GETBULK	(ASN_CONTEXT|ASN_CONSTRUCTOR|0x5)
#define SNMP_MSG_INFORM		(ASN_CONTEXT|ASN_CONSTRUCTOR|0x6)
#define SNMP_MSG_TRAPV2		(ASN_CONTEXT|ASN_CONSTRUCTOR|0x7)
#define SNMP_MSG_REPORT		(ASN_CONTEXT|ASN_CONSTRUCTOR|0x8)

/* There are 66 varbinds.  Each varbind requires 2 or 3 bytes of sequence tag
 * and length, 2 bytes of OID tag an length, 12 bytes of OID value, 2 bytes of
 * value tag and length and then the value payload.  So the overhead (above
 * the value payload) is 2+2+12+2 = 18 bytes of overhead per varbind.  With 66
 * varbinds, there is 18 x 66 = 1188 bytes of overhead.  Also note that not
 * all 66 varbinds are necessary.  Usually only about 20 varbinds are
 * necessary, meaning an overhead of 19 x 20 = 360 bytes. */

/* 1.3.6.1.4.1.29591.17.825 */
#define OID_cdr2MIB (unsigned char *) "\x2b\x06\x01\x04\x01\x81\xe7\x17\x11\x86\x39"
#define LEN_cdr2MIB 11
/* 2.1.11.1.1 */
#define OID_callDetailRecordLogR2Entry OID_cdr2MIB "\x02\x01\x0b\x01\x01"
#define LEN_callDetailRecordLogR2Entry LEN_cdr2MIB + 5
/* 2.0 */
#define OID_cdr2Notifications OID_cdr2MIB "\x02\00"
#define LEN_cdr2Notifications LEN_cdr2MIB + 2
/* 2.1.1 */
#define OID_cdr2Scalars OID_cdr2MIB "\x02\x01\x01"
#define LEN_cdr2Scalars LEN_cdr2MIB + 3

struct varbind_list {
	unsigned char *oid;
	size_t oidlen;
	bool required;
	unsigned char type;
	char *name;
	int index;
	unsigned char *pointer;
	unsigned char *endpointer;
};

static struct varbind_list vars[] = {
	/* *INDENT-OFF* */
	{ OID_cdr2Scalars "\x01\x00",			 16, false, ASN_OCTET_STR, "fileName",		  0},
	{ OID_cdr2Scalars "\x02\x00",			 16, false, ASN_INTEGER,   "reasonForOutput",	  0},
	{ OID_cdr2Scalars "\x03\x00",			 16, false, ASN_INTEGER,   "sequenceNumber",	  0},
	{ OID_cdr2Scalars "\x04\x00",			 16, false, ASN_OPAQUE,    "callRecord",	  0},
	{ OID_callDetailRecordLogR2Entry "\x01\x00",	 18,  true, ASN_COUNTER64, "sequenceId",	  1},
	{ OID_callDetailRecordLogR2Entry "\x02\x00",	 18,  true, ASN_COUNTER64, "zoneId",		  2},
	{ OID_callDetailRecordLogR2Entry "\x03\x00",	 18,  true, ASN_COUNTER64, "recSeqId",		  3},
	{ OID_callDetailRecordLogR2Entry "\x04\x00",	 18,  true, ASN_COUNTER64, "fileSeqId",		  4},
	{ OID_callDetailRecordLogR2Entry "\x05\x00",	 18,  true, ASN_OCTET_STR, "callId",		  5},
	{ OID_callDetailRecordLogR2Entry "\x06\x00",	 18,  true, ASN_OCTET_STR, "lastUpdate",	  6},
	{ OID_callDetailRecordLogR2Entry "\x07\x00",	 18,  true, ASN_OCTET_STR, "eventOrder",	  7},
	{ OID_callDetailRecordLogR2Entry "\x08\x00",	 18,  true, ASN_INTEGER,   "status",		  8},
	{ OID_callDetailRecordLogR2Entry "\x09\x00",	 18, false, ASN_INTEGER,   "callType",		  9},
	{ OID_callDetailRecordLogR2Entry "\x0a\x00",	 18, false, ASN_OCTET_STR, "clngPtyNbr",	 10},
	{ OID_callDetailRecordLogR2Entry "\x0b\x00",	 18, false, ASN_OCTET_STR, "chargeNbr",		 11},
	{ OID_callDetailRecordLogR2Entry "\x0c\x00",	 18, false, ASN_OCTET_STR, "clldPtyNbr",	 12},
	{ OID_callDetailRecordLogR2Entry "\x0d\x00",	 18, false, ASN_INTEGER,   "clldPtyNoa",	 13},
	{ OID_callDetailRecordLogR2Entry "\x0e\x00",	 18, false, ASN_OCTET_STR, "origLnInfo",	 14},
	{ OID_callDetailRecordLogR2Entry "\x0f\x00",	 18, false, ASN_OCTET_STR, "ingLrn",		 15},
	{ OID_callDetailRecordLogR2Entry "\x10\x00",	 18, false, ASN_OCTET_STR, "ingCic",		 16},
	{ OID_callDetailRecordLogR2Entry "\x11\x00",	 18, false, ASN_INTEGER,   "ingCsi",		 17},
	{ OID_callDetailRecordLogR2Entry "\x12\x00",	 18, false, ASN_UINTEGER,  "ingCceId",		 18},
	{ OID_callDetailRecordLogR2Entry "\x13\x00",	 18, false, ASN_INTEGER,   "ingTgProt",		 19},
	{ OID_callDetailRecordLogR2Entry "\x14\x00",	 18, false, ASN_INTEGER,   "ingTgType",		 20},
	{ OID_callDetailRecordLogR2Entry "\x15\x00",	 18, false, ASN_UINTEGER,  "ingTgId",		 21},
	{ OID_callDetailRecordLogR2Entry "\x16\x00",	 18, false, ASN_OCTET_STR, "ingCallStart",	 22},
	{ OID_callDetailRecordLogR2Entry "\x17\x00",	 18, false, ASN_UINTEGER,  "ingGateId",		 23},
	{ OID_callDetailRecordLogR2Entry "\x18\x00",	 18, false, ASN_UINTEGER,  "ingCardId",		 24},
	{ OID_callDetailRecordLogR2Entry "\x19\x00",	 18, false, ASN_UINTEGER,  "ingSpanId",		 25},
	{ OID_callDetailRecordLogR2Entry "\x1a\x00",	 18, false, ASN_UINTEGER,  "ingChanId",		 26},
	{ OID_callDetailRecordLogR2Entry "\x1b\x00",	 18, false, ASN_UINTEGER,  "ingIsdnDChan",	 27},
	{ OID_callDetailRecordLogR2Entry "\x1c\x00",	 18, false, ASN_UINTEGER,  "ingIsdnCrn",	 28},
	{ OID_callDetailRecordLogR2Entry "\x1d\x00",	 18, false, ASN_OCTET_STR, "ingCreatConnCplt",	 29},
	{ OID_callDetailRecordLogR2Entry "\x1e\x00",	 18, false, ASN_OCTET_STR, "ingAddrCplt",	 30},
	{ OID_callDetailRecordLogR2Entry "\x1f\x00",	 18, false, ASN_OCTET_STR, "ingCallAns",	 31},
	{ OID_callDetailRecordLogR2Entry "\x20\x00",	 18, false, ASN_OCTET_STR, "transNbr",		 32},
	{ OID_callDetailRecordLogR2Entry "\x21\x00",	 18, false, ASN_OCTET_STR, "termLrn",		 33},
	{ OID_callDetailRecordLogR2Entry "\x22\x00",	 18, false, ASN_OCTET_STR, "transCic",		 34},
	{ OID_callDetailRecordLogR2Entry "\x23\x00",	 18, false, ASN_OCTET_STR, "ingCallRls",	 35},
	{ OID_callDetailRecordLogR2Entry "\x24\x00",	 18, false, ASN_UINTEGER,  "ingRlsCause",	 36},
	{ OID_callDetailRecordLogR2Entry "\x25\x00",	 18, false, ASN_UINTEGER,  "egrCceId",		 37},
	{ OID_callDetailRecordLogR2Entry "\x26\x00",	 18, false, ASN_INTEGER,   "egrTgProt",		 38},
	{ OID_callDetailRecordLogR2Entry "\x27\x00",	 18, false, ASN_INTEGER,   "egrTgType",		 39},
	{ OID_callDetailRecordLogR2Entry "\x28\x00",	 18, false, ASN_UINTEGER,  "egrTgId",		 40},
	{ OID_callDetailRecordLogR2Entry "\x29\x00",	 18, false, ASN_OCTET_STR, "egrCallStart",	 41},
	{ OID_callDetailRecordLogR2Entry "\x2a\x00",	 18, false, ASN_UINTEGER,  "egrGateId",		 42},
	{ OID_callDetailRecordLogR2Entry "\x2b\x00",	 18, false, ASN_UINTEGER,  "egrCardId",		 43},
	{ OID_callDetailRecordLogR2Entry "\x2c\x00",	 18, false, ASN_UINTEGER,  "egrSpanId",		 44},
	{ OID_callDetailRecordLogR2Entry "\x2d\x00",	 18, false, ASN_UINTEGER,  "egrChanId",		 45},
	{ OID_callDetailRecordLogR2Entry "\x2e\x00",	 18, false, ASN_UINTEGER,  "egrIsdnDChan",	 46},
	{ OID_callDetailRecordLogR2Entry "\x2f\x00",	 18, false, ASN_UINTEGER,  "egrIsdnCrn",	 47},
	{ OID_callDetailRecordLogR2Entry "\x30\x00",	 18, false, ASN_OCTET_STR, "egrCreatConnCplt",	 48},
	{ OID_callDetailRecordLogR2Entry "\x31\x00",	 18, false, ASN_OCTET_STR, "egrAddrCplt",	 49},
	{ OID_callDetailRecordLogR2Entry "\x32\x00",	 18, false, ASN_OCTET_STR, "egrCallAns",	 50},
	{ OID_callDetailRecordLogR2Entry "\x33\x00",	 18, false, ASN_OCTET_STR, "egrCallRls",	 51},
	{ OID_callDetailRecordLogR2Entry "\x34\x00",	 18, false, ASN_UINTEGER,  "egrRlsCause",	 52},
	{ OID_callDetailRecordLogR2Entry "\x35\x00",	 18, false, ASN_OCTET_STR, "chkptFirst",	 53},
	{ OID_callDetailRecordLogR2Entry "\x36\x00",	 18, false, ASN_OCTET_STR, "chkptLast",		 54},
	{ OID_callDetailRecordLogR2Entry "\x37\x00",	 18, false, ASN_OCTET_STR, "ingGateName",	 55},
	{ OID_callDetailRecordLogR2Entry "\x38\x00",	 18, false, ASN_OCTET_STR, "egrGateName",	 56},
	{ OID_callDetailRecordLogR2Entry "\x39\x00",	 18, false, ASN_OCTET_STR, "ingTgName",		 57},
	{ OID_callDetailRecordLogR2Entry "\x3a\x00",	 18, false, ASN_OCTET_STR, "egrTgName",		 58},
	{ OID_callDetailRecordLogR2Entry "\x3b\x00",	 18, false, ASN_OCTET_STR, "origGateIp",	 59},
	{ OID_callDetailRecordLogR2Entry "\x3c\x00",	 18, false, ASN_OCTET_STR, "termGateIp",	 60},
	{ OID_callDetailRecordLogR2Entry "\x3d\x00",	 18, false, ASN_OCTET_STR, "h323ConfId",	 61},
	{ OID_callDetailRecordLogR2Entry "\x3e\x00",	 18, false, ASN_UINTEGER,  "ingCardPort",	 62},
	{ OID_callDetailRecordLogR2Entry "\x3f\x00",	 18, false, ASN_UINTEGER,  "ingCardPath",	 63},
	{ OID_callDetailRecordLogR2Entry "\x40\x00",	 18, false, ASN_UINTEGER,  "egrCardPort",	 64},
	{ OID_callDetailRecordLogR2Entry "\x41\x00",	 18, false, ASN_UINTEGER,  "egrCardPath",	 65},
	{ OID_callDetailRecordLogR2Entry "\x42\x00",	 18, false, ASN_UINTEGER,  "ingTg",		 66},
	{ OID_callDetailRecordLogR2Entry "\x43\x00",	 18, false, ASN_UINTEGER,  "egrTg",		 67},
	{ OID_callDetailRecordLogR2Entry "\x44\x00",	 18, false, ASN_OCTET_STR, "dialedNbr",		 68},
	{ OID_callDetailRecordLogR2Entry "\x45\x00",	 18, false, ASN_INTEGER,   "dialedNoa",		 69},
	{ OID_callDetailRecordLogR2Entry "\x46\x00",	 18, false, ASN_OCTET_STR, "redirNbr",		 70},
	{ OID_callDetailRecordLogR2Entry "\x47\x00",	 18, false, ASN_INTEGER,   "redirNoa",		 71},
	{ OID_callDetailRecordLogR2Entry "\x48\x00",	 18, false, ASN_INTEGER,   "redirInd",		 72},
	{ OID_callDetailRecordLogR2Entry "\x49\x00",	 18, false, ASN_INTEGER,   "redirRsn",		 73},
	{ OID_callDetailRecordLogR2Entry "\x4a\x00",	 18, false, ASN_OCTET_STR, "calledNbr",		 74},
	{ OID_callDetailRecordLogR2Entry "\x4b\x00",	 18, false, ASN_INTEGER,   "calledNoa",		 75},
	{ OID_callDetailRecordLogR2Entry "\x4c\x00",	 18, false, ASN_INTEGER,   "redirRsnOrig",	 76},
	{ OID_callDetailRecordLogR2Entry "\x4d\x00",	 18, false, ASN_UINTEGER,  "redirCntr",		 77},
	{ OID_callDetailRecordLogR2Entry "\x4e\x00",	 18, false, ASN_INTEGER,   "redirPrsntnInd",	 78},
	{ OID_callDetailRecordLogR2Entry "\x4f\x00",	 18, false, ASN_OCTET_STR, "ingJuris",		 79},
	{ OID_callDetailRecordLogR2Entry "\x50\x00",	 18, false, ASN_OCTET_STR, "egrJuris",		 80},
	{ OID_callDetailRecordLogR2Entry "\x51\x00",	 18, false, ASN_INTEGER,   "ingTrunkBearCap",	 81},
	{ OID_callDetailRecordLogR2Entry "\x52\x00",	 18, false, ASN_INTEGER,   "egrTrunkBearCap",	 82},
	{ OID_callDetailRecordLogR2Entry "\x53\x00",	 18, false, ASN_OCTET_STR, "transNetSelCarr",	 83},
	{ OID_callDetailRecordLogR2Entry "\x54\x00",	 18, false, ASN_UINTEGER,  "clngPtyNoa",	 84},
	{ OID_callDetailRecordLogR2Entry "\x55\x00",	 18, false, ASN_OCTET_STR, "ingRlsCauseIntrnl",	 85},
	{ OID_callDetailRecordLogR2Entry "\x56\x00",	 18, false, ASN_OCTET_STR, "egrRlsCauseIntrnl",	 86},
	{ OID_callDetailRecordLogR2Entry "\x57\x00",	 18, false, ASN_OCTET_STR, "egrClldNbr",	 87},
	{ OID_callDetailRecordLogR2Entry "\x58\x00",	 18, false, ASN_INTEGER,   "egrClldNoa",	 88},
	{ OID_callDetailRecordLogR2Entry "\x59\x00",	 18, false, ASN_OCTET_STR, "egrCnndNbr",	 89},
	{ OID_callDetailRecordLogR2Entry "\x5a\x00",	 18, false, ASN_INTEGER,   "egrCnndNoa",	 90},
	{ OID_callDetailRecordLogR2Entry "\x5b\x00",	 18, false, ASN_INTEGER,   "clngPtyPrsntnInd",	 91},
	{ OID_callDetailRecordLogR2Entry "\x5c\x00",	 18, false, ASN_INTEGER,   "ingIri",		 92},
	{ OID_callDetailRecordLogR2Entry "\x5d\x00",	 18, false, ASN_INTEGER,   "egrOri",		 93},
	{ OID_callDetailRecordLogR2Entry "\x5e\x00",	 18, false, ASN_OCTET_STR, "ingCallidExtrnl",	 94},
	{ OID_callDetailRecordLogR2Entry "\x5f\x00",	 18, false, ASN_OCTET_STR, "egrCallidExtrnl",	 95},
	{ OID_callDetailRecordLogR2Entry "\x60\x00",	 18, false, ASN_OCTET_STR, "ingChargeInfo",	 96},
	{ OID_callDetailRecordLogR2Entry "\x61\x00",	 18, false, ASN_OCTET_STR, "egrChargeInfo",	 97},
	{ OID_callDetailRecordLogR2Entry "\x62\x00",	 18, false, ASN_OCTET_STR, "ingPrtlInd",	 98},
	{ OID_callDetailRecordLogR2Entry "\x63\x00",	 18, false, ASN_INTEGER,   "ingNatfwdCliblkInd", 99},
	{ OID_callDetailRecordLogR2Entry "\x64\x00",	 18, false, ASN_INTEGER,   "ingNatfwdNtaInd",	100},
	{ OID_callDetailRecordLogR2Entry "\x65\x00",	 18, false, ASN_OCTET_STR, "ingLastDvrtLnDigs",	101},
	{ OID_callDetailRecordLogR2Entry "\x66\x00",	 18, false, ASN_INTEGER,   "ingLastDvrtLnNoa",	102},
	{ OID_callDetailRecordLogR2Entry "\x67\x00",	 18, false, ASN_OCTET_STR, "j7IngAddlPtyCat",	103},
	{ OID_callDetailRecordLogR2Entry "\x68\x00",	 18, false, ASN_OCTET_STR, "j7IngChgAreaInfo",	104},
	{ OID_callDetailRecordLogR2Entry "\x69\x00",	 18, false, ASN_OCTET_STR, "j7IngFwdCallInd",	105},
	{ OID_callDetailRecordLogR2Entry "\x6a\x00",	 18, false, ASN_OCTET_STR, "j7EgrAddlPtyCat",	106},
	{ OID_callDetailRecordLogR2Entry "\x6b\x00",	 18, false, ASN_OCTET_STR, "j7EgrChgAreaInfo",	107},
	{ OID_callDetailRecordLogR2Entry "\x6c\x00",	 18, false, ASN_OCTET_STR, "j7EgrBkwCallInd",	108},
	{ OID_callDetailRecordLogR2Entry "\x6d\x00",	 18, false, ASN_OCTET_STR, "j7CarrInfoXfer",	109},
	{ OID_callDetailRecordLogR2Entry "\x6e\x00",	 18, false, ASN_OCTET_STR, "ingSs7GnrcParm",	110},
	{ OID_callDetailRecordLogR2Entry "\x6f\x00",	 18, false, ASN_OCTET_STR, "egrSs7GnrcParm",	111},
	{ OID_callDetailRecordLogR2Entry "\x70\x00",	 18, false, ASN_UINTEGER,  "ingPktsSent",	112},
	{ OID_callDetailRecordLogR2Entry "\x71\x00",	 18, false, ASN_UINTEGER,  "ingPktsRcvd",	113},
	{ OID_callDetailRecordLogR2Entry "\x72\x00",	 18, false, ASN_UINTEGER,  "ingPktsLost",	114},
	{ OID_callDetailRecordLogR2Entry "\x73\x00",	 18, false, ASN_UINTEGER,  "ingPktsXfer",	115},
	{ OID_callDetailRecordLogR2Entry "\x74\x00",	 18, false, ASN_UINTEGER,  "ingJitter",		116},
	{ OID_callDetailRecordLogR2Entry "\x75\x00",	 18, false, ASN_UINTEGER,  "ingLtncy",		117},
	{ OID_callDetailRecordLogR2Entry "\x76\x00",	 18, false, ASN_UINTEGER,  "egrPktsSent",	118},
	{ OID_callDetailRecordLogR2Entry "\x77\x00",	 18, false, ASN_UINTEGER,  "egrPktsRcvd",	119},
	{ OID_callDetailRecordLogR2Entry "\x78\x00",	 18, false, ASN_UINTEGER,  "egrPktsLost",	120},
	{ OID_callDetailRecordLogR2Entry "\x79\x00",	 18, false, ASN_UINTEGER,  "egrPktsXfer",	121},
	{ OID_callDetailRecordLogR2Entry "\x7a\x00",	 18, false, ASN_UINTEGER,  "egrJitter",		122},
	{ OID_callDetailRecordLogR2Entry "\x7b\x00",	 18, false, ASN_UINTEGER,  "egrLtncy",		123},
	{ OID_callDetailRecordLogR2Entry "\x7c\x00",	 18, false, ASN_UINTEGER,  "ingCrctId",		124},
	{ OID_callDetailRecordLogR2Entry "\x7d\x00",	 18, false, ASN_UINTEGER,  "egrCrctId",		125},
	{ OID_callDetailRecordLogR2Entry "\x7e\x00",	 18, false, ASN_UINTEGER,  "ingCodec",		126},
	{ OID_callDetailRecordLogR2Entry "\x7f\x00",	 18, false, ASN_UINTEGER,  "egrCodec",		127},
	{ OID_callDetailRecordLogR2Entry "\x81\x00\x00", 19, false, ASN_UINTEGER,  "ingLocGateId",	128},
	{ OID_callDetailRecordLogR2Entry "\x81\x01\x00", 19, false, ASN_UINTEGER,  "egrLocGateId",	129},
	{ OID_callDetailRecordLogR2Entry "\x81\x02\x00", 19, false, ASN_OCTET_STR, "ingCasCrctSzr",	130},
	{ OID_callDetailRecordLogR2Entry "\x81\x03\x00", 19, false, ASN_OCTET_STR, "egrCasCrctSzr",	131},
	{ OID_callDetailRecordLogR2Entry "\x81\x04\x00", 19, false, ASN_OCTET_STR, "ingZz",		132},
	{ OID_callDetailRecordLogR2Entry "\x81\x05\x00", 19, false, ASN_OCTET_STR, "egrZz",		133},
	{ OID_callDetailRecordLogR2Entry "\x81\x06\x00", 19, false, ASN_OCTET_STR, "ingCtryAddrType",	134},
	{ OID_callDetailRecordLogR2Entry "\x81\x07\x00", 19, false, ASN_OCTET_STR, "egrCtryAddrType",	135},
	{ OID_callDetailRecordLogR2Entry "\x81\x08\x00", 19, false, ASN_UINTEGER,  "ingPartition",	136},
	{ OID_callDetailRecordLogR2Entry "\x81\x09\x00", 19, false, ASN_UINTEGER,  "egrPartition",	137},
	{ OID_callDetailRecordLogR2Entry "\x81\x0a\x00", 19, false, ASN_UINTEGER,  "ingClngPtyCat",	138},
	{ OID_callDetailRecordLogR2Entry "\x81\x0b\x00", 19, false, ASN_UINTEGER,  "ingRlsCauseLctn",	139},
	{ OID_callDetailRecordLogR2Entry "\x81\x0c\x00", 19, false, ASN_UINTEGER,  "egrRlsCauseLctn",	140},
	{ OID_callDetailRecordLogR2Entry "\x81\x0d\x00", 19, false, ASN_UINTEGER,  "clldPtyCat",	141},
	{ OID_callDetailRecordLogR2Entry "\x81\x0e\x00", 19, false, ASN_UINTEGER,  "callDuration",	142},
	{ OID_callDetailRecordLogR2Entry "\x81\x0f\x00", 19, false, ASN_UINTEGER,  "ingRlsDrctn",	143},
	{ OID_callDetailRecordLogR2Entry "\x81\x00\x00", 19, false, ASN_UINTEGER,  "egrRlsDrctn",	144},
	{ OID_callDetailRecordLogR2Entry "\x81\x01\x00", 19, false, ASN_OCTET_STR, "ingAnsLctim",	145},
	{ OID_callDetailRecordLogR2Entry "\x81\x02\x00", 19, false, ASN_OCTET_STR, "egrAnsLctim",	146},
	{ OID_callDetailRecordLogR2Entry "\x81\x03\x00", 19, false, ASN_UINTEGER,  "ingHlcChrsId",	147},
	{ OID_callDetailRecordLogR2Entry "\x81\x04\x00", 19, false, ASN_UINTEGER,  "egrHlcChrsId",	148},
	{ OID_callDetailRecordLogR2Entry "\x81\x05\x00", 19, false, ASN_UINTEGER,  "ingLlcXferCap",	149},
	{ OID_callDetailRecordLogR2Entry "\x81\x06\x00", 19, false, ASN_UINTEGER,  "egrLlcXferCap",	150},
	{ OID_callDetailRecordLogR2Entry "\x81\x07\x00", 19, false, ASN_OCTET_STR, "routeList",		151},
	{ OID_callDetailRecordLogR2Entry "\x81\x08\x00", 19, false, ASN_UINTEGER,  "ingPointCode",	152},
	{ OID_callDetailRecordLogR2Entry "\x81\x09\x00", 19, false, ASN_UINTEGER,  "egrPointCode",	153},
	{ NULL, }
	/* *INDENT-ON* */
};

static struct varbind_list traps[] = {

	/* *INDENT-OFF* */
	{ OID_cdr2Notifications "\x01",			 14, false, ASN_NULL, "blockRecordNotification",        1},
	{ OID_cdr2Notifications "\x02",			 14, false, ASN_NULL, "callDetailRecordNotification",   2},
	{ OID_cdr2Notifications "\x03",			 14,  true, ASN_NULL, "fileCreationNotification",       3},
	{ OID_cdr2Notifications "\x04",			 14,  true, ASN_NULL, "blockRecordNotificationR2",      4},
	{ OID_cdr2Notifications "\x05",			 14,  true, ASN_NULL, "callDetailRecordNotificationR2", 5},
	{ NULL, }
	/* *INDENT-ON* */
};

#define TRAP_CALL_RECORD 4 /* index of call record trap above */

static void
fprint_octet_string(FILE *f, unsigned char *p, unsigned char *e)
{
	fprintf(f, "'");
	while (p < e)
		fprintf(f, "%02X", *p++);
	fprintf(f, "'X");
}

void
dump_pdu(FILE *f, unsigned char *p, unsigned char *e)
{
	fprintf(f, "D: ");
	fprint_octet_string(f, p, e);
	fprintf(f, "\n");
}

/** @brief Unpack tag and length from an ASN.1 BER type header.
  * @param e a pointer to a pointer to the end of the enveloping extent.
  * @param tag a pointer to an unsigned integer into which to receive the tag.
  * @param cls a pointer to an unsigned char into which to receive the class.
  *
  * This function unpacks the ASN.1 BER tag and class associated with an ASN.1
  * type.  It handles extended tags (up to 4 extension octets) and extended
  * lengths (up to 4 extension octets), and it does handle (to some degree)
  * indefinite lengths in that it does not change the end of the containing
  * envelope.
  *
  * When successful, zero (0) is returned and both the p and e pointers are
  * adjusted to the new interior envelope described by the header.  When
  * unsuccessful, minus one (-1) is returned and the p and e pointers, and the
  * tag and class values, are invalidated.
  *
  * This function is drawn from the OpenSS7 TCAP implementation.
  */
static int
unpack_taglen(unsigned char **p, unsigned char **e, uint *tag, unsigned char *cls)
{
	uint len, plen;

	if (*p >= *e) {
		fprintf(stderr, "E: expecting tag octet\n");
		goto error;
	}
#if 0
	if (debug) {
		fprintf(stderr, "D: unpacking tag from: ");
		dump_pdu(stderr, *p, *e);
	}
#endif
	*cls = *(*p)++;
	if ((*cls & 0x1f) != 0x1f) {	/* tag is not extended */
		*tag = (*cls & 0x1f);
	} else {
		if (debug)
			fprintf(stderr, "D: extended tag\n");
		for (plen = 5, *tag = 0;;) {
			if (*p >= *e) {
				fprintf(stderr, "E: expecting more tag octets\n");
				goto error;
			}
			*tag <<= 7;
			*tag |= (**p & 0x7f);
			if (!(*(*p)++ & 0x80))
				break;
			if (--plen == 0) {
				fprintf(stderr, "E: tag too big\n");
				goto error;
			}
		}
	}
	if (*p >= *e) {
		fprintf(stderr, "E: expecting length octet\n");
		goto error;
	}
	len = *(*p)++;
	if ((len & 0x80) != 0x00) {	/* extended length */
		if (debug)
			fprintf(stderr, "D: extended length\n");
		if ((plen = (len & 0x7f)) > 4) {
			fprintf(stderr, "E: length too big\n");
			goto error;
		}
		if (plen == 0)	{ /* indefinite form */
			if (debug)
				fprintf(stderr, "D: got indefinite form\n");
			goto indefinite;
		}
		len = 0;
		while (plen--) {
			if (*p >= *e) {
				fprintf(stderr, "E: expecting more length octets\n");
				goto error;
			}
			len <<= 8;
			len |= *(*p)++;
		}
		if (*p + len > *e) {
			fprintf(stderr, "E: length too great for envelope\n");
			fprintf(stderr, "E: length is %x, envelope is %lx\n", (uint)len, (ulong)(*e - *p));
			fprintf(stderr, "E: start %p, end %p\n", *p, *e);
			goto error;
		}
	}
	*e = *p + len;
      indefinite:
	*cls &= ~0x1f;
	return (0);
      error:
	return (-1);
}

/** @brief Unpack expected tag an class.
  * @param p a pointer to a pointer to the start of the envelope.
  * @param e a pointer to a pointer to the end of the envelope.
  * @param tag the expected tag.
  * @param cls the expected class.
  *
  * This function unpacks tag and class and compares them to an expected tag and
  * class.
  *
  * When successful (the header is syntactically correct and the extracted tag
  * and class match those specified), the function returns zero (0) and the p
  * and e pointers are adjusted to the start and end of the envelope enclosed by
  * header.
  *
  * When unsuccessful, the function returns minus one (-1) and the p and e
  * pointers are invalidated.
  */
static int
unpack_long_tag_and_class(unsigned char **p, unsigned char **e, uint tag, unsigned char cls)
{
	unsigned char mycls;
	uint mytag;

	if (unpack_taglen(p, e, &mytag, &mycls)) {
		fprintf(stderr, "E: could not extract tag and class\n");
		return (-1);
	}
	if ((mycls & 0xe0) == cls && (mytag == tag))
		return (0);
	fprintf(stderr, "E: got tag %x, expected %x\n", (uint) mytag, (uint) tag);
	fprintf(stderr, "E: got cls %x, expected %x\n", (uint) mycls, (uint) cls);
	dump_pdu(stderr, *p, *e);
	return (1);
}

/** @brief Unpack specified tag and class.
  * @param p a pointer to a pointer to the start of the envelope.
  * @param e a pointer to a pointer to the end of the envelope.
  * @param tagcls the expected tag and class OR'ed together.
  *
  * This function unpacks a tage and class and compares then to an expected
  * tag and class provided as a single octet.  This, of course, only works for
  * tags with values of 30 or less (i.e. where the tag and class fit into one
  * octet): which is true for most base types.
  *
  * The function returns minus (-1) if there was a problem with the syntax of
  * the tag, class or length; one (1) if the tag and class did not match that
  * expected; or zero (0) on success.
  */
static int
unpack_tagcls(unsigned char **p, unsigned char **e, unsigned char tagcls)
{
	uint newtag = (tagcls & 0x1f);
	uint newcls = (tagcls & 0xe0);

	return unpack_long_tag_and_class(p, e, newtag, newcls);
}

/** @brief Unpack a general integer.
  * @param p a pointer to the start of the envelope.
  * @param e a pointer to the end of the envelope.
  * @param val a pointer to a signed 64- or 128-bit integer to received the value.
  *
  * This function unpacks BER IMPLICIT encoded INTEGER content octsts as short
  * as 0 octets and as long as 8 octets.
  */
static int
unpack_integer(unsigned char *p, unsigned char *e, long long *val)
{
	uint len;

	if ((len = e - p) > sizeof(*val) || len < 0) {
		fprintf(stderr, "E: integer length (%u) out of bounds\n", len);
		return (-1);
	}
	for (*val = (len && (*p & 0x80)) ? -1 : 0; len > 0; len--) {
		*val <<= 8;
		*val |= *p++;
	}
	return (0);
}

/** @brief Unpack a general unsigned integer.
  * @param p a pointer to a pointer to the start of the envelope.
  * @param e a pointer to the end of the envelope.
  * @param val a pointer an unsigned 64- or 128-bit integer to receive the value.
  *
  * This function unpacks BER IMPLICIT encoded INTEGER content octets as short
  * as 0 octets and as long as 8 octets.
  */
static int
unpack_unsigned(unsigned char **p, unsigned char *e, unsigned long long *val)
{
	uint len;

	if ((len = e - *p) > sizeof(*val) || len < 0) {
		fprintf(stderr, "E: unsigned length (%u) out of bounds\n", len);
		return (-1);
	}
	for (*val = 0; len > 0; len--) {
		*val <<= 8;
		*val |= *(*p)++;
	}
	return (0);
}

/** @brief Unpack a boolean.
  * @param p a pointer to a pointer to the start of the envelope.
  * @param e a pointer to the end of the envelope.
  * @param val a pointer bool to receive the value.
  *
  * This function unpacks BER BOOLEAN content octets.
  */
static int
unpack_boolean(unsigned char **p, unsigned char *e, bool *val)
{
	uint len;

	(void) unpack_boolean;
	if ((len = e - *p) != 1) {
		fprintf(stderr, "E: boolean length (%u) out of bounds\n", len);
		return (-1);
	}
	*val = (*p == 0) ? false : true;
	return (0);
}

/** @brief Print an OID to an output stream.
  * @param f output stream to which to print.
  * @param p a pointer to the start of the envelope.
  * @param e a pointer to the end of the envelope.
  *
  * This function unpacks and displays a BER encoded OBJECT IDENTIFIER from a
  * set of contents octets.
  */
static void
fprint_oid(FILE *f, unsigned char *p, unsigned char *e)
{
	uint component = 0;

	if (p < e) {
		fprintf(f, "%u.%u", *p/40, *p - (*p/40)*40);
		if (p < e)
			fputc('.', f);
		p++;
	}
	while (p < e) {
		component <<= 7;
		component |= (*p & 0x7f);
		if (*p++ & 0x80)
			continue;
		fprintf(f, "%u", component);
		if (p < e)
			fputc('.', f);
		component = 0;
	}
}

/** @brief Print octets in decimal and dots notation.
  * @param f output stream to which to print.
  * @param p a pointer to the start of the envelope.
  * @param e a pointer to the end of the envelope.
  *
  * This function unpacks a displays a set of BER contents octets using
  * the decimal and dots notiation popularized by IP addresses.
  */
static void
fprint_decimal_and_dots(FILE *f, unsigned char *p, unsigned char *e)
{
	while (p < e) {
		fprintf(f, "%d", (int) (*p++));
		if (p < e)
			fputc('.', f);
	}
}

/** @brief Print invalid value indication.
  * @param f output stream to which to print.
  * @param p a pointer to the start of the envelope.
  * @param e a pointer to the end of the envelope.
  *
  * This function simply indicates invalid contents octets and then display
  * the actual contents octet as an octet string.
  */
static void
fprint_invalid(FILE *f, unsigned char *p, unsigned char *e)
{
	fprintf(f, "(invalid ");
	fprint_octet_string(f, p, e);
	fprintf(f, ")");
}

/** @brief Print octets as a bit string.
  * @param f output stream to which to print.
  * @param p a pointer to the start of the envelope.
  * @param e a pointer to the end of the envelope.
  *
  * For bit strings, the first octet in the contents octets represnts the
  * numbe of unused bits in the final contents octet and must have a value
  * between zero (0) and seven (7) inclusive.  The remaining contents octets
  * contain bits except the last contents octets which only has 8 - 'n' bits
  * where 'n' is the value of the first contents octet as described above.
  */
static void
fprint_bit_string(FILE *f, unsigned char *p, unsigned char *e)
{
	int b;
	unsigned char v, s;

	if (p >= e || (s = *p++) > 7) {
		fprint_invalid(f, p, e);
		return;
	}

	fprintf(f, "'");
	while (p < e)
		for (v = *p++, b = 8; b > 0 && (p != e || b > s); v >>= 1, b--)
			fprintf(f, "%d", (int) (v & 0x01));
	fprintf(f, "'B");
}

static void
fprint_visible_string(FILE *f, unsigned char *p, unsigned char *e)
{
	fprintf(f, "\"");
	while (p < e)
		if (isprint(*p) && !iscntrl(*p))
			fputc(*p++, f);
		else
			fprintf(f, "\\x%x", (uint) *p++);
	fprintf(f, "\"");
}

static void
fprint_visible_or_octet_string(FILE *f, unsigned char *p, unsigned char *e)
{
	bool printable;
	unsigned char *v;

	for (printable = true, v = p; v < e; v++)
		printable = printable && isprint(*p) && !iscntrl(*p) && *p >= 32;
	if (printable)
		fprint_visible_string(f, p, e);
	else
		fprint_octet_string(f, p, e);
}

static void
fprint_integer(FILE *f, unsigned char *p, unsigned char *e)
{
	long long value;

	if (p + sizeof(value) < e || unpack_integer(p, e, &value)) {
		/* too big */
		fprint_octet_string(f, p, e);
		return;
	}
	fprintf(f, "%lld", value);
}

static void
fprint_unsigned(FILE *f, unsigned char *p, unsigned char *e)
{
	unsigned long long value;

	if (p + sizeof(value) < e || unpack_unsigned(&p, e, &value)) {
		/* too big */
		fprint_octet_string(f, p, e);
		return;
	}
	fprintf(f, "%llu", value);
}

static void
fprint_boolean(FILE *f, unsigned char *p, unsigned char *e)
{
	/* should be one zero or non-zero octet */
	if (p != e - 1) {
		fprint_invalid(f, p, e);
		return;
	}
	else if (*p)
		fprintf(f, "TRUE");
	else
		fprintf(f, "FALSE");
}

static void
fprint_null(FILE *f, unsigned char *p, unsigned char *e)
{
	if (p < e) {
		fprint_invalid(f, p, e);
		return;
	}
	fprintf(f, "(null)");

}

static void
fprint_value(FILE *f, unsigned char *p, unsigned char *e, unsigned char tagcls)
{
	switch (tagcls) {
	case ASN_BOOLEAN:
		fprint_boolean(f, p, e);
		break;
	case ASN_INTEGER:
	case ASN_INTEGER64:
		fprint_integer(f, p, e);
		break;
	case ASN_BIT_STR:
		fprint_bit_string(f, p, e);
		break;
	case ASN_OCTET_STR:
		fprint_visible_or_octet_string(f, p, e);
		break;
	case ASN_NULL:
		fprint_null(f, p, e);
		break;
	case ASN_OBJECT_ID:
		fprint_oid(f, p, e);
		break;
	case ASN_IPADDRESS:
		fprint_decimal_and_dots(f, p, e);
		break;
	case ASN_COUNTER:
	// case ASN_GAUGE: /* same as ASN_UNSIGNED */
	case ASN_UNSIGNED:
	case ASN_TIMETICKS:
	case ASN_COUNTER64:
	case ASN_UINTEGER:
	case ASN_UNSIGNED64:
		fprint_unsigned(f, p, e);
		break;
	case ASN_NSAP:
		fprint_visible_string(f, p, e);
		break;
	case ASN_OPAQUE:
	case ASN_FLOAT:
	case ASN_DOUBLE:
	default:
		fprint_octet_string(f, p, e);
		break;
	}
}

static void
fprint_type(FILE *f, unsigned char tagcls)
{
	const char *s;

	switch (tagcls) {
	case ASN_BOOLEAN:
		s = "BOOLEAN";
		break;
	case ASN_INTEGER:
		s = "INTEGER";
		break;
	case ASN_INTEGER64:
		s = "Integer64";
		break;
	case ASN_BIT_STR:
		s = "BITS";
		break;
	case ASN_OCTET_STR:
		s = "OCTET STRING";
		break;
	case ASN_NULL:
		s = "NULL";
		break;
	case ASN_OBJECT_ID:
		s = "OBJECT IDENTIFIER";
		break;
	case ASN_IPADDRESS:
		s = "IPAddress";
		break;
	case ASN_COUNTER:
		s = "Counter32";
		break;
	// case ASN_GAUGE: /* same as ASN_UNSIGNED */
	case ASN_UNSIGNED:
		s = "Gauge32";
		break;
	case ASN_TIMETICKS:
		s = "TimeTicks";
		break;
	case ASN_COUNTER64:
		s = "Counter64";
		break;
	case ASN_UINTEGER:
		s = "Unsigned32";
		break;
	case ASN_UNSIGNED64:
		s = "Gauge64";
		break;
	case ASN_NSAP:
		s = "NSAP";
		break;
	case ASN_OPAQUE:
		s = "Opaque";
		break;
	case ASN_FLOAT:
		s = "Float";
		break;
	case ASN_DOUBLE:
		s = "Double";
		break;
	default:
		s = "(invalid)";
		break;
	}
	fprintf(f, "%-18s", s);
}

static int
process_record(bool corrupt)
{
	int var;

	for (var = 0; vars[var].oid != NULL; var++) {
		if (vars[var].index == 0 && vars[var].pointer == NULL)
			continue;
		/* print the tags if verbose */
		if (verbose > 1) {
			if (vars[var].index == 0)
				fprintf(stdout, "      ");
			else
				fprintf(stdout, "[%3d] ", vars[var].index);
		}
		fprintf(stdout, "%-20s ", vars[var].name);
		/* print the oid if verbose */
		if (verbose > 1) {
			fprint_value(stdout, vars[var].oid, vars[var].oid + vars[var].oidlen, ASN_OBJECT_ID);
			fputc(' ', stdout);
		}
		fprintf(stdout, "= ");
		if (vars[var].pointer == NULL) {
			if (vars[var].required)
				fprintf(stdout, "(missing)");
			else
				fprintf(stdout, "(absent)");
		} else
			fprint_value(stdout, vars[var].pointer, vars[var].endpointer,
				     vars[var].type);
		fprintf(stdout, "\n");
		vars[var].pointer = NULL;
		vars[var].endpointer = NULL;
	}
	return (0);
}

static void
clear_msg(void)
{
	int var;

	for (var = 0; vars[var].oid != NULL; var++) {
		vars[var].pointer = NULL;
		vars[var].endpointer = NULL;
	}
}

static int
parse_trap(unsigned char *p, unsigned char **e, int version)
{
	unsigned char *m = *e;
	int trap;

	if (unpack_tagcls(&p, &m, ASN_SEQUENCE)) {
		fprintf(stderr, "E: poorly formatted trap varbind\n");
		goto error;
	}
	*e = m;
	if (unpack_tagcls(&p, &m, ASN_OBJECT_ID)) {
		fprintf(stderr, "E: poorly formatted OID in trap varbind\n");
		goto error;
	}
	/* FIXME: might be different for other version, this works for version 2c. */
	/* SHMPv2-MIB:snmpTrap.0 1.3.6.1.6.3.1.1.4.1.0 */
	if (m - p != 10 || memcmp(p, "\x2b\x06\x01\x06\x03\x01\x01\x04\x01\x00", 10)) {
		fprintf(stderr, "E: incorrect OID for trap varbind\n");
		goto error;
	}
	p = m;
	m = *e;
	if (unpack_tagcls(&p, &m, ASN_OBJECT_ID)) {
		fprintf(stderr, "E: poorly formatted value in trap varbind\n");
		goto error;
	}
	if (m < *e) {
		fprintf(stderr, "W: %ld bytes follow varbind value\n", (long) (*e - m));
		goto error;
	}
	/* is this a trap that is known to us? */
	for (trap = 0; traps[trap].oid != NULL; trap++) {
		if (m - p != traps[trap].oidlen)
			continue;
		if (memcmp(p, traps[trap].oid, traps[trap].oidlen))
			continue;
		if (!traps[trap].required)
			continue;
		break;
	}
	if (traps[trap].oid == NULL) {
		fprintf(stderr, "E: trap oid (");
		fprint_value(stderr, p, m, ASN_OBJECT_ID);
		fprintf(stderr, ") is unknown to us\n");
		goto error;
	}
	return (trap);
      error:
	return (-1);
}

static int
parse_callrecord(unsigned char *p, unsigned char *e)
{
	unsigned char *m = e, cls;
	unsigned int tag;
	bool corrupt = false;
	int var;

	/* open up record */
	if (unpack_tagcls(&p, &m, ASN_SEQUENCE)) {
		fprintf(stderr, "E: no enveloping sequence in call record\n");
		goto error;
	}
	if (m < e)
		fprintf(stderr, "W: %ld bytes follow call record sequence\n", (long) (e - m));
	for (p = m, m = e; p < e; p = m, m = e) {
		if (unpack_taglen(&p, &m, &tag, &cls)) {
			fprintf(stderr, "E: illegal construct in call record: aborting\n");
			corrupt = true;
			break;
		}
		for (var = 0; vars[var].oid != NULL; var++)
			if (vars[var].index == tag)
				break;
		if (vars[var].oid == NULL) {
			fprintf(stderr, "E: unknown tag %u in record: skipping\n", tag);
			corrupt = true;
			continue;
		}
		vars[var].pointer = p;
		vars[var].endpointer = m;
		if (debug) {
			fprintf(stdout, "D: [%u] %s = ", var, vars[var].name);
			fprint_value(stdout, p, m, vars[var].type);
			fprintf(stdout, "\n");
		}
	}
	if (process_record(corrupt))
		goto error;
	return (0);
      error:
	return (-1);
}

static int
parse_varbind(unsigned char *p, unsigned char *e, int trap)
{
	unsigned char *m = e, cls;
	unsigned int tag;
	int var;

	if (unpack_tagcls(&p, &m, ASN_OBJECT_ID)) {
		/* now here's the nice thing about ASN.1 BER */
		fprintf(stderr, "Poorly consructed OID in varbind: skipping\n");
		goto error;
	}
	/* look for this varbind */
	for (var = 0; vars[var].oid != NULL; var++) {
		if (m - p != vars[var].oidlen)
			continue;
		if (memcmp(p, vars[var].oid, vars[var].oidlen))
			continue;
		break;
	}
	if (vars[var].oid == NULL) {
		/* didn't find it so ignore it */
		fprintf(stderr, "Unknown varbind (");
		fprint_value(stderr, p, m, ASN_OBJECT_ID);
		fprintf(stderr, ") in list (skipping)\n");
		p = e;
		return (0);
	}
	p = m;
	m = e;
	/* unpack the value */
	if (unpack_taglen(&p, &m, &tag, &cls)) {
		fprintf(stderr, "Badly constructed value in varbind: skipping\n");
		goto error;
	}
	if (m < e)
		fprintf(stderr, "W: %ld bytes follow varbind value\n", (long) (e - m));
	if ((tag | cls) != vars[var].type) {
		fprintf(stderr, "W: varbind %s has bad value type\n", vars[var].name);
		fprintf(stderr, "W: expecting %02x (", vars[var].type);
		fprint_type(stderr, vars[var].type);
		fprintf(stderr, "), got %02x (", (uint)(tag | cls));
		fprint_type(stderr, (tag | cls));
		fprintf(stderr, "): winging it\n");
	}
	vars[var].pointer = p;
	vars[var].endpointer = m;
	if ((tag | cls) == ASN_OPAQUE && trap == 0 && var == 3)
		if (parse_callrecord(p, m))
			goto error;
	return (0);
      error:
	return (-1);
}

static int
parse_varbinds(unsigned char *p, unsigned char *e, int trap)
{
	unsigned char *m = p;
	bool corrupt = false;

	for (p = m, m = e; p < e; p = m, m = e) {
		if (unpack_tagcls(&p, &m, ASN_SEQUENCE)) {
			fprintf(stderr, "E: poortly formated varbind sequence\n");
			goto error;
		}
		if (parse_varbind(p, m, trap)) {
			corrupt = true;
			continue;
		}
	}
	if (trap == TRAP_CALL_RECORD)
		if (process_record(corrupt))
			goto error;
	return (0);
      error:
	return (-1);
}

static int
parse_varbind_list(unsigned char *p, unsigned char *e, int version, int trap)
{
	unsigned char *m = e;

	if (unpack_tagcls(&p, &m, ASN_SEQUENCE)) {
		fprintf(stderr, "E: poorly formatted varbind list\n");
		goto error;
	}
	if (m < e) {
		fprintf(stderr, "W: %d bytes follow varbind list\n", (int) (e - m));
		e = m;
	}
	if (version != 0 && trap == -1) {
		if ((trap = parse_trap(p, &m, version)) == -1) {
			fprintf(stderr, "E: bad trap varbind in message\n");
			goto error;
		}
	}
	p = m;
	m = e;
	if (parse_varbinds(p, m, trap)) {
		fprintf(stderr, "E: bad varbinds in message\n");
		goto error;
	}
	return (0);
      error:
	return (-1);
}

static int
parse_v1_pdu(unsigned char *p, unsigned char *e, int version)
{
	long long value;
	unsigned char *m = e, *t;
	int oidlen, generic, trapnum, trap;

	/* pull out enterprise oid */
	if (unpack_tagcls(&p, &m, ASN_OBJECT_ID)) {
		fprintf(stderr, "E: poorly formatted enterprise oid syntax\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: entreprise-OID: ");
		fprint_value(stderr, p, m, ASN_OBJECT_ID);
		fprintf(stderr, "\n");
	}
	/* remember trap oid location */
	t = p;
	oidlen = m - p;
	p = m;
	m = e;
	/* pull out agent address */
	if (unpack_tagcls(&p, &m, ASN_IPADDRESS)) {
		fprintf(stderr, "E: poorly formatted agent IP address syntax\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: agent-address: ");
		fprint_value(stderr, p, m, ASN_IPADDRESS);
		fprintf(stderr, "\n");
	}
	p = m;
	m = e;
	/* pull out generic trap type */
	if (unpack_tagcls(&p, &m, ASN_INTEGER)) {
		fprintf(stderr, "E: poorly formatted generic trap number syntax\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: trap type: ");
		fprint_value(stderr, p, m, ASN_INTEGER);
		fprintf(stderr, "\n");
	}
	if (unpack_integer(p, m, &value)) {
		fprintf(stderr, "E: poorly formatted generic trap number value\n");
		goto error;
	}
	generic = value;
	p = m;
	m = e;
	/* pull out specific trap type */
	if (unpack_tagcls(&p, &m, ASN_INTEGER)) {
		fprintf(stderr, "E: poorly formatted specific trap number syntax\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: specific trap type: ");
		fprint_value(stderr, p, m, ASN_INTEGER);
		fprintf(stderr, "\n");
	}
	if (unpack_integer(p, m, &value)) {
		fprintf(stderr, "E: poorly formatted specific trap number value\n");
		goto error;
	}
	trapnum = value;
	p = m;
	m = e;
	/* pull out timestamp */
	if (unpack_tagcls(&p, &m, ASN_TIMETICKS)) {
		fprintf(stderr, "E: poorly formatted timestamp syntax\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: timestamp: ");
		fprint_value(stderr, p, m, ASN_TIMETICKS);
		fprintf(stderr, "\n");
	}
	/* look for this trap in the trap table */
	for (trap = 0; traps[trap].oid != NULL; trap++) {
		if (trapnum != traps[trap].index)
			continue;
		if (traps[trap].oidlen != oidlen + 2)
			continue;
		if (memcmp(t, traps[trap].oid, oidlen))
			continue;
		if (!traps[trap].required)
			continue;
	}
	if (traps[trap].oid == NULL) {
		fprintf(stderr, "E: trap oid (");
		fprint_value(stderr, t, t + oidlen, ASN_OBJECT_ID);
		fprintf(stderr, ".0.%d", trapnum);
		fprintf(stderr, ") is unknown\n");
		goto error;
	}
	p = m;
	m = e;
	if (parse_varbind_list(p, m, version, trap))
		goto error;
	return (0);
      error:
	return (-1);
}

static int
parse_v2_pdu(unsigned char *p, unsigned char *e, int version)
{
	long long value;
	unsigned char *m = e;
	int request_id, error_status, error_index;

	/* pull out request-id */
	if (unpack_tagcls(&p, &m, ASN_INTEGER)) {
		fprintf(stderr, "E: poorly formatted request-id syntax\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: request-id = ");
		fprint_value(stderr, p, m, ASN_INTEGER);
		fprintf(stderr, "\n");
	}
	if (unpack_integer(p, m, &value)) {
		fprintf(stderr, "E: poorly formatted request-id value\n");
		goto error;
	}
	request_id = value;
	p = m;
	m = e;
	/* pull out error-status */
	if (unpack_tagcls(&p, &m, ASN_INTEGER)) {
		fprintf(stderr, "E: poorly formatted error-status syntax\n");
		goto error;
	}
	if (unpack_integer(p, m, &value)) {
		fprintf(stderr, "E: poorly formatted error-status value\n");
		goto error;
	}
	error_status = value;
	p = m;
	m = e;
	/* pull out error-index */
	if (unpack_tagcls(&p, &m, ASN_INTEGER)) {
		fprintf(stderr, "E: poorly formatted error-index syntax\n");
		goto error;
	}
	if (unpack_integer(p, m, &value)) {
		fprintf(stderr, "E: poorly formatted error-index value\n");
		goto error;
	}
	error_index = value;
	p = m;
	m = e;
	if (parse_varbind_list(p, m, version, -1))
		goto error;
	return (0);
      error:
	return (-1);
}

static int
parse_pdu(unsigned char *p, unsigned char *e, int version)
{
	unsigned char *m = e, cls;
	unsigned int tag;

	if (unpack_taglen(&p, &m, &tag, &cls)) {
		fprintf(stderr, "E: badly formatted pdu\n");
		goto error;
	}
	if (cls != (ASN_CONTEXT | ASN_CONSTRUCTOR)) {
		fprintf(stderr, "E: badly formatted pdu\n");
		goto error;
	}
	if (m < e)
		fprintf(stderr, "W: %ld bytes follows pdu", (long) (e - m));
	switch ((tag | cls)) {
	case SNMP_MSG_TRAP:
		if (version != 0) {
			fprintf(stderr, "E: wrong version %d for snmpv1-trap pdu\n", version);
			goto error;
		}
		if (debug)
			fprintf(stderr, "I: received v1 trap\n");
		if (parse_v1_pdu(p, m, version))
			goto error;
		break;
	case SNMP_MSG_TRAPV2:
		if (version == 0) {
			fprintf(stderr, "E: wrong version %d for snmpv2-trap pdu\n", version);
			goto error;
		}
		if (debug)
			fprintf(stderr, "I: received v2 trap\n");
		if (parse_v2_pdu(p, m, version))
			goto error;
		break;
	case SNMP_MSG_INFORM:
		if (version == 0) {
			fprintf(stderr, "E: wrong version %d for inform-request pdu\n", version);
			goto error;
		}
		if (debug)
			fprintf(stderr, "I: received inform request\n");
		if (parse_v2_pdu(p, m, version))
			goto error;
		break;
	default:
		fprintf(stderr, "E: pdu type %u, should be %u, %u or %u\n", tag,
			(uint) SNMP_MSG_TRAP, (uint) SNMP_MSG_TRAPV2, (uint) SNMP_MSG_INFORM);
		goto error;
	}
	return (0);
      error:
	return (-1);
}

static int
parse_msg(unsigned char *p, unsigned char *e)
{
	unsigned char *m = e;
	long long value;
	int version;

	if (!p || p >= e) {
		fprintf(stderr, "E: invalid frame size\n");
		goto error;
	}
	/* open message sequence */
	if (unpack_tagcls(&p, &m, ASN_SEQUENCE)) {
		fprintf(stderr, "E: poorly formatted message sequence\n");
		goto error;
	}
	if (m < e) {
		fprintf(stderr, "W: %ld bytes follows message sequence", (long) (e - m));
		e = m;
	}
	/* pull out version - its an INTEGER */
	if (unpack_tagcls(&p, &m, ASN_INTEGER)) {
		fprintf(stderr, "E: poorly formatted version syntax\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: version = ");
		fprint_value(stderr, p, m, ASN_INTEGER);
		fprintf(stderr, "\n");
	}
	if (unpack_integer(p, m, &value)) {
		fprintf(stderr, "E: poorly formatted version value\n");
		goto error;
	}
	version = value;
	if (version < 0 || version > 3) {
		fprintf(stderr, "E: bad version %d, expecting 0, 1, 2 or 3\n", version);
		goto error;
	}
	p = m;
	m = e;
	/* pull out community string - its an OCTET STRING */
	if (unpack_tagcls(&p, &m, ASN_OCTET_STR)) {
		fprintf(stderr, "E: poorly formatted community string\n");
		goto error;
	}
	if (debug) {
		fprintf(stderr, "D: community = ");
		fprint_value(stderr, p, m, ASN_OCTET_STR);
		fprintf(stderr, "\n");
	}
	p = m;
	m = e;
	/* skip community string for now */
	if (parse_pdu(p, m, version)) {
		fprintf(stderr, "E: could not parse PDU\n");
		goto error;
	}
	return (0);
      error:
	return (-1);
}

static struct sockaddr_in loc_addr = { AF_INET, 0, {INADDR_ANY}, };
static struct sockaddr_in rem_addr = { AF_INET, 0, {INADDR_ANY}, };

static int
test_snmp(void)
{
	int fd;
	int len = 0;
	struct pollfd pfd[1] = { {0, POLLIN | POLLOUT | POLLERR | POLLHUP, 0} };

	unsigned char msg[1 << 16];

	fprintf(stderr, "Opening socket\n");

	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		goto dead;
	}
	fprintf(stderr, "Binding socket to %s:%d\n", inet_ntoa(loc_addr.sin_addr),
		ntohs(loc_addr.sin_port));

	if (bind(fd, (struct sockaddr *) &loc_addr, sizeof(loc_addr)) == -1) {
		perror("bind");
		goto dead;
	}

	for (;;) {
		pfd[0].fd = fd;
		pfd[0].events = POLLIN | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		if (poll(&pfd[0], 1, -1) < 0) {
			if (errno == EINTR)
				continue;
			perror("poll");
			goto dead;
		}
		if (pfd[0].revents & POLLERR) {
			perror("POLLERR");
			goto dead;
		}
		if (pfd[0].revents & POLLHUP) {
			perror("POLLHUP");
			goto dead;
		}
		if (pfd[0].revents & POLLIN) {
			socklen_t socklen = sizeof(rem_addr);

			if ((len = recvfrom(fd, msg, sizeof(msg), 0, (struct sockaddr *) &rem_addr,
					    &socklen)) == -1) {
				perror("recvfrom");
				goto dead;
			}
			if (len <= 0)
				continue;

			if (debug) {
				fprintf(stderr, "D: got message from %s:%d\n",
					inet_ntoa(rem_addr.sin_addr), ntohs(rem_addr.sin_port));
				dump_pdu(stderr, msg, msg + len);
			}

			if (parse_msg(msg, msg + len)) {
				fprintf(stderr, "E: problem processing packet\n");
				continue;
			}
			clear_msg();
			/* otherwise send it back */
			if (sendto(fd, msg, len, 0, (struct sockaddr *) &rem_addr, socklen)
			    == -1) {
				perror("sendto");
				goto dead;
			}
			continue;
		}
	}
      dead:
	close(fd);
	return (0);
}

void
dump(int argc, char *argv[])
{
	int var, trap;

	fprintf(stdout, "\nThe following VARBINDS are recognizd by %s:\n\n", argv[0]);
	for (var = 0; vars[var].oid != NULL; var++) {
		fprintf(stdout, "  [%3d] ", vars[var].index);
		fprintf(stdout, "%-20s", vars[var].name);
		fprint_type(stdout, vars[var].type);
		fputc(' ', stdout);
		fprint_value(stdout, vars[var].oid, vars[var].oid + vars[var].oidlen, ASN_OBJECT_ID);
		fprintf(stdout, "\n");
	}
	fprintf(stdout, "\nThe following TRAPS are recognizd by %s:\n\n", argv[0]);
	for (trap = 0; traps[trap].oid != NULL; trap++) {
		fprintf(stdout, "  [%3d] ", traps[trap].index);
		fprintf(stdout, "%-31s", traps[trap].name);
		fprint_value(stdout, traps[trap].oid, traps[trap].oid + traps[trap].oidlen, ASN_OBJECT_ID);
		fprintf(stdout, "\n");
	}
}

static void
splash(int argc, char *argv[])
{
	if (verbose <= 0)
		return;
	fprintf(stdout, "\
%1$s: UDP Performance Test Program\n\
%2$s\n\
\n\
Copyright (c) 2001-2008 OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001 Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Unauthorized distribution or duplication is prohibited.\n\
\n\
This software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompilation.\n\
No part of this software or related documentation may  be reproduced in any form\n\
by any means without the prior  written  authorization of the  copyright holder,\n\
and licensors, if any.\n\
\n\
The recipient of this document,  by its retention and use, warrants that the re-\n\
cipient  will protect this  information and  keep it confidential,  and will not\n\
disclose the information contained  in this document without the written permis-\n\
sion of its owner.\n\
\n\
The author reserves the right to revise  this software and documentation for any\n\
reason,  including but not limited to, conformity with standards  promulgated by\n\
various agencies, utilization of advances in the state of the technical arts, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   The author  is under no  obligation to\n\
provide any feature listed herein.\n\
\n\
As an exception to the above,  this software may be  distributed  under the  GNU\n\
Affero  General  Public  License  (AGPL)  Version  3, so long as the software is\n\
distributed with,  and only used for the testing of,  OpenSS7 modules,  drivers,\n\
and libraries.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the  U.S. Government  (\"Government\"),  the following provisions apply to you.\n\
If the Software is  supplied by the Department of Defense (\"DoD\"), it is classi-\n\
fied as  \"Commercial Computer Software\"  under paragraph 252.227-7014 of the DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-Government  users).  If\n\
the Software is supplied to any unit or agency of the Government other than DoD,\n\
it is classified as  \"Restricted Computer Software\" and the  Government's rights\n\
in the  Software are defined in  paragraph 52.227-19 of the Federal  Acquisition\n\
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
\n\
", argv[0], ident);
}

static void
version(int argc, char *argv[])
{
	if (verbose <= 0)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 3,\n\
    incorporated here by reference.\n\
", argv[0], ident);
}

static void
usage(int argc, char *argv[])
{
	if (verbose <= 0)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

#define DEFAULT_PORT 162

static void
help(int argc, char *argv[])
{
	if (verbose <= 0)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    -l, --loc_host\n\
        Local host (bind) address            [default: 0.0.0.0]\n\
    -p, --port PORTNUM\n\
        Remote port (connect) number         [default: %3$d]\n\
    -w, --length LENGTH\n\
        Length of message in bytes           [default: %2$d]\n\
    -n, --nagle\n\
        Suppress Nagle algorithm\n\
    -t, --rep_time INTERVAL\n\
        Sets the report time INTERVAL (secs) [default: 1]\n\
    -D, --dump\n\
        Print variable and trap tables and exit\n\
    -q, --quiet\n\
        Suppress normal output\n\
        (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        Increase verbosity or set to LEVEL   [default 1]\n\
        This option may be repeated\n\
    -d, --debug [LEVEL]\n\
        Increase debugging or set to LEVEL   [default 0]\n\
        This option may be repeated\n\
    -h, --help\n\
        Prints this usage message and exits\n\
    -V, --version\n\
        Prints the version and exits\n\
    -C, --copying\n\
	Prints copyright and copying information and exits\n\
", argv[0], MSG_LEN, DEFAULT_PORT);
}

int
main(int argc, char *argv[])
{
	char *host = "127.0.0.1";
	char hostbuf[BUFSIZ];
	char **hostp = &host;
	short port = DEFAULT_PORT;

	while (1) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"host",    required_argument,	    NULL, 'H'},
			{"port",    required_argument,	    NULL, 'p'},
			{"dump",    no_argument,	    NULL, 'D'},
			{"quiet",   no_argument,	    NULL, 'q'},
			{"verbose", optional_argument,	    NULL, 'v'},
			{"debug",   optional_argument,	    NULL, 'd'},
			{"help",    no_argument,	    NULL, 'h'},
			{"version", no_argument,	    NULL, 'V'},
			{"copying", no_argument,	    NULL, 'C'},
			{"?",	    no_argument,	    NULL, 'h'},
			{NULL,	    0,			    NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "H:p:DqvdhVC?", long_options, &option_index);
#else
		c = getopt(argc, argv, "H:p:DqvdhVC?");
#endif
		if (c == -1)
			break;
		switch (c) {
		case 'H':
			strncpy(hostbuf, optarg, BUFSIZ);
			host = hostbuf;
			hostp = &host;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'D':
			dump(argc, argv);
			exit(0);
		case 'q':
			verbose = 0;
			break;
		case 'v':
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 'd':
			if (optarg == NULL) {
				debug++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'h':
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
			exit(0);
		case 'C':
			splash(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				while (optind < argc)
					fprintf(stderr, "%s ", argv[optind++]);
				fprintf(stderr, "\n");
				fflush(stderr);
			}
			usage(argc, argv);
			exit(2);
		}
	}
	if (optind < argc)
		goto bad_nonopt;
	splash(argc, argv);
	loc_addr.sin_port = htons(port);
	test_snmp();
	exit(0);
}

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
