/*****************************************************************************

 @(#) $RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 08:41:40 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002 OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@dallas.net>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written
 authorization of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that
 the recipient will protect this information and keep it confidential, and
 will not disclose the information contained in this document without the
 written permission of its owner.

 The author reserves the right to revise this software and documentation
 for any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of
 the technical arts, or the reflection of changes in the design of any
 techniques, or procedures embodied, described, or referred to herein.
 The author is under no obligation to provide any feature listed herein.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"),
 it is classified as "Commercial Computer Software" under paragraph
 252.227-7014 of the DoD Supplement to the Federal Acquisition Regulations
 ("DFARS") (or any successor regulations) and the Government is acquiring
 only the license rights granted herein (the license rights customarily
 provided to non-Government users).  If the Software is supplied to any unit
 or agency of the Government other than DoD, it is classified as "Restricted
 Computer Software" and the Government's rights in the Software are defined
 in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR") (or
 any success regulations) or, in the cases of NASA, in paragraph 18.52.227-86
 of the NASA Supplement to the FAR (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/06/22 08:41:40 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 08:41:40 $"

static char const ident[] =
    "$RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 08:41:40 $";

/* 
 *  This file is for testing the sctp_n driver.  It is provided for the
 *  purpose of testing the OpenSS7 sctp_n driver only.
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
#include "../include/sys/npi.h"
#include "../include/sys/npi_sctp.h"
#else
#include <sys/npi.h>
#include <sys/npi_sctp.h>
#endif

static int verbose = 1;
static int summary = 0;
static int timer_scale = 1;
static int show_msg = 0;

#define BUFSIZE 5*4096
#define FFLUSH(stream)

#define SHORT_WAIT 10
#define NORMAL_WAIT 100
#define LONG_WAIT 500

ulong seq[10] = { 0, };
ulong tok[10] = { 0, };
ulong tsn[10] = { 0, };
ulong sid[10] = { 0, };

union {
	np_ulong prim;
	union N_primitives npi;
	char cbuf[BUFSIZE];
} cmd;

char dbuf[BUFSIZE];
struct strbuf ctrl = { BUFSIZE, 0, cmd.cbuf };
struct strbuf data = { BUFSIZE, 0, dbuf };

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
N_qos_sel_data_sctp_t qos_data = {
	N_QOS_SEL_DATA_SCTP,		/* n_qos_type */
	10,				/* ppi */
	0,				/* sid */
	0,				/* ssn */
	0,				/* tsn */
	0				/* more */
};
N_qos_sel_conn_sctp_t qos_conn = {
	N_QOS_SEL_CONN_SCTP,		/* n_qos_type */
	1,				/* i_streams */
	1				/* o_streams */
};

N_qos_sel_info_sctp_t qos_info = {
	N_QOS_SEL_INFO_SCTP,		/* n_qos_type */
	1,				/* i_streams */
	1,				/* o_streams */
	10,				/* ppi */
	0,				/* sid */
	12,				/* max_inits */
	12,				/* max_retrans */
	-1UL,				/* ck_life */
	-1UL,				/* ck_inc */
	-1UL,				/* hmac */
	-1UL,				/* throttle */
	0,				/* max_sack */
	0,				/* rto_ini */
	0,				/* rto_min */
	0,				/* rto_max */
	0,				/* rtx_path */
	200,				/* hb_itvl */
	0				/* options */
};

char *
err_string(ulong error)
{
	switch (error) {
	case NBADADDR:
		return ("NBADADDR");
	case NBADOPT:
		return ("NBADOPT");
	case NACCESS:
		return ("NACCESS");
	case NNOADDR:
		return ("NNOADDR");
	case NOUTSTATE:
		return ("NOUTSTATE");
	case NBADSEQ:
		return ("NBADSEQ");
	case NSYSERR:
		return ("NSYSERR");
	case NBADDATA:
		return ("NBADDATA");
	case NBADFLAG:
		return ("NBADFLAG");
	case NNOTSUPPORT:
		return ("NNOTSUPPORT");
	case NBOUND:
		return ("NBOUND");
	case NBADQOSPARAM:
		return ("NBADQOSPARAM");
	case NBADQOSTYPE:
		return ("NBADQOSTYPE");
	case NBADTOKEN:
		return ("NBADTOKEN");
	case NNOPROTOID:
		return ("NNOPROTOID");
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
	case N_CONN_REQ:
		return ("N_CONN_REQ");
	case N_CONN_RES:
		return ("N_CONN_RES");
	case N_DISCON_REQ:
		return ("N_DISCON_REQ");
	case N_DATA_REQ:
		return ("N_DATA_REQ");
	case N_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case N_INFO_REQ:
		return ("N_INFO_REQ");
	case N_BIND_REQ:
		return ("N_BIND_REQ");
	case N_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case N_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case N_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case N_DATACK_REQ:
		return ("N_DATACK_REQ");
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_CONN_IND:
		return ("N_CONN_IND");
	case N_CONN_CON:
		return ("N_CONN_CON");
	case N_DISCON_IND:
		return ("N_DISCON_IND");
	case N_DATA_IND:
		return ("N_DATA_IND");
	case N_EXDATA_IND:
		return ("N_EXDATA_IND");
	case N_INFO_ACK:
		return ("N_INFO_ACK");
	case N_BIND_ACK:
		return ("N_BIND_ACK");
	case N_OK_ACK:
		return ("N_OK_ACK");
	case N_ERROR_ACK:
		return ("N_ERROR_ACK");
	case N_DATACK_IND:
		return ("N_DATACK_IND");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_CON:
		return ("N_RESET_CON");
	case FAILURE:
		return ("(nothing)");
	default:
		return ("(unexpected)");
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
	case NS_UNBND:
		printf("NS_UNBND");
		break;
	case NS_WACK_BREQ:
		printf("NS_WACK_BREQ");
		break;
	case NS_WACK_UREQ:
		printf("NS_WACK_UREQ");
		break;
	case NS_IDLE:
		printf("NS_IDLE");
		break;
	case NS_WACK_OPTREQ:
		printf("NS_WACK_OPTREQ");
		break;
	case NS_WACK_RRES:
		printf("NS_WACK_RRES");
		break;
	case NS_WCON_CREQ:
		printf("NS_WCON_CREQ");
		break;
	case NS_WRES_CIND:
		printf("NS_WRES_CIND");
		break;
	case NS_WACK_CRES:
		printf("NS_WACK_CRES");
		break;
	case NS_DATA_XFER:
		printf("NS_DATA_XFER");
		break;
	case NS_WCON_RREQ:
		printf("NS_WCON_RREQ");
		break;
	case NS_WRES_RIND:
		printf("NS_WRES_RIND");
		break;
	case NS_WACK_DREQ6:
		printf("NS_WACK_DREQ6");
		break;
	case NS_WACK_DREQ7:
		printf("NS_WACK_DREQ7");
		break;
	case NS_WACK_DREQ9:
		printf("NS_WACK_DREQ9");
		break;
	case NS_WACK_DREQ10:
		printf("NS_WACK_DREQ10");
		break;
	case NS_WACK_DREQ11:
		printf("NS_WACK_DREQ11");
		break;
	default:
		printf("(unknown [%lu])", state);
		break;
	}
	printf("\n");
	return;
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
			       (a->addr[i] >> 8) & 0xff, (a->addr[i] >> 16) & 0xff,
			       (a->addr[i] >> 24) & 0xff);
		}
	} else
		printf("(no address)");
	printf("\n");
}

void
print_proto(char *pro_ptr, size_t pro_len)
{
	uint32_t *p = (uint32_t *) pro_ptr;
	size_t pnum = pro_len / sizeof(p[0]);
	if (pro_len) {
		int i;
		if (!pnum)
			printf("(PROTOID_length = %d)", pro_len);
		for (i = 0; i < pnum; i++) {
			printf("%s%d", i ? "," : "", p[i]);
		}
	} else
		printf("(no protoids)");
	printf("\n");
}

void
print_qos(char *qos_ptr, size_t qos_len)
{
	N_qos_sctp_t *qos = (N_qos_sctp_t *) qos_ptr;
	if (qos_len) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_CONN_SCTP:
			printf("CONN:");
			printf(" i_streams=%ld,", (long) qos->n_qos_conn.i_streams);
			printf(" o_streams=%ld ", (long) qos->n_qos_conn.o_streams);
			break;

		case N_QOS_SEL_DATA_SCTP:
			printf("DATA: ");
			printf(" ppi=%ld,", (long) qos->n_qos_data.ppi);
			printf(" sid=%ld,", (long) qos->n_qos_data.sid);
			printf(" ssn=%ld,", (long) qos->n_qos_data.ssn);
			printf(" tsn=%lu,", (ulong) qos->n_qos_data.tsn);
			printf(" more=%ld", (long) qos->n_qos_data.more);
			break;

		case N_QOS_SEL_INFO_SCTP:
			printf("INFO: ");
			printf(" i_streams=%ld,", (long) qos->n_qos_info.i_streams);
			printf(" o_streams=%ld,", (long) qos->n_qos_info.o_streams);
			printf(" ppi=%ld,", (long) qos->n_qos_info.ppi);
			printf(" sid=%ld,", (long) qos->n_qos_info.sid);
			printf("\n                     ");
			printf(" max_inits=%ld,", (long) qos->n_qos_info.max_inits);
			printf(" max_retrans=%ld,", (long) qos->n_qos_info.max_retrans);
			printf(" max_sack=%ld,", (long) qos->n_qos_info.max_sack);
			printf("\n                     ");
			printf(" ck_life=%ld,", (long) qos->n_qos_info.ck_life);
			printf(" ck_inc=%ld,", (long) qos->n_qos_info.ck_inc);
			printf(" hmac=%ld,", (long) qos->n_qos_info.hmac);
			printf(" throttle=%ld,", (long) qos->n_qos_info.throttle);
			printf("\n                     ");
			printf(" rto_ini=%ld,", (long) qos->n_qos_info.rto_ini);
			printf(" rto_min=%ld,", (long) qos->n_qos_info.rto_min);
			printf(" rto_max=%ld,", (long) qos->n_qos_info.rto_max);
			printf(" rtx_path=%ld,", (long) qos->n_qos_info.rtx_path);
			printf(" hb_itvl=%ld", (long) qos->n_qos_info.hb_itvl);
			printf("\n                     ");
			printf(" options=");
			if (!qos->n_qos_info.options)
				printf("(none)");
			if (qos->n_qos_info.options & SCTP_OPTION_DROPPING)
				printf(" DEBUG-DROPPING");
			if (qos->n_qos_info.options & SCTP_OPTION_BREAK)
				printf(" DEBUG-BREAK");
			if (qos->n_qos_info.options & SCTP_OPTION_DBREAK)
				printf(" DEBUG-DBREAK");
			if (qos->n_qos_info.options & SCTP_OPTION_RANDOM)
				printf(" DEBUG-RANDOM");
			break;

		case N_QOS_RANGE_INFO_SCTP:
			printf("RANGE: ");
			break;

		default:
			printf("(unknown qos structure %lu)\n", qos->n_qos_type);
			break;
		}
	} else
		printf("(no qos)");
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
			switch (cmd.prim) {
			case N_INFO_REQ:
				printf("%d-N_INFO_REQ:\n", fd);
				break;

			case N_INFO_ACK:
				printf("%d-N_INFO_ACK:\n", fd);
				printf("  NSDU_size      = ");
				print_size(cmd.npi.info_ack.NSDU_size);
				printf("  ENSDU_size     = ");
				print_size(cmd.npi.info_ack.ENSDU_size);
				printf("  CDATA_size     = ");
				print_size(cmd.npi.info_ack.CDATA_size);
				printf("  DDATA_size     = ");
				print_size(cmd.npi.info_ack.DDATA_size);
				printf("  ADDR_size      = ");
				print_size(cmd.npi.info_ack.ADDR_size);
				printf("  ADDR           = ");
				print_addr(cmd.cbuf + cmd.npi.info_ack.ADDR_offset,
					   cmd.npi.info_ack.ADDR_length);
				printf("  QOS            = ");
				print_qos(cmd.cbuf + cmd.npi.info_ack.QOS_offset,
					  cmd.npi.info_ack.QOS_length);
				printf("  QOR            = ");
				print_qos(cmd.cbuf + cmd.npi.info_ack.QOS_range_offset,
					  cmd.npi.info_ack.QOS_range_length);
				printf("  OPTIONS_flags  =");
				if (cmd.npi.info_ack.OPTIONS_flags & REC_CONF_OPT)
					printf(" REC_CONF_OPT");
				if (cmd.npi.info_ack.OPTIONS_flags & EX_DATA_OPT)
					printf(" EX_DATA_OPT");
				printf("\n");
				printf("  NIDU_size      = ");
				print_size(cmd.npi.info_ack.NIDU_size);
				printf("  SERV_type      =");
				if (cmd.npi.info_ack.SERV_type & N_CONS)
					printf(" N_CONS");
				if (cmd.npi.info_ack.SERV_type & N_CLNS)
					printf(" N_CLNS");
				printf("\n");
				printf("  CURRENT_state  = ");
				print_state(cmd.npi.info_ack.CURRENT_state);
				printf("  PROVIDER_type  = ");
				switch (cmd.npi.info_ack.PROVIDER_type) {
				case N_SNICFP:
					printf("N_SNICFP\n");
					break;
				case N_SUBNET:
					printf("N_SUBNET\n");
					break;
				default:
					printf("(unknown %lu)\n", cmd.npi.info_ack.PROVIDER_type);
					break;
				}
				printf("  NODU_size      = ");
				print_size(cmd.npi.info_ack.NODU_size);
				printf("  PROTOID        = ");
				print_proto(cmd.cbuf + cmd.npi.info_ack.PROTOID_offset,
					    cmd.npi.info_ack.PROTOID_length);
				printf("  NPI_version    = %lu\n", cmd.npi.info_ack.NPI_version);
				break;

			case N_OPTMGMT_REQ:
				printf("%d-N_OPTMGMT_REQ:\n", fd);
				printf("  QOS            = ");
				print_qos(cmd.cbuf + cmd.npi.optmgmt_req.QOS_offset,
					  cmd.npi.optmgmt_req.QOS_length);
				printf("  OPTMGMT_flags  = ");
				if (cmd.npi.optmgmt_req.OPTMGMT_flags & DEFAULT_RC_SEL)
					printf(" DEFAULT_RC_SEL");
				printf("\n");
				break;

			case N_BIND_REQ:
				printf("%d-N_BIND_REQ:\n", fd);
				printf("  ADDR           = ");
				print_addr(cmd.cbuf + cmd.npi.bind_req.ADDR_offset,
					   cmd.npi.bind_req.ADDR_length);
				printf("  CONIND_number  = %lu\n", cmd.npi.bind_req.CONIND_number);
				printf("  BIND_flags     =");
				if (cmd.npi.bind_req.BIND_flags & DEFAULT_LISTENER)
					printf(" DEFAULT_LISTENER");
				if (cmd.npi.bind_req.BIND_flags & TOKEN_REQUEST)
					printf(" TOKEN_REQUEST");
				if (cmd.npi.bind_req.BIND_flags & DEFAULT_DEST)
					printf(" DEFAULT_DEST");
				printf("\n");
				printf("  PROTOID        = ");
				print_proto(cmd.cbuf + cmd.npi.bind_req.PROTOID_offset,
					    cmd.npi.bind_req.PROTOID_length);
				break;

			case N_BIND_ACK:
				printf("%d-N_BIND_ACK:\n", fd);
				printf("  ADDR           = ");
				print_addr(cmd.cbuf + cmd.npi.bind_ack.ADDR_offset,
					   cmd.npi.bind_ack.ADDR_length);
				printf("  CONIND_number  = %lu\n", cmd.npi.bind_ack.CONIND_number);
				printf("  TOKEN_value    = %lu\n", cmd.npi.bind_ack.TOKEN_value);
				printf("  PROTOID        = ");
				print_proto(cmd.cbuf + cmd.npi.bind_ack.PROTOID_offset,
					    cmd.npi.bind_ack.PROTOID_length);
				tok[fd] = cmd.npi.bind_ack.TOKEN_value;
				break;

			case N_UNBIND_REQ:
				printf("%d-N_UNBIND_REQ:\n", fd);
				break;

			case N_ERROR_ACK:
				printf("%d-N_ERROR_ACK:\n", fd);
				printf("  ERROR_prim     = ");
				print_prim(cmd.npi.error_ack.ERROR_prim);
				printf("\n");
				printf("  NPI_error      = ");
				print_error(cmd.npi.error_ack.NPI_error);
				printf("  UNIX_error     = %lu (%s)\n",
				       cmd.npi.error_ack.UNIX_error,
				       strerror(cmd.npi.error_ack.UNIX_error));
				break;

			case N_OK_ACK:
				printf("%d-N_OK_ACK:\n", fd);
				printf("  CORRECT_prim   = ");
				print_prim(cmd.npi.error_ack.ERROR_prim);
				printf("\n");
				break;

			case N_CONN_REQ:
				printf("%d-N_CONN_REQ:\n", fd);
				printf("  DEST           = ");
				print_addr(cmd.cbuf + cmd.npi.conn_req.DEST_offset,
					   cmd.npi.conn_req.DEST_length);
				printf("  CONN_flags     =");
				if (cmd.npi.conn_req.CONN_flags & REC_CONF_OPT)
					printf(" REC_CONF_OPT");
				if (cmd.npi.conn_req.CONN_flags & EX_DATA_OPT)
					printf(" EX_DATA_OPT");
				printf("\n");
				printf("  QOS            = ");
				print_qos(cmd.cbuf + cmd.npi.conn_req.QOS_offset,
					  cmd.npi.conn_req.QOS_length);
				if (data.len)
					printf("  DATA           = %s\n", dbuf);
				break;

			case N_CONN_IND:
				printf("%d-N_CONN_IND:\n", fd);
				printf("  DEST           = ");
				print_addr(cmd.cbuf + cmd.npi.conn_ind.DEST_offset,
					   cmd.npi.conn_ind.DEST_length);
				printf("  SRC            = ");
				print_addr(cmd.cbuf + cmd.npi.conn_ind.SRC_offset,
					   cmd.npi.conn_ind.SRC_length);
				printf("  SEQ_number     = %lx\n", cmd.npi.conn_ind.SEQ_number);
				printf("  CONN_flags     =");
				if (cmd.npi.conn_ind.CONN_flags & REC_CONF_OPT)
					printf(" REC_CONF_OPT");
				if (cmd.npi.conn_ind.CONN_flags & EX_DATA_OPT)
					printf(" EX_DATA_OPT");
				printf("\n");
				printf("  QOS            = ");
				print_qos(cmd.cbuf + cmd.npi.conn_ind.QOS_offset,
					  cmd.npi.conn_ind.QOS_length);
				seq[fd] = cmd.npi.conn_ind.SEQ_number;
				break;

			case N_CONN_RES:
				printf("%d-N_CONN_RES:\n", fd);
				printf("  TOKEN_value    = %lu\n", cmd.npi.conn_res.TOKEN_value);
				printf("  RES            = ");
				print_addr(cmd.cbuf + cmd.npi.conn_res.RES_offset,
					   cmd.npi.conn_res.RES_length);
				printf("  SEQ_number     = %lx\n", cmd.npi.conn_res.SEQ_number);
				printf("  CONN_flags     =");
				if (cmd.npi.conn_res.CONN_flags & REC_CONF_OPT)
					printf(" REC_CONF_OPT");
				if (cmd.npi.conn_res.CONN_flags & EX_DATA_OPT)
					printf(" EX_DATA_OPT");
				printf("\n");
				printf("  QOS            = ");
				print_qos(cmd.cbuf + cmd.npi.conn_res.QOS_offset,
					  cmd.npi.conn_res.QOS_length);
				if (data.len)
					printf("  DATA           = %s\n", dbuf);
				break;

			case N_CONN_CON:
				printf("%d-N_CONN_CON:\n", fd);
				printf("  RES            = ");
				print_addr(cmd.cbuf + cmd.npi.conn_con.RES_offset,
					   cmd.npi.conn_con.RES_length);
				printf("  CONN_flags     =");
				if (cmd.npi.conn_con.CONN_flags & REC_CONF_OPT)
					printf(" REC_CONF_OPT");
				if (cmd.npi.conn_con.CONN_flags & EX_DATA_OPT)
					printf(" EX_DATA_OPT");
				printf("\n");
				printf("  QOS            = ");
				print_qos(cmd.cbuf + cmd.npi.conn_con.QOS_offset,
					  cmd.npi.conn_res.QOS_length);

				break;

			case N_DATA_REQ:
				printf("%d-N_DATA_REQ:\n", fd);
				printf("  DATA_xfer_flags=");
				if (cmd.npi.data_req.DATA_xfer_flags & N_MORE_DATA_FLAG)
					printf(" N_MORE_DATA_FLAG");
				if (cmd.npi.data_req.DATA_xfer_flags & N_RC_FLAG)
					printf(" N_RC_FLAG");
				printf("\n");
				printf("  QOS            = ");
				print_qos(cmd.cbuf + sizeof(cmd.npi.data_req),
					  ctrl.len - sizeof(cmd.npi.data_req));
				if (data.len)
					printf("  DATA           = %s\n", dbuf);
				break;

			case N_DATA_IND:
				printf("%d-N_DATA_IND:\n", fd);
				printf("  DATA_xfer_flags=");
				if (cmd.npi.data_ind.DATA_xfer_flags & N_MORE_DATA_FLAG)
					printf(" N_MORE_DATA_FLAG");
				if (cmd.npi.data_ind.DATA_xfer_flags & N_RC_FLAG)
					printf(" N_RC_FLAG");
				printf("\n");
				printf("  QOS            = ");
				print_qos(cmd.cbuf + sizeof(cmd.npi.data_ind),
					  ctrl.len - sizeof(cmd.npi.data_ind));
				tsn[fd] =
				    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
								sizeof(cmd.npi.data_ind)))->tsn;
				sid[fd] =
				    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
								sizeof(cmd.npi.data_ind)))->sid;
				if (data.len)
					printf("  DATA           = %s\n", dbuf);
				break;

			case N_EXDATA_REQ:
				printf("%d-N_EXDATA_REQ:\n", fd);
				printf("  QOS            = ");
				print_qos(cmd.cbuf + sizeof(cmd.npi.exdata_req),
					  ctrl.len - sizeof(cmd.npi.exdata_req));
				if (data.len)
					printf("  DATA           = %s\n", dbuf);
				break;

			case N_EXDATA_IND:
				printf("%d-N_EXDATA_IND:\n", fd);
				printf("  QOS            = ");
				print_qos(cmd.cbuf + sizeof(cmd.npi.exdata_ind),
					  ctrl.len - sizeof(cmd.npi.exdata_ind));
				tsn[fd] =
				    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
								sizeof(cmd.npi.exdata_ind)))->tsn;
				sid[fd] =
				    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
								sizeof(cmd.npi.exdata_ind)))->sid;
				if (data.len)
					printf("  DATA           = %s\n", dbuf);
				break;

			case N_DATACK_REQ:
				printf("%d-N_DATACK_REQ:\n", fd);
				printf("  QOS            = ");
				print_qos(cmd.cbuf + sizeof(cmd.npi.datack_req),
					  ctrl.len - sizeof(cmd.npi.datack_req));
				break;

			case N_DATACK_IND:
				printf("%d-N_DATACK_IND:\n", fd);
				printf("  QOS            = ");
				print_qos(cmd.cbuf + sizeof(cmd.npi.datack_ind),
					  ctrl.len - sizeof(cmd.npi.datack_ind));
				tsn[fd] =
				    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
								sizeof(cmd.npi.datack_ind)))->tsn;
				sid[fd] =
				    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
								sizeof(cmd.npi.datack_ind)))->sid;
				break;

			case N_RESET_REQ:
				printf("%d-N_RESET_REQ:\n", fd);
				printf("  RESET_reason   = %lu\n", cmd.npi.reset_req.RESET_reason);
				break;

			case N_RESET_IND:
				printf("%d-N_RESET_IND:\n", fd);
				printf("  RESET_orig     = %lu\n", cmd.npi.reset_ind.RESET_orig);
				printf("  RESET_reason   = %lu\n", cmd.npi.reset_ind.RESET_reason);
				break;

			case N_RESET_RES:
				printf("%d-N_RESET_RES:\n", fd);
				break;

			case N_RESET_CON:
				printf("%d-N_RESET_CON:\n", fd);
				break;

			case N_DISCON_REQ:
				printf("%d-N_DISCON_REQ:\n", fd);
				printf("  DISCON_reason  = %lu\n",
				       cmd.npi.discon_req.DISCON_reason);
				printf("  RES            = ");
				print_addr(cmd.cbuf + cmd.npi.discon_req.RES_offset,
					   cmd.npi.discon_req.RES_length);
				printf("  SEQ_number     = %lx\n", cmd.npi.discon_req.SEQ_number);
				break;

			case N_DISCON_IND:
				printf("%d-N_DISCON_IND:\n", fd);
				printf("  DISCON_orig    = %lu\n", cmd.npi.discon_ind.DISCON_orig);
				printf("  DISCON_reason  = %lu\n",
				       cmd.npi.discon_ind.DISCON_reason);
				printf("  RES            = ");
				print_addr(cmd.cbuf + cmd.npi.discon_ind.RES_offset,
					   cmd.npi.discon_ind.RES_length);
				printf("  SEQ_number     = %lx\n", cmd.npi.discon_ind.SEQ_number);
				break;

			default:
				printf("Unrecognized primitive %lu!\n", cmd.prim);
				break;
			}
			break;
		case 1:
			if (fd == fd1) {
				switch (cmd.npi.type) {
				case N_INFO_REQ:
					printf
					    ("N_INFO_REQ    ----->|                               |  |                    \n");
					break;
				case N_INFO_ACK:
					printf
					    ("N_INFO_ACK    <-----|                               |  |                    \n");
					break;
				case N_OPTMGMT_REQ:
					printf
					    ("N_OPTMGMT_REQ ----->|                               |  |                    \n");
					break;
				case N_BIND_REQ:
					printf
					    ("N_BIND_REQ    ----->|                               |  |                    \n");
					break;
				case N_BIND_ACK:
					printf
					    ("N_BIND_ACK    <-----|                               |  |                    \n");
					tok[fd] = cmd.npi.bind_ack.TOKEN_value;
					break;
				case N_UNBIND_REQ:
					printf
					    ("N_UNBIND_REQ  ----->|                               |  |                    \n");
					break;
				case N_ERROR_ACK:
					printf
					    ("N_ERROR_ACK   <----/|                               |  |                    \n");
					printf
					    ("[%-11s]       |                               |  |                    \n",
					     err_string(cmd.npi.error_ack.NPI_error));
					break;
				case N_OK_ACK:
					printf
					    ("N_OK_ACK      <----/|                               |  |                    \n");
					break;
				case N_CONN_REQ:
					printf
					    ("N_CONN_REQ    ----->|--------------->|              |  |                    \n");
					break;
				case N_CONN_IND:
					printf
					    ("N_CONN_IND    <-----|<---------------|              |  |                    \n");
					seq[fd] = cmd.npi.conn_ind.SEQ_number;
					break;
				case N_CONN_RES:
					printf
					    ("N_CONN_RES    ----->|--------------->|              |  |                    \n");
					break;
				case N_CONN_CON:
					printf
					    ("N_CONN_CON    <-----|<---------------|              |  |                    \n");
					break;
				case N_DATA_REQ:
					printf
					    ("N_DATA_REQ%c%c  ----->|---(%03lu:%03lu)--->|                 |       %6d bytes \n",
					     cmd.npi.data_req.
					     DATA_xfer_flags & N_RC_FLAG ? '!' : ' ',
					     cmd.npi.data_req.
					     DATA_xfer_flags & N_MORE_DATA_FLAG ? '+' : ' ',
					     qos_data.sid, qos_data.ssn, data.len);
					break;
				case N_DATA_IND:
					printf
					    ("N_DATA_IND%c%c  <-----|<--(%03lu:%03lu)----|  [%010lu]   |                    \n",
					     cmd.npi.data_ind.
					     DATA_xfer_flags & N_RC_FLAG ? '!' : ' ',
					     cmd.npi.data_ind.
					     DATA_xfer_flags & N_MORE_DATA_FLAG ? '+' : ' ',
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     ssn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     tsn);
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.data_ind)))->
					    tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.data_ind)))->
					    sid;
					break;
				case N_EXDATA_REQ:
					printf
					    ("N_EXDATA_REQ  ----->|---(%03lu:%03lu)--->|                 |                    \n",
					     qos_data.sid, qos_data.ssn);
					break;
				case N_EXDATA_IND:
					printf
					    ("N_EXDATA_IND  <-----|<--(%03lu:%03lu)----|  [%010lu]   |                    \n",
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     ssn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     tsn);
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       exdata_ind)))->tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       exdata_ind)))->sid;
					break;
				case N_DATACK_REQ:
					printf
					    ("N_DATACK_REQ  ----->|---(%03lu:%03lu)--->|                 |                    \n",
					     qos_data.sid, qos_data.ssn);
					break;
				case N_DATACK_IND:
					printf
					    ("N_DATACK_IND  <-----|<--(%03lu:%03lu)----|  [%010lu]   |                    \n",
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     ssn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     tsn);
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       datack_ind)))->tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       datack_ind)))->sid;
					break;
				case N_DISCON_REQ:
					printf
					    ("N_DISCON_REQ  ----->|--------------->|              |  |                    \n");
					break;
				case N_DISCON_IND:
					printf
					    ("N_DISCON_IND  <-----|<---------------|              |  |                    \n");
					break;
				case N_RESET_REQ:
					printf
					    ("N_RESET_REQ   ----->|--------------->|              |  |                    \n");
					break;
				case N_RESET_IND:
					printf
					    ("N_RESET_IND   <-----|<---------------|              |  |                    \n");
					break;
				case N_RESET_RES:
					printf
					    ("N_RESET_RES   ----->|--------------->|              |  |                    \n");
					break;
				case N_RESET_CON:
					printf
					    ("N_RESET_CON   <-----|<---------------|              |  |                    \n");
					break;
				default:
					printf
					    ("????%4ld????  ?----?|                                  |                    \n",
					     cmd.npi.type);
					break;
				}
			}
			if (fd == fd2) {
				switch (cmd.npi.type) {
				case N_INFO_REQ:
					printf
					    ("                    |                               |<-+------ N_INFO_REQ   \n");
					break;
				case N_INFO_ACK:
					printf
					    ("                    |                               |--+-----> N_INFO_ACK   \n");
					break;
				case N_OPTMGMT_REQ:
					printf
					    ("                    |                               |<-+------ N_OPTMGMT_REQ\n");
					break;
				case N_BIND_REQ:
					printf
					    ("                    |                               |<-+------ N_BIND_REQ   \n");
					break;
				case N_BIND_ACK:
					printf
					    ("                    |                               |--+-----> N_BIND_ACK   \n");
					tok[fd] = cmd.npi.bind_ack.TOKEN_value;
					break;
				case N_UNBIND_REQ:
					printf
					    ("                    |                               |<-+------ N_UNBIND_REQ \n");
					break;
				case N_ERROR_ACK:
					printf
					    ("                    |                               |\\-+-----> N_ERROR_ACK  \n");
					printf
					    ("                    |                               |          [%-11s]\n",
					     err_string(cmd.npi.error_ack.NPI_error));
					break;
				case N_OK_ACK:
					printf
					    ("                    |                               |\\-+-----> N_OK_ACK     \n");
					break;
				case N_CONN_REQ:
					printf
					    ("                    |                |<-------------|<-+------ N_CONN_REQ   \n");
					break;
				case N_CONN_IND:
					printf
					    ("                    |                |------------->|--+-----> N_CONN_IND   \n");
					seq[fd] = cmd.npi.conn_ind.SEQ_number;
					break;
				case N_CONN_RES:
					printf
					    ("                    |                |<-------------|<-+------ N_CONN_RES   \n");
					break;
				case N_CONN_CON:
					printf
					    ("                    |                |------------->|--+-----> N_CONN_CON   \n");
					break;
				case N_DATA_REQ:
					printf
					    ("                    |                |<--(%03lu:%03lu)--|<-+------ N_DATA_REQ%c%c \n",
					     qos_data.sid, qos_data.ssn,
					     cmd.npi.data_req.
					     DATA_xfer_flags & N_RC_FLAG ? '!' : ' ',
					     cmd.npi.data_req.
					     DATA_xfer_flags & N_MORE_DATA_FLAG ? '+' : ' ');
					break;
				case N_DATA_IND:
					printf
					    ("                    |  [%010lu]  |---(%03lu:%03lu)->|--+-----> N_DATA_IND%c%c \n",
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     tsn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     ssn,
					     cmd.npi.data_ind.
					     DATA_xfer_flags & N_RC_FLAG ? '!' : ' ',
					     cmd.npi.data_ind.
					     DATA_xfer_flags & N_MORE_DATA_FLAG ? '+' : ' ');
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.data_ind)))->
					    tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.data_ind)))->
					    sid;
					break;
				case N_EXDATA_REQ:
					printf
					    ("                    |                |<--(%03lu:%03lu)--|<-+------ N_EXDATA_REQ \n",
					     qos_data.sid, qos_data.ssn);
					break;
				case N_EXDATA_IND:
					printf
					    ("                    |  [%010lu]  |---(%03lu:%03lu)->|--+-----> N_EXDATA_IND \n",
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     tsn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     ssn);
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       exdata_ind)))->tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       exdata_ind)))->sid;
					break;
				case N_DATACK_REQ:
					printf
					    ("                    |                |<--(%03lu:%03lu)--|<-+------ N_DATACK_REQ \n",
					     qos_data.sid, qos_data.ssn);
					break;
				case N_DATACK_IND:
					printf
					    ("                    |  [%010lu]  |---(%03lu:%03lu)->|--+-----> N_DATACK_IND \n",
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     tsn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     ssn);
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       datack_ind)))->tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       datack_ind)))->sid;
					break;
				case N_DISCON_REQ:
					printf
					    ("                    |                |<-------------|<-+------ N_DISCON_REQ \n");
					break;
				case N_DISCON_IND:
					printf
					    ("                    |                |------------->|--+-----> N_DISCON_IND \n");
					break;
				case N_RESET_REQ:
					printf
					    ("                    |                |<-------------|<-+------ N_RESET_REQ  \n");
					break;
				case N_RESET_IND:
					printf
					    ("                    |                |------------->|--+-----> N_RESET_IND  \n");
					break;
				case N_RESET_RES:
					printf
					    ("                    |                |<-------------|<-+------ N_RESET_RES  \n");
					break;
				case N_RESET_CON:
					printf
					    ("                    |                |------------->|--+-----> N_RESET_CON  \n");
					break;
				default:
					printf
					    ("                    |                               |?-+-----? ????%4ld???? \n",
					     cmd.npi.type);
					break;
				}
			}
			if (fd == fd3) {
				switch (cmd.npi.type) {
				case N_INFO_REQ:
					printf
					    ("                    |                               |  |<----- N_INFO_REQ   \n");
					break;
				case N_INFO_ACK:
					printf
					    ("                    |                               |  |-----> N_INFO_ACK   \n");
					break;
				case N_OPTMGMT_REQ:
					printf
					    ("                    |                               |  |<----- N_OPTMGMT_REQ\n");
					break;
				case N_BIND_REQ:
					printf
					    ("                    |                               |  |<----- N_BIND_REQ   \n");
					break;
				case N_BIND_ACK:
					printf
					    ("                    |                               |  |-----> N_BIND_ACK   \n");
					tok[fd] = cmd.npi.bind_ack.TOKEN_value;
					break;
				case N_UNBIND_REQ:
					printf
					    ("                    |                               |  |<----- N_UNBIND_REQ \n");
					break;
				case N_ERROR_ACK:
					printf
					    ("                    |                               |  |\\----> N_ERROR_ACK  \n");
					printf
					    ("                    |                               |  |       [%-11s]\n",
					     err_string(cmd.npi.error_ack.NPI_error));
					break;
				case N_OK_ACK:
					printf
					    ("                    |                               |  |\\----> N_OK_ACK     \n");
					break;
				case N_CONN_REQ:
					printf
					    ("                    |                |<----------------|<----- N_CONN_REQ   \n");
					break;
				case N_CONN_IND:
					printf
					    ("                    |                |---------------->|-----> N_CONN_IND   \n");
					seq[fd] = cmd.npi.conn_ind.SEQ_number;
					break;
				case N_CONN_RES:
					printf
					    ("                    |                |<----------------|<----- N_CONN_RES   \n");
					break;
				case N_CONN_CON:
					printf
					    ("                    |                |---------------->|-----> N_CONN_CON   \n");
					break;
				case N_DATA_REQ:
					printf
					    ("                    |                |<---(%03lu:%03lu)----|<----- N_DATA_REQ%c%c \n",
					     qos_data.sid, qos_data.ssn,
					     cmd.npi.data_req.
					     DATA_xfer_flags & N_RC_FLAG ? '!' : ' ',
					     cmd.npi.data_req.
					     DATA_xfer_flags & N_MORE_DATA_FLAG ? '+' : ' ');
					break;
				case N_DATA_IND:
					printf
					    ("       %6d bytes |  [%010lu]  |----(%03lu:%03lu)--->|-----> N_DATA_IND%c%c \n",
					     data.len,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     tsn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											data_ind)))->
					     ssn,
					     cmd.npi.data_ind.
					     DATA_xfer_flags & N_RC_FLAG ? '!' : ' ',
					     cmd.npi.data_ind.
					     DATA_xfer_flags & N_MORE_DATA_FLAG ? '+' : ' ');
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.data_ind)))->
					    tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.data_ind)))->
					    sid;
					break;
				case N_EXDATA_REQ:
					printf
					    ("                    |                |<---(%03lu:%03lu)----|<----- N_EXDATA_REQ \n",
					     qos_data.sid, qos_data.ssn);
					break;
				case N_EXDATA_IND:
					printf
					    ("                    |  [%010lu]  |----(%03lu:%03lu)--->|-----> N_EXDATA_IND \n",
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     tsn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											exdata_ind)))->
					     ssn);
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       exdata_ind)))->tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       exdata_ind)))->sid;
					break;
				case N_DATACK_REQ:
					printf
					    ("                    |                |<---(%03lu:%03lu)----|<----- N_DATACK_REQ \n",
					     qos_data.sid, qos_data.ssn);
					break;
				case N_DATACK_IND:
					printf
					    ("                    |  [%010lu]  |----(%03lu:%03lu)--->|-----> N_DATACK_IND \n",
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     tsn,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     sid,
					     (ulong) ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
										 sizeof(cmd.npi.
											datack_ind)))->
					     ssn);
					tsn[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       datack_ind)))->tsn;
					sid[fd] =
					    ((N_qos_sel_data_sctp_t *) (cmd.cbuf +
									sizeof(cmd.npi.
									       datack_ind)))->sid;
					break;
				case N_DISCON_REQ:
					printf
					    ("                    |                |<----------------|<----- N_DISCON_REQ \n");
					break;
				case N_DISCON_IND:
					printf
					    ("                    |                |---------------->|-----> N_DISCON_IND \n");
					break;
				case N_RESET_REQ:
					printf
					    ("                    |                |<----------------|<----- N_RESET_REQ  \n");
					break;
				case N_RESET_IND:
					printf
					    ("                    |                |---------------->|-----> N_RESET_IND  \n");
					break;
				case N_RESET_RES:
					printf
					    ("                    |                |<----------------|<----- N_RESET_RES  \n");
					break;
				case N_RESET_CON:
					printf
					    ("                    |                |---------------->|-----> N_RESET_CON  \n");
					break;
				default:
					printf
					    ("                    |                               |  |?----? ????%4ld???? \n",
					     cmd.npi.type);
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
		return (cmd.npi.type);
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
				while ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
					switch (errno) {
					default:
						printf("ERROR: getmsg: [%d] %s\n", errno,
						       strerror(errno));
						return (FAILURE);
					case EINTR:
					case ERESTART:
					case EAGAIN:
						continue;
					}
				}
				gettimeofday(&when, NULL);
				if (show)
					print_msg(fd);
				return (cmd.prim);
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
sctp_n_open(void)
{
	int fd;
	if ((fd = open("/dev/sctp_n", O_NONBLOCK | O_RDWR)) < 0)
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
	ctrl.len = sizeof(cmd.npi.info_req);
	cmd.prim = N_INFO_REQ;
	return put_msg(fd, 0, MSG_HIPRI, 0);
}

int
sctp_optmgmt_req(int fd, ulong flags)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.optmgmt_req) + sizeof(qos_info);
	cmd.prim = N_OPTMGMT_REQ;
	cmd.npi.optmgmt_req.OPTMGMT_flags = flags;
	cmd.npi.optmgmt_req.QOS_length = sizeof(qos_info);
	cmd.npi.optmgmt_req.QOS_offset = sizeof(cmd.npi.optmgmt_req);
	bcopy(&qos_info, (cmd.cbuf + sizeof(cmd.npi.optmgmt_req)), sizeof(qos_info));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_bind_req(int fd, addr_t * addr, int coninds)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.bind_req) + sizeof(*addr);
	cmd.prim = N_BIND_REQ;
	cmd.npi.bind_req.ADDR_length = sizeof(*addr);
	cmd.npi.bind_req.ADDR_offset = sizeof(cmd.npi.bind_req);
	cmd.npi.bind_req.CONIND_number = coninds;
	cmd.npi.bind_req.BIND_flags = TOKEN_REQUEST;
	cmd.npi.bind_req.PROTOID_length = 0;
	cmd.npi.bind_req.PROTOID_offset = 0;
	bcopy(addr, (&cmd.npi.bind_req) + 1, sizeof(*addr));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_unbind_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.unbind_req);
	cmd.prim = N_UNBIND_REQ;
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
	ctrl.len = sizeof(cmd.npi.conn_req) + sizeof(*addr) + sizeof(qos_conn);
	cmd.prim = N_CONN_REQ;
	cmd.npi.conn_req.DEST_length = sizeof(*addr);
	cmd.npi.conn_req.DEST_offset = sizeof(cmd.npi.conn_req);
	cmd.npi.conn_req.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	cmd.npi.conn_req.QOS_length = sizeof(qos_conn);
	cmd.npi.conn_req.QOS_offset = sizeof(cmd.npi.conn_req) + sizeof(*addr);
	bcopy(addr, (cmd.cbuf + sizeof(cmd.npi.conn_req)), sizeof(*addr));
	bcopy(&qos_conn, (cmd.cbuf + sizeof(cmd.npi.conn_req) + sizeof(*addr)), sizeof(qos_conn));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_conn_res(int fd, int fd2, const char *dat)
{
	if (!dat)
		data.len = 0;
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.npi.conn_res);
	cmd.prim = N_CONN_RES;
	cmd.npi.conn_res.TOKEN_value = tok[fd2];
	cmd.npi.conn_res.RES_offset = 0;
	cmd.npi.conn_res.RES_length = 0;
	cmd.npi.conn_res.SEQ_number = seq[fd];
	cmd.npi.conn_res.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	cmd.npi.conn_res.QOS_offset = 0;
	cmd.npi.conn_res.QOS_length = 0;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
sctp_discon_req(int fd, ulong seq)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.discon_req);
	cmd.prim = N_DISCON_REQ;
	cmd.npi.discon_req.DISCON_reason = 0;
	cmd.npi.discon_req.RES_length = 0;
	cmd.npi.discon_req.RES_offset = 0;
	cmd.npi.discon_req.SEQ_number = seq;
	return put_msg(fd, 0, MSG_BAND, 0);
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
	ctrl.len = sizeof(cmd.npi.data_req) + sizeof(qos_data);
	cmd.prim = N_DATA_REQ;
	cmd.npi.data_req.DATA_xfer_flags = flags;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.data_req), sizeof(qos_data));
	return put_msg(fd, 0, MSG_BAND, wait);
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
	ctrl.len = sizeof(cmd.npi.data_req) + sizeof(qos_data);
	cmd.prim = N_DATA_REQ;
	cmd.npi.data_req.DATA_xfer_flags = flags;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.data_req), sizeof(qos_data));
	ret = put_msg(fd, 0, MSG_BAND, wait);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int
sctp_exdata_req(int fd, const char *dat)
{
	if (!dat)
		return (FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.npi.exdata_req) + sizeof(qos_data);
	cmd.prim = N_EXDATA_REQ;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.exdata_req), sizeof(qos_data));
	return put_msg(fd, 1, MSG_BAND, 0);
}

int
sctp_datack_req(int fd, ulong tsn)
{
	int ret;
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.datack_req) + sizeof(qos_data);
	cmd.prim = N_DATACK_REQ;
	qos_data.tsn = tsn;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.datack_req), sizeof(qos_data));
	ret = put_msg(fd, 0, MSG_BAND, 0);
	qos_data.tsn = 0;
	return (ret);
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
	if ((fd1 = sctp_n_open()) < 0)
		return (FAILURE);
	if ((fd2 = sctp_n_open()) < 0)
		return (FAILURE);
	if ((fd3 = sctp_n_open()) < 0)
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
			if ((fd1 = sctp_n_open()) < 0)
				break;
			state = 1;
		case 1:
			if ((fd2 = sctp_n_open()) < 0)
				break;
			state = 2;
		case 2:
			if ((fd3 = sctp_n_open()) < 0)
				break;
			state = 3;
		case 3:
			if (sctp_optmgmt_req(fd1, 0) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (sctp_bind_req(fd1, &addr1, 0) == FAILURE)
				break;
			state = 6;
		case 6:
			if (expect(fd1, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
				break;
			state = 7;
		case 7:
			if (sctp_optmgmt_req(fd2, 0) == FAILURE)
				break;
			state = 8;
		case 8:
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 9;
		case 9:
			if (sctp_bind_req(fd2, &addr2, 5) == FAILURE)
				break;
			state = 10;
		case 10:
			if (expect(fd2, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
				break;
			state = 11;
		case 11:
			if (sctp_optmgmt_req(fd3, 0) == FAILURE)
				break;
			state = 12;
		case 12:
			if (expect(fd3, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 13;
		case 13:
			if (sctp_bind_req(fd3, &addr3, 0) == FAILURE)
				break;
			state = 14;
		case 14:
			if (expect(fd3, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
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
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 17;
		case 17:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 18;
		case 18:
			if (sctp_conn_res(fd2, fd3, NULL) == FAILURE)
				break;
			state = 19;
		case 19:
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 20;
		case 20:
			if (expect(fd1, LONG_WAIT, N_CONN_CON) == FAILURE)
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
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 17;
		case 17:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 18;
		case 18:
			if (sctp_conn_res(fd2, fd3, "Hello There!") == FAILURE)
				break;
			state = 19;
		case 19:
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 20;
		case 20:
			if (expect(fd1, LONG_WAIT, N_CONN_CON) == FAILURE)
				break;
			state = 21;
		case 21:
			if (expect(fd3, NORMAL_WAIT, N_EXDATA_IND) == FAILURE)
				break;
			state = 22;
		case 22:
			if (expect(fd1, NORMAL_WAIT, N_EXDATA_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

int
preamble_2c(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (preamble_1() == FAILURE)
				break;
			state = 3;
		case 3:
			sctp_optmgmt_req(fd1, DEFAULT_RC_SEL);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_optmgmt_req(fd2, DEFAULT_RC_SEL);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			sctp_optmgmt_req(fd3, DEFAULT_RC_SEL);
			if (expect(fd3, SHORT_WAIT, N_OK_ACK) == FAILURE)
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
	qos_info.i_streams = 32;
	qos_info.o_streams = 32;
	qos_conn.i_streams = 32;
	qos_conn.o_streams = 32;
	return preamble_2();
}

int
preamble_4(void)
{
	// qos_info.options = SCTP_OPTION_RANDOM;
	qos_info.options = SCTP_OPTION_DROPPING;
	return preamble_2();
}

int
preamble_5(void)
{
	// qos_info.options = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	qos_info.options = SCTP_OPTION_BREAK;
	return preamble_2();
}

int
preamble_6(void)
{
	qos_info.options = SCTP_OPTION_RANDOM;
	return preamble_3b();
}

int
preamble_7(void)
{
	qos_info.hmac = SCTP_HMAC_SHA_1;
	return preamble_1();
}

int
preamble_8(void)
{
	qos_info.hmac = SCTP_HMAC_MD5;
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
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				failed = state;
			state = 1;
		case 1:
			if (sctp_unbind_req(fd2) == FAILURE) {
				failed = state;
				state = 2;
				continue;
			}
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				failed = state;
			state = 2;
		case 2:
			if (sctp_unbind_req(fd3) == FAILURE) {
				failed = state;
				state = 3;
				continue;
			}
			if (expect(fd3, SHORT_WAIT, N_OK_ACK) == FAILURE)
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
		qos_data.sid = 0;
		qos_info.hmac = SCTP_HMAC_NONE;
		qos_info.options = 0;
		qos_info.i_streams = 1;
		qos_info.o_streams = 1;
		qos_conn.i_streams = 1;
		qos_conn.o_streams = 1;
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
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE) {
				failed = state;
				state = 4;
				continue;
			}
			state = 2;
		case 2:
			if (expect(fd3, LONG_WAIT, N_DISCON_IND) == FAILURE) {
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
			if (expect(fd3, SHORT_WAIT, N_OK_ACK) == FAILURE)
				failed = state;
			state = 6;
		case 6:
			if (sctp_unbind_req(fd1) == FAILURE)
				failed = state;
			else if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				failed = state;
			state = 7;
		case 7:
			if (sctp_unbind_req(fd2) == FAILURE)
				failed = state;
			else if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				failed = state;
			state = 8;
		case 8:
			if (sctp_unbind_req(fd3) == FAILURE)
				failed = state;
			else if (expect(fd3, SHORT_WAIT, N_OK_ACK) == FAILURE)
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
		qos_data.sid = 0;
		qos_info.hmac = SCTP_HMAC_NONE;
		qos_info.options = 0;
		qos_info.i_streams = 1;
		qos_info.o_streams = 1;
		qos_conn.i_streams = 1;
		qos_conn.o_streams = 1;
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
			sctp_optmgmt_req(fd1, 0);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			sctp_info_req(fd1);
			if (expect(fd1, NORMAL_WAIT, N_INFO_ACK) == FAILURE)
				break;
			state = 1;
		case 1:
			sctp_bind_req(fd1, &addr1, 3);
			if (expect(fd1, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_optmgmt_req(fd1, 0);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			sctp_info_req(fd1);
			if (expect(fd1, NORMAL_WAIT, N_INFO_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			sctp_unbind_req(fd1);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
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
on stream as listener."
int
test_case_1b(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			sctp_bind_req(fd1, &addr1, 0);
			if (expect(fd1, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
				break;
			state = 1;
		case 1:
			sctp_bind_req(fd2, &addr2, 5);
			if (expect(fd2, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_bind_req(fd3, &addr3, 0);
			if (expect(fd3, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			sctp_unbind_req(fd1);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_unbind_req(fd2);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			sctp_unbind_req(fd3);
			if (expect(fd3, SHORT_WAIT, N_OK_ACK) == FAILURE)
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
Test Case 2(a):\n\
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
			if (sctp_optmgmt_req(fd1, 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 2;
		case 2:
			if (sctp_bind_req(fd1, &addr1, 0) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, NORMAL_WAIT, N_BIND_ACK) == FAILURE)
				break;
			state = 4;
		case 4:
			if (sctp_conn_req(fd1, &addr2, NULL) == FAILURE)
				break;
			state = 5;
		case 5:
			for (i = 0; i < 25; i++)
				if (expect(fd1, LONG_WAIT, N_DISCON_IND) == SUCCESS)
					break;
				else
					sctp_sleep(qos_info.rto_max * (qos_info.max_inits + 1) /
						   100);
			if (i == 25)
				break;
			state = 6;
		case 6:
			if (sctp_unbind_req(fd1) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Attempt and withdraw a connection request.
 */
#define desc_case_2b "\
Test Case 2(a):\n\
Attempts and then withdraws a connection request.  The connection\n\
should disconnect at both ends."
int
test_case_2b(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_discon_req(fd1, 0);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd2, LONG_WAIT, N_DISCON_IND) == FAILURE)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Attempt and refuse a connection request.
 */
#define desc_case_3 "\
Test Case 3(a):\n\
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
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_discon_req(fd2, seq[fd2]);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, N_DISCON_IND) == FAILURE)
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
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			for (i = 0; i < 25; i++)
				if (expect(fd1, LONG_WAIT, N_DISCON_IND) == SUCCESS)
					break;
				else
					sctp_sleep(qos_info.rto_max * (qos_info.max_inits + 1) /
						   100);
			if (i == 25)
				break;
			state = 3;
		case 3:
			sctp_discon_req(fd2, seq[fd2]);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
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
Test Case 4(a):\n\
Accept a connection and then disconnect.  This connection attempt\n\
should be successful."
int
test_case_4(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_conn_res(fd2, fd3, NULL);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, N_CONN_CON) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_discon_req(fd3, 0);
			if (expect(fd3, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd1, LONG_WAIT, N_DISCON_IND) == FAILURE)
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
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			for (i = 0; i < 25; i++)
				if (expect(fd1, LONG_WAIT, N_DISCON_IND) == SUCCESS)
					break;
				else
					sctp_sleep(qos_info.rto_max * (qos_info.max_inits + 1) /
						   100 + 1);
			if (i == 25)
				break;
			state = 3;
		case 3:
			sctp_conn_res(fd2, fd3, NULL);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd3, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 5;
		case 5:
			if (sctp_data_req(fd3, 0, "Hello!", 0) == FAILURE)
				break;
			if (expect(fd3, LONG_WAIT, N_DISCON_IND) == FAILURE)
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
Test Case 5(a):\n\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the SCTP_HMAC_NONE signature on its cookie."
int
test_case_5(void)
{
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_conn_res(fd2, fd3, NULL);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, N_CONN_CON) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_discon_req(fd1, 0);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd3, LONG_WAIT, N_DISCON_IND) == FAILURE)
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
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_conn_res(fd2, fd3, NULL);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, N_CONN_CON) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_discon_req(fd1, 0);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd3, LONG_WAIT, N_DISCON_IND) == FAILURE)
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
			sctp_conn_req(fd1, &addr2, NULL);
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			sctp_conn_res(fd2, fd3, NULL);
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, N_CONN_CON) == FAILURE)
				break;
			state = 4;
		case 4:
			sctp_discon_req(fd1, 0);
			if (expect(fd1, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 5;
		case 5:
			if (expect(fd3, LONG_WAIT, N_DISCON_IND) == FAILURE)
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
Test Case 6(a):\n\
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
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE ||
			    expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE ||
			    expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
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
				if (expect(fd1, NORMAL_WAIT, N_DATA_IND) == FAILURE)
					break;
			if (i != 21)
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
			if (sctp_data_req(fd1, N_MORE_DATA_FLAG, "Hello There.", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, N_MORE_DATA_FLAG, "Hello There.", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, N_MORE_DATA_FLAG, "Hello There.", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, 0, "Hello There.", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			if (sctp_data_req(fd1, N_RC_FLAG | 0, "Hello There.", 0) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == FAILURE)
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
			qos_data.sid = 0;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 1;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 2;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 3;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 0;
			sctp_exdata_req(fd1, "Hi There.");
			qos_data.sid = 0;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 1;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 2;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 3;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 0;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 1;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 2;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 3;
			if (sctp_data_req(fd1, N_RC_FLAG | N_MORE_DATA_FLAG, "Hello There.", 0) ==
			    FAILURE)
				break;
			qos_data.sid = 0;
			if (sctp_data_req(fd1, N_RC_FLAG | 0, "Hello There.", 0) == FAILURE)
				break;
			qos_data.sid = 1;
			if (sctp_data_req(fd1, N_RC_FLAG | 0, "Hello There.", 0) == FAILURE)
				break;
			qos_data.sid = 2;
			if (sctp_data_req(fd1, N_RC_FLAG | 0, "Hello There.", 0) == FAILURE)
				break;
			qos_data.sid = 3;
			if (sctp_data_req(fd1, N_RC_FLAG | 0, "Hello There.", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, N_EXDATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			for (i = 0; i < 16; i++)
				if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
					break;
			if (i < 16)
				break;
			state = 3;
		case 3:
			for (i = 0; i < 4; i++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == FAILURE)
					break;
			if (i < 4)
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
			if (j < 4 && sctp_exdata_req(fd1, urg) == SUCCESS)
				j++;
			if (k < 4 && sctp_data_req(fd1, 0, nrm, 0) == SUCCESS)
				k++;
			state = 1;
		case 1:
			if (len < i * sizeof(lbuf) + j * sizeof(urg) + k * sizeof(nrm)) {
				int ret;
				switch ((ret = get_msg(fd3, LONG_WAIT))) {
				case N_EXDATA_IND:
				case N_DATA_IND:
					len += data.len;
				case FAILURE:
					ret = 0;
					break;
#if 0
				default:
					printf("Huh? Got return %d\n", ret);
					ret = 0;
					break;
#endif
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
			state = 6;
		case 6:
			// if ( expect(fd1, NORMAL_WAIT, N_DATACK_IND) == FAILURE
			// || expect(fd1, NORMAL_WAIT, N_DATACK_IND) == FAILURE ) break;
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
			if (sctp_data_req(fd1, N_MORE_DATA_FLAG, "Hi There.", 0) == FAILURE)
				break;
			snd_bytes += data.len;
			if (s++ < 4)
				continue;
			print_less(fd1);
			state = 1;
		case 1:
			while (sctp_data_req(fd1, N_MORE_DATA_FLAG, "Hi There.", 0) == SUCCESS) {
				snd_bytes += data.len;
				s++;
			}
			print_more();
			state = 2;
		case 2:
			if (expect(fd3, 0, N_DATA_IND) == FAILURE)
				break;
			rcv_bytes += data.len;
			if (r++ < 4)
				continue;
			print_less(fd3);
			state = 3;
		case 3:
			if (expect(fd3, 0, N_DATA_IND) == FAILURE)
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
				if (expect(fd3, 0, N_DATA_IND) == FAILURE)
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
 *  Connect with data.
 */
#define desc_case_8 "\
Test Case 8(a):\n\
Connect with data in the connection request and response, and\n\
send data with receipt confirmation set.  Check that the data\n\
returns an acknowledgement when it is SACK'ed by the other end."
int
test_case_8(void)
{
	int i, j;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_data_req(fd1, N_RC_FLAG, "Hellos Again!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, N_RC_FLAG, "Hellos Again!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, N_RC_FLAG, "Hellos Again!", 0) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE ||
			    expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE ||
			    expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			for (j = 0; j < 10; j++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == SUCCESS)
					break;
			if (j == 10)
				break;
			for (j = 0; j < 10; j++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == SUCCESS)
					break;
			if (j == 10)
				break;
			for (j = 0; j < 10; j++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == SUCCESS)
					break;
			if (j == 10)
				break;
			state = 3;
		case 3:
			for (i = 0; i < 21; i++)
				if (sctp_data_req(fd3, N_RC_FLAG, "Hello Too!", 0) == FAILURE)
					break;
			if (i < 21)
				break;
			state = 4;
		case 4:
			sctp_sleep(qos_info.hb_itvl / 50 + 1);	/* wait for heartbeats */
			for (i = 0; i < 21; i++)
				if (expect(fd1, NORMAL_WAIT, N_DATA_IND) == FAILURE)
					break;
			if (i != 21)
				break;
			state = 5;
		case 5:
			for (i = 0; i < 21; i++) {
				for (j = 0; j < 10; j++)
					if (expect(fd3, NORMAL_WAIT, N_DATACK_IND) == SUCCESS)
						break;
				if (j == 10)
					break;
			}
			if (i != 21)
				break;
			state = 6;
		case 6:
			if (sctp_data_req(fd1, N_RC_FLAG, "Hellos Again!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, N_RC_FLAG, "Hellos Again!", 0) == FAILURE)
				break;
			if (sctp_data_req(fd1, N_RC_FLAG, "Hellos Again!", 0) == FAILURE)
				break;
			state = 7;
		case 7:
			if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE ||
			    expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE ||
			    expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
				break;
			state = 8;
		case 8:
			for (j = 0; j < 10; j++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == SUCCESS)
					break;
			if (j == 10)
				break;
			for (j = 0; j < 10; j++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == SUCCESS)
					break;
			if (j == 10)
				break;
			for (j = 0; j < 10; j++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == SUCCESS)
					break;
			if (j == 10)
				break;
			return (SUCCESS);
		}
		return (FAILURE);
	}
}

/* 
 *  Data with default receipt confirmation.
 */
#define desc_case_8b "\
Test Case 8(b):\n\
Connect with data and transfer data with default receipt confirmation.\n\
This would require that an explicit acknowledgement is requested for\n\
each data (but we don't support this as SCTP cannot handle it)."
int
test_case_8b(void)
{
	int i;
	uint32_t tsn1[10], tsn3[10];
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (sctp_conn_req(fd1, &addr2, "Hello World!") == FAILURE)
				break;
			if (expect(fd1, SHORT_WAIT, FAILURE) == FAILURE)
				break;
			state = 1;
		case 1:
			if (expect(fd2, LONG_WAIT, N_CONN_IND) == FAILURE)
				break;
			state = 2;
		case 2:
			if (sctp_conn_res(fd2, fd3, "Hello There!") == FAILURE)
				break;
			if (expect(fd2, SHORT_WAIT, N_OK_ACK) == FAILURE)
				break;
			state = 3;
		case 3:
			if (expect(fd1, LONG_WAIT, N_CONN_CON) == FAILURE)
				break;
			state = 4;
		case 4:
			if (expect(fd3, NORMAL_WAIT, N_EXDATA_IND) == FAILURE)
				break;
			tsn3[0] = tsn[fd3];
			state = 5;
		case 5:
			if (expect(fd1, NORMAL_WAIT, N_EXDATA_IND) == FAILURE)
				break;
			tsn1[0] = tsn[fd1];
			state = 6;
		case 6:
			for (i = 1; i < 5; i++)
				if (sctp_data_req(fd1, N_RC_FLAG, "Hellos Again!", 0) == FAILURE)
					break;
			if (i < 5)
				break;
			state = 7;
		case 7:
			sctp_sleep(qos_info.rto_max / 100 + 1);
			for (i = 1; i < 5; i++)
				if (expect(fd3, NORMAL_WAIT, N_DATA_IND) == FAILURE)
					break;
				else
					tsn3[i] = tsn[fd3];
			if (i != 5)
				break;
			state = 8;
		case 8:
			if (sctp_datack_req(fd1, tsn1[0]) == FAILURE)
				break;
			for (i = 0; i < 5; i++)
				sctp_datack_req(fd3, tsn3[i]);
			state = 9;
		case 9:
			sctp_sleep(qos_info.rto_max / 100 + 1);
			for (i = 1; i < 5; i++)
				if (expect(fd1, NORMAL_WAIT, N_DATACK_IND) == FAILURE)
					break;
			if (i != 5)
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
	int i = 0, j = 0, k = 0, l = 0, m = 0, n = 0, f = 0;
	int wait = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (i < TEST_PACKETS) {
				if (sctp_data_req(fd1, N_RC_FLAG, "Pattern-1", 0) == FAILURE)
					break;
				i++;
			}
			while (j < TEST_PACKETS || n < TEST_PACKETS) {
				switch (get_msg(fd3, wait)) {
				case N_DATA_IND:
					j++;
					continue;
				case N_DATACK_IND:
					n++;
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
				if (sctp_data_req(fd3, N_RC_FLAG, "Pattern-3", 0) == FAILURE)
					break;
				l++;
			}
			while (m < TEST_PACKETS || k < TEST_PACKETS) {
				switch (get_msg(fd1, wait)) {
				case N_DATA_IND:
					m++;
					continue;
				case N_DATACK_IND:
					k++;
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
			if (i < TEST_PACKETS
			    || j < TEST_PACKETS
			    || k < TEST_PACKETS || l < TEST_PACKETS || m < TEST_PACKETS ||
			    n < TEST_PACKETS)
				continue;
			return (SUCCESS);
		}
	      test_case_9a_failure:
		printf("%d sent %d inds %d acks %d\n", fd1, i, j, k);
		printf("%d sent %d inds %d acks %d\n", fd3, l, m, n);
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
				sctp_exdata_req(fd1, "Pattern-1");
				i++;
			}
			while (j < TEST_PACKETS) {
				switch (get_msg(fd3, wait)) {
				case N_EXDATA_IND:
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
				sctp_exdata_req(fd3, "Pattern-3");
				l++;
			}
			while (m < TEST_PACKETS) {
				switch (get_msg(fd1, wait)) {
				case N_EXDATA_IND:
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
	int k[TEST_STREAMS] = { 0, };
	int l[TEST_STREAMS] = { 0, };
	int m[TEST_STREAMS] = { 0, };
	int n[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, K = 0, L = 0, M = 0, N = 0;
	int s = 0, f = 0;
	int wait = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
				qos_data.sid = s;
				if (sctp_data_req(fd1, N_RC_FLAG, "Pattern-1", 0) == FAILURE)
					break;
				i[s]++;
				I++;
			}
			while (J < TEST_TOTAL || N < TEST_TOTAL) {
				switch (get_msg(fd3, wait)) {
				case N_DATA_IND:
					j[sid[fd3]]++;
					J++;
					break;
				case N_DATACK_IND:
					n[sid[fd3]]++;
					N++;
					break;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9c_failure;
				}
				break;
			}
			if (L < TEST_TOTAL && l[s] < TEST_PACKETS) {
				qos_data.sid = s;
				if (sctp_data_req(fd3, N_RC_FLAG, "Pattern-3", 0) == FAILURE)
					break;
				l[s]++;
				L++;
			}
			while (M < TEST_TOTAL || K < TEST_TOTAL) {
				switch (get_msg(fd1, wait)) {
				case N_DATA_IND:
					m[sid[fd1]]++;
					M++;
					break;
				case N_DATACK_IND:
					k[sid[fd1]]++;
					K++;
					break;
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
			if (J < TEST_TOTAL || K < TEST_TOTAL || M < TEST_TOTAL || N < TEST_TOTAL)
				continue;
			for (s = 0; s < TEST_STREAMS; s++) {
				if (j[s] != TEST_PACKETS
				    || k[s] != TEST_PACKETS || m[s] != TEST_PACKETS ||
				    n[s] != TEST_PACKETS)
					goto test_case_9c_failure;
			}
			return (SUCCESS);
		}
	      test_case_9c_failure:
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d acks %d\n", fd1, i[s], j[s], k[s]);
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d acks %d\n", fd3, l[s], m[s], n[s]);
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
	int k[TEST_STREAMS] = { 0, };
	int l[TEST_STREAMS] = { 0, };
	int m[TEST_STREAMS] = { 0, };
	int n[TEST_STREAMS] = { 0, };
	int o[TEST_STREAMS] = { 0, };
	int p[TEST_STREAMS] = { 0, };
	int q[TEST_STREAMS] = { 0, };
	int r[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, K = 0, L = 0, M = 0, N = 0, O = 0, P = 0, Q = 0, R = 0;
	int s = 0, f = 0;
	int wait = 0;
	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			if (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
				qos_data.sid = s;
				if (sctp_data_req(fd1, N_RC_FLAG, "Pattern-1", 0) == FAILURE)
					break;
				i[s]++;
				I++;
			}
			while (J < TEST_TOTAL || N < TEST_TOTAL || P < TEST_TOTAL) {
				switch (get_msg(fd3, wait)) {
				case N_DATA_IND:
					j[sid[fd3]]++;
					J++;
					break;
				case N_DATACK_IND:
					n[sid[fd3]]++;
					N++;
					break;
				case N_EXDATA_IND:
					p[sid[fd3]]++;
					P++;
					break;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9d_failure;
				}
				break;
			}
			if (O < TEST_TOTAL && o[s] < TEST_PACKETS) {
				qos_data.sid = s;
				sctp_exdata_req(fd1, "Pattern-1");
				o[s]++;
				O++;
			}
			while (J < TEST_TOTAL || N < TEST_TOTAL || P < TEST_TOTAL) {
				switch (get_msg(fd3, wait)) {
				case N_DATA_IND:
					j[sid[fd3]]++;
					J++;
					break;
				case N_DATACK_IND:
					n[sid[fd3]]++;
					N++;
					break;
				case N_EXDATA_IND:
					p[sid[fd3]]++;
					P++;
					break;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9d_failure;
				}
				break;
			}
			if (L < TEST_TOTAL && l[s] < TEST_PACKETS) {
				qos_data.sid = s;
				if (sctp_data_req(fd3, N_RC_FLAG, "Pattern-3", 0) == FAILURE)
					break;
				l[s]++;
				L++;
			}
			while (M < TEST_TOTAL || K < TEST_TOTAL || R < TEST_TOTAL) {
				switch (get_msg(fd1, wait)) {
				case N_DATA_IND:
					m[sid[fd1]]++;
					M++;
					break;
				case N_DATACK_IND:
					k[sid[fd1]]++;
					K++;
					break;
				case N_EXDATA_IND:
					r[sid[fd1]]++;
					R++;
					break;
				case FAILURE:
					f++;
					break;
				default:
					goto test_case_9d_failure;
				}
				break;
			}
			if (Q < TEST_TOTAL && q[s] < TEST_PACKETS) {
				qos_data.sid = s;
				sctp_exdata_req(fd3, "Pattern-3");
				q[s]++;
				Q++;
			}
			while (M < TEST_TOTAL || K < TEST_TOTAL || R < TEST_TOTAL) {
				switch (get_msg(fd1, wait)) {
				case N_DATA_IND:
					m[sid[fd1]]++;
					M++;
					break;
				case N_DATACK_IND:
					k[sid[fd1]]++;
					K++;
					break;
				case N_EXDATA_IND:
					r[sid[fd1]]++;
					R++;
					break;
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
			if (J < TEST_TOTAL
			    || K < TEST_TOTAL || M < TEST_TOTAL || N < TEST_TOTAL || P < TEST_TOTAL
			    || R < TEST_TOTAL)
				continue;
			for (s = 0; s < TEST_STREAMS; s++) {
				if (j[s] != TEST_PACKETS
				    || k[s] != TEST_PACKETS
				    || m[s] != TEST_PACKETS
				    || n[s] != TEST_PACKETS || p[s] != TEST_PACKETS ||
				    r[s] != TEST_PACKETS)
					goto test_case_9d_failure;
			}
			return (SUCCESS);
		}
	      test_case_9d_failure:
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d acks %d\n", fd1, i[s], j[s], k[s]);
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d\n", fd1, o[s], p[s]);
		for (s = 0; s < TEST_STREAMS; s++)
			printf("%d send %d inds %d acks %d\n", fd3, l[s], m[s], n[s]);
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
					if (sctp_data_req(fd1, 0, "Test Pattern-1", 0) == FAILURE)
						break;
					if (sctp_data_req(fd3, 0, "Test Pattern-3", 0) == FAILURE)
						break;
				}
				if (i < 20)
					break;
				for (i = 0, k = 0; i < 20 || k < 20;) {
					switch (sctp_wait(fd1, SHORT_WAIT)) {
					case N_DATA_IND:
						i++;
						continue;
					case FAILURE:
						break;
					default:
						print_msg(fd1);
						return (FAILURE);
					}
					switch (sctp_wait(fd3, SHORT_WAIT)) {
					case N_DATA_IND:
						k++;
						continue;
					case FAILURE:
						break;
					default:
						print_msg(fd3);
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
		uint ack_idx;
		struct timeval ack;
	};
	struct result times[SETS * REPS];
	ulong itotal = 0;
	ulong atotal = 0;
	bzero(times, sizeof(times));
	state = 0;
	ret = FAILURE;
	for (;;) {
		switch (state) {
		case 0:
			show = 0;
			for (j = 0; j < SETS; j++) {
				for (i = 0; i < REPS; i++) {
					// if ( sctp_data_req(fd1, N_RC_FLAG, "This is a much
					// longer test pattern that is being used to see whether we 
					// 
					// can generate some congestion and it is called Test
					// Pattern-1", 0) == FAILURE ) break;
					if (sctp_data_req(fd1, N_RC_FLAG, "Test Pattern-1", 0) ==
					    FAILURE)
						break;
					times[j * REPS + i].req = when;
					times[j * REPS + i].req_idx = n++;
				}
				if (i < REPS)
					break;
				for (i = 0, k = 0; i < REPS || k < REPS;) {
					if (k < REPS)
						switch (sctp_wait(fd3, SHORT_WAIT)) {
						case N_DATA_IND:
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
					if (i < REPS)
						switch (sctp_wait(fd1, SHORT_WAIT)) {
						case N_DATACK_IND:
							times[j * REPS + i].ack = when;
							times[j * REPS + i].ack_idx = n++;
							i++;
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
		j = 0;
		k = 0;
		for (n = 0; n < 3 * SETS * REPS; n++) {
			for (i = 0; i < SETS * REPS; i++) {
				if (times[i].req_idx == n) {
					printf
					    ("N_DATA_REQ    ----->|--------------->|                 |                    \n");
					break;
				}
				if (times[i].ind_idx == n) {
					printf
					    ("                    |                |-%9ld usec->|-----> N_DATA_IND   \n",
					     time_sub(&times[i].ind, &times[i].req));
					k++;
					itotal += time_sub(&times[i].ind, &times[i].req);
					break;
				}
				if (times[i].ack_idx == n) {
					printf
					    ("N_DATACK_IND  <-----|<-%9ld usec|                 |                    \n",
					     time_sub(&times[i].ack, &times[i].req));
					j++;
					atotal += time_sub(&times[i].ack, &times[i].req);
					break;
				}
			}
		}
		itotal /= k;
		atotal /= j;
		printf
		    ("                    |                |                 |                    \n");
		printf("                    |    ind average = %9ld usec  |                    \n",
		       itotal);
		printf("                    |    ack average = %9ld usec  |                    \n",
		       atotal);
		printf
		    ("                    |                |                 |                    \n");
		return (ret);
	}
}

struct test_case {
	const char *numb;		/* test case number */
	const char *name;		/* test case name */
	int (*preamble) (void);		/* test case preamble */
	int (*testcase) (void);		/* test case proper */
	int (*postamble) (void);	/* test case postamble */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} tests[] = {
	{
	"1(a)", desc_case_1, &preamble_0, &test_case_1, &postamble_0, 0, 0}, {
	"1(b)", desc_case_1b, &preamble_0, &test_case_1b, &postamble_0, 0, 0}, {
	"2(a)", desc_case_2, &preamble_0, &test_case_2, &postamble_0, 0, 0}, {
	"2(b)", desc_case_2b, &preamble_1, &test_case_2b, &postamble_1, 0, 0}, {
	"3(a)", desc_case_3, &preamble_1, &test_case_3, &postamble_1, 0, 0}, {
	"3(b)", desc_case_3b, &preamble_1, &test_case_3b, &postamble_1, 0, 0}, {
	"4(a)", desc_case_4, &preamble_1, &test_case_4, &postamble_1, 0, 0}, {
	"4(b)", desc_case_4b, &preamble_1, &test_case_4b, &postamble_1, 0, 0}, {
	"5(a)", desc_case_5, &preamble_1, &test_case_5, &postamble_1, 0, 0}, {
	"5(b)", desc_case_5b, &preamble_8, &test_case_5b, &postamble_1, 0, 0}, {
	"5(c)", desc_case_5c, &preamble_7, &test_case_5c, &postamble_1, 0, 0}, {
	"6(a)", desc_case_6, &preamble_2b, &test_case_6, &postamble_2, 0, 0}, {
	"6(b)", desc_case_6b, &preamble_2, &test_case_6b, &postamble_2, 0, 0}, {
	"6(c)", desc_case_6c, &preamble_3b, &test_case_6c, &postamble_2, 0, 0}, {
	"7(a)", desc_case_7, &preamble_2, &test_case_7, &postamble_2, 0, 0}, {
	"7(b)", desc_case_7b, &preamble_2, &test_case_7b, &postamble_2, 0, 0}, {
	"8(a)", desc_case_8, &preamble_2b, &test_case_8, &postamble_2, 0, 0}, {
	"8(b)", desc_case_8b, &preamble_2c, &test_case_8b, &postamble_2, 0, 0}, {
	"9(a)", desc_case_9a, &preamble_4, &test_case_9a, &postamble_2, 0, 0}, {
	"9(b)", desc_case_9b, &preamble_4, &test_case_9b, &postamble_2, 0, 0}, {
	"9(c)", desc_case_9c, &preamble_6, &test_case_9c, &postamble_2, 0, 0}, {
	"9(d)", desc_case_9d, &preamble_6, &test_case_9d, &postamble_2, 0, 0}, {
	"10(a)", desc_case_10a, &preamble_5, &test_case_10a, &postamble_2, 0, 0}, {
	"10(b)", desc_case_10b, &preamble_5, &test_case_10b, &postamble_2, 0, 0}, {
	NULL,}
};

int
do_tests(void)
{
	int i;
	int result = INCONCLUSIVE;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	int num_exit;
	if (verbose) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout,
			"\n\nRFC 2960 SCTP - OpenSS7 STREAMS SCTP - Conformance Test Program.\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (begin_tests() == SUCCESS) {
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (tests[i].result)
				continue;
			if (!tests[i].run) {
				tests[i].result = INCONCLUSIVE;
				continue;
			}
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "\nTest Case SCTP NPI:%s:\n%s\n", tests[i].numb,
					tests[i].name);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			fprintf(stdout,
				"---------------------------------Preamble-----------------------------------\n");
			if (tests[i].preamble && tests[i].preamble() != SUCCESS) {
				fprintf(stdout,
					"                    |???????? INCONCLUSIVE %2d ??????|??|                    \n",
					state);
				result = INCONCLUSIVE;
			} else {
				fprintf(stdout,
					"--------------------|--------------Test-------------|--|--------------------\n");
				if (tests[i].testcase && tests[i].testcase() == FAILURE) {
					fprintf(stdout,
						"                    |XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    \n");
					result = FAILURE;
				} else {
					fprintf(stdout,
						"                    |************ PASSED ***********|**|                    \n");
					result = SUCCESS;
				}
			}
			fprintf(stdout,
				"--------------------|------------Postamble----------|--|--------------------\n");
			if (tests[i].postamble && tests[i].postamble() != SUCCESS) {
				fprintf(stdout,
					"                    |???????? INCONCLUSIVE %2d ??????|??|                    \n",
					state);
				if (result == SUCCESS)
					result = INCONCLUSIVE;
			}
			fprintf(stdout,
				"----------------------------------------------------------------------------\n");
			switch (result) {
			case SUCCESS:
				successes++;
				fprintf(stdout, "*********\n");
				fprintf(stdout, "********* Test Case SUCCESSFUL\n");
				fprintf(stdout, "*********\n\n");
				break;
			case FAILURE:
				failures++;
				fprintf(stdout, "XXXXXXXXX\n");
				fprintf(stdout, "XXXXXXXXX Test Case FAILED\n");
				fprintf(stdout, "XXXXXXXXX\n\n");
				break;
			default:
			case INCONCLUSIVE:
				inconclusive++;
				fprintf(stdout, "?????????\n");
				fprintf(stdout, "????????? Test Case INCONCLUSIVE\n");
				fprintf(stdout, "?????????\n\n");
				break;
			}
			tests[i].result = result;
		}
	} else
		fprintf(stdout, "Test setup failed!\n");
	end_tests();
	lockf(fileno(stdout), F_LOCK, 0);
	if (summary && verbose) {
		fprintf(stdout, "\n\n");
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (tests[i].run) {
				fprintf(stdout, "Test Case SCTP NPI:%-10s  ", tests[i].numb);
				switch (tests[i].result) {
				case SUCCESS:
					fprintf(stdout, "SUCCESS\n");
					break;
				case FAILURE:
					fprintf(stdout, "FAILURE\n");
					break;
				default:
				case INCONCLUSIVE:
					fprintf(stdout, "INCONCLUSIVE\n");
					break;
				}
			}
		}
		fflush(stdout);
	}
	fprintf(stdout, "Done.\n\n");
	fprintf(stdout, "========= %2d successes   \n", successes);
	fprintf(stdout, "========= %2d failures    \n", failures);
	fprintf(stdout, "========= %2d inconclusive\n", inconclusive);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (0);
}

void
splash(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
RFC 2960 SCTP - OpenSS7 STREAMS SCTP - Conformance Test Suite\n\
\n\
Copyright (c) 2001-2004 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
Regulations  (\"FAR\") (or any success  regulations) or, in the  cases of NASA, in\n\
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
    Copyright (c) 2003-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
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
Arguments:\n\
    (none)\n\
Options:\n\
    -f, --fast [SCALE]\n\
        Increase speed of tests by scaling timers [default: 50]\n\
    -s, --summary\n\
        Print a test case summary at end of testing [default: off]\n\
    -o, --onetest [TESTCASE]\n\
        Run a single test case.\n\
    -t, --tests [RANGE]\n\
        Run a range of test cases.\n\
    -m, --messages\n\
        Display messages. [default: off]\n\
    -q, --quiet\n\
        Suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        Increase verbosity or set to LEVEL [default: 1]\n\
	This option may be repeated.\n\
    -h, --help, -?, --?\n\
        Prints this usage message and exists\n\
    -V, --version\n\
        Prints the version and exists\n\
", argv[0]);
}

int
main(int argc, char *argv[])
{
	size_t l, n;
	int range = 0;
	struct test_case *t;
	int tests_to_run = 0;
	for (t = tests; t->numb; t++) {
		if (!t->result) {
			t->run = 1;
			tests_to_run++;
		}
	}
	for (;;) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"fast",	optional_argument,	NULL, 'f'},
			{"summary",	no_argument,		NULL, 's'},
			{"onetest",	required_argument,	NULL, 'o'},
			{"tests",	required_argument,	NULL, 't'},
			{"messages",	no_argument,		NULL, 'm'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL, }
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "f::so:t:mqvhV?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "f::so:t:mqvhV?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'f':
			if (optarg)
				timer_scale = atoi(optarg);
			else
				timer_scale = 50;
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n",
				timer_scale);
			break;
		case 's':
			summary = 1;
			break;
		case 'o':
			if (!range) {
				for (t = tests; t->numb; t++)
					t->run = 0;
				tests_to_run = 0;
			}
			range = 1;
			for (n = 0, t = tests; t->numb; t++)
				if (!strncmp(t->numb, optarg, 16)) {
					if (!t->result) {
						t->run = 1;
						n++;
						tests_to_run++;
					}
				}
			if (!n) {
				fprintf(stderr, "WARNING: specification `%s' matched no test\n",
					optarg);
				fflush(stderr);
				goto bad_option;
			}
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
		case 't':
			l = strnlen(optarg, 16);
			if (!range) {
				for (t = tests; t->numb; t++)
					t->run = 0;
				tests_to_run = 0;
			}
			range = 1;
			for (n = 0, t = tests; t->numb; t++)
				if (!strncmp(t->numb, optarg, l)) {
					if (!t->result) {
						t->run = 1;
						n++;
						tests_to_run++;
					}
				}
			if (!n) {
				fprintf(stderr, "WARNING: specification `%s' matched no test\n",
					optarg);
				fflush(stderr);
				goto bad_option;
			}
			break;
		case 'm':
			show_msg = 1;
			break;
		case 'H':	/* -H */
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
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
	if (!tests_to_run) {
		if (verbose) {
			fprintf(stderr, "%s: error: no tests to run\n", argv[0]);
			fflush(stderr);
		}
		exit(2);
	}
	splash(argc, argv);
	do_tests();
	exit(0);
}
