/*****************************************************************************

 @(#) $RCSfile: test-sctp.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/12 02:23:24 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/03/12 02:23:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sctp.c,v $
 Revision 0.9.2.6  2007/03/12 02:23:24  brian
 - updating tests

 Revision 0.9.2.5  2006/09/26 00:52:38  brian
 - rationalized to embedded packages

 Revision 0.9.2.4  2005/05/14 08:31:32  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctp.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/12 02:23:24 $"

static char const ident[] = "$RCSfile: test-sctp.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/12 02:23:24 $";

/* 
 *  This file is for testing the sctp_n driver.
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

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#define BUFSIZE 300
#define FFLUSH(stream)

int verbose = 1;

union {
	np_ulong prim;
	union N_primitives npi;
	char cbuf[BUFSIZE];
} cmd;

char dbuf[BUFSIZE];
struct strbuf ctrl = { BUFSIZE, 0, cmd.cbuf };
struct strbuf data = { BUFSIZE, 0, dbuf };

typedef struct addr {
	uint16_t port;
	struct in_addr addr[1];
} addr_t;

void
print_error(ulong error)
{
	switch (error) {
	case NBADADDR:
		printf("NBADADDR");
		break;
	case NBADOPT:
		printf("NBADOPT");
		break;
	case NACCESS:
		printf("NACCESS");
		break;
	case NNOADDR:
		printf("NNOADDR");
		break;
	case NOUTSTATE:
		printf("NOUTSTATE");
		break;
	case NBADSEQ:
		printf("NBADSEQ");
		break;
	case NSYSERR:
		printf("NSYSERR");
		break;
	case NBADDATA:
		printf("NBADDATA");
		break;
	case NBADFLAG:
		printf("NBADFLAG");
		break;
	case NNOTSUPPORT:
		printf("NNOTSUPPORT");
		break;
	case NBOUND:
		printf("NBOUND");
		break;
	case NBADQOSPARAM:
		printf("NBADQOSPARAM");
		break;
	case NBADQOSTYPE:
		printf("NBADQOSTYPE");
		break;
	case NBADTOKEN:
		printf("NBADTOKEN");
		break;
	case NNOPROTOID:
		printf("NNOPROTOID");
		break;
	default:
		printf("(unknown [%lu])", error);
		break;
	}
	printf("\n");
	return;
}

void
print_prim(ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		printf("N_CONN_REQ");
		break;
	case N_CONN_RES:
		printf("N_CONN_RES");
		break;
	case N_DISCON_REQ:
		printf("N_DISCON_REQ");
		break;
	case N_DATA_REQ:
		printf("N_DATA_REQ");
		break;
	case N_EXDATA_REQ:
		printf("N_EXDATA_REQ");
		break;
	case N_INFO_REQ:
		printf("N_INFO_REQ");
		break;
	case N_BIND_REQ:
		printf("N_BIND_REQ");
		break;
	case N_UNBIND_REQ:
		printf("N_UNBIND_REQ");
		break;
	case N_UNITDATA_REQ:
		printf("N_UNITDATA_REQ");
		break;
	case N_OPTMGMT_REQ:
		printf("N_OPTMGMT_REQ");
		break;
	case N_DATACK_REQ:
		printf("N_DATACK_REQ");
		break;
	case N_RESET_REQ:
		printf("N_RESET_REQ");
		break;
	case N_RESET_RES:
		printf("N_RESET_RES");
		break;
	default:
		printf("(unexpected [%lu])", prim);
		break;
	}
	printf("\n");
	return;
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
	int i;
	size_t anum = (add_len - sizeof(a->port)) / sizeof(*a->addr);

	printf("[%d]", ntohs(a->port));
	for (i = 0; i < anum; i++) {
		printf(" %d.%d.%d.%d", (a->addr[i] >> 0) & 0xff, (a->addr[i] >> 8) & 0xff, (a->addr[i] >> 16) & 0xff, (a->addr[i] >> 24) & 0xff);
	}
	printf("\n");
}

void
print_qos(char *qos_ptr, size_t add_len)
{
	N_qos_sctp_t *qos = (N_qos_sctp_t *) qos_ptr;

	switch (qos->n_qos_type) {
	case N_QOS_SEL_CONN_SCTP:
		printf("CONN:");
		printf(" i_streams = %ld,", (long) qos->n_qos_conn.i_streams);
		printf(" o_streams = %ld", (long) qos->n_qos_conn.o_streams);
		break;

	case N_QOS_SEL_DATA_SCTP:
		printf("DATA: ");
		printf(" ppi = %lu,", (ulong) qos->n_qos_data.ppi);
		printf(" sid = %ld,", (long) qos->n_qos_data.sid);
		printf(" ssn = %ld,", (long) qos->n_qos_data.ssn);
		printf(" more = %ld", (long) qos->n_qos_data.more);
		break;

	case N_QOS_SEL_INFO_SCTP:
		printf("INFO: ");
		break;

	case N_QOS_RANGE_INFO_SCTP:
		printf("RANGE: ");
		break;

	default:
		printf("(unknown qos structure %lu)\n", (ulong) qos->n_qos_type);
		break;
	}
	printf("\n");
}

void
print_msg(int fd)
{
	if (ctrl.len > 0) {
		switch (cmd.prim) {
		case N_INFO_REQ:
			printf("%d-N_INFO_REQ:\n", fd);
			break;

		case N_INFO_ACK:
			printf("%d-N_INFO_ACK:\n", fd);
			printf("  NSDU_size      = %lu\n", (ulong) cmd.npi.info_ack.NSDU_size);
			printf("  ENSDU_size     = %lu\n", (ulong) cmd.npi.info_ack.ENSDU_size);
			printf("  CDATA_size     = %lu\n", (ulong) cmd.npi.info_ack.CDATA_size);
			printf("  DDATA_size     = %lu\n", (ulong) cmd.npi.info_ack.DDATA_size);
			printf("  ADDR_size      = %lu\n", (ulong) cmd.npi.info_ack.ADDR_size);
			printf("  ADDR           = ");
			print_addr(cmd.cbuf + cmd.npi.info_ack.ADDR_offset, cmd.npi.info_ack.ADDR_length);
			printf("  QOS            = ");
			print_qos(cmd.cbuf + cmd.npi.info_ack.QOS_offset, cmd.npi.info_ack.QOS_length);
			printf("  QOR            = ");
			print_qos(cmd.cbuf + cmd.npi.info_ack.QOS_range_offset, cmd.npi.info_ack.QOS_range_length);
			printf("  OPTIONS_flags  =");
			if (cmd.npi.info_ack.OPTIONS_flags & REC_CONF_OPT)
				printf(" REC_CONF_OPT");
			if (cmd.npi.info_ack.OPTIONS_flags & EX_DATA_OPT)
				printf(" EX_DATA_OPT");
			printf("\n");
			printf("  NIDU_size      = %lu\n", (ulong) cmd.npi.info_ack.NIDU_size);
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
				printf("(unknown %lu)\n", (ulong) cmd.npi.info_ack.PROVIDER_type);
				break;
			}
			printf("  NODU_size      = %lu\n", (ulong) cmd.npi.info_ack.NODU_size);
			printf("  PROTOID_length = %lu\n", (ulong) cmd.npi.info_ack.PROTOID_length);
			printf("  PROTOID_offset = %lu\n", (ulong) cmd.npi.info_ack.PROTOID_offset);
			printf("  NPI_version    = %lu\n", (ulong) cmd.npi.info_ack.NPI_version);
			break;

		case N_BIND_REQ:
			printf("%d-N_BIND_REQ:\n", fd);
			printf("  ADDR           = ");
			print_addr(cmd.cbuf + cmd.npi.bind_req.ADDR_offset, cmd.npi.bind_req.ADDR_length);
			printf("  CONIND_number  = %lu\n", (ulong) cmd.npi.bind_req.CONIND_number);
			printf("  BIND_flags     =");
			if (cmd.npi.bind_req.BIND_flags & DEFAULT_LISTENER)
				printf(" DEFAULT_LISTENER");
			if (cmd.npi.bind_req.BIND_flags & TOKEN_REQUEST)
				printf(" TOKEN_REQUEST");
			if (cmd.npi.bind_req.BIND_flags & DEFAULT_DEST)
				printf(" DEFAULT_DEST");
			printf("\n");
			printf("  PROTOID_length = %lu\n", (ulong) cmd.npi.bind_req.PROTOID_length);
			printf("  PROTOID_offset = %lu\n", (ulong) cmd.npi.bind_req.PROTOID_offset);
			break;

		case N_BIND_ACK:
			printf("%d-N_BIND_ACK:\n", fd);
			printf("  ADDR           = ");
			print_addr(cmd.cbuf + cmd.npi.bind_ack.ADDR_offset, cmd.npi.bind_ack.ADDR_length);
			printf("  CONIND_number  = %lu\n", (ulong) cmd.npi.bind_ack.CONIND_number);
			printf("  TOKEN_value    = %lu\n", (ulong) cmd.npi.bind_ack.TOKEN_value);
			printf("  PROTOID_length = %lu\n", (ulong) cmd.npi.bind_ack.PROTOID_length);
			printf("  PROTOID_offset = %lu\n", (ulong) cmd.npi.bind_ack.PROTOID_offset);
			break;

		case N_ERROR_ACK:
			printf("%d-N_ERROR_ACK:\n", fd);
			printf("  ERROR_prim     = ");
			print_prim(cmd.npi.error_ack.ERROR_prim);
			printf("  NPI_error      = ");
			print_error(cmd.npi.error_ack.NPI_error);
			printf("  UNIX_error     = %lu (%s)\n", (ulong) cmd.npi.error_ack.UNIX_error, strerror(cmd.npi.error_ack.UNIX_error));
			break;

		case N_OK_ACK:
			printf("%d-N_OK_ACK:\n", fd);
			printf("  CORRECT_prim   = ");
			print_prim(cmd.npi.error_ack.ERROR_prim);
			break;

		case N_CONN_REQ:
			printf("%d-N_CONN_REQ:\n", fd);
			printf("  DEST           = ");
			print_addr(cmd.cbuf + cmd.npi.conn_req.DEST_offset, cmd.npi.conn_req.DEST_length);
			printf("  CONN_flags     =");
			if (cmd.npi.conn_req.CONN_flags & REC_CONF_OPT)
				printf(" REC_CONF_OPT");
			if (cmd.npi.conn_req.CONN_flags & EX_DATA_OPT)
				printf(" EX_DATA_OPT");
			printf("\n");
			printf("  QOS            = ");
			print_qos(cmd.cbuf + cmd.npi.conn_req.QOS_offset, cmd.npi.conn_req.QOS_length);
			break;

		case N_CONN_IND:
			printf("%d-N_CONN_IND:\n", fd);
			printf("  DEST           = ");
			print_addr(cmd.cbuf + cmd.npi.conn_ind.DEST_offset, cmd.npi.conn_ind.DEST_length);
			printf("  SRC            = ");
			print_addr(cmd.cbuf + cmd.npi.conn_ind.SRC_offset, cmd.npi.conn_ind.SRC_length);
			printf("  SEQ_number     = %lu\n", (ulong) cmd.npi.conn_ind.SEQ_number);
			printf("  CONN_flags     =");
			if (cmd.npi.conn_ind.CONN_flags & REC_CONF_OPT)
				printf(" REC_CONF_OPT");
			if (cmd.npi.conn_ind.CONN_flags & EX_DATA_OPT)
				printf(" EX_DATA_OPT");
			printf("\n");
			printf("  QOS            = ");
			print_qos(cmd.cbuf + cmd.npi.conn_ind.QOS_offset, cmd.npi.conn_ind.QOS_length);
			break;

		case N_CONN_RES:
			printf("%d-N_CONN_RES:\n", fd);
			printf("  TOKEN_value    = %lu\n", (ulong) cmd.npi.conn_res.TOKEN_value);
			printf("  RES            = ");
			print_addr(cmd.cbuf + cmd.npi.conn_res.RES_offset, cmd.npi.conn_res.RES_length);
			printf("  SEQ_number     = %lu\n", (ulong) cmd.npi.conn_res.SEQ_number);
			printf("  CONN_flags     =");
			if (cmd.npi.conn_res.CONN_flags & REC_CONF_OPT)
				printf(" REC_CONF_OPT");
			if (cmd.npi.conn_res.CONN_flags & EX_DATA_OPT)
				printf(" EX_DATA_OPT");
			printf("\n");
			printf("  QOS            = ");
			print_qos(cmd.cbuf + cmd.npi.conn_res.QOS_offset, cmd.npi.conn_res.QOS_length);
			break;

		case N_CONN_CON:
			printf("%d-N_CONN_CON:\n", fd);
			printf("  RES            = ");
			print_addr(cmd.cbuf + cmd.npi.conn_con.RES_offset, cmd.npi.conn_con.RES_length);
			printf("  CONN_flags     =");
			if (cmd.npi.conn_con.CONN_flags & REC_CONF_OPT)
				printf(" REC_CONF_OPT");
			if (cmd.npi.conn_con.CONN_flags & EX_DATA_OPT)
				printf(" EX_DATA_OPT");
			printf("\n");
			printf("  QOS            = ");
			print_qos(cmd.cbuf + cmd.npi.conn_con.QOS_offset, cmd.npi.conn_res.QOS_length);

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
			print_qos(cmd.cbuf + sizeof(cmd.npi.data_req), ctrl.len - sizeof(cmd.npi.data_req));
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
			print_qos(cmd.cbuf + sizeof(cmd.npi.data_ind), ctrl.len - sizeof(cmd.npi.data_ind));
			break;

		case N_EXDATA_REQ:
			printf("%d-N_EXDATA_REQ:\n", fd);
			printf("  QOS            = ");
			print_qos(cmd.cbuf + sizeof(cmd.npi.exdata_req), ctrl.len - sizeof(cmd.npi.exdata_req));
			break;

		case N_EXDATA_IND:
			printf("%d-N_EXDATA_IND:\n", fd);
			printf("  QOS            = ");
			print_qos(cmd.cbuf + sizeof(cmd.npi.exdata_ind), ctrl.len - sizeof(cmd.npi.exdata_ind));
			break;

		case N_DATACK_REQ:
			printf("%d-N_DATACK_REQ:\n", fd);
			printf("  QOS            = ");
			print_qos(cmd.cbuf + sizeof(cmd.npi.datack_req), ctrl.len - sizeof(cmd.npi.datack_req));
			break;

		case N_DATACK_IND:
			printf("%d-N_DATACK_IND:\n", fd);
			printf("  QOS            = ");
			print_qos(cmd.cbuf + sizeof(cmd.npi.datack_ind), ctrl.len - sizeof(cmd.npi.datack_ind));
			break;

		case N_RESET_REQ:
			printf("%d-N_RESET_REQ:\n", fd);
			printf("  RESET_reason   = %lu\n", (ulong) cmd.npi.reset_req.RESET_reason);
			break;

		case N_RESET_IND:
			printf("%d-N_RESET_IND:\n", fd);
			printf("  RESET_orig     = %lu\n", (ulong) cmd.npi.reset_ind.RESET_orig);
			printf("  RESET_reason   = %lu\n", (ulong) cmd.npi.reset_ind.RESET_reason);
			break;

		case N_RESET_RES:
			printf("%d-N_RESET_RES:\n", fd);
			break;

		case N_RESET_CON:
			printf("%d-N_RESET_CON:\n", fd);
			break;

		case N_DISCON_REQ:
			printf("%d-N_DISCON_REQ:\n", fd);
			printf("  DISCON_reason  = %lu\n", (ulong) cmd.npi.discon_req.DISCON_reason);
			printf("  RES            = ");
			print_addr(cmd.cbuf + cmd.npi.discon_req.RES_offset, cmd.npi.discon_req.RES_length);
			printf("  SEQ_number     = %lu\n", (ulong) cmd.npi.discon_req.SEQ_number);
			break;

		case N_DISCON_IND:
			printf("%d-N_DISCON_IND:\n", fd);
			printf("  DISCON_orig    = %lu\n", (ulong) cmd.npi.discon_ind.DISCON_orig);
			printf("  DISCON_reason  = %lu\n", (ulong) cmd.npi.discon_ind.DISCON_reason);
			printf("  RES            = ");
			print_addr(cmd.cbuf + cmd.npi.discon_ind.RES_offset, cmd.npi.discon_ind.RES_length);
			printf("  SEQ_number     = %lu\n", (ulong) cmd.npi.discon_ind.SEQ_number);
			break;

		default:
			printf("Unrecognized primitive %lu!\n", (ulong) cmd.prim);
			break;
		}
		FFLUSH(stdout);
	}
}

int
get_only(int fd, int wait)
{
	int ret;

	do {
		struct pollfd pfd[] = {
			{fd, POLLIN | POLLPRI, 0}
		};

		if (!(ret = poll(pfd, 1, wait)))
			return (-1);
		if (ret == 1 || ret == 2) {
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				int flags = 0;

				if (getmsg(fd, &ctrl, &data, &flags) == 0) {
					print_msg(fd);
					return (cmd.prim);
				}
				return (-1);
			}
		}
	} while (1);
}

int
put_only(int fd, int flags)
{
	print_msg(fd);
	if (putmsg(fd, &ctrl, NULL, flags) < 0) {
		printf("ERROR: putmsg: [%d] %s\n", errno, strerror(errno));
		return (-errno);
	}
	return (0);
}

void
put_and_get(int fd, int flags)
{
	if (put_only(fd, flags))
		return;
	get_only(fd, 10);
}

#ifdef LFS
static const char sctpname[] = "/dev/streams/clone/sctp_n";
#else
static const char sctpname[] = "/dev/sctp_n";
#endif

int
sctp_n_open(void)
{
	int fd;

	printf("\nOPEN: sctp_n\n");
	if ((fd = open(sctpname, O_NONBLOCK | O_RDWR)) < 0) {
		printf("ERROR: open: [%d] %s\n", errno, strerror(errno));
		exit(2);
	}
	return (fd);
}

void
sctp_close(int fd)
{
	printf("\n%d-CLOSE:\n", fd);
	if (close(fd) < 0) {
		printf("ERROR: close: [%d] %s\n", errno, strerror(errno));
		return;
	}
}
void
sctp_info_req(int fd)
{
	ctrl.len = sizeof(cmd.npi.info_req);
	cmd.prim = N_INFO_REQ;
	put_and_get(fd, RS_HIPRI);
}

void
sctp_bind_req(int fd, addr_t * addr, int coninds)
{
	ctrl.len = sizeof(cmd.npi.bind_req) + sizeof(*addr);
	cmd.prim = N_BIND_REQ;
	cmd.npi.bind_req.ADDR_length = sizeof(*addr);
	cmd.npi.bind_req.ADDR_offset = sizeof(cmd.npi.bind_req);
	cmd.npi.bind_req.CONIND_number = coninds;
	cmd.npi.bind_req.BIND_flags = TOKEN_REQUEST;
	cmd.npi.bind_req.PROTOID_length = 0;
	cmd.npi.bind_req.PROTOID_offset = 0;
	bcopy(addr, (&cmd.npi.bind_req) + 1, sizeof(*addr));
	put_and_get(fd, RS_HIPRI);
}

void
sctp_unbind(int fd)
{
	ctrl.len = sizeof(cmd.npi.unbind_req);
	cmd.prim = N_UNBIND_REQ;
	put_and_get(fd, RS_HIPRI);
}

void
sctp_conn_req(int fd, addr_t * addr)
{
	N_qos_sel_conn_sctp_t qos = { 0, };

	qos.n_qos_type = N_QOS_SEL_CONN_SCTP;
	qos.i_streams = 1;
	qos.o_streams = 1;
	ctrl.len = sizeof(cmd.npi.conn_req) + sizeof(*addr);
	cmd.prim = N_CONN_REQ;
	cmd.npi.conn_req.DEST_length = sizeof(*addr);
	cmd.npi.conn_req.DEST_offset = sizeof(cmd.npi.conn_req);
	cmd.npi.conn_req.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	cmd.npi.conn_req.QOS_length = sizeof(qos);
	cmd.npi.conn_req.QOS_offset = sizeof(cmd.npi.conn_req) + sizeof(*addr);
	bcopy(addr, (cmd.cbuf + sizeof(cmd.npi.conn_req)), sizeof(*addr));
	bcopy(&qos, (cmd.cbuf + sizeof(cmd.npi.conn_req) + sizeof(*addr)), sizeof(qos));
	put_only(fd, RS_HIPRI);
}

void
sctp_discon(int fd)
{
	ctrl.len = sizeof(cmd.npi.discon_req);
	cmd.prim = N_DISCON_REQ;
	cmd.npi.discon_req.DISCON_reason = 0;
	cmd.npi.discon_req.RES_length = 0;
	cmd.npi.discon_req.RES_offset = 0;
	cmd.npi.discon_req.SEQ_number = 0;
	put_only(fd, RS_HIPRI);
}

void
sctp_data(int fd, int fd2, const char *data)
{
	N_qos_sel_data_sctp_t qos;

	qos.n_qos_type = N_QOS_SEL_DATA_SCTP;
	qos.ppi = 3;
	qos.sid = 0;
	qos.ssn = 0;
	qos.more = 0;
	ctrl.len = sizeof(cmd.npi.data_req) + sizeof(qos);
	cmd.prim = N_DATA_REQ;
	cmd.npi.data_req.DATA_xfer_flags = N_RC_FLAG;
	put_and_get(fd, RS_HIPRI);
	get_only(fd2, 1);
}

void
sctp_wait(int fd)
{
	get_only(fd, 1);
}

int
do_tests(void)
{
	int fd1, fd2;
	addr_t addr1, addr2;

	printf("Simple test program for streams-sctp driver.\n");

	addr1.port = 10000;
	inet_aton("127.0.0.1", addr1.addr);

	addr2.port = 10001;
	inet_aton("127.0.0.1", addr2.addr);

	fd1 = sctp_n_open();
	fd2 = sctp_n_open();

	sctp_info_req(fd1);
	sctp_bind_req(fd1, &addr1, 0);
	sctp_info_req(fd1);

	sctp_info_req(fd2);
	sctp_bind_req(fd2, &addr2, 5);
	sctp_info_req(fd2);

	sctp_conn_req(fd1, &addr2);

	sctp_wait(fd2);
	sctp_wait(fd1);

	sctp_info_req(fd1);
	sctp_info_req(fd2);

	sctp_data(fd1, fd2, "Hello There fd2");
	sctp_data(fd1, fd2, "Hello There fd1");

	sctp_discon(fd1);
	sctp_wait(fd2);

	sctp_info_req(fd1);
	sctp_info_req(fd2);

	sctp_unbind(fd1);
	sctp_unbind(fd2);

	sctp_close(fd1);
	sctp_close(fd2);

	printf("Done.\n");

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
Copyright (c) 2001-2007 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
General Public License (GPL) Version 2,  so long as the  software is distributed\n\
with, and only used for the testing of, OpenSS7 modules, drivers, and libraries.\n\
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
    Copyright (c) 2001-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
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
			{NULL, }
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
