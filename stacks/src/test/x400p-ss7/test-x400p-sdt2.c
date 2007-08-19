/*****************************************************************************

 @(#) $RCSfile: test-x400p-sdt2.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/08/14 12:20:14 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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
 General Public License (GPL) Version 3, so long as the software is distributed
 with, and only used for the testing of, OpenSS7 modules, drivers, and
 libraries.

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

 Last Modified $Date: 2007/08/14 12:20:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-x400p-sdt2.c,v $
 Revision 0.9.2.7  2007/08/14 12:20:14  brian
 - GPLv3 header updates

 Revision 0.9.2.6  2006/03/07 01:18:19  brian
 - 64bit issues

 Revision 0.9.2.5  2006/03/04 13:00:44  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 Revision 0.9.2.4  2005/05/14 08:31:40  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-x400p-sdt2.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/08/14 12:20:14 $"

static char const ident[] = "$RCSfile: test-x400p-sdt2.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/08/14 12:20:14 $";

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define BUFSIZE 272

int verbose = 1;

const char *
lmi_strreason(unsigned int reason)
{
	const char *r;

	switch (reason) {
	default:
	case LMI_UNSPEC:
		r = "Unknown or unspecified";
		break;
	case LMI_BADADDRESS:
		r = "Address was invalid";
		break;
	case LMI_BADADDRTYPE:
		r = "Invalid address type";
		break;
	case LMI_BADDIAL:
		r = "(not used)";
		break;
	case LMI_BADDIALTYPE:
		r = "(not used)";
		break;
	case LMI_BADDISPOSAL:
		r = "Invalid disposal parameter";
		break;
	case LMI_BADFRAME:
		r = "Defective SDU received";
		break;
	case LMI_BADPPA:
		r = "Invalid PPA identifier";
		break;
	case LMI_BADPRIM:
		r = "Unregognized primitive";
		break;
	case LMI_DISC:
		r = "Disconnected";
		break;
	case LMI_EVENT:
		r = "Protocol-specific event ocurred";
		break;
	case LMI_FATALERR:
		r = "Device has become unusable";
		break;
	case LMI_INITFAILED:
		r = "Link initialization failed";
		break;
	case LMI_NOTSUPP:
		r = "Primitive not supported by this device";
		break;
	case LMI_OUTSTATE:
		r = "Primitive was issued from invalid state";
		break;
	case LMI_PROTOSHORT:
		r = "M_PROTO block too short";
		break;
	case LMI_SYSERR:
		r = "UNIX system error";
		break;
	case LMI_WRITEFAIL:
		r = "Unitdata request failed";
		break;
	case LMI_CRCERR:
		r = "CRC or FCS error";
		break;
	case LMI_DLE_EOT:
		r = "DLE EOT detected";
		break;
	case LMI_FORMAT:
		r = "Format error detected";
		break;
	case LMI_HDLC_ABORT:
		r = "Aborted frame detected";
		break;
	case LMI_OVERRUN:
		r = "Input overrun";
		break;
	case LMI_TOOSHORT:
		r = "Frame too short";
		break;
	case LMI_INCOMPLETE:
		r = "Partial frame received";
		break;
	case LMI_BUSY:
		r = "Telephone was busy";
		break;
	case LMI_NOANSWER:
		r = "Connection went unanswered";
		break;
	case LMI_CALLREJECT:
		r = "Connection rejected";
		break;
	case LMI_HDLC_IDLE:
		r = "HDLC line went idle";
		break;
	case LMI_HDLC_NOTIDLE:
		r = "HDLC link no longer idle";
		break;
	case LMI_QUIESCENT:
		r = "Line being reassigned";
		break;
	case LMI_RESUMED:
		r = "Line has been reassigned";
		break;
	case LMI_DSRTIMEOUT:
		r = "Did not see DSR in time";
		break;
	case LMI_LAN_COLLISIONS:
		r = "LAN excessive collisions";
		break;
	case LMI_LAN_REFUSED:
		r = "LAN message refused";
		break;
	case LMI_LAN_NOSTATION:
		r = "LAN no such station";
		break;
	case LMI_LOSTCTS:
		r = "Lost Clear to Send signal";
		break;
	case LMI_DEVERR:
		r = "Start of device-specific error codes";
		break;
	}
	return r;
}

void
do_lmi_get_msg(int fd)
{
	int ret, flags = 0;
	struct strbuf ctrl;
	struct strbuf data;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	union SDT_primitives *s = (union SDT_primitives *) cbuf;
	union SDL_primitives *q = (union SDL_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = 0;
	ctrl.buf = cbuf;
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
      do_get_again:
	printf("\nAttempting getmsg\n");
	if ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		return;
	} else
		printf("Getmsg succeeded!\n");
	if (ret == 0) {
		if (ctrl.len > 0) {
			switch ((long) p->lmi_primitive) {
			case LMI_INFO_ACK:
			{
				int ppalen = ctrl.len - sizeof(p->info_ack);

				printf("LMI_INFO_ACK:\n");
				printf("Version = 0x%08lx\n", p->info_ack.lmi_version);
				printf("State = %lu\n", p->info_ack.lmi_state);
				printf("Max sdu = %lu\n", p->info_ack.lmi_max_sdu);
				printf("Min sdu = %lu\n", p->info_ack.lmi_min_sdu);
				printf("Header len = %lu\n", p->info_ack.lmi_header_len);
				printf("PPA style = %lu\n", p->info_ack.lmi_ppa_style);
				printf("PPA length = %d\n", ppalen);
				printf("  card = %d\n", (*((ushort *) (&p->info_ack + 1)) >> 12) & 0xf);
				printf("  span = %d\n", (*((ushort *) (&p->info_ack + 1)) >> 8) & 0xf);
				printf("  chan = %d\n", (*((ushort *) (&p->info_ack + 1)) >> 0) & 0xff);
				return;
			}
			case LMI_OK_ACK:
				printf("LMI_OK_ACK:\n");
				printf("Correct primitive = %ld\n", p->ok_ack.lmi_correct_primitive);
				printf("State = %ld\n", p->ok_ack.lmi_state);
				return;
			case LMI_ERROR_ACK:
				printf("LMI_ERROR_ACK:\n");
				printf("Error number = %lu\n", p->error_ack.lmi_errno);
				printf("Error string = %s\n", strerror(p->error_ack.lmi_errno));
				printf("Reason number = %lu\n", p->error_ack.lmi_reason);
				printf("Reason string = %s\n", lmi_strreason(p->error_ack.lmi_reason));
				printf("Error primitive = %lu\n", p->error_ack.lmi_error_primitive);
				printf("State = %lu\n", p->error_ack.lmi_state);
				return;
			case LMI_ERROR_IND:
				printf("LMI_ERROR_IND:\n");
				printf("Error number = %lu\n", p->error_ind.lmi_errno);
				printf("Error string = %s\n", strerror(p->error_ind.lmi_errno));
				printf("Reason number = %lu\n", p->error_ind.lmi_reason);
				printf("Reason string = %s\n", lmi_strreason(p->error_ind.lmi_reason));
				printf("State = %lu\n", p->error_ind.lmi_state);
				goto do_get_again;
			case LMI_ENABLE_CON:
				printf("LMI_ENABLE_CON:\n");
				printf("State = %lu\n", p->enable_con.lmi_state);
				return;
			case LMI_DISABLE_CON:
				printf("LMI_DISABLE_CON:\n");
				printf("State = %lu\n", p->enable_con.lmi_state);
				return;
			case LMI_STATS_IND:
				printf("LMI_STATS_IND:\n");
				return;
			case LMI_EVENT_IND:
				printf("LMI_EVENT_IND:\n");
				return;
			case SDL_BITS_FOR_TRANSMISSION_REQ:
				printf("SDL_BITS_FOR_TRANSMISSION_REQ:\n");
				return;
			case SDL_CONNECT_REQ:
				printf("SDL_CONNECT_REQ:\n");
				printf("Direction = %s %s\n", (q->connect_req.sdl_flags & SDL_RX_DIRECTION) ? "RX" : "", (q->connect_req.sdl_flags & SDL_TX_DIRECTION) ? "TX" : "");
				return;
			case SDL_DISCONNECT_REQ:
				printf("SDL_DISCONNECT_REQ:\n");
				return;
			case SDL_RECEIVED_BITS_IND:
				printf("SDL_RECEIVED_BITS_IND:\n");
				return;
			case SDL_DISCONNECT_IND:
				printf("SDL_DISCONNECT_IND:\n");
				printf("Direction = %s %s\n", (q->connect_req.sdl_flags & SDL_RX_DIRECTION) ? "RX" : "", (q->connect_req.sdl_flags & SDL_TX_DIRECTION) ? "TX" : "");
				return;
			case SDT_DAEDT_TRANSMISSION_REQ:
				printf("SDT_DAEDT_TRANSMISSION_REQ:\n");
				return;
			case SDT_DAEDT_START_REQ:
				printf("SDT_DAEDT_START_REQ:\n");
				return;
			case SDT_DAEDR_START_REQ:
				printf("SDT_DAEDR_START_REQ:\n");
				return;
			case SDT_AERM_START_REQ:
				printf("SDT_AERM_START_REQ:\n");
				return;
			case SDT_AERM_SET_TI_TO_TIN_REQ:
				printf("SDT_AERM_SET_TI_TO_TIN_REQ:\n");
				return;
			case SDT_AERM_SET_TI_TO_TIE_REQ:
				printf("SDT_AERM_SET_TI_TO_TIE_REQ:\n");
				return;
			case SDT_SUERM_START_REQ:
				printf("SDT_SUERM_START_REQ:\n");
				return;
			case SDT_SUERM_STOP_REQ:
				printf("SDT_SUERM_STOP_REQ:\n");
				return;
			case SDT_RC_SIGNAL_UNIT_IND:
				printf("SDT_RC_SIGNAL_UNIT_IND:\n");
				if (data.len > 0) {
					int i;
					uint8_t *c = (uint8_t *) data.buf;

					printf("Message[%d x %lu]: ", data.len, s->rc_signal_unit_ind.sdt_count);
					for (i = 0; i < data.len; i++, c++)
						printf("%02X ", *c);
					printf("\n");
				}
				return;
			case SDT_RC_CONGESTION_ACCEPT_IND:
				printf("SDT_RC_CONGESTION_ACCEPT_IND:\n");
				return;
			case SDT_RC_CONGESTION_DISCARD_IND:
				printf("SDT_RC_CONGESTION_DISCARD_IND:\n");
				return;
			case SDT_RC_NO_CONGESTION_IND:
				printf("SDT_RC_NO_CONGESTION_IND:\n");
				return;
			case SDT_IAC_CORRECT_SU_IND:
				printf("SDT_IAC_CORRECT_SU_IND:\n");
				return;
			case SDT_LSC_LINK_FAILURE_IND:
				printf("SDT_LSC_LINK_FAILURE_IND:\n");
				return;
			case SDT_TXC_TRANSMISSION_REQUEST_IND:
				printf("SDT_TXC_TRANSMISSION_REQUEST_IND:\n");
				return;
			default:
				printf("Unrecognized response primitive %ld!\n", (long) p->lmi_primitive);
				goto do_get_again;
			}
		}
	}
}

void
info_req(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->info_req);
	ctrl.buf = cbuf;
	p->lmi_primitive = LMI_INFO_REQ;
	printf("\nAttempting info request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	do_lmi_get_msg(fd);
}

void
attach_req(int fd, uint16_t ppa)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->attach_req) + sizeof(ppa);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	bcopy(&ppa, p->attach_req.lmi_ppa, sizeof(ppa));
	printf("\nAttempting attach request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	do_lmi_get_msg(fd);
}

void
detach_req(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->lmi_primitive = LMI_DETACH_REQ;
	printf("\nAttempting detach request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	do_lmi_get_msg(fd);
}

void
enable_req(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->enable_req);
	ctrl.buf = cbuf;
	p->lmi_primitive = LMI_ENABLE_REQ;
	printf("\nAttempting enable request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	do_lmi_get_msg(fd);
}

void
disable_req(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->disable_req);
	ctrl.buf = cbuf;
	p->lmi_primitive = LMI_DISABLE_REQ;
	printf("\nAttempting disable request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	do_lmi_get_msg(fd);
}

void
sdl_connect(int fd, ulong flag)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union SDL_primitives *p = (union SDL_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->connect_req);
	ctrl.buf = cbuf;
	p->sdl_primitive = SDL_CONNECT_REQ;
	p->connect_req.sdl_flags = flag;
	printf("\nAttempting CONNECT request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	// do_lmi_get_msg(fd);
}

void
sdl_disconnect(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union SDL_primitives *p = (union SDL_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->disconnect_req);
	ctrl.buf = cbuf;
	p->sdl_primitive = SDL_DISCONNECT_REQ;
	p->disconnect_req.sdl_flags = SDL_RX_DIRECTION | SDL_TX_DIRECTION;
	printf("\nAttempting DISCONNECT request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	// do_lmi_get_msg(fd);
}

void
sdl_write(int fd)
{
	const size_t len = 64;
	const uint8_t buf[] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
	};
	int i, ret;

	i = 0;
	printf("\nAttempting write\n");
	for (;;) {
		ret = write(fd, buf, len);
		if (ret < 0) {
			if (errno == EINTR || errno == ERESTART) {
				if (i++ < 100)
					continue;
			}
			if (errno == EAGAIN) {
				return;
			}
			printf("error = %d\n", errno);
			perror("test");
			exit(2);
		} else {
			int i;

			printf("Write succeeded, wrote %d bytes!\n", ret);
			printf("Message[%ld]: ", (long) len);
			for (i = 0; i < len; i++) {
				printf("%02X ", buf[i]);
			}
			printf("\n");
		}
	}
}

void
sdl_read(int fd)
{
	uint8_t buf[4096];
	int i, ret;

	i = 0;
	// printf("\nAttempting read\n");
	for (;;) {
		ret = read(fd, buf, 4096);
		if (ret < 0) {
			if (errno == EINTR || errno == ERESTART) {
				if (i++ < 100)
					continue;
			}
			if (errno == EAGAIN) {
				return;
			}
			if (errno == EBADMSG) {
				do_lmi_get_msg(fd);
				continue;
			}
			printf("error = %d\n", errno);
			perror("test");
			exit(2);
		}
		// else printf("Read succeeded, read %d bytes!\n", ret);
		if (ret == 0)
			return;
		if (ret > 0) {
			int i;
			uint8_t *c = buf;

			printf("Message[%d]: ", ret);
			for (i = 0; i < ret; i++, c++) {
				printf("%02X ", *c);
			}
			printf("\n");
		}
	}
}

void
sdt_daedt_start(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union SDT_primitives *p = (union SDT_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->daedt_start_req);
	ctrl.buf = cbuf;
	p->sdt_primitive = SDT_DAEDT_START_REQ;
	printf("\nAttempting DAEDT start request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	// do_lmi_get_msg(fd);
}

void
sdt_daedr_start(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union SDT_primitives *p = (union SDT_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->daedr_start_req);
	ctrl.buf = cbuf;
	p->sdt_primitive = SDT_DAEDR_START_REQ;
	printf("\nAttempting DAEDR start request\n");
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("error = %d\n", errno);
		perror("test");
		exit(2);
	} else
		printf("Putmsg succeeded!\n");
	// do_lmi_get_msg(fd);
}

void
sdt_write(int fd)
{
#if 0
	const size_t len = 13;
	const char buf[] = "Hello World!";
#else
#if 0
	const size_t len = 46;
	const char buf[] = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOGS";
#else
#if 0
	const size_t len = 46;
	const uint8_t buf[] = {
		0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f,
		0xff, 0xff, 0xfe, 0xfc, 0xfa, 0xf8, 0xf0, 0xe0,
		0xc0, 0xa0, 0x80, 0x00, 0x01, 0x03, 0x07, 0x0f,
		0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xfe, 0xfc, 0xfa,
		0xf8, 0xf0, 0xe0, 0xc0, 0xa0, 0x80, 0x00, 0x01,
		0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f
	};
#else
#if 0
	const size_t len = 80;
	const uint8_t buf[] = {
		0x7f, 0x7f, 0x3f, 0x01, 0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
		0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
		0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
		0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
		0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
		0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
		0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
		0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
		0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d
	};
#else
	uint len = 4;
	unsigned char buf[] = {
		0xff, 0xff, 0x85, 0x00
	};
#endif
#endif
#endif
#endif
	int i, ret;

	i = 0;
	for (;;) {
		// printf("\nAttempting write\n");
		ret = write(fd, buf, len);
		if (ret < 0) {
			if (errno == EINTR || errno == ERESTART) {
				if (++i < 100)
					continue;
			}
			if (errno == EAGAIN) {
				return;
			}
			printf("error = %d\n", errno);
			perror("test");
			exit(2);
		}
#if 0
		else
			printf("Write succeeded, wrote %d bytes!\n", ret);
		if (ret > 0) {
			int j;

			printf("Message[%d]: ", len);
			for (j = 0; j < len; j++) {
				printf("%02x ", buf[j]);
			}
			printf("\n");
		}
#endif
	}
}

void
sdt_read(int fd)
{
	uint8_t buf[256];
	int i, ret;

	i = 0;
	printf("\nAttempting read\n");
	for (;;) {
		ret = read(fd, buf, 256);
		if (ret < 0) {
			if (errno == EAGAIN) {
				if (i++ < 100)
					continue;
			}
			if (errno == EBADMSG) {
				do_lmi_get_msg(fd);
				continue;
			}
			printf("error = %d\n", errno);
			perror("test");
			break;
		} else
			printf("Read succeeded, read %d bytes!\n", ret);
		if (ret > 0) {
			int i;
			uint8_t *c = buf;

			printf("Message[%d]: ", ret);
			for (i = 0; i < ret; i++, c++) {
				printf("%02X ", *c);
			}
			printf("\n");
		}
	}
}

int
sdt_ioctl(int fd, int cmd, void *arg, int len)
{
	struct strioctl ctl = { cmd, 0, len, arg };
	int ret;

	ret = ioctl(fd, I_STR, &ctl);
	if (ret < 0) {
		printf("return = %d\n", ret);
		printf("error = %d\n", errno);
		perror("sdt_ioctl");
		exit(2);
	}
	return ctl.ic_len;
}

void
do_ioctl(int fd, int cmd, int arg)
{
	int ret;

	ret = ioctl(fd, cmd, arg);
	if (ret < 0) {
		printf("return = %d\n", ret);
		printf("error = %d\n", errno);
		perror("do_ioctl");
		exit(2);
	}
}

void
sdl_config(int fd)
{
	unsigned char buf[256];
	sdl_config_t *c = (sdl_config_t *) buf;
	sdl_statem_t *s = (sdl_statem_t *) buf;

	sdt_ioctl(fd, SDL_IOCGCONFIG, buf, sizeof(sdl_config_t));
	printf("Config:\n");
	printf("  ifflags       = %lu\n", c->ifflags);
	printf("  iftype        = %lu\n", c->iftype);
	printf("  ifrate        = %lu\n", c->ifrate);
	printf("  ifgtype       = %lu\n", c->ifgtype);
	printf("  ifgrate       = %lu\n", c->ifgrate);
	printf("  ifgcrc        = %lu\n", c->ifgcrc);
	printf("  ifmode        = %lu\n", c->ifmode);
	printf("  ifclock       = %lu\n", c->ifclock);
	printf("  ifcoding      = %lu\n", c->ifcoding);
	printf("  ifframing     = %lu\n", c->ifframing);
	printf("  ifblksize     = %lu\n", c->ifblksize);
	printf("  ifleads       = %lu\n", c->ifleads);
	printf("  ifalarms      = %lu\n", c->ifalarms);
	printf("  ifrxlevel     = %lu\n", c->ifrxlevel);
	printf("  iftxlevel     = %lu\n", c->iftxlevel);
	printf("  ifsync        = %lu\n", c->ifsync);
	printf("  ifsyncsrc[0]  = %lu\n", c->ifsyncsrc[0]);
	printf("  ifsyncsrc[1]  = %lu\n", c->ifsyncsrc[1]);
	printf("  ifsyncsrc[2]  = %lu\n", c->ifsyncsrc[2]);
	printf("  ifsyncsrc[3]  = %lu\n", c->ifsyncsrc[3]);
	sdt_ioctl(fd, SDL_IOCGSTATEM, buf, sizeof(sdl_statem_t));
	printf("State:\n");
	printf("  tx_state      = %lu\n", s->tx_state);
	printf("  rx_state      = %lu\n", s->rx_state);
}

void
sdt_config(int fd)
{
	unsigned char buf[256];
	sdt_config_t *c = (sdt_config_t *) buf;
	sdt_statem_t *s = (sdt_statem_t *) buf;

	printf("Getting configuration\n");
	sdt_ioctl(fd, SDT_IOCGCONFIG, buf, sizeof(sdt_config_t));
	printf("Config:\n");
	printf("  t8  = %lu\n", c->t8);
	printf("  Tin = %lu\n", c->Tin);
	printf("  Tie = %lu\n", c->Tie);
	printf("  T   = %lu\n", c->T);
	printf("  D   = %lu\n", c->D);
	printf("  Te  = %lu\n", c->Te);
	printf("  De  = %lu\n", c->De);
	printf("  Ue  = %lu\n", c->Ue);
	printf("  N   = %lu\n", c->N);
	printf("  m   = %lu\n", c->m);
	printf("  b   = %lu\n", c->b);
	sdt_ioctl(fd, SDT_IOCGSTATEM, buf, sizeof(sdt_statem_t));
	printf("State:\n");
	printf("  aerm_state.. = %lu\n", s->aerm_state);
	printf("    aborted_proving.... = %lu\n", s->aborted_proving);
	printf("    Ca................. = %lu\n", s->Ca);
	printf("    Ti................. = %lu\n", s->Ti);
	printf("  suerm_state. = %lu\n", s->suerm_state);
	printf("    Cs................. = %lu\n", s->Cs);
	printf("    Ns................. = %lu\n", s->Ns);
	printf("  eim_state... = %lu\n", s->eim_state);
	printf("    Ce................. = %lu\n", s->Ce);
	printf("    su_received........ = %lu\n", s->su_received);
	printf("    interval_error..... = %lu\n", s->interval_error);
	printf("  daedt_state. = %lu\n", s->daedt_state);
	printf("  daedr_state. = %lu\n", s->daedr_state);
	printf("    octet_counting_mode = %lu\n", s->octet_counting_mode);
	sdl_config(fd);
}

void
sdl_stats(int fd)
{
	unsigned char buf[256];
	sdl_stats_t *s = (sdl_stats_t *) buf;

	printf("Attempting stats collection\n");
	sdt_ioctl(fd, SDL_IOCGSTATS, buf, sizeof(sdl_stats_t));
	printf("Stats:\n");
	printf("  rx_octets............ = %lu\n", s->rx_octets);
	printf("  tx_octets............ = %lu\n", s->tx_octets);
	printf("  rx_overruns.......... = %lu\n", s->rx_overruns);
	printf("  tx_underruns......... = %lu\n", s->tx_underruns);
	printf("  rx_buffer_overflows.. = %lu\n", s->rx_buffer_overflows);
	printf("  tx_buffer_overflows.. = %lu\n", s->tx_buffer_overflows);
	printf("  lead_cts_lost........ = %lu\n", s->lead_cts_lost);
	printf("  lead_dcd_lost........ = %lu\n", s->lead_dcd_lost);
	printf("  carrier_lost......... = %lu\n", s->carrier_lost);
}

void
sdt_stats(int fd)
{
	unsigned char buf[256];
	sdt_stats_t *s = (sdt_stats_t *) buf;

	printf("Attempting stats collection\n");
	sdt_ioctl(fd, SDT_IOCGSTATS, buf, sizeof(sdt_stats_t));
	printf("Stats:\n");
	printf("  tx_bytes............. = %lu\n", s->tx_bytes);
	printf("  tx_sus............... = %lu\n", s->tx_sus);
	printf("  tx_sus_repeated...... = %lu\n", s->tx_sus_repeated);
	printf("  tx_underruns......... = %lu\n", s->tx_underruns);
	printf("  tx_aborts............ = %lu\n", s->tx_aborts);
	printf("  tx_buffer_overflows.. = %lu\n", s->tx_buffer_overflows);
	printf("  tx_sus_in_error...... = %lu\n", s->tx_sus_in_error);
	printf("  rx_bytes............. = %lu\n", s->rx_bytes);
	printf("  rx_sus............... = %lu\n", s->rx_sus);
	printf("  rx_sus_compressed.... = %lu\n", s->rx_sus_compressed);
	printf("  rx_overruns.......... = %lu\n", s->rx_overruns);
	printf("  rx_aborts............ = %lu\n", s->rx_aborts);
	printf("  rx_buffer_overflows.. = %lu\n", s->rx_buffer_overflows);
	printf("  rx_sus_in_error...... = %lu\n", s->rx_sus_in_error);
	printf("  rx_sync_transitions.. = %lu\n", s->rx_sync_transitions);
	printf("  rx_bits_octet_counted = %lu\n", s->rx_bits_octet_counted);
	printf("  rx_crc_errors........ = %lu\n", s->rx_crc_errors);
	printf("  rx_frame_errors...... = %lu\n", s->rx_frame_errors);
	printf("  rx_frame_overflows... = %lu\n", s->rx_frame_overflows);
	printf("  rx_frame_too_long.... = %lu\n", s->rx_frame_too_long);
	printf("  rx_frame_too_short... = %lu\n", s->rx_frame_too_short);
	printf("  rx_residue_errors.... = %lu\n", s->rx_residue_errors);
	printf("  rx_length_error...... = %lu\n", s->rx_length_error);
	printf("  carrier_cts_lost..... = %lu\n", s->carrier_cts_lost);
	printf("  carrier_dcd_lost..... = %lu\n", s->carrier_dcd_lost);
	printf("  carrier_lost......... = %lu\n", s->carrier_lost);
	sdl_stats(fd);
}

int
do_tests(void)
{
	int pfd[2], k;
	uint16_t ppa[2] = { 0x0012, 0x0112 };

	printf("Opening one stream\n");
	if ((pfd[0] = open("/dev/x400p-sl", O_NONBLOCK | O_RDWR)) < 0) {
		perror("open");
		exit(2);
	}
	printf("Opening other stream\n");
	if ((pfd[1] = open("/dev/x400p-sl", O_NONBLOCK | O_RDWR)) < 0) {
		perror("open");
		exit(2);
	}

	do_ioctl(pfd[0], I_SRDOPT, RMSGD);
	do_ioctl(pfd[1], I_SRDOPT, RMSGD);

	do_ioctl(pfd[0], I_SETCLTIME, 0);
	do_ioctl(pfd[1], I_SETCLTIME, 0);

	info_req(pfd[0]);
	info_req(pfd[1]);

	attach_req(pfd[0], ppa[0]);
	attach_req(pfd[1], ppa[1]);

	info_req(pfd[0]);
	info_req(pfd[1]);

	enable_req(pfd[0]);
	enable_req(pfd[1]);

	fprintf(stderr, "Allowing spans to stabilize for 5 seconds\n");
	sleep(5);

	sdt_config(pfd[0]);
	sdl_config(pfd[1]);
	sdt_stats(pfd[0]);
	sdl_stats(pfd[1]);

	fprintf(stderr, "DAEDR/DAEDT start and CONNECT in 5 seconds\n");
	sleep(5);
	sdt_daedt_start(pfd[0]);
	sdl_connect(pfd[1], SDL_TX_DIRECTION);
	sdt_daedr_start(pfd[0]);
	sdl_connect(pfd[1], SDL_RX_DIRECTION);

	/* 
	 *  Do some reads and writes.
	 */

	for (k = 0; k < 100; k++) {
		sdt_write(pfd[0]);
		sdl_read(pfd[1]);
	}

	sdt_stats(pfd[0]);
	sdl_stats(pfd[1]);

	fprintf(stderr, "Disable one in 5 seconds\n");
	sleep(5);
	disable_req(pfd[0]);
	fprintf(stderr, "Disable other in 5 seconds\n");
	sleep(5);
	disable_req(pfd[1]);

	fprintf(stderr, "Detach one in 5 seconds\n");
	sleep(5);
	detach_req(pfd[1]);
	fprintf(stderr, "Detach other in 5 seconds\n");
	sleep(5);
	detach_req(pfd[0]);

	fprintf(stderr, "Close in 5 seconds\n");
	sleep(5);
	printf("Closing one side\n");
	close(pfd[0]);
	printf("Closing other side\n");
	close(pfd[1]);

	printf("Done\n");
	return (0);
}

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
ITU-T RECOMMENDATAION Q.781 - Conformance Test Suite\n\
\n\
Copyright (c) 2001-2006 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
General Public License (GPL) Version 3,  so long as the  software is distributed\n\
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
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock\n\
\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision$ $Date$");
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
			goto bad_usage;
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
