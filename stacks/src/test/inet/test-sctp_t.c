/*****************************************************************************

 @(#) $RCSfile: test-sctp_t.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/05/14 08:31:28 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written authorization
 of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that the
 recipient will protect this information and keep it confidential, and will
 not disclose the information contained in this document without the written
 permission of its owner.

 The author reserves the right to revise this software and documentation for
 any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of the
 technical arts, or the reflection of changes in the design of any techniques,
 or procedures embodied, described, or referred to herein.  The author is
 under no obligation to provide any feature listed herein.

 -----------------------------------------------------------------------------

 As an exception to the above, this software may be distributed under the GNU
 General Public License (GPL) Version 2 or later, so long as the software is
 distributed with, and only used for the testing of, OpenSS7 modules, drivers,
 and libraries.

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

 Last Modified $Date: 2005/05/14 08:31:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sctp_t.c,v $
 Revision 0.9.2.3  2005/05/14 08:31:28  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctp_t.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/05/14 08:31:28 $"

static char const ident[] = "$RCSfile: test-sctp_t.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/05/14 08:31:28 $";

/* 
 *  This file is for testing the sctp_t driver.  It is provided for the
 *  purpose of testing the OpenSS7 sctp_t driver only.
 */
#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#if 0
#include "../include/sys/tpi.h"
#include "../include/sys/tpi_sctp.h"
#include "../include/sys/xti_ip.h"
#include "../include/sys/xti_sctp.h"
#else
#include <sys/tpi.h>
#include <sys/tpi_sctp.h>
#include <sys/xti_ip.h>
#include <sys/xti_sctp.h>
#endif

#define BUFSIZE 5*4096
#define FFLUSH(stream)

int verbose = 1;

#define SHORT_WAIT 10
#define NORMAL_WAIT 100
#define LONG_WAIT 500

ulong seq[10] = { 0, };
ulong tok[10] = { 0, };
ulong tsn[10] = { 0, };
ulong sid[10] = { 0, };

union {
	union T_primitives tpi;
	char cbuf[BUFSIZE];
} cmd;

char dbuf[BUFSIZE];
struct strbuf ctrl = { BUFSIZE, 0, cmd.cbuf };
struct strbuf data = { BUFSIZE, 0, dbuf };

struct strfdinsert fdi = {
	{BUFSIZE, 0, cmd.cbuf},
	{BUFSIZE, 0, dbuf},
	0,
	0,
	0
};

typedef struct addr {
	uint16_t port __attribute__ ((packed));
	struct in_addr addr[3] __attribute__ ((packed));
} addr_t;

struct timeval when;

#define INCONCLUSIVE -2
#define FAILURE -1
#define SUCCESS 0

int show = 1;
int state;
int fd1, fd2, fd3;
addr_t addr1, addr2, addr3;

/* 
 *  Options
 */
/* data options */
struct {
	struct t_opthdr ppi_hdr __attribute__ ((packed));
	t_scalar_t ppi_val __attribute__ ((packed));
	struct t_opthdr sid_hdr __attribute__ ((packed));
	t_scalar_t sid_val __attribute__ ((packed));
} opt_data = {
	{
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS}
	, 10, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS}
, 0};

/* receive data options */
typedef struct rdat_opt {
	struct t_opthdr ppi_hdr __attribute__ ((packed));
	t_scalar_t ppi_val __attribute__ ((packed));
	struct t_opthdr sid_hdr __attribute__ ((packed));
	t_scalar_t sid_val __attribute__ ((packed));
	struct t_opthdr ssn_hdr __attribute__ ((packed));
	t_scalar_t ssn_val __attribute__ ((packed));
	struct t_opthdr tsn_hdr __attribute__ ((packed));
	t_scalar_t tsn_val __attribute__ ((packed));
} rdat_opt_t;
/* connect options */
struct {
	struct t_opthdr ist_hdr __attribute__ ((packed));
	t_scalar_t ist_val __attribute__ ((packed));
	struct t_opthdr ost_hdr __attribute__ ((packed));
	t_scalar_t ost_val __attribute__ ((packed));
} opt_conn = {
	{
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS,
		    T_SUCCESS}
	, 1, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS,
		    T_SUCCESS}
, 1};

/* management options */
struct {
	struct t_opthdr nod_hdr __attribute__ ((packed));
	t_scalar_t nod_val __attribute__ ((packed));
	struct t_opthdr crk_hdr __attribute__ ((packed));
	t_scalar_t crk_val __attribute__ ((packed));
	struct t_opthdr ppi_hdr __attribute__ ((packed));
	t_scalar_t ppi_val __attribute__ ((packed));
	struct t_opthdr sid_hdr __attribute__ ((packed));
	t_scalar_t sid_val __attribute__ ((packed));
	struct t_opthdr rcv_hdr __attribute__ ((packed));
	t_scalar_t rcv_val __attribute__ ((packed));
	struct t_opthdr ckl_hdr __attribute__ ((packed));
	t_scalar_t ckl_val __attribute__ ((packed));
	struct t_opthdr skd_hdr __attribute__ ((packed));
	t_scalar_t skd_val __attribute__ ((packed));
	struct t_opthdr prt_hdr __attribute__ ((packed));
	t_scalar_t prt_val __attribute__ ((packed));
	struct t_opthdr art_hdr __attribute__ ((packed));
	t_scalar_t art_val __attribute__ ((packed));
	struct t_opthdr irt_hdr __attribute__ ((packed));
	t_scalar_t irt_val __attribute__ ((packed));
	struct t_opthdr hbi_hdr __attribute__ ((packed));
	t_scalar_t hbi_val __attribute__ ((packed));
	struct t_opthdr rin_hdr __attribute__ ((packed));
	t_scalar_t rin_val __attribute__ ((packed));
	struct t_opthdr rmn_hdr __attribute__ ((packed));
	t_scalar_t rmn_val __attribute__ ((packed));
	struct t_opthdr rmx_hdr __attribute__ ((packed));
	t_scalar_t rmx_val __attribute__ ((packed));
	struct t_opthdr ist_hdr __attribute__ ((packed));
	t_scalar_t ist_val __attribute__ ((packed));
	struct t_opthdr ost_hdr __attribute__ ((packed));
	t_scalar_t ost_val __attribute__ ((packed));
	struct t_opthdr cin_hdr __attribute__ ((packed));
	t_scalar_t cin_val __attribute__ ((packed));
	struct t_opthdr tin_hdr __attribute__ ((packed));
	t_scalar_t tin_val __attribute__ ((packed));
	struct t_opthdr mac_hdr __attribute__ ((packed));
	t_scalar_t mac_val __attribute__ ((packed));
	struct t_opthdr dbg_hdr __attribute__ ((packed));
	t_scalar_t dbg_val __attribute__ ((packed));
} opt_optm = {
	{
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_NODELAY, T_SUCCESS}
	, T_YES, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_CORK, T_SUCCESS}
	, T_YES, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS}
	, 10, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS}
	, 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RECVOPT, T_SUCCESS}
	, T_NO, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_LIFE,
		    T_SUCCESS}
	, 60000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SACK_DELAY,
		    T_SUCCESS}
	, 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PATH_MAX_RETRANS,
		    T_SUCCESS}
	, 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ASSOC_MAX_RETRANS,
		    T_SUCCESS}
	, 12, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAX_INIT_RETRIES,
		    T_SUCCESS}
	, 12, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_HEARTBEAT_ITVL,
		    T_SUCCESS}
	, 200, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_INITIAL,
		    T_SUCCESS}
	, 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MIN, T_SUCCESS}
	, 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MAX, T_SUCCESS}
	, 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS,
		    T_SUCCESS}
	, 1, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS,
		    T_SUCCESS}
	, 1, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_INC,
		    T_SUCCESS}
	, 1000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_THROTTLE_ITVL,
		    T_SUCCESS}
	, 50, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAC_TYPE,
		    T_SUCCESS}
	, SCTP_HMAC_NONE, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_DEBUG, T_SUCCESS}
, 0};

char *
err_string(ulong error)
{
	switch (error) {
	case TBADADDR:
		return ("TBADADDR");
	case TBADOPT:
		return ("TBADOPT");
	case TACCES:
		return ("TACCES");
	case TBADF:
		return ("TBADF");
	case TNOADDR:
		return ("TNOADDR");
	case TOUTSTATE:
		return ("TOUTSTATE");
	case TBADSEQ:
		return ("TBADSEQ");
	case TSYSERR:
		return ("TSYSERR");
	case TLOOK:
		return ("TLOOK");
	case TBADDATA:
		return ("TBADDATA");
	case TBUFOVFLW:
		return ("TBUFOVFLW");
	case TFLOW:
		return ("TFLOW");
	case TNODATA:
		return ("TNODATA");
	case TNODIS:
		return ("TNODIS");
	case TNOUDERR:
		return ("TNOUDERR");
	case TBADFLAG:
		return ("TBADFLAG");
	case TNOREL:
		return ("TNOREL");
	case TNOTSUPPORT:
		return ("TNOTSUPPORT");
	case TSTATECHNG:
		return ("TSTATECHNG");
	case TNOSTRUCTYPE:
		return ("TNOSTRUCTTYPE");
	case TBADNAME:
		return ("TBADNAME");
	case TBADQLEN:
		return ("TBADQLEN");
	case TADDRBUSY:
		return ("TADDRBUSY");
	case TINDOUT:
		return ("TINDOUT");
	case TPROVMISMATCH:
		return ("TPROVMISMATCH");
	case TRESQLEN:
		return ("TRESQLEN");
	case TRESADDR:
		return ("TRESADDR");
	case TQFULL:
		return ("TQFULL");
	case TPROTO:
		return ("TPROTO");
	default:
		return ("(unknown)");
	}
}

void
print_error(ulong error)
{
	printf("%s\n", err_string(error));
}

char *
prim_string(ulong prim)
{
	switch (prim) {
	case T_CONN_REQ:
		return ("T_CONN_REQ");
	case T_CONN_RES:
		return ("T_CONN_RES");
	case T_DISCON_REQ:
		return ("T_DISCON_REQ");
	case T_DATA_REQ:
		return ("T_DATA_REQ");
	case T_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case T_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case T_INFO_REQ:
		return ("T_INFO_REQ");
	case T_BIND_REQ:
		return ("T_BIND_REQ");
	case T_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case T_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case T_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case T_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case T_CONN_IND:
		return ("T_CONN_IND");
	case T_CONN_CON:
		return ("T_CONN_CON");
	case T_DISCON_IND:
		return ("T_DISCON_IND");
	case T_DATA_IND:
		return ("T_DATA_IND");
	case T_EXDATA_IND:
		return ("T_EXDATA_IND");
	case T_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case T_INFO_ACK:
		return ("T_INFO_ACK");
	case T_BIND_ACK:
		return ("T_BIND_ACK");
	case T_ERROR_ACK:
		return ("T_ERROR_ACK");
	case T_OK_ACK:
		return ("T_OK_ACK");
	case T_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case T_UDERROR_IND:
		return ("T_UDERROR_IND");
	case T_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case T_ORDREL_IND:
		return ("T_ORDREL_IND");
	case T_ADDR_REQ:
		return ("T_ADDR_REQ");
	case T_ADDR_ACK:
		return ("T_ADDR_ACK");
	case FAILURE:
		return ("(nothing)");
	default:
		return ("(unexpected");
	}
}

void
print_prim(ulong prim)
{
	printf("%s", prim_string(prim));
}

void
print_state(ulong state)
{
	switch (state) {
	case TS_UNBND:
		printf("TS_UNBND");
		break;
	case TS_WACK_BREQ:
		printf("TS_WACK_BREQ");
		break;
	case TS_WACK_UREQ:
		printf("TS_WACK_UREQ");
		break;
	case TS_IDLE:
		printf("TS_IDLE");
		break;
	case TS_WACK_OPTREQ:
		printf("TS_WACK_OPTREQ");
		break;
	case TS_WACK_CREQ:
		printf("TS_WACK_CREQ");
		break;
	case TS_WCON_CREQ:
		printf("TS_WCON_CREQ");
		break;
	case TS_WRES_CIND:
		printf("TS_WRES_CIND");
		break;
	case TS_WACK_CRES:
		printf("TS_WACK_CRES");
		break;
	case TS_DATA_XFER:
		printf("TS_DATA_XFER");
		break;
	case TS_WIND_ORDREL:
		printf("TS_WIND_ORDREL");
		break;
	case TS_WREQ_ORDREL:
		printf("TS_WRES_ORDREL");
		break;
	case TS_WACK_DREQ6:
		printf("TS_WACK_DREQ6");
		break;
	case TS_WACK_DREQ7:
		printf("TS_WACK_DREQ7");
		break;
	case TS_WACK_DREQ9:
		printf("TS_WACK_DREQ9");
		break;
	case TS_WACK_DREQ10:
		printf("TS_WACK_DREQ10");
		break;
	case TS_WACK_DREQ11:
		printf("TS_WACK_DREQ11");
		break;
	default:
		printf("(unknown [%lu])", state);
		break;
	}
	printf("\n");
}

void
print_addr(char *add_ptr, size_t add_len)
{
	sctp_addr_t *a = (sctp_addr_t *) add_ptr;
	size_t anum =
	    add_len >= sizeof(a->port) ? (add_len - sizeof(a->port)) / sizeof(a->addr[0]) : 0;
	if (add_len) {
		int i;
		if (add_len != sizeof(a->port) + anum * sizeof(a->addr[0]))
			printf("Aaarrg! add_len = %d, anum = %d, ", add_len, anum);
		printf("[%d]", ntohs(a->port));
		for (i = 0; i < anum; i++) {
			printf("%s%d.%d.%d.%d",
			       i ? "," : "",
			       (a->addr[i] >> 0) & 0xff,
			       (a->addr[i] >> 8) & 0xff,
			       (a->addr[i] >> 16) & 0xff, (a->addr[i] >> 24) & 0xff);
		}
	} else
		printf("(no address)");
	printf("\n");
}

void
print_opt(char *opt_ptr, size_t opt_len)
{
	struct t_opthdr *oh = (struct t_opthdr *) opt_ptr;
	size_t opt_rem = opt_len;

	if (opt_len) {
		while (opt_rem && opt_rem > sizeof(*oh) && oh->len <= opt_rem) {
			char result[128] = "(failure)";

			long val = *((t_scalar_t *) (oh + 1));

			printf("\n\t");
			switch (oh->level) {
			case T_INET_IP:
				switch (oh->name) {
				case T_IP_OPTIONS:
					printf("T_IP_OPTIONS = ");
					sprintf(result, "%s", "(not impl)");
					break;
				case T_IP_TOS:
					printf("T_IP_TOS = ");
					sprintf(result, "0x%lx", val);
					break;
				case T_IP_TTL:
					printf("T_IP_TTL = ");
					sprintf(result, "%ld", val);
					break;
				case T_IP_REUSEADDR:
					printf("T_IP_REUSEADDR = ");
					sprintf(result, "%s", val == T_YES ? "T_YES" : "T_NO");
					break;
				case T_IP_DONTROUTE:
					printf("T_IP_DONTROUTE = ");
					sprintf(result, "%s", val == T_YES ? "T_YES" : "T_NO");
					break;
				case T_IP_BROADCAST:
					printf("T_IP_BROADCAST = ");
					sprintf(result, "%s", val == T_YES ? "T_YES" : "T_NO");
					break;
				default:
					printf("(bad option name %lu)", oh->name);
					break;
				}
				break;
			case T_INET_SCTP:
				switch (oh->name) {
				case T_SCTP_NODELAY:
					printf("T_SCTP_NODELAY = ");
					sprintf(result, "%s", val == T_YES ? "T_YES" : "T_NO");
					break;
				case T_SCTP_CORK:
					printf("T_SCTP_CORK = ");
					sprintf(result, "%s", val == T_YES ? "T_YES" : "T_NO");
					break;
				case T_SCTP_PPI:
					printf("T_SCTP_PPI = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_SID:
					printf("T_SCTP_SID = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_SSN:
					printf("T_SCTP_SSN = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_TSN:
					printf("T_SCTP_TSN = ");
					sprintf(result, "%lu", (ulong) val);
					break;
				case T_SCTP_RECVOPT:
					printf("T_SCTP_RECVOPT = ");
					sprintf(result, "%s", val == T_YES ? "T_YES" : "T_NO");
					break;
				case T_SCTP_COOKIE_LIFE:
					printf("T_SCTP_COOKIE_LIFE = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_SACK_DELAY:
					printf("T_SCTP_SACK_DELAY = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_PATH_MAX_RETRANS:
					printf("T_SCTP_PATH_MAX_RETRANS = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_ASSOC_MAX_RETRANS:
					printf("T_SCTP_ASSOC_MAX_RETRANS = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_MAX_INIT_RETRIES:
					printf("T_SCTP_MAX_INIT_RETRIES = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_HEARTBEAT_ITVL:
					printf("T_SCTP_HEARTBEAT_ITVL = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_RTO_INITIAL:
					printf("T_SCTP_RTO_INITIAL = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_RTO_MIN:
					printf("T_SCTP_RTO_MIN = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_RTO_MAX:
					printf("T_SCTP_RTO_MAX = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_OSTREAMS:
					printf("T_SCTP_OSTREAMS = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_ISTREAMS:
					printf("T_SCTP_ISTREAMS = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_COOKIE_INC:
					printf("T_SCTP_COOKIE_INC = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_THROTTLE_ITVL:
					printf("T_SCTP_THROTTLE_ITVL = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_MAC_TYPE:
					printf("T_SCTP_MAC_TYPE = ");
					switch (val) {
					default:
					case SCTP_HMAC_NONE:
						sprintf(result, "NONE");
						break;
					case SCTP_HMAC_SHA_1:
						sprintf(result, "SHA_1");
						break;
					case SCTP_HMAC_MD5:
						sprintf(result, "MD5");
						break;
					}
					break;
				case T_SCTP_HB:
					printf("T_SCTP_HB = ");
					sprintf(result, "%s", "(undone)");
					break;
				case T_SCTP_RTO:
					printf("T_SCTP_RTO = ");
					sprintf(result, "%s", "(undone)");
					break;
				case T_SCTP_MAXSEG:
					printf("T_SCTP_MAXSEG = ");
					sprintf(result, "%ld", val);
					break;
				case T_SCTP_STATUS:
					printf("T_SCTP_STATUS = ");
					sprintf(result, "%s", "(undone)");
					break;
				case T_SCTP_DEBUG:
					printf("T_SCTP_DEBUG = ");
					if (!val)
						sprintf(result, "(none)");
					else {
						char *where = result;
						if (val & SCTP_OPTION_DROPPING)
							where += sprintf(where, " DROPPING");
						if (val & SCTP_OPTION_BREAK)
							where += sprintf(where, " BREAK");
						if (val & SCTP_OPTION_DBREAK)
							where += sprintf(where, " DBREAK");
						if (val & SCTP_OPTION_RANDOM)
							where += sprintf(where, " RANDOM");
					}
					break;
				default:
					printf("(bad option name %lu)", oh->name);
					break;
				}
				break;
			default:
				printf("(bad option level %lu)", oh->level);
				break;
			}
			printf("%s", result);
			opt_rem -= oh->len;
			oh = (struct t_opthdr *) (((char *) oh) + oh->len);
		}
		if (opt_rem)
			printf("\n\tBadly formatted options.");
	} else
		printf("(no opt)");
	printf("\n");
}

void
print_size(ulong size)
{
	switch (size) {
	case -1UL:
		printf("UNLIMITED\n");
		break;
	case -2UL:
		printf("UNDEFINED\n");
		break;
	default:
		printf("%lu\n", size);
		break;
	}
}

void
print_msg(int fd)
{
	if (ctrl.len > 0) {
		switch (show) {
		case 0:
			return;

		case 2:
			printf("%d-", fd);
			print_prim(cmd.tpi.type);
			printf(":\n");

			switch (cmd.tpi.type) {
			case T_INFO_REQ:
				break;

			case T_INFO_ACK:
				printf("  TSDU_size      = ");
				print_size(cmd.tpi.info_ack.TSDU_size);
				printf("  ETSDU_size     = ");
				print_size(cmd.tpi.info_ack.ETSDU_size);
				printf("  CDATA_size     = ");
				print_size(cmd.tpi.info_ack.CDATA_size);
				printf("  DDATA_size     = ");
				print_size(cmd.tpi.info_ack.DDATA_size);
				printf("  ADDR_size      = ");
				print_size(cmd.tpi.info_ack.ADDR_size);
				printf("  OPT_size       = ");
				print_size(cmd.tpi.info_ack.OPT_size);
				printf("  TIDU_size      = ");
				print_size(cmd.tpi.info_ack.TIDU_size);
				printf("  SERV_type      = ");
				switch (cmd.tpi.info_ack.SERV_type) {
				case T_COTS:
					printf("T_COTS\n");
					break;
				case T_COTS_ORD:
					printf("T_COTS_ORD\n");
					break;
				case T_CLTS:
					printf("T_CLTS\n");
					break;
				default:
					printf("(unknown)\n");
					break;
				}
				printf("  CURRENT_state  = ");
				print_state(cmd.tpi.info_ack.CURRENT_state);
				printf("  PROVIDER_flag  = ");
				if (cmd.tpi.info_ack.PROVIDER_flag & T_SNDZERO)
					printf("T_SNDZERO\n");
				if (cmd.tpi.info_ack.PROVIDER_flag & XPG4_1)
					printf(" XPG4_1\n");
				break;

			case T_OPTMGMT_REQ:
				printf("  MGMT_flags     = ");
				switch (cmd.tpi.optmgmt_req.MGMT_flags) {
				case T_NEGOTIATE:
					printf("T_NEGOTIATE\n");
					break;
				case T_CHECK:
					printf("T_CHECK\n");
					break;
				case T_CURRENT:
					printf("T_CURRENT\n");
					break;
				case T_DEFAULT:
					printf("T_DEFAULT\n");
					break;
				default:
					break;
				}
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.optmgmt_req.OPT_offset,
					  cmd.tpi.optmgmt_req.OPT_length);
				break;

			case T_OPTMGMT_ACK:
				printf("  MGMT_flags     = ");
				switch (cmd.tpi.optmgmt_ack.MGMT_flags) {
				case T_NEGOTIATE:
					printf("T_NEGOTIATE\n");
					break;
				case T_CHECK:
					printf("T_CHECK\n");
					break;
				case T_CURRENT:
					printf("T_CURRENT\n");
					break;
				case T_DEFAULT:
					printf("T_DEFAULT\n");
					break;
				default:
					break;
				}
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.optmgmt_ack.OPT_offset,
					  cmd.tpi.optmgmt_ack.OPT_length);
				break;

			case T_BIND_REQ:
				printf("  ADDR           = ");
				print_addr(cmd.cbuf + cmd.tpi.bind_req.ADDR_offset,
					   cmd.tpi.bind_req.ADDR_length);
				printf("  CONIND_number  = %lu\n", cmd.tpi.bind_req.CONIND_number);
				break;

			case T_BIND_ACK:
				printf("  ADDR           = ");
				print_addr(cmd.cbuf + cmd.tpi.bind_ack.ADDR_offset,
					   cmd.tpi.bind_ack.ADDR_length);
				printf("  CONIND_number  = %lu\n", cmd.tpi.bind_ack.CONIND_number);
				break;

			case T_ADDR_REQ:
				break;

			case T_ADDR_ACK:
				printf("  LOCADDR        = ");
				print_addr(cmd.cbuf + cmd.tpi.addr_ack.LOCADDR_offset,
					   cmd.tpi.addr_ack.LOCADDR_length);
				printf("  REMADDR        = ");
				print_addr(cmd.cbuf + cmd.tpi.addr_ack.REMADDR_offset,
					   cmd.tpi.addr_ack.REMADDR_length);
				break;

			case T_UNBIND_REQ:
				break;

			case T_ERROR_ACK:
				printf("  ERROR_prim     = ");
				print_prim(cmd.tpi.error_ack.ERROR_prim);
				printf("\n");
				printf("  TLI_error      = ");
				print_error(cmd.tpi.error_ack.TLI_error);
				printf("  UNIX_error     = %lu (%s)\n",
				       cmd.tpi.error_ack.UNIX_error,
				       strerror(cmd.tpi.error_ack.UNIX_error));
				break;

			case T_OK_ACK:
				printf("  CORRECT_prim   = ");
				print_prim(cmd.tpi.ok_ack.CORRECT_prim);
				printf("\n");
				break;

			case T_CONN_REQ:
				printf("  DEST           = ");
				print_addr(cmd.cbuf + cmd.tpi.conn_req.DEST_offset,
					   cmd.tpi.conn_req.DEST_length);
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.conn_req.OPT_offset,
					  cmd.tpi.conn_req.OPT_length);
				break;

			case T_CONN_IND:
				printf("  SRC            = ");
				print_addr(cmd.cbuf + cmd.tpi.conn_ind.SRC_offset,
					   cmd.tpi.conn_ind.SRC_length);
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.conn_ind.OPT_offset,
					  cmd.tpi.conn_ind.OPT_length);
				printf("  SEQ_number     = %lx\n", cmd.tpi.conn_ind.SEQ_number);
				seq[fd] = cmd.tpi.conn_ind.SEQ_number;
				break;

			case T_CONN_RES:
				printf("  ACCEPTOR_id    = %d\n", fdi.fildes);
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.conn_res.OPT_offset,
					  cmd.tpi.conn_res.OPT_length);
				printf("  SEQ_number     = %lx\n", cmd.tpi.conn_res.SEQ_number);
				break;

			case T_CONN_CON:
				printf("  RES            = ");
				print_addr(cmd.cbuf + cmd.tpi.conn_con.RES_offset,
					   cmd.tpi.conn_con.RES_length);
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.conn_con.OPT_offset,
					  cmd.tpi.conn_con.OPT_length);
				break;

			case T_DATA_REQ:
				printf("  MORE_flag      = %lu\n", cmd.tpi.data_req.MORE_flag);
				printf("  DATA           = %s\n", dbuf);
				break;

			case T_DATA_IND:
				printf("  MORE_flag      = %lu\n", cmd.tpi.data_ind.MORE_flag);
				printf("  DATA           = %s\n", dbuf);
				break;

			case T_EXDATA_REQ:
				printf("  MORE_flag      = %lu\n", cmd.tpi.exdata_req.MORE_flag);
				printf("  DATA           = %s\n", dbuf);
				break;

			case T_EXDATA_IND:
				printf("  MORE_flag      = %lu\n", cmd.tpi.exdata_ind.MORE_flag);
				printf("  DATA           = %s\n", dbuf);
				break;

			case T_OPTDATA_REQ:
				printf("  DATA_flag      = ");
				if (cmd.tpi.optdata_req.DATA_flag & T_ODF_MORE)
					printf("T_ODF_MORE ");
				if (cmd.tpi.optdata_req.DATA_flag & T_ODF_EX)
					printf("T_ODF_EX ");
				printf("\n");
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.optdata_req.OPT_offset,
					  cmd.tpi.optdata_req.OPT_length);
				printf("  DATA           = %s\n", dbuf);
				break;

			case T_OPTDATA_IND:
				printf("  DATA_flag      = ");
				if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_MORE)
					printf("T_ODF_MORE ");
				if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX)
					printf("T_ODF_EX ");
				printf("\n");
				printf("  OPT            = ");
				print_opt(cmd.cbuf + cmd.tpi.optdata_ind.OPT_offset,
					  cmd.tpi.optdata_ind.OPT_length);
				printf("  DATA           = %s\n", dbuf);
				tsn[fd] =
				    ((rdat_opt_t *) (cmd.cbuf +
						     cmd.tpi.optdata_ind.OPT_offset))->tsn_val;
				sid[fd] =
				    ((rdat_opt_t *) (cmd.cbuf +
						     cmd.tpi.optdata_ind.OPT_offset))->sid_val;
				break;

			case T_DISCON_REQ:
				printf("  SEQ_number     = %lx\n", cmd.tpi.discon_req.SEQ_number);
				break;

			case T_DISCON_IND:
			{
				long reason = (long) cmd.tpi.discon_ind.DISCON_reason;
				if (reason >= 0)
					printf("  DISCON_reason  = %lu\n", (ulong) reason);
				else
					printf("  DISCON_reason  = %ld (%s)\n", reason,
					       strerror(-reason));
				printf("  SEQ_number     = %lx\n", cmd.tpi.discon_ind.SEQ_number);
				break;
			}

			case T_ORDREL_REQ:
				break;

			case T_ORDREL_IND:
				break;

			default:
				printf("Unrecognized primitive %lu!\n", cmd.tpi.type);
				break;
			}
			break;
		case 1:
			if (fd == fd1) {
				switch (cmd.tpi.type) {
				case T_INFO_REQ:
					printf
					    ("T_INFO_REQ    ----->|                               |  |                    \n");
					break;
				case T_INFO_ACK:
					printf
					    ("T_INFO_ACK    <-----|                               |  |                    \n");
					break;
				case T_OPTMGMT_REQ:
					printf
					    ("T_OPTMGMT_REQ ----->|                               |  |                    \n");
					break;
				case T_OPTMGMT_ACK:
					printf
					    ("T_OPTMGMT_ACK <-----|                               |  |                    \n");
					break;
				case T_BIND_REQ:
					printf
					    ("T_BIND_REQ    ----->|                               |  |                    \n");
					break;
				case T_BIND_ACK:
					printf
					    ("T_BIND_ACK    <-----|                               |  |                    \n");
					break;
				case T_ADDR_REQ:
					printf
					    ("T_ADDR_REQ    ----->|                               |  |                    \n");
					break;
				case T_ADDR_ACK:
					printf
					    ("T_ADDR_ACK    <-----|                               |  |                    \n");
					break;
				case T_UNBIND_REQ:
					printf
					    ("T_UNBIND_REQ  ----->|                               |  |                    \n");
					break;
				case T_ERROR_ACK:
					printf
					    ("T_ERROR_ACK   <----/|                               |  |                    \n");
					printf
					    ("[%-11s]       |                               |  |                    \n",
					     err_string(cmd.tpi.error_ack.TLI_error));
					break;
				case T_OK_ACK:
					printf
					    ("T_OK_ACK      <----/|                |              |  |                    \n");
					break;
				case T_CONN_REQ:
					printf
					    ("T_CONN_REQ    ----->|--------------->|              |  |                    \n");
					break;
				case T_CONN_IND:
					printf
					    ("T_CONN_IND    <-----|<---------------|              |  |                    \n");
					seq[fd] = cmd.tpi.conn_ind.SEQ_number;
					break;
				case T_CONN_RES:
					printf
					    ("T_CONN_RES    ----->|--------------->|              |  |                    \n");
					break;
				case T_CONN_CON:
					printf
					    ("T_CONN_CON    <-----|<---------------|              |  |                    \n");
					break;
				case T_DATA_REQ:
					printf
					    ("T_DATA_REQ    ----->|--------------->|                 |                    \n");
					break;
				case T_DATA_IND:
					printf
					    ("T_DATA_IND    <-----|<---------------|                 |                    \n");
					break;
				case T_EXDATA_REQ:
					printf
					    ("T_EXDATA_REQ  ----->|--------------->|                 |                    \n");
					break;
				case T_EXDATA_IND:
					printf
					    ("T_EXDATA_IND  <-----|<---------------|                 |                    \n");
					break;
				case T_OPTDATA_REQ:
					if (cmd.tpi.optdata_req.DATA_flag & T_ODF_EX)
						printf
						    ("T_OPTDATA_REQ ----->|---(%03lu:-U-)--->|                 |                    \n",
						     opt_data.sid_val);
					else
						printf
						    ("T_OPTDATA_REQ ----->|---(%03lu:---)--->|                 |                    \n",
						     opt_data.sid_val);
					break;
				case T_OPTDATA_IND:
					if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX)
						printf
						    ("T_OPTDATA_IND <-----|<--(%03lu:-U-)----|  [%010lu]   |                    \n",
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->sid_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->
						     tsn_val);
					else
						printf
						    ("T_OPTDATA_IND <-----|<--(%03lu:%03lu)----|  [%010lu]   |                    \n",
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->sid_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->ssn_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->
						     tsn_val);
					tsn[fd] =
					    ((rdat_opt_t *) (cmd.cbuf +
							     cmd.tpi.optdata_ind.OPT_offset))->
					    tsn_val;
					sid[fd] =
					    ((rdat_opt_t *) (cmd.cbuf +
							     cmd.tpi.optdata_ind.OPT_offset))->
					    sid_val;
					break;
				case T_DISCON_REQ:
					printf
					    ("T_DISCON_REQ  ----->|--------------->|              |  |                    \n");
					break;
				case T_DISCON_IND:
					printf
					    ("T_DISCON_IND  <-----|<---------------|              |  |                    \n");
					break;
				case T_ORDREL_REQ:
					printf
					    ("T_ORDREL_REQ  ----->|--------------->|              |  |                    \n");
					break;
				case T_ORDREL_IND:
					printf
					    ("T_ORDREL_IND  <-----|<---------------|              |  |                    \n");
					break;
				default:
					printf
					    ("????%4ld????  ?----?|                                  |                    \n",
					     cmd.tpi.type);
					break;
				}
			}
			if (fd == fd2) {
				switch (cmd.tpi.type) {
				case T_INFO_REQ:
					printf
					    ("                    |                               |<-+------ T_INFO_REQ   \n");
					break;
				case T_INFO_ACK:
					printf
					    ("                    |                               |--+-----> T_INFO_ACK   \n");
					break;
				case T_OPTMGMT_REQ:
					printf
					    ("                    |                               |<-+------ T_OPTMGMT_REQ\n");
					break;
				case T_OPTMGMT_ACK:
					printf
					    ("                    |                               |--+-----> T_OPTMGMT_ACK\n");
					break;
				case T_BIND_REQ:
					printf
					    ("                    |                               |<-+------ T_BIND_REQ   \n");
					break;
				case T_BIND_ACK:
					printf
					    ("                    |                               |--+-----> T_BIND_ACK   \n");
					break;
				case T_ADDR_REQ:
					printf
					    ("                    |                               |<-+------ T_ADDR_REQ   \n");
					break;
				case T_ADDR_ACK:
					printf
					    ("                    |                               |--+-----> T_ADDR_ACK   \n");
					break;
				case T_UNBIND_REQ:
					printf
					    ("                    |                               |<-+------ T_UNBIND_REQ \n");
					break;
				case T_ERROR_ACK:
					printf
					    ("                    |                               |\\-+-----> T_ERROR_ACK  \n");
					printf
					    ("                    |                               |          [%-11s]\n",
					     err_string(cmd.tpi.error_ack.TLI_error));
					break;
				case T_OK_ACK:
					printf
					    ("                    |                |              |\\-+-----> T_OK_ACK     \n");
					break;
				case T_CONN_REQ:
					printf
					    ("                    |                |<-------------|<-+------ T_CONN_REQ   \n");
					break;
				case T_CONN_IND:
					printf
					    ("                    |                |------------->|--+-----> T_CONN_IND   \n");
					seq[fd] = cmd.tpi.conn_ind.SEQ_number;
					break;
				case T_CONN_RES:
					printf
					    ("                    |                |<-------------|<-+------ T_CONN_RES   \n");
					break;
				case T_CONN_CON:
					printf
					    ("                    |                |------------->|--+-----> T_CONN_CON   \n");
					break;
				case T_DATA_REQ:
					printf
					    ("                    |                |<-------------|<-+------ T_DATA_REQ   \n");
					break;
				case T_DATA_IND:
					printf
					    ("                    |                |------------->|--+-----> T_DATA_IND   \n");
					break;
				case T_EXDATA_REQ:
					printf
					    ("                    |                |<-------------|<-+------ T_EXDATA_REQ \n");
					break;
				case T_EXDATA_IND:
					printf
					    ("                    |                |------------->|--+-----> T_EXDATA_IND \n");
					break;
				case T_OPTDATA_REQ:
					if (cmd.tpi.optdata_req.DATA_flag & T_ODF_EX)
						printf
						    ("                    |                |<--(%03lu:-U-)--|<-+------ T_OPTDATA_REQ\n",
						     opt_data.sid_val);
					else
						printf
						    ("                    |                |<--(%03lu:---)--|<-+------ T_OPTDATA_REQ\n",
						     opt_data.sid_val);
					break;
				case T_OPTDATA_IND:
					if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX)
						printf
						    ("                    |  [%010lu]  |---(%03lu:-U-)->|--+-----> T_OPTDATA_IND\n",
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->tsn_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->
						     sid_val);
					else
						printf
						    ("                    |  [%010lu]  |---(%03lu:%03lu)->|--+-----> T_OPTDATA_IND\n",
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->tsn_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->sid_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->
						     ssn_val);
					tsn[fd] =
					    ((rdat_opt_t *) (cmd.cbuf +
							     cmd.tpi.optdata_ind.OPT_offset))->
					    tsn_val;
					sid[fd] =
					    ((rdat_opt_t *) (cmd.cbuf +
							     cmd.tpi.optdata_ind.OPT_offset))->
					    sid_val;
					break;
				case T_DISCON_REQ:
					printf
					    ("                    |                |<-------------|<-+------ T_DISCON_REQ \n");
					break;
				case T_DISCON_IND:
					printf
					    ("                    |                |------------->|--+-----> T_DISCON_IND \n");
					break;
				case T_ORDREL_REQ:
					printf
					    ("                    |                |<-------------|<-+------ T_ORDREL_REQ \n");
					break;
				case T_ORDREL_IND:
					printf
					    ("                    |                |------------->|--+-----> T_ORDREL_IND \n");
					break;
				default:
					printf
					    ("                    |                               |?-+-----? ????%4ld???? \n",
					     cmd.tpi.type);
					break;
				}
			}
			if (fd == fd3) {
				switch (cmd.tpi.type) {
				case T_INFO_REQ:
					printf
					    ("                    |                               |  |<----- T_INFO_REQ   \n");
					break;
				case T_INFO_ACK:
					printf
					    ("                    |                               |  |-----> T_INFO_ACK   \n");
					break;
				case T_OPTMGMT_REQ:
					printf
					    ("                    |                               |  |<----- T_OPTMGMT_REQ\n");
					break;
				case T_OPTMGMT_ACK:
					printf
					    ("                    |                               |  |-----> T_OPTMGMT_ACK\n");
					break;
				case T_BIND_REQ:
					printf
					    ("                    |                               |  |<----- T_BIND_REQ   \n");
					break;
				case T_BIND_ACK:
					printf
					    ("                    |                               |  |-----> T_BIND_ACK   \n");
					break;
				case T_ADDR_REQ:
					printf
					    ("                    |                               |  |<----- T_ADDR_REQ   \n");
					break;
				case T_ADDR_ACK:
					printf
					    ("                    |                               |  |-----> T_ADDR_ACK   \n");
					break;
				case T_UNBIND_REQ:
					printf
					    ("                    |                               |  |<----- T_UNBIND_REQ \n");
					break;
				case T_ERROR_ACK:
					printf
					    ("                    |                               |  |\\----> T_ERROR_ACK  \n");
					printf
					    ("                    |                               |  |       [%-11s]\n",
					     err_string(cmd.tpi.error_ack.TLI_error));
					break;
				case T_OK_ACK:
					printf
					    ("                    |                |              |  |\\----> T_OK_ACK     \n");
					break;
				case T_CONN_REQ:
					printf
					    ("                    |                |<----------------|<----- T_CONN_REQ   \n");
					break;
				case T_CONN_IND:
					printf
					    ("                    |                |---------------->|-----> T_CONN_IND   \n");
					seq[fd] = cmd.tpi.conn_ind.SEQ_number;
					break;
				case T_CONN_RES:
					printf
					    ("                    |                |<----------------|<----- T_CONN_RES   \n");
					break;
				case T_CONN_CON:
					printf
					    ("                    |                |---------------->|-----> T_CONN_CON   \n");
					break;
				case T_DATA_REQ:
					printf
					    ("                    |                |<----------------|<----- T_DATA_REQ   \n");
					break;
				case T_DATA_IND:
					printf
					    ("                    |                |---------------->|-----> T_DATA_IND   \n");
					break;
				case T_EXDATA_REQ:
					printf
					    ("                    |                |<----------------|<----- T_EXDATA_REQ \n");
					break;
				case T_EXDATA_IND:
					printf
					    ("                    |                |---------------->|-----> T_EXDATA_IND \n");
					break;
				case T_OPTDATA_REQ:
					if (cmd.tpi.optdata_req.DATA_flag & T_ODF_EX)
						printf
						    ("                    |                |<---(%03lu:-U-)----|<----- T_OPTDATA_REQ\n",
						     opt_data.sid_val);
					else
						printf
						    ("                    |                |<---(%03lu:---)----|<----- T_OPTDATA_REQ\n",
						     opt_data.sid_val);
					break;
				case T_OPTDATA_IND:
					if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX)
						printf
						    ("                    |  [%010lu]  |----(%03lu:-U-)--->|-----> T_OPTDATA_IND\n",
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->tsn_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->
						     sid_val);
					else
						printf
						    ("                    |  [%010lu]  |----(%03lu:%03lu)--->|-----> T_OPTDATA_IND\n",
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->tsn_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->sid_val,
						     (ulong) ((rdat_opt_t *) (cmd.cbuf +
									      cmd.tpi.optdata_ind.
									      OPT_offset))->
						     ssn_val);
					tsn[fd] =
					    ((rdat_opt_t *) (cmd.cbuf +
							     cmd.tpi.optdata_ind.OPT_offset))->
					    tsn_val;
					sid[fd] =
					    ((rdat_opt_t *) (cmd.cbuf +
							     cmd.tpi.optdata_ind.OPT_offset))->
					    sid_val;
					break;
				case T_DISCON_REQ:
					printf
					    ("                    |                |<----------------|<----- T_DISCON_REQ \n");
					break;
				case T_DISCON_IND:
					printf
					    ("                    |                |---------------->|-----> T_DISCON_IND \n");
					break;
				case T_ORDREL_REQ:
					printf
					    ("                    |                |<----------------|<----- T_ORDREL_REQ \n");
					break;
				case T_ORDREL_IND:
					printf
					    ("                    |                |---------------->|-----> T_ORDREL_IND \n");
					break;
				default:
					printf
					    ("                    |                               |  |?----? ????%4ld???? \n",
					     cmd.tpi.type);
					break;
				}
			}
			break;
		}
	}
	FFLUSH(stdout);
}

void
print_less(int fd)
{
	switch (show) {
	case 1:
		switch (fd) {
		case 0:
			printf
			    (" .         .  <---->|                .                 |<---->  .         . \n");
			printf
			    (" .  (more) .  <---->|                .                 |<---->  . (more)  . \n");
			printf
			    (" .         .  <---->|                .                 |<---->  .         . \n");
			break;
		case 3:
			printf
			    (" .         .  <---->|                .              |  |                    \n");
			printf
			    (" .  (more) .  <---->|                .              |  |                    \n");
			printf
			    (" .         .  <---->|                .              |  |                    \n");
			break;
		case 4:
			printf
			    ("                    |                .              |<-+----->  .         . \n");
			printf
			    ("                    |                .              |<-+----->  . (more)  . \n");
			printf
			    ("                    |                .              |<-+----->  .         . \n");
			break;
		case 5:
			printf
			    ("                    |                .              |  |<---->  .         . \n");
			printf
			    ("                    |                .              |  |<---->  . (more)  . \n");
			printf
			    ("                    |                .              |  |<---->  .         . \n");
			break;
		}
		break;
	case 2:
		printf("%d-(more) ...\n", fd);
		break;
	}
	show = 0;
	return;
}

void
print_more(void)
{
	show = 1;
}

int
get_msg(int fd, int wait)
{
	int ret;
	int flags = 0;
	while ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
		switch (errno) {
		default:
		case EPROTO:
		case EINVAL:
			printf("ERROR: getmsg: [%d] %s\n", errno, strerror(errno));
			exit(2);
		case EINTR:
		case ERESTART:
			continue;
		case EAGAIN:
			break;
		}
		break;
	}
	if (!ret) {
		gettimeofday(&when, NULL);
		if (show)
			print_msg(fd);
		return (cmd.tpi.type);
	}
	if (!wait) {
		/* printf("Nothing to get on getmsg\n"); */
		return (FAILURE);
	}
	do {
		struct pollfd pfd[] = {
			{fd, POLLIN | POLLPRI, 0}
		};
		if (!(ret = poll(pfd, 1, wait))) {
			/* printf("Timeout on poll for getmsg\n"); */
			return (FAILURE);
		}
		if (ret == 1 || ret == 2) {
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				flags = 0;
				if (getmsg(fd, &ctrl, &data, &flags) == 0) {
					gettimeofday(&when, NULL);
					if (show)
						print_msg(fd);
					return (cmd.tpi.type);
				}
				printf("ERROR: getmsg: [%d] %s\n", errno, strerror(errno));
				return (FAILURE);
			}
		}
		if (ret == -1) {
			printf("ERROR: poll: [%d] %s\n", errno, strerror(errno));
			return (FAILURE);
		}
	} while (1);
}

int
expect(int fd, int wait, int want)
{
	int got;
	if ((got = get_msg(fd, wait)) == want)
		return (SUCCESS);
	else {
		switch (show) {
		case 1:
			switch (fd) {
			case 3:
				printf
				    ("(%-12s)<-????X[Expected]                     X  X                    \n",
				     prim_string(want));
				break;
			case 4:
				printf
				    ("                    X                     [Expected]X--X\?\?\?\?->(%-12s)\n",
				     prim_string(want));
				break;
			case 5:
				printf
				    ("                    X                     [Expected]X  X\?\?\?\?->(%-12s)\n",
				     prim_string(want));
				break;
			}
			break;
		case 2:
			printf("%d-ERROR: Expected ", fd);
			print_prim(want);
			printf(" got ");
			print_prim(got);
			printf("\n");
			break;
		}
		return (FAILURE);
	}
}

int
put_msg(int fd, int band, int flags, int wait)
{
	int ret;
	struct strbuf *mydata = data.len ? &data : NULL;
	while ((ret = putpmsg(fd, &ctrl, mydata, band, flags)) < 0) {
		switch (errno) {
		default:
			printf("ERROR: putpmsg: [%d] %s\n", errno, strerror(errno));
			exit(2);
			break;
		case EINTR:
		case ERESTART:
			continue;
		case EAGAIN:
			break;
		}
		break;
	}
	if (!ret) {
		gettimeofday(&when, NULL);
		if (show)
			print_msg(fd);
		return (SUCCESS);
	}
	if (!wait) {
		/* printf("Nothing put on putpmsg\n"); */
		return (FAILURE);
	}
	do {
		int flag = band ? POLLWRNORM | POLLOUT : POLLWRBAND;
		struct pollfd pfd[] = {
			{fd, flag, 0}
		};
		if (!(ret = poll(pfd, 1, wait))) {
			/* printf("Timeout on poll for putpmsg\n"); */
			return (FAILURE);
		}
		if (ret == 1 || ret == 2) {
			if (pfd[0].revents & (POLLOUT | POLLWRNORM | POLLWRBAND)) {
				if (putpmsg(fd, &ctrl, mydata, band, flags) == 0) {
					gettimeofday(&when, NULL);
					if (show)
						print_msg(fd);
					return (SUCCESS);
				}
				printf("ERROR: putpmsg: [%d] %s\n", errno, strerror(errno));
				return (FAILURE);
			}
		}
		if (ret == -1) {
			printf("ERROR: poll: [%d] %s\n", errno, strerror(errno));
			return (FAILURE);
		}
	} while (1);
}

int
put_fdi(int fd, int fd2, int offset, int flags)
{
	fdi.flags = flags;
	fdi.fildes = fd2;
	fdi.offset = offset;
	if (show)
		print_msg(fd);
	gettimeofday(&when, NULL);
	if (ioctl(fd, I_FDINSERT, &fdi) < 0) {
		printf("ERROR: ioctl: [%d] %s\n", errno, strerror(errno));
		exit(2);
	}
	return (SUCCESS);
}

int
sctp_t_open(void)
{
	int fd;
	if ((fd = open("/dev/sctp_t", O_NONBLOCK | O_RDWR)) < 0)
		printf("ERROR: open: [%d] %s\n", errno, strerror(errno));
	else
		switch (show) {
		case 1:
			switch (fd) {
			case 3:
				printf
				    ("OPEN          ----->v                                                       \n");
				break;
			case 4:
				printf
				    ("                    |                               v<-------- OPEN         \n");
				break;
			case 5:
				printf
				    ("                    |                               |  v<----- OPEN         \n");
				break;
			}
			break;
		case 2:
			printf("\n%d-OPEN: sctp_n\n", fd);
			break;
		}
	return (fd);
}

int
sctp_close(int fd)
{
	int ret;
	if ((ret = close(fd)) < 0)
		printf("ERROR: close: [%d] %s\n", errno, strerror(errno));
	else
		switch (show) {
		case 1:
			switch (fd) {
			case 3:
				printf
				    ("CLOSE         ----->X                               |  |                    \n");
				break;
			case 4:
				printf
				    ("                                                    X<-+------ CLOSE        \n");
				break;
			case 5:
				printf
				    ("                                                       X<----- CLOSE        \n");
				break;
			}
			break;
		case 2:
			printf("\n%d-CLOSE:\n", fd);
			break;
		}
	return (ret);
}

int
sctp_info_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.info_req);
	cmd.tpi.type = T_INFO_REQ;
	return put_msg(fd, 0, MSG_HIPRI, 0);
}

int
sctp_optmgmt_req(int fd, ulong flags)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.optmgmt_req) + sizeof(opt_optm);
	cmd.tpi.type = T_OPTMGMT_REQ;
	cmd.tpi.optmgmt_req.MGMT_flags = flags;
	cmd.tpi.optmgmt_req.OPT_length = sizeof(opt_optm);
	cmd.tpi.optmgmt_req.OPT_offset = sizeof(cmd.tpi.optmgmt_req);
	bcopy(&opt_optm, (cmd.cbuf + sizeof(cmd.tpi.optmgmt_req)), sizeof(opt_optm));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_bind_req(int fd, addr_t * addr, int coninds)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.bind_req) + sizeof(*addr);
	cmd.tpi.type = T_BIND_REQ;
	cmd.tpi.bind_req.ADDR_length = sizeof(*addr);
	cmd.tpi.bind_req.ADDR_offset = sizeof(cmd.tpi.bind_req);
	cmd.tpi.bind_req.CONIND_number = coninds;
	bcopy(addr, (&cmd.tpi.bind_req) + 1, sizeof(*addr));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_unbind_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.unbind_req);
	cmd.tpi.type = T_UNBIND_REQ;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_conn_req(int fd, addr_t * addr, const char *dat)
{
	if (!dat)
		data.len = 0;
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.conn_req) + sizeof(*addr) + sizeof(opt_conn);
	cmd.tpi.type = T_CONN_REQ;
	cmd.tpi.conn_req.DEST_length = sizeof(*addr);
	cmd.tpi.conn_req.DEST_offset = sizeof(cmd.tpi.conn_req);
	cmd.tpi.conn_req.OPT_length = sizeof(opt_conn);
	cmd.tpi.conn_req.OPT_offset = sizeof(cmd.tpi.conn_req) + sizeof(*addr);
	bcopy(addr, (cmd.cbuf + sizeof(cmd.tpi.conn_req)), sizeof(*addr));
	bcopy(&opt_conn, (cmd.cbuf + sizeof(cmd.tpi.conn_req) + sizeof(*addr)), sizeof(opt_conn));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_conn_res(int fd, int fd2, const char *dat)
{
	if (!dat)
		fdi.databuf.len = 0;
	else {
		fdi.databuf.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	fdi.ctlbuf.len = sizeof(cmd.tpi.conn_res);
	cmd.tpi.type = T_CONN_RES;
	cmd.tpi.conn_res.ACCEPTOR_id = 0;
	cmd.tpi.conn_res.SEQ_number = seq[fd];
	cmd.tpi.conn_res.OPT_offset = 0;
	cmd.tpi.conn_res.OPT_length = 0;
	return put_fdi(fd, fd2, sizeof(cmd.tpi.type), 0);
}

int
sctp_discon_req(int fd, ulong seq)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.discon_req);
	cmd.tpi.type = T_DISCON_REQ;
	cmd.tpi.discon_req.SEQ_number = seq;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_ordrel_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.ordrel_req);
	cmd.tpi.type = T_ORDREL_REQ;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_ndata_req(int fd, ulong flags, const char *dat, size_t len, int wait)
{
	int ret;
	if (!dat)
		return (FAILURE);
	else {
		data.maxlen = len;
		data.len = len;
		data.buf = (char *) dat;
	}
	ctrl.len = sizeof(cmd.tpi.data_req);
	cmd.tpi.type = T_DATA_REQ;
	cmd.tpi.data_req.MORE_flag = flags & T_MORE;
	ret = put_msg(fd, 0, MSG_BAND, wait);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int
sctp_data_req(int fd, ulong flags, const char *dat, int wait)
{
	if (!dat)
		return (FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.data_req);
	cmd.tpi.type = T_DATA_REQ;
	cmd.tpi.data_req.MORE_flag = flags & T_MORE;
	return put_msg(fd, 0, MSG_BAND, wait);
}

int
sctp_exdata_req(int fd, ulong flags, const char *dat)
{
	if (!dat)
		return (FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.exdata_req);
	cmd.tpi.type = T_EXDATA_REQ;
	cmd.tpi.exdata_req.MORE_flag = flags & T_MORE;
	return put_msg(fd, 1, MSG_BAND, 0);
}

int
sctp_optdata_req(int fd, ulong flags, const char *dat, int wait)
{
	if (!dat)
		return (FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.optdata_req) + sizeof(opt_data);
	cmd.tpi.type = T_OPTDATA_REQ;
	cmd.tpi.optdata_req.DATA_flag = flags;
	cmd.tpi.optdata_req.OPT_length = sizeof(opt_data);
	cmd.tpi.optdata_req.OPT_offset = sizeof(cmd.tpi.optdata_req);
	bcopy(&opt_data, cmd.cbuf + sizeof(cmd.tpi.optdata_req), sizeof(opt_data));
	return put_msg(fd, flags & T_ODF_EX ? 1 : 0, MSG_BAND, wait);
}

int
sctp_wait(int fd, int wait)
{
	return get_msg(fd, wait);
}

void
sctp_sleep(unsigned long t)
{
	switch (show) {
	case 1:
		printf
		    ("/ / / / / / / / / / / / / / Waiting %04lu seconds / / / / / / / / / / / / / /\n",
		     t);
		break;
	case 2:
		printf("Waiting for %lu seconds...\n", t);
		break;
	}
	sleep(t);
	if (show == 2)
		printf(" ...done\n");
}

int
begin_tests(void)
{
	addr1.port = htons(10000);
	inet_aton("127.0.0.3", &addr1.addr[0]);
	inet_aton("127.0.0.2", &addr1.addr[1]);
	inet_aton("127.0.0.1", &addr1.addr[2]);

	addr2.port = htons(10001);
	inet_aton("127.0.0.3", &addr2.addr[0]);
	inet_aton("127.0.0.2", &addr2.addr[1]);
	inet_aton("127.0.0.1", &addr2.addr[2]);

	addr3.port = htons(10001);
	inet_aton("127.0.0.3", &addr3.addr[0]);
	inet_aton("127.0.0.2", &addr3.addr[1]);
	inet_aton("127.0.0.1", &addr3.addr[2]);

	state = 0;

	return (SUCCESS);
}

int
end_tests(void)
{
	return (SUCCESS);
}

int
preamble_0(void)
{
	if ((fd1 = sctp_t_open()) < 0)
		return (FAILURE);
	if ((fd2 = sctp_t_open()) < 0)
		return (FAILURE);
	if ((fd3 = sctp_t_open()) < 0)
		return (FAILURE);

	return (SUCCESS);
}

int
postamble_0(void)
{
	do {
		if (sctp_close(fd1) == FAILURE)
			break;
		if (sctp_close(fd2) == FAILURE)
			break;
		if (sctp_close(fd3) == FAILURE)
			break;
		return (SUCCESS);
	}
	while (0);
	return (FAILURE);
}

int
preamble_1(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if ((fd1 = sctp_t_open()) < 0)
				break;
			state = 1;
		case 1:
			if ((fd2 = sctp_t_open()) < 0)
				break;
			state = 2;
		case 2:
			if ((fd3 = sctp_t_open()) < 0)
				break;
			state = 3;
		case 3:
			if (sctp_optmgmt_req(fd1, T_NEGOTIATE) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd1, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (sctp_bind_req(fd1, &addr1, 0) == FAILURE)
				break;
			state = 6;
		case 6:
			if (expect(fd1, NORMAL_WAIT, T_BIND_ACK) == FAILURE)
				break;
			state = 7;
		case 7:
			if (sctp_optmgmt_req(fd2, T_NEGOTIATE) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd2, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 9;
		case 9:
			if (sctp_bind_req(fd2, &addr2, 5) == FAILURE)
				break;
			state = 10;
		case 10:
			if (expect(fd2, NORMAL_WAIT, T_BIND_ACK) == FAILURE)
				break;
			state = 11;
		case 11:
			if (sctp_optmgmt_req(fd3, T_NEGOTIATE) == FAILURE)
				break;
			state = 12;
		case 12:
			if (expect(fd3, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 13;
		case 13:
			if (sctp_bind_req(fd3, &addr3, 0) == FAILURE)
				break;
			state = 14;
		case 14:
			if (expect(fd3, NORMAL_WAIT, T_BIND_ACK) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

int
preamble_2(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (preamble_1() == FAILURE)
				break;
			state = 15;
		case 15:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 16;
		case 16:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 17;
		case 17:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 18;
		case 18:
			if (sctp_conn_res(fd2, fd3, NULL) == FAILURE)
				break;
			state = 19;
		case 19:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 20;
		case 20:
			if (expect(fd1, LONG_WAIT, T_CONN_CON) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

int
preamble_2b(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (preamble_1() == FAILURE)
				break;
			state = 15;
		case 15:
			if (sctp_conn_req(fd1, &addr2, "Hello World!") == FAILURE)
				break;
			state = 16;
		case 16:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 17;
		case 17:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 18;
		case 18:
			if (sctp_conn_res(fd2, fd3, "Hello There!") == FAILURE)
				break;
			state = 19;
		case 19:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 20;
		case 20:
			if (expect(fd1, LONG_WAIT, T_CONN_CON) == FAILURE)
				break;
			state = 21;
		case 21:
			if (expect(fd3, NORMAL_WAIT, T_EXDATA_IND) == FAILURE)
				break;
			state = 22;
		case 22:
			if (expect(fd1, NORMAL_WAIT, T_EXDATA_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}
int
preamble_3(void)
{
	return preamble_1();
}

int
preamble_3b(void)
{
	opt_optm.rcv_val = T_YES;
	opt_optm.ist_val = 32;
	opt_optm.ost_val = 32;
	opt_conn.ist_val = 32;
	opt_conn.ost_val = 32;
	return preamble_2();
}

int
preamble_4(void)
{
	opt_optm.dbg_val = SCTP_OPTION_DROPPING;
	return preamble_2();
}

int
preamble_4b(void)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_2();
}

int
preamble_5(void)
{
	// opt_optm.dbg_val = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	opt_optm.dbg_val = SCTP_OPTION_BREAK;
	return preamble_2();
}

int
preamble_6(void)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_3b();
}

int
preamble_7(void)
{
	opt_optm.mac_val = SCTP_HMAC_SHA_1;
	return preamble_1();
}

int
preamble_8(void)
{
	opt_optm.mac_val = SCTP_HMAC_MD5;
	return preamble_1();
}

int
postamble_1(void)
{
	uint failed = -1;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_unbind_req(fd1) == FAILURE) {
				failed = state;
				state = 1;
				continue;
			}
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 1;
		case 1:
			if (sctp_unbind_req(fd2) == FAILURE) {
				failed = state;
				state = 2;
				continue;
			}
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 2;
		case 2:
			if (sctp_unbind_req(fd3) == FAILURE) {
				failed = state;
				state = 3;
				continue;
			}
			if (expect(fd3, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 3;
		case 3:
			if (sctp_close(fd1) == FAILURE)
				failed = state;
			if (sctp_close(fd2) == FAILURE)
				failed = state;
			if (sctp_close(fd3) == FAILURE)
				failed = state;
		}
		opt_data.sid_val = 0;
		opt_optm.mac_val = SCTP_HMAC_NONE;
		opt_optm.dbg_val = 0;
		opt_optm.rcv_val = T_NO;
		opt_optm.ist_val = 1;
		opt_optm.ost_val = 1;
		opt_conn.ist_val = 1;
		opt_conn.ost_val = 1;
		if (failed != -1) {
			state = failed;
			return (FAILURE);
		}
		return (SUCCESS);
	}
}

int
postamble_2(void)
{
	uint failed = -1;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_discon_req(fd1, 0) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 2;
		case 2:
			if (expect(fd3, LONG_WAIT, T_DISCON_IND) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 6;
			continue;
		case 4:
			if (sctp_discon_req(fd3, 0) == FAILURE) {
				failed = state;
				state = 6;
				continue;
			}
			state = 5;
		case 5:
			if (expect(fd3, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 6;
		case 6:
			if (sctp_unbind_req(fd1) == FAILURE)
				failed = state;
			else if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 7;
		case 7:
			if (sctp_unbind_req(fd2) == FAILURE)
				failed = state;
			else if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 8;
		case 8:
			if (sctp_unbind_req(fd3) == FAILURE)
				failed = state;
			else if (expect(fd3, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 9;
		case 9:
			if (sctp_close(fd1) == FAILURE)
				failed = state;
			if (sctp_close(fd2) == FAILURE)
				failed = state;
			if (sctp_close(fd3) == FAILURE)
				failed = state;
		}
		opt_data.sid_val = 0;
		opt_optm.mac_val = SCTP_HMAC_NONE;
		opt_optm.dbg_val = 0;
		opt_optm.rcv_val = T_NO;
		opt_optm.ist_val = 1;
		opt_optm.ost_val = 1;
		opt_conn.ist_val = 1;
		opt_conn.ost_val = 1;
		if (failed != -1) {
			state = failed;
			return (FAILURE);
		}
		return (SUCCESS);
	}
}

int
postamble_3(void)
{
	uint failed = -1;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_ordrel_req(fd1) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 1;
		case 1:
			if (expect(fd3, LONG_WAIT, T_ORDREL_IND) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 2;
		case 2:
			if (sctp_ordrel_req(fd3) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, T_ORDREL_IND) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 6;
			continue;
		case 4:
			if (postamble_2() == FAILURE)
				failed = state;
			break;
		case 6:
			if (sctp_unbind_req(fd1) == FAILURE)
				failed = state;
			else if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 7;
		case 7:
			if (sctp_unbind_req(fd2) == FAILURE)
				failed = state;
			else if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 8;
		case 8:
			if (sctp_unbind_req(fd3) == FAILURE)
				failed = state;
			else if (expect(fd3, SHORT_WAIT, T_OK_ACK) == FAILURE)
				failed = state;
			state = 9;
		case 9:
			if (sctp_close(fd1) == FAILURE)
				failed = state;
			if (sctp_close(fd2) == FAILURE)
				failed = state;
			if (sctp_close(fd3) == FAILURE)
				failed = state;
		}
		opt_data.sid_val = 0;
		opt_optm.mac_val = SCTP_HMAC_NONE;
		opt_optm.dbg_val = 0;
		opt_optm.rcv_val = T_NO;
		opt_optm.ist_val = 1;
		opt_optm.ost_val = 1;
		opt_conn.ist_val = 1;
		opt_conn.ost_val = 1;
		if (failed != -1) {
			state = failed;
			return (FAILURE);
		}
		return (SUCCESS);
	}
}

/* 
 *  Do options management.
 */
#define desc_case_1 "\
Test Case 1(a):\n\
Checks that options management can be performed on several streams\n\
and that one stream can be bound and unbound."
int
test_case_1(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			sctp_optmgmt_req(fd1, T_NEGOTIATE);
			if (expect(fd1, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 1;
		case 1:
			sctp_info_req(fd1);
			if (expect(fd1, NORMAL_WAIT, T_INFO_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_optmgmt_req(fd1, T_CURRENT);
			if (expect(fd1, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			sctp_optmgmt_req(fd2, T_NEGOTIATE);
			if (expect(fd2, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_info_req(fd2);
			if (expect(fd2, NORMAL_WAIT, T_INFO_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			sctp_optmgmt_req(fd2, T_CURRENT);
			if (expect(fd2, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 6;
		case 6:
			sctp_optmgmt_req(fd3, T_NEGOTIATE);
			if (expect(fd3, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 7;
		case 7:
			sctp_info_req(fd3);
			if (expect(fd3, NORMAL_WAIT, T_INFO_ACK) == FAILURE)
				break;
			state = 8;
		case 8:
			sctp_optmgmt_req(fd3, T_CURRENT);
			if (expect(fd3, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Bind and unbind three streams.
 */
#define desc_case_1b "\
Test Case 1(b):\n\
Checks that three streams can be bound and unbound with\n\
one stream as listener."
int
test_case_1b(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			sctp_bind_req(fd1, &addr1, 0);
			if (expect(fd1, NORMAL_WAIT, T_BIND_ACK) == FAILURE)
				break;
			state = 1;
		case 1:
			sctp_bind_req(fd2, &addr2, 5);
			if (expect(fd2, NORMAL_WAIT, T_BIND_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_bind_req(fd3, &addr3, 0);
			if (expect(fd3, NORMAL_WAIT, T_BIND_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			sctp_unbind_req(fd1);
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_unbind_req(fd2);
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			sctp_unbind_req(fd3);
			if (expect(fd3, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Attempt a connection with no listener.
 */
#define desc_case_2 "\
Test Case 2:\n\
Attempts a connection with no listener.  The connection attempt\n\
should time out."
int
test_case_2(void)
{
	int i;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_optmgmt_req(fd1, T_NEGOTIATE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OPTMGMT_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (sctp_bind_req(fd1, &addr1, 0) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, NORMAL_WAIT, T_BIND_ACK) == FAILURE)
				break;
			state = 4;
		case 4:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 6;
		case 6:
			for (i = 0; i < 25; i++)
				if (expect(fd1, LONG_WAIT, T_DISCON_IND) == SUCCESS)
					break;
				else
					sctp_sleep((opt_optm.rmx_val * opt_optm.irt_val +
						    999) / 1000);
			if (i == 25)
				break;
			state = 7;
		case 7:
			if (sctp_unbind_req(fd1) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Attempt and withdraw a connection request.
 */
#define desc_case_2a "\
Test Case 2(a):\n\
Attempts and then withdraws a connection request.  The connection\n\
should disconnect at both ends."
int
test_case_2a(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_discon_req(fd1, 0) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd2, LONG_WAIT, T_DISCON_IND) == FAILURE)
				break;
			state = 6;
		case 6:
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Attempt and refuse a connection request.
 */
#define desc_case_3 "\
Test Case 3:\n\
Attempts a connection which is refused by the receiving end.\n\
The connection should disconnect at the attempting end."
int
test_case_3(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_discon_req(fd2, seq[fd2]);
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, T_DISCON_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Attempt and delayed refuse a connection request.
 */
#define desc_case_3b "\
Test Case 3(b):\n\
Attempts a delayed refusal of a connection requrest.  This delayed\n\
refusal should come after the connector has already timed out."
int
test_case_3b(void)
{
	int i;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 3;
		case 3:
			for (i = 0; i < 25; i++)
				if (expect(fd1, LONG_WAIT, T_DISCON_IND) == SUCCESS)
					break;
				else
					sctp_sleep((opt_optm.rmx_val * opt_optm.irt_val +
						    999) / 1000);
			if (i == 25)
				break;
			state = 4;
		case 4:
			if (sctp_discon_req(fd2, seq[fd2]) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Accept a connection.
 */
#define desc_case_4 "\
Test Case 4:\n\
Accept a connection and then disconnect.  This connection attempt\n\
should be successful."
int
test_case_4(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_conn_res(fd2, fd3, NULL) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd1, LONG_WAIT, T_CONN_CON) == FAILURE)
				break;
			state = 6;
		case 6:
			if (sctp_discon_req(fd3, 0) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd3, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd1, LONG_WAIT, T_DISCON_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Attempt and delayed accept a connection request.
 */
#define desc_case_4b "\
Test Case 4(b):\n\
Attempt a connection and delay the acceptance of the connection request.\n\
This should result in a disconnection indication after the connection is\n\
accepted.  "
int
test_case_4b(void)
{
	int i;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 3;
		case 3:
			for (i = 0; i < 25; i++)
				if (expect(fd1, LONG_WAIT, T_DISCON_IND) == SUCCESS)
					break;
				else
					sctp_sleep((opt_optm.rmx_val * opt_optm.irt_val +
						    999) / 1000 + 1);
			if (i == 25)
				break;
			state = 4;
		case 4:
			if (sctp_conn_res(fd2, fd3, NULL) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 6;
		case 6:
			if (expect(fd3, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 7;
		case 7:
			if (sctp_data_req(fd3, 0, "Hello!", 0) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd3, LONG_WAIT, T_DISCON_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Accept a connection.
 */
#define desc_case_5 "\
Test Case 5:\n\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the SCTP_HMAC_NONE signature on its cookie."
int
test_case_5(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_conn_res(fd2, fd3, NULL) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd1, LONG_WAIT, T_CONN_CON) == FAILURE)
				break;
			state = 6;
		case 6:
			if (sctp_discon_req(fd1, 0) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd3, LONG_WAIT, T_DISCON_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Accept a connection (MD5 hashed cookie)
 */
#define desc_case_5b "\
Test Case 5(b):\n\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the SCTP_HMAC_MD5 signature on its cookie."
int
test_case_5b(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_conn_res(fd2, fd3, NULL) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd1, LONG_WAIT, T_CONN_CON) == FAILURE)
				break;
			state = 6;
		case 6:
			if (sctp_discon_req(fd1, 0) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd3, LONG_WAIT, T_DISCON_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Accept a connection (SHA1 hashed cookie)
 */
#define desc_case_5c "\
Test Case 5(c):\n\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the SCTP_HMAC_SHA_1 signature on its cookie."
int
test_case_5c(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd2, LONG_WAIT, T_CONN_IND) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_conn_res(fd2, fd3, NULL) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd2, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd1, LONG_WAIT, T_CONN_CON) == FAILURE)
				break;
			state = 6;
		case 6:
			if (sctp_discon_req(fd1, 0) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd1, SHORT_WAIT, T_OK_ACK) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd3, LONG_WAIT, T_DISCON_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Connect with data.
 */
#define desc_case_6 "\
Test Case 6:\n\
Attempt and accept a connection where data is also passed in the\n\
connection request and the connection response.  This should result\n\
in DATA chunks being bundled with the COOKIE-ECHO and COOKIE-ACK\n\
chunks in the SCTP messages."
int
test_case_6(void)
{
	int i;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, 0, "Hellos Again!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hellos Again!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hellos Again!", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 3;
		case 3:
			for (i = 0; i < 21; i++)
				if (sctp_data_req(fd3, 0, "Hello Too!", 0) == FAILURE)
					break;
			if (i < 21)
				break;
			state = 4;
		case 4:
			for (i = 0; i < 21; i++)
				if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
					break;
			if (i < 21)
				break;
			state = 5;
		case 5:
			if (expect(fd3, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Connect and send partial data.
 */
#define desc_case_6b "\
Test Case 6(b):\n\
Connect and send partial data (i.e., data with more flag set)."
int
test_case_6b(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello There.", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Connect and send partial data.
 */
#define desc_case_6c "\
Test Case 6(c):\n\
Connect and send partial data and expedited data on multiple streams.\n\
Expedited data should be delivered between ordered data fragments on\n\
the same stream and delivered to the user first."
int
test_case_6c(void)
{
	int i;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			opt_data.sid_val = 0;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 1;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 2;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 3;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 0;
			if (sctp_exdata_req(fd1, 0, "Hello There.") == FAILURE)
				break;
			opt_data.sid_val = 0;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 1;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 2;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 3;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 0;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 1;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 2;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 3;
			if (sctp_optdata_req(fd1, T_MORE, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 0;
			if (sctp_optdata_req(fd1, 0, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 1;
			if (sctp_optdata_req(fd1, 0, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 2;
			if (sctp_optdata_req(fd1, 0, "Hello There.", 0) == FAILURE)
				break;
			opt_data.sid_val = 3;
			if (sctp_optdata_req(fd1, 0, "Hello There.", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, T_OPTDATA_IND) == FAILURE)
				break;
			if (!(cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX))
				break;
			state = 2;
		case 2:
			for (i = 0; i < 16; i++)
				if (expect(fd3, NORMAL_WAIT, T_OPTDATA_IND) == FAILURE)
					break;
			if (i < 16)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Test fragmentation by sending very large packets.
 */
#define desc_case_7 "\
Test Case 7(a):\n\
Connect and send very large packets to test fragmentation."
int
test_case_7(void)
{
	unsigned char lbuf[100000];
	int i = 0, j = 0, k = 0, f = 0;
	size_t len = 0;
	const char nrm[] = "Hello.";
	const char urg[] = "Urgent.";
	bzero(lbuf, sizeof(lbuf));
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (i < 4 && sctp_ndata_req(fd1, 0, lbuf, sizeof(lbuf), 0) == SUCCESS)
				i++;
			if (j < 4 && sctp_exdata_req(fd1, 0, urg) == SUCCESS)
				j++;
			if (k < 4 && sctp_data_req(fd1, 0, nrm, 0) == SUCCESS)
				k++;
			state = 1;
		case 1:
			if (len < i * sizeof(lbuf) + j * sizeof(urg) + k * sizeof(nrm)) {
				int ret;
				switch ((ret = get_msg(fd3, SHORT_WAIT))) {
				case T_EXDATA_IND:
				case T_DATA_IND:
					len += data.len;
				case FAILURE:
					ret = 0;
					break;
				}
				if (ret)
					break;
			}
			if (f++ < 10000) {
				if (i < 4 || j < 4 || k < 4) {
					state = 0;
					continue;
				}
				if (len < i * sizeof(lbuf) + j * sizeof(urg) + k * sizeof(nrm)) {
					state = 1;
					continue;
				}
			} else {
				printf("i = %d, j = %d, k = %d\n", i, j, k);
				printf("Received %u bytes, expecting %u\n", len,
				       i * sizeof(lbuf) + j * sizeof(urg) + k * sizeof(nrm));
				break;
			}
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Test coallescing packets by sending many small fragmented pieces.
 */
#define desc_case_7b "\
Test Case 7(b):\n\
Connect and send many small packets to test coallescing of packets."
int
test_case_7b(void)
{
	int s = 0, r = 0;
	size_t snd_bytes = 0;
	size_t rcv_bytes = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, T_MORE, "Hi There.", 0) == FAILURE)
				break;
			snd_bytes += data.len;
			if (s++ < 4)
				continue;
			print_less(fd1);
			state = 1;
		case 1:
			while (sctp_data_req(fd1, T_MORE, "Hi There.", 0) == SUCCESS) {
				snd_bytes += data.len;
				s++;
			}
			print_more();
			state = 2;
		case 2:
			if (expect(fd3, 0, T_DATA_IND) == FAILURE)
				break;
			rcv_bytes += data.len;
			if (r++ < 4)
				continue;
			print_less(fd3);
			state = 3;
		case 3:
			if (expect(fd3, 0, T_DATA_IND) == FAILURE)
				break;
			rcv_bytes += data.len;
			r++;
			if (data.len != 10) {
				print_more();
				print_msg(fd3);
				state = 5;
				continue;
			}
			if (rcv_bytes < snd_bytes)
				continue;
			state = 4;
		case 4:
			print_more();
			state = 5;
		case 5:
			if (rcv_bytes < snd_bytes) {
				if (expect(fd3, 0, T_DATA_IND) == FAILURE)
					break;
				rcv_bytes += data.len;
				r++;
				continue;
			}
			if (rcv_bytes != snd_bytes)
				break;
			return (SUCCESS);
		}
		printf("Sent %3d messages making %6u bytes.\n", s, snd_bytes);
		printf("Rcvd %3d messages making %6u bytes.\n", r, rcv_bytes);
		print_more();
		return (FAILURE);
	}
}

/* 
 *  Connect with transfer data and orderly release.
 */
#define desc_case_8 "\
Test Case 8(a):\n\
Connect, transfer data and perform orderly release."
int
test_case_8(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 4;
		case 4:
			if (sctp_ordrel_req(fd1) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 6;
		case 6:
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd3, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			state = 8;
		case 8:
			if (sctp_ordrel_req(fd3) == FAILURE)
				break;
			state = 9;
		case 9:
			if (expect(fd1, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Connect with orderly release and late data transfer.
 */
#define desc_case_8b "\
Test Case 8(b):\n\
Connect, transfer data and perform orderly release but transfer\n\
data after release has been initiated"
int
test_case_8b(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 4;
		case 4:
			if (sctp_ordrel_req(fd1) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 6;
		case 6:
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd3, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			state = 8;
		case 8:
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 9;
		case 9:
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 10;
		case 10:
			if (sctp_ordrel_req(fd3) == FAILURE)
				break;
			state = 11;
		case 11:
			if (expect(fd1, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Connect with attempted simultaneous orderly release.
 */
#define desc_case_8c "\
Test Case 8(c):\n\
Connect, transfer data and perform orderly release but attempt\n\
to perform a simultaneous release from both sides.  (This might\n\
or might not result in a simultaneous release attempt.)"
int
test_case_8c(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 3;
		case 3:
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 4;
		case 4:
			if (sctp_ordrel_req(fd1) == FAILURE)
				break;
			state = 5;
		case 5:
			if (sctp_ordrel_req(fd3) == FAILURE)
				break;
			state = 6;
		case 6:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd3, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			state = 9;
		case 9:
			if (expect(fd1, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Connect with orderly release under noise.
 */
#define desc_case_8d "\
Test Case 8(d):\n\
Connect, transfer data and perform orderly release under noise."
int
test_case_8d(void)
{
	int i;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			for (i = 0; i < 20; i++)
				if (sctp_data_req(fd1, 0, "Hello World!", 0) == FAILURE)
					break;
			if (i < 20)
				break;
			state = 3;
		case 3:
			if (sctp_ordrel_req(fd1) == FAILURE)
				break;
			state = 4;
		case 4:
			for (i = 0; i < 20; i++)
				if (sctp_data_req(fd3, 0, "Hello World!", 0) == FAILURE)
					break;
			if (i < 20)
				break;
			state = 5;
		case 5:
			if (sctp_ordrel_req(fd3) == FAILURE)
				break;
			state = 6;
		case 6:
			for (i = 0; i < 20; i++) {
				if (expect(fd3, NORMAL_WAIT, T_DATA_IND) == FAILURE)
					break;
				if (expect(fd1, NORMAL_WAIT, T_DATA_IND) == FAILURE)
					break;
			}
			if (i < 20)
				break;
			state = 7;
		case 7:
			if (expect(fd3, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd1, LONG_WAIT, T_ORDREL_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Delivering ordered data under noise.
 */
#define desc_case_9a "\
Test Case 9(a):\n\
Delivery of ordered data under noise with acknowledgement."
#define TEST_PACKETS 300
int
test_case_9a(void)
{
	int i = 0, j = 0, l = 0, m = 0, f = 0;
	int wait = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (i < TEST_PACKETS) {
				sctp_data_req(fd1, 0, "Pattern-1", 0);
				i++;
			}
			while (j < TEST_PACKETS) {
				switch (get_msg(fd3, wait)) {
				case T_DATA_IND:
					j++;
					continue;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9a_failure;
				}
				break;
			}
			if (l < TEST_PACKETS) {
				sctp_data_req(fd3, 0, "Pattern-3", 0);
				l++;
			}
			while (m < TEST_PACKETS) {
				switch (get_msg(fd1, wait)) {
				case T_DATA_IND:
					m++;
					continue;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9a_failure;
				}
				break;
			}
			if (i >= TEST_PACKETS && l >= TEST_PACKETS)
				wait = 20;
			if (f > TEST_PACKETS * 10)
				goto test_case_9a_failure;
			if (i < TEST_PACKETS || j < TEST_PACKETS || l < TEST_PACKETS ||
			    m < TEST_PACKETS)
				continue;
			return (SUCCESS);
		}
	      test_case_9a_failure:
		printf("%d sent %d inds %d\n", fd1, i, j);
		printf("%d sent %d inds %d\n", fd3, l, m);
		printf("%d failures\n", f);
		return (FAILURE);
	}
}

/* 
 *  Delivering out-of-order data under noise.
 */
#define desc_case_9b "\
Test Case 9(b):\n\
Delivery of un-ordered data under noise."
int
test_case_9b(void)
{
	int i = 0, j = 0, l = 0, m = 0, f = 0;
	int wait = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (i < TEST_PACKETS) {
				sctp_exdata_req(fd1, 0, "Pattern-1");
				i++;
			}
			while (j < TEST_PACKETS) {
				switch (get_msg(fd3, wait)) {
				case T_EXDATA_IND:
					j++;
					continue;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9b_failure;
				}
				break;
			}
			if (l < TEST_PACKETS) {
				sctp_exdata_req(fd3, 0, "Pattern-3");
				l++;
			}
			while (m < TEST_PACKETS) {
				switch (get_msg(fd1, wait)) {
				case T_EXDATA_IND:
					m++;
					continue;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9b_failure;
				}
				break;
			}
			if (i >= TEST_PACKETS && l >= TEST_PACKETS)
				wait = 20;
			if (f > TEST_PACKETS * 10)
				goto test_case_9b_failure;
			if (i < TEST_PACKETS || j < TEST_PACKETS || l < TEST_PACKETS ||
			    m < TEST_PACKETS)
				continue;
			return (SUCCESS);
		}
	      test_case_9b_failure:
		printf("%d sent %d inds %d\n", fd1, i, j);
		printf("%d sent %d inds %d\n", fd3, l, m);
		printf("%d failures\n", f);
		return (FAILURE);
	}
}

#undef TEST_PACKETS

#define TEST_PACKETS 10
#define TEST_STREAMS 32
#define TEST_TOTAL (TEST_PACKETS*TEST_STREAMS)
/* 
 *  Delivering ordered data in multiple streams under noise.
 */
#define desc_case_9c "\
Test Case 9(c):\n\
Delivery of ordered data in multiple streams under noise."
int
test_case_9c(void)
{
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int l[TEST_STREAMS] = { 0, };
	int m[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, L = 0, M = 0;
	int s = 0, f = 0;
	int wait = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
				opt_data.sid_val = s;
				sctp_optdata_req(fd1, 0, "Pattern-1", 0);
				i[s]++;
				I++;
			}
			while (J < TEST_TOTAL) {
				switch (get_msg(fd3, wait)) {
				case T_OPTDATA_IND:
					j[sid[fd3]]++;
					J++;
					continue;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9c_failure;
				}
				break;
			}
			if (L < TEST_TOTAL && l[s] < TEST_PACKETS) {
				opt_data.sid_val = s;
				sctp_optdata_req(fd3, 0, "Pattern-3", 0);
				l[s]++;
				L++;
			}
			while (M < TEST_TOTAL) {
				switch (get_msg(fd1, wait)) {
				case T_OPTDATA_IND:
					m[sid[fd1]]++;
					M++;
					continue;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9c_failure;
				}
				break;
			}
			if (++s >= TEST_STREAMS)
				s = 0;
			if (I >= TEST_TOTAL && L >= TEST_TOTAL)
				wait = 20;
			if (f > TEST_TOTAL * 10)
				goto test_case_9c_failure;
			if (i[s] < TEST_PACKETS || l[s] < TEST_PACKETS)
				continue;
			if (J < TEST_TOTAL || M < TEST_TOTAL)
				continue;
			for (s = 0; s < TEST_STREAMS; s++) {
				if (j[s] != TEST_PACKETS || m[s] != TEST_PACKETS)
					goto test_case_9c_failure;
			}
			return (SUCCESS);
		}
	      test_case_9c_failure:
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d\n", fd1, i[s], j[s]);
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d\n", fd3, l[s], m[s]);
		printf("%d failures\n", f);
		return (FAILURE);
	}
}

/* 
 *  Delivering ordered and unordered data in multiple streams under noise.
 */
#define desc_case_9d "\
Test Case 9(d):\n\
Delivery of ordered and un-ordered data in multiple streams under noise."
int
test_case_9d(void)
{
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int l[TEST_STREAMS] = { 0, };
	int m[TEST_STREAMS] = { 0, };
	int o[TEST_STREAMS] = { 0, };
	int p[TEST_STREAMS] = { 0, };
	int q[TEST_STREAMS] = { 0, };
	int r[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, L = 0, M = 0, O = 0, P = 0, Q = 0, R = 0;
	int s = 0, f = 0;
	int wait = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
				opt_data.sid_val = s;
				sctp_optdata_req(fd1, 0, "Pattern-1", 0);
				i[s]++;
				I++;
			}
			while (J < TEST_TOTAL || P < TEST_TOTAL) {
				switch (get_msg(fd3, wait)) {
				case T_OPTDATA_IND:
					if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX) {
						p[sid[fd3]]++;
						P++;
						continue;
					} else {
						j[sid[fd3]]++;
						J++;
						continue;
					}
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9d_failure;
				}
				break;
			}
			if (O < TEST_TOTAL && o[s] < TEST_PACKETS) {
				opt_data.sid_val = s;
				sctp_optdata_req(fd1, T_ODF_EX, "Pattern-1", 0);
				o[s]++;
				O++;
			}
			while (J < TEST_TOTAL || P < TEST_TOTAL) {
				switch (get_msg(fd3, wait)) {
				case T_OPTDATA_IND:
					if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX) {
						p[sid[fd3]]++;
						P++;
						continue;
					} else {
						j[sid[fd3]]++;
						J++;
						continue;
					}
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9d_failure;
				}
				break;
			}
			if (L < TEST_TOTAL && l[s] < TEST_PACKETS) {
				opt_data.sid_val = s;
				sctp_optdata_req(fd3, 0, "Pattern-3", 0);
				l[s]++;
				L++;
			}
			while (M < TEST_TOTAL || R < TEST_TOTAL) {
				switch (get_msg(fd1, wait)) {
				case T_OPTDATA_IND:
					if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX) {
						r[sid[fd1]]++;
						R++;
						continue;
					} else {
						m[sid[fd1]]++;
						M++;
						continue;
					}
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9d_failure;
				}
				break;
			}
			if (Q < TEST_TOTAL && q[s] < TEST_PACKETS) {
				opt_data.sid_val = s;
				sctp_optdata_req(fd3, T_ODF_EX, "Pattern-3", 0);
				q[s]++;
				Q++;
			}
			while (M < TEST_TOTAL || R < TEST_TOTAL) {
				switch (get_msg(fd1, wait)) {
				case T_OPTDATA_IND:
					if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX) {
						r[sid[fd1]]++;
						R++;
						continue;
					} else {
						m[sid[fd1]]++;
						M++;
						continue;
					}
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9d_failure;
				}
				break;
			}
			if (++s >= TEST_STREAMS)
				s = 0;
			if (I >= TEST_TOTAL && L >= TEST_TOTAL && O >= TEST_TOTAL &&
			    Q >= TEST_TOTAL)
				wait = 20;
			if (f > TEST_TOTAL * 10)
				goto test_case_9d_failure;
			if (i[s] < TEST_PACKETS || l[s] < TEST_PACKETS)
				continue;
			if (o[s] < TEST_PACKETS || q[s] < TEST_PACKETS)
				continue;
			if (J < TEST_TOTAL || M < TEST_TOTAL || P < TEST_TOTAL || R < TEST_TOTAL)
				continue;
			for (s = 0; s < TEST_STREAMS; s++) {
				if (j[s] != TEST_PACKETS
				    || m[s] != TEST_PACKETS || p[s] != TEST_PACKETS ||
				    r[s] != TEST_PACKETS)
					goto test_case_9d_failure;
			}
			return (SUCCESS);
		}
	      test_case_9d_failure:
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d\n", fd1, i[s], j[s]);
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d\n", fd1, o[s], p[s]);
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d\n", fd3, l[s], m[s]);
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d\n", fd3, q[s], r[s]);
		printf("%d failures\n", f);
		return (FAILURE);
	}
}

/* 
 *  Data for destination failure testing.
 */
#define desc_case_10a "\
Test Case 10(a):\n\
Delivery of ordered data with destination failure."
int
test_case_10a(void)
{
	int i, j, k;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			for (j = 0; j < 20; j++) {
				for (i = 0; i < 20; i++) {
					sctp_data_req(fd1, 0, "Test Pattern-1", 0);
					sctp_data_req(fd3, 0, "Test Pattern-3", 0);
				}
				for (i = 0, k = 0; i < 20 || k < 20;) {
					switch (sctp_wait(fd1, SHORT_WAIT)) {
					case T_DATA_IND:
						i++;
					case FAILURE:
						break;
					default:
						return (FAILURE);
					}
					switch (sctp_wait(fd3, SHORT_WAIT)) {
					case T_DATA_IND:
						k++;
					case FAILURE:
						break;
					default:
						return (FAILURE);
					}
				}
			}
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

long
time_sub(struct timeval *t1, struct timeval *t2)
{
	return ((t1->tv_sec - t2->tv_sec) * 1000000 + (t1->tv_usec - t2->tv_usec));
}

/* 
 *  Data for destination failure testing.
 */
#define desc_case_10b "\
Test Case 10(b):\n\
Delivery of ordered data with destination failure."
int
test_case_10b(void)
{
#define SETS 1000
#define REPS 1
	int ret, i, j, k, n = 0;
	struct result {
		uint req_idx;
		struct timeval req;
		uint ind_idx;
		struct timeval ind;
	};
	struct result times[SETS * REPS];
	ulong total = 0;
	bzero(times, sizeof(times));
	state = 0;
	ret = FAILURE;
	for (;;) {
		switch (state) {
		case 0:
			show = 0;
			for (j = 0; j < SETS; j++) {
				for (i = 0; i < REPS; i++) {
					// sctp_data_req(fd1, 0, "This is a much longer test
					// pattern that is being used to see whether we can
					// generate some congestion and it is called Test
					// Pattern-1", 0);
					sctp_data_req(fd1, 0, "Test Pattern-1", 0);
					times[j * REPS + i].req = when;
					times[j * REPS + i].req_idx = n++;
				}
				for (k = 0; k < REPS;) {
					switch (sctp_wait(fd3, SHORT_WAIT)) {
					case T_DATA_IND:
						times[j * REPS + k].ind = when;
						times[j * REPS + k].ind_idx = n++;
						k++;
						break;
					case FAILURE:
						break;
					default:
						show = 1;
						goto test_case_10b_failed;
					}
				}
			}
			show = 1;
			ret = SUCCESS;
		}
	      test_case_10b_failed:
		k = 0;
		for (n = 0; n < 2 * SETS * REPS; n++) {
			for (i = 0; i < SETS * REPS; i++) {
				if (times[i].req_idx == n) {
					printf
					    ("T_DATA_REQ    ----->|--------------->|                 |                    \n");
					break;
				}
				if (times[i].ind_idx == n) {
					printf
					    ("                    |                |-%9ld usec->|-----> T_DATA_IND   \n",
					     time_sub(&times[i].ind, &times[i].req));
					k++;
					total += time_sub(&times[i].ind, &times[i].req);
					break;
				}
			}
		}
		total /= k;
		printf
		    ("                    |                |                 |                    \n");
		printf("                    |        average = %9ld usec  |                    \n",
		       total);
		printf
		    ("                    |                |                 |                    \n");
		return (ret);
	}
}

struct test_case {
	const char *name;
	int (*preamble) (void);
	int (*testcase) (void);
	int (*postamble) (void);
} tests[] = {
	{
	desc_case_1, &preamble_0, &test_case_1, &postamble_0}
	, {
	desc_case_1b, &preamble_0, &test_case_1b, &postamble_0}
	, {
	desc_case_2, &preamble_0, &test_case_2, &postamble_0}
	, {
	desc_case_2a, &preamble_1, &test_case_2a, &postamble_1}
	, {
	desc_case_3, &preamble_1, &test_case_3, &postamble_1}
	, {
	desc_case_3b, &preamble_1, &test_case_3b, &postamble_1}
	, {
	desc_case_4, &preamble_1, &test_case_4, &postamble_1}
	, {
	desc_case_4b, &preamble_1, &test_case_4b, &postamble_1}
	, {
	desc_case_5, &preamble_1, &test_case_5, &postamble_1}
	, {
	desc_case_5b, &preamble_8, &test_case_5b, &postamble_1}
	, {
	desc_case_5c, &preamble_7, &test_case_5c, &postamble_1}
	, {
	desc_case_6, &preamble_2b, &test_case_6, &postamble_3}
	, {
	desc_case_6b, &preamble_2, &test_case_6b, &postamble_3}
	, {
	desc_case_6c, &preamble_3b, &test_case_6c, &postamble_3}
	, {
	desc_case_7, &preamble_2, &test_case_7, &postamble_3}
	, {
	desc_case_7b, &preamble_2, &test_case_7b, &postamble_3}
	, {
	desc_case_8, &preamble_2, &test_case_8, &postamble_1}
	, {
	desc_case_8b, &preamble_2, &test_case_8b, &postamble_1}
	, {
	desc_case_8c, &preamble_2, &test_case_8c, &postamble_1}
	, {
	desc_case_8d, &preamble_4b, &test_case_8d, &postamble_1}
	, {
	desc_case_9a, &preamble_4, &test_case_9a, &postamble_3}
	, {
	desc_case_9b, &preamble_4, &test_case_9b, &postamble_3}
	, {
	desc_case_9c, &preamble_6, &test_case_9c, &postamble_3}
	, {
	desc_case_9d, &preamble_6, &test_case_9d, &postamble_3}
	, {
	desc_case_10a, &preamble_5, &test_case_10a, &postamble_3}
	, {
	desc_case_10b, &preamble_5, &test_case_10b, &postamble_3}
};

int
do_tests(void)
{
	int i;
	int result = INCONCLUSIVE;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	printf("Simple test program for streams-sctp driver.\n");
	if (begin_tests() == SUCCESS) {
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)); i++) {
			printf("\n%s\n", tests[i].name);
			printf
			    ("---------------------------------Preamble-----------------------------------\n");
			if (tests[i].preamble() != SUCCESS) {
				printf
				    ("                    |???????? INCONCLUSIVE %2d ??????|??|                    \n",
				     state);
				result = INCONCLUSIVE;
			} else {
				printf
				    ("--------------------|--------------Test-------------|--|--------------------\n");
				if (tests[i].testcase() == FAILURE) {
					printf
					    ("                    |XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    \n");
					result = FAILURE;
				} else {
					printf
					    ("                    |************ PASSED ***********|**|                    \n");
					result = SUCCESS;
				}
			}
			printf
			    ("--------------------|------------Postamble----------|--|--------------------\n");
			if (tests[i].postamble() != SUCCESS) {
				printf
				    ("                    |???????? INCONCLUSIVE %2d ??????|??|                    \n",
				     state);
				if (result == SUCCESS)
					result = INCONCLUSIVE;
			}
			printf
			    ("----------------------------------------------------------------------------\n");
			switch (result) {
			case SUCCESS:
				successes++;
				printf("*********\n");
				printf("********* Test Case SUCCESSFUL\n");
				printf("*********\n\n");
				break;
			case FAILURE:
				failures++;
				printf("XXXXXXXXX\n");
				printf("XXXXXXXXX Test Case FAILED\n");
				printf("XXXXXXXXX\n\n");
				break;
			default:
			case INCONCLUSIVE:
				inconclusive++;
				printf("?????????\n");
				printf("????????? Test Case INCONCLUSIVE\n");
				printf("?????????\n\n");
				break;
			}
		}
	} else
		printf("Test setup failed!\n");
	end_tests();
	printf("Done.\n\n");
	printf("========= %2d successes   \n", successes);
	printf("========= %2d failures    \n", failures);
	printf("========= %2d inconclusive\n", inconclusive);
	return (0);
}

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
RFC 2960 SCTP - OpenSS7 STREAMS SCTP - Conformance Test Suite\n\
\n\
Copyright (c) 2001-2005 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
General Public License  (GPL)  Version 2  or later,  so long as  the software is\n\
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
");
}

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!verbose)
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
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        increase verbosity or set to LEVEL [default: 1]\n\
        this option may be repeated.\n\
    -h, --help, -?, --?\n\
        print this usage message and exit\n\
    -V, --version\n\
        print the version and exit\n\
    -C, --copying\n\
        print copying permissions and exit\n\
", argv[0]);
}

int
main(int argc, char *argv[])
{
	for (;;) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{ 0, }
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "qvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'v':
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 'H':	/* -H */
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
			exit(0);
		case 'C':
			copying(argc, argv);
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
		      bad_usage:
			usage(argc, argv);
			exit(2);
		}
	}
	/* 
	 * dont' ignore non-option arguments
	 */
	if (optind < argc)
		goto bad_nonopt;
	copying(argc, argv);
	do_tests();
	exit(0);
}
