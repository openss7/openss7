/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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
 Affero General Public License (AGPL) Version 3, so long as the software is
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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define BUFSIZE 280

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
				printf("Version = 0x%08x\n", p->info_ack.lmi_version);
				printf("State = %u\n", p->info_ack.lmi_state);
				printf("Max sdu = %u\n", p->info_ack.lmi_max_sdu);
				printf("Min sdu = %u\n", p->info_ack.lmi_min_sdu);
				printf("Header len = %u\n", p->info_ack.lmi_header_len);
				printf("PPA style = %u\n", p->info_ack.lmi_ppa_style);
				printf("PPA length = %d\n", ppalen);
				return;
			}
			case LMI_OK_ACK:
				printf("LMI_OK_ACK:\n");
				printf("Correct primitive = %d\n", p->ok_ack.lmi_correct_primitive);
				printf("State = %d\n", p->ok_ack.lmi_state);
				return;
			case LMI_ERROR_ACK:
				printf("LMI_ERROR_ACK:\n");
				printf("Error number = %u\n", p->error_ack.lmi_errno);
				printf("Error string = %s\n", strerror(p->error_ack.lmi_errno));
				printf("Reason number = %u\n", p->error_ack.lmi_reason);
				printf("Reason string = %s\n", lmi_strreason(p->error_ack.lmi_reason));
				printf("Error primitive = %u\n", p->error_ack.lmi_error_primitive);
				printf("State = %u\n", p->error_ack.lmi_state);
				return;
			case LMI_ERROR_IND:
				printf("LMI_ERROR_IND:\n");
				printf("Error number = %u\n", p->error_ind.lmi_errno);
				printf("Error string = %s\n", strerror(p->error_ind.lmi_errno));
				printf("Reason number = %u\n", p->error_ind.lmi_reason);
				printf("Reason string = %s\n", lmi_strreason(p->error_ind.lmi_reason));
				printf("State = %u\n", p->error_ind.lmi_state);
				goto do_get_again;
			case LMI_ENABLE_CON:
				printf("LMI_ENABLE_CON:\n");
				printf("State = %u\n", p->enable_con.lmi_state);
				return;
			case LMI_DISABLE_CON:
				printf("LMI_DISABLE_CON:\n");
				printf("State = %u\n", p->disable_con.lmi_state);
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
				return;
			case SDL_DISCONNECT_REQ:
				printf("SDL_DISCONNECT_REQ:\n");
				return;
			case SDL_RECEIVED_BITS_IND:
				printf("SDL_RECEIVED_BITS_IND:\n");
				return;
			case SDL_DISCONNECT_IND:
				printf("SDL_DISCONNECT_IND:\n");
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
sdl_connect(int fd)
{
	int ret;
	struct strbuf ctrl;
	char cbuf[BUFSIZE];
	union SDL_primitives *p = (union SDL_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->connect_req);
	ctrl.buf = cbuf;
	p->sdl_primitive = SDL_CONNECT_REQ;
	p->connect_req.sdl_flags = SDL_RX_DIRECTION | SDL_TX_DIRECTION;
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
	p->connect_req.sdl_flags = SDL_RX_DIRECTION | SDL_TX_DIRECTION;
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
		} else
			printf("Write succeeded, wrote %d bytes!\n", ret);
		if (ret > 0) {
			int i;

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
	uint8_t buf[256];
	int i, ret;

	i = 0;
	printf("\nAttempting read\n");
	for (;;) {
		ret = read(fd, buf, 256);
		if (ret < 0) {
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART) {
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
sdl_ioctl(int fd, int cmd, void *arg, int len)
{
	struct strioctl ctl = { cmd, 0, len, arg };
	int ret;

	ret = ioctl(fd, I_STR, &ctl);
	if (ret < 0) {
		printf("return = %d\n", ret);
		printf("error = %d\n", errno);
		perror("sdl_ioctl");
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

	printf("Getting configuration\n");
	sdl_ioctl(fd, SDL_IOCGCONFIG, buf, sizeof(sdl_config_t));
	printf("Config:\n");
	printf("  ifflags       = %u\n", c->ifflags);
	printf("  iftype        = %u\n", c->iftype);
	printf("  ifrate        = %u\n", c->ifrate);
	printf("  ifgtype       = %u\n", c->ifgtype);
	printf("  ifgrate       = %u\n", c->ifgrate);
	printf("  ifgcrc        = %u\n", c->ifgcrc);
	printf("  ifmode        = %u\n", c->ifmode);
	printf("  ifclock       = %u\n", c->ifclock);
	printf("  ifcoding      = %u\n", c->ifcoding);
	printf("  ifframing     = %u\n", c->ifframing);
	printf("  ifblksize     = %u\n", c->ifblksize);
	printf("  ifleads       = %u\n", c->ifleads);
	printf("  ifalarms      = %u\n", c->ifalarms);
	printf("  ifrxlevel     = %u\n", c->ifrxlevel);
	printf("  iftxlevel     = %u\n", c->iftxlevel);
	printf("  ifsync        = %u\n", c->ifsync);
	printf("  ifsyncsrc[0]  = %u\n", c->ifsyncsrc[0]);
	printf("  ifsyncsrc[1]  = %u\n", c->ifsyncsrc[1]);
	printf("  ifsyncsrc[2]  = %u\n", c->ifsyncsrc[2]);
	printf("  ifsyncsrc[3]  = %u\n", c->ifsyncsrc[3]);
	sdl_ioctl(fd, SDL_IOCGSTATEM, buf, sizeof(sdl_statem_t));
	printf("State:\n");
	printf("  tx_state      = %u\n", s->tx_state);
	printf("  rx_state      = %u\n", s->rx_state);
}

void
sdl_stats(int fd)
{
	unsigned char buf[256];
	sdl_stats_t *s = (sdl_stats_t *) buf;

	printf("Attempting stats collection\n");
	sdl_ioctl(fd, SDL_IOCGSTATS, buf, sizeof(sdl_stats_t));
	printf("Stats:\n");
	printf("  rx_octets............ = %u\n", s->rx_octets);
	printf("  tx_octets............ = %u\n", s->tx_octets);
	printf("  rx_overruns.......... = %u\n", s->rx_overruns);
	printf("  tx_underruns......... = %u\n", s->tx_underruns);
	printf("  rx_buffer_overflows.. = %u\n", s->rx_buffer_overflows);
	printf("  tx_buffer_overflows.. = %u\n", s->tx_buffer_overflows);
	printf("  lead_cts_lost........ = %u\n", s->lead_cts_lost);
	printf("  lead_dcd_lost........ = %u\n", s->lead_dcd_lost);
	printf("  carrier_lost......... = %u\n", s->carrier_lost);
}

int
do_tests(void)
{
	int i, pfd[2];
	uint16_t ppa[2] = { 0x0012, 0x0112 };

	printf("Opening one stream\n");
	if ((pfd[0] = open("/dev/x400p-sdl", O_NONBLOCK | O_RDWR)) < 0) {
		perror("open");
		exit(2);
	}
	printf("Opening other stream\n");
	if ((pfd[1] = open("/dev/x400p-sdl", O_NONBLOCK | O_RDWR)) < 0) {
		perror("open");
		exit(2);
	}
	// do_ioctl(pfd[0], I_SRDOPT, RMSGD);
	// do_ioctl(pfd[1], I_SRDOPT, RMSGD);

	do_ioctl(pfd[0], I_SETCLTIME, 0);
	do_ioctl(pfd[1], I_SETCLTIME, 0);

	info_req(pfd[0]);
	info_req(pfd[1]);

	attach_req(pfd[0], ppa[0]);
	attach_req(pfd[1], ppa[1]);

	sdl_config(pfd[0]);
	sdl_config(pfd[1]);
	sdl_stats(pfd[0]);
	sdl_stats(pfd[1]);

	enable_req(pfd[0]);
	enable_req(pfd[1]);

	sdl_config(pfd[0]);
	sdl_config(pfd[1]);
	sdl_stats(pfd[0]);
	sdl_stats(pfd[1]);

	sdl_connect(pfd[0]);
	sdl_connect(pfd[1]);

	sdl_config(pfd[0]);
	sdl_config(pfd[1]);
	sdl_stats(pfd[0]);
	sdl_stats(pfd[1]);

	// sleep(20);

	sdl_read(pfd[0]);
	sdl_read(pfd[1]);

	/* 
	 *  Do some reads and writes.
	 */
	for (i = 0; i < 100; i++) {
		sdl_write(pfd[0]);
		sdl_read(pfd[1]);
		sdl_write(pfd[1]);
		sdl_read(pfd[0]);
	}

	sdl_config(pfd[0]);
	sdl_config(pfd[1]);
	sdl_stats(pfd[0]);
	sdl_stats(pfd[1]);

	sdl_disconnect(pfd[1]);
	sdl_disconnect(pfd[0]);

	sdl_config(pfd[0]);
	sdl_config(pfd[1]);
	sdl_stats(pfd[0]);
	sdl_stats(pfd[1]);

	disable_req(pfd[1]);
	disable_req(pfd[0]);

	sdl_config(pfd[0]);
	sdl_config(pfd[1]);
	sdl_stats(pfd[0]);
	sdl_stats(pfd[1]);

	detach_req(pfd[1]);
	detach_req(pfd[0]);

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
Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
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
Copyright (c) 2008, 2009  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU Affero General Public License Version 3,\n\
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
        print the version and exit\n\
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
