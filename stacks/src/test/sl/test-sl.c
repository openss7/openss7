/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

int board = 1;
int span = 1;
int channel = 16;

int output = 1;

lmi_option_t lmi_conf = {
	.pvar = SS7_PVAR_ITUT_96,
	.popt = 0,
}, lmi_conf_read;

#ifndef HZ
#define HZ 100
#endif

const sl_config_t sl_conf_itu = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};
const sl_config_t sl_conf_ansi = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};

sl_config_t sl_conf, sl_conf_read;

const sdt_config_t sdt_conf_itu = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 793544,
	.De = 11328000,
	.Ue = 198384000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
#if 0
	.f = SDT_FLAGS_SHARED,
	.f = SDT_FLAGS_TWO,
	.f = SDT_FLAGS_THREE,
#endif
};
const sdt_config_t sdt_conf_ansi = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 577169,
	.De = 9308000,
	.Ue = 144292000,
	.N = 16,
	.m = 272,
	.b = 7,
	.f = SDT_FLAGS_ONE,
#if 0
	.f = SDT_FLAGS_SHARED,
	.f = SDT_FLAGS_TWO,
	.f = SDT_FLAGS_THREE,
#endif
};
sdt_config_t sdt_conf, sdt_conf_read;

const sdl_config_t sdl_conf_itu = {
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0,
	.ifrate = 64000,
	.ifgtype = SDL_GTYPE_E1,
	.ifgrate = 2048000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC5,	/* E1 only */
	.ifclock = SDL_CLOCK_SLAVE,
	.ifcoding = SDL_CODING_HDB3,	/* E1 only */
	.ifframing = SDL_FRAMING_CCS,	/* E1 only */
	.ifblksize = 8,
	.ifleads = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 1,
	.ifsync = 0,
	.ifsyncsrc = {1, 2, 0, 0}
};
const sdl_config_t sdl_conf_ansi = {
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0A,
	.ifrate = 56000,
	.ifgtype = SDL_GTYPE_NONE,
	.ifgrate = 0,
	.ifmode = SDL_MODE_NONE,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_NONE,	/* T1 only */
	.ifclock = SDL_CLOCK_NONE,
	.ifcoding = SDL_CODING_NONE,	/* T1 only */
	.ifframing = SDL_FRAMING_NONE,	/* T1 only */
	.ifblksize = 0,
	.ifleads = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 0,
	.ifsync = 0,
	.ifsyncsrc = {0, 0, 0, 0}
};
sdl_config_t sdl_conf, sdl_conf_read;

void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
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
General Public License (GPL) Version 3,  so long as the  software is distributed\n\
with, and only used for management of, OpenSS7 modules, drivers, and libraries.\n\
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
");
}

void
version(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007  OpenSS7 Corporation\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for copying permission.\n\
", NAME, PACKAGE, VERSION, "$Revision: 0.9.2.9 $ $Date: 2007/08/19 11:57:40 $");
}

void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\n\
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
	if (!output)
		return;
	fprintf(stdout, "\n\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
General Options:\n\
    -q, --quiet						(default: off)\n\
	suppress output\n\
    -v, --verbose					(default: off)\n\
	increase verbosity of output\n\
Command Options:\n\
    -h, --help\n\
	prints this usage information and exits\n\
    -V, --version\n\
	prints the version and exits\n\
    -C, --copying\n\
	prints the copying permissions and exits\n\
Setting Options:\n\
    -b, --board board					(default: %2$d)\n\
	board specifies the board number to open\n\
    -s, --span span					(default: %3$d)\n\
	span specifies the span on the card to open\n\
    -c, --channel channel				(default: %4$d)\n\
	channel specifies the time slot in the span to open\n\
    --flags number					(default: %5$d)\n\
	number specifies the number of flags (0 means 1)\n\
    --crc4, --crc5, --crc6				(default: crc6)\n\
	specifies the crc to use\n\
    --hdb3, --ami, --b8zs				(default: b8zs)\n\
	specifies the line coding to use\n\
    --cas, --ccs, --sf, --esf				(default: esf)\n\
	specifies the framing to use\n\
    --t1, --e1						(default: t1)\n\
	sets default settings for T1 or E1 operation\n\
\n\
This program opens and attaches a Signalling Link channel, activates the\n\
links, and reads and writes SLTM/SLTA to and from the channel.  The program\n\
prints continuous information to stdout.\n\
\n", argv[0], board, span, channel, (int) sdt_conf.f);
};

int fd;

typedef unsigned short ppa_t;
ppa_t ppa;

#ifndef HZ
#define HZ 100
#endif

#define BUFSIZE 300

char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strioctl ctl;
struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
struct strbuf data = { sizeof(*dbuf), 0, dbuf };
union LMI_primitives *m = (union LMI_primitives *) cbuf;
union SL_primitives *p = (union SL_primitives *) cbuf;

int timer_timeout = 0;

void
timer_handler(int signum)
{
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

int
timer_sethandler(void)
{
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = timer_handler;
	act.sa_flags = SA_RESTART | SA_ONESHOT;
	act.sa_restorer = NULL;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return -1;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	return 0;
}

/*
 *  Start an interval timer as the overall test timer.
 */
int
start_timer(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (timer_sethandler())
		return -1;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return -1;
	timer_timeout = 0;
	return 0;
}

int
stop_timer(void)
{
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	act.sa_restorer = NULL;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return -1;
	timer_timeout = 0;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	return 0;
}

#define TIMEOUT (-2)

int
decode_msg(void)
{
	if (ctrl.len >= sizeof(ulong))
		return *(ulong *) cbuf;
	return -1;
}

int
decode_data(void)
{
	if (data.len > 5)
		return SL_PDU_IND;
	return -1;
}

int
wait_event(int wait)
{
	while (1) {
		struct pollfd pfd[] = {
			{fd, POLLIN | POLLPRI, 0}
		};

		if (timer_timeout) {
			timer_timeout = 0;
			return TIMEOUT;
		}
		switch (poll(pfd, 1, wait)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			perror(__FUNCTION__);
			exit(1);
		case 0:
			return (0);
		case 1:
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				int ret, flags = 0;

				ctrl.maxlen = BUFSIZE;
				ctrl.len = 0;
				ctrl.buf = cbuf;
				data.maxlen = BUFSIZE;
				data.len = 0;
				data.buf = dbuf;
				if (getmsg(fd, &ctrl, &data, &flags) < 0) {
					perror(__FUNCTION__);
					exit(1);
				}
				if (ctrl.len > 0 && (ret = decode_msg()) != -1)
					return ret;
				if (data.len > 0 && (ret = decode_data()) != -1)
					return ret;
			}
		default:
			break;
		}
	}
}

void
handle_message(void)
{
	/* turn around SLTM into SLTA only */
	unsigned char *q = dbuf;
	uint sio, dpc, opc, sls, h0, h1, b;
	int i;

	printf("Received:   ");
	for (i = 0; i < data.len; i++)
		printf("%02x ", dbuf[i] & 0xff);
	printf("\n");
	fflush(stdout);
	sio = *q++;
	if ((sio & 0xf) == 1 || (sio & 0xf) == 2) {
		b = *q++ & 0x00ff;
		dpc = ((b << 0) & 0xff);
		b = *q++ & 0x00ff;
		dpc |= ((b << 8) & 0x3f00);
		opc = ((b >> 6) & 0x03);
		b = *q++ & 0x00ff;
		opc |= ((b << 2) & 0x3fc);
		b = *q++ & 0x00ff;
		opc |= ((b << 10) & 0x3c00);
		sls = (b >> 4) & 0x0f;
		h0 = *q & 0x0f;
		h1 = *q++ >> 4;
		if (h0 == 1 && h1 == 1) {
			/* send back */
			ctrl.maxlen = BUFSIZE;
			ctrl.len = sizeof(p->pdu_req);
			ctrl.buf = cbuf;
			p->pdu_req.sl_primitive = SL_PDU_REQ;
			p->pdu_req.sl_mp = 0x3;
			if (putmsg(fd, &ctrl, &data, 0) < 0)
				perror(__FUNCTION__);
			q = dbuf;
			h1 = 2;
			q++;
			*q++ = opc;
			*q++ = ((opc >> 8) & 0x3f) | (dpc << 6);
			*q++ = (dpc >> 2);
			*q++ = ((dpc >> 10) & 0x0f) | (sls << 4);
			*q++ = h0 | (h1 << 4);
			printf("Responding: ");
			for (i = 0; i < data.len; i++)
				printf("%02x ", dbuf[i] & 0xff);
			printf("\n");
			printf("DPC = %04x, OPC = %04x\n", dpc, opc);
			fflush(stdout);
			/* send back */
			ctrl.maxlen = BUFSIZE;
			ctrl.len = sizeof(p->pdu_req);
			ctrl.buf = cbuf;
			p->pdu_req.sl_primitive = SL_PDU_REQ;
			p->pdu_req.sl_mp = 0x3;
			if (putmsg(fd, &ctrl, &data, 0) < 0)
				perror(__FUNCTION__);
		}
	}
}

void
handle_message_ansi(void)
{
	/* turn around SLTM into SLTA only */
	unsigned char *q = dbuf;
	uint sio, dpc, opc, sls, h0, h1, b, tag;
	int i;

	printf("Received:   ");
	for (i = 0; i < data.len; i++)
		printf("%02x ", dbuf[i] & 0xff);
	printf("\n");
	fflush(stdout);
	sio = *q++;
	b = *q++ & 0x00ff;
	dpc = b << 0;
	b = *q++ & 0x00ff;
	dpc |= b << 8;
	b = *q++ & 0x00ff;
	dpc |= b << 16;
	b = *q++ & 0x00ff;
	opc = b << 0;
	b = *q++ & 0x00ff;
	opc |= b << 8;
	b = *q++ & 0x00ff;
	opc |= b << 16;
	b = *q++ & 0x00ff;
	sls = b;
	h0 = *q & 0x0f;
	h1 = (*q++ >> 4) & 0x0f;
	tag = ((h0 & 0xf) << 4) | (h1 & 0xf);
	switch (sio & 0xf) {
	case 0:		/* SNMM */
		switch (tag) {
		case 0x11:	/* coo */
		case 0x12:	/* coa */
		case 0x15:	/* cbd */
		case 0x16:	/* cba */
		case 0x21:	/* eco */
		case 0x22:	/* eca */
		case 0x31:	/* rct */
		case 0x32:	/* tfc */
		case 0x41:	/* tfp */
		case 0x42:	/* tcp */
		case 0x43:	/* tfr */
		case 0x44:	/* tcr */
		case 0x45:	/* tfa */
		case 0x46:	/* tca */
		case 0x51:	/* rst */
		case 0x52:	/* rsr */
		case 0x53:	/* rcp */
		case 0x54:	/* rcr */
		case 0x61:	/* lin */
		case 0x62:	/* lun */
		case 0x63:	/* lia */
		case 0x64:	/* lua */
		case 0x65:	/* lid */
		case 0x66:	/* lfu */
		case 0x67:	/* llt */
		case 0x68:	/* lrt */
		case 0x71:	/* tra */
		case 0x72:	/* trw */
		case 0x81:	/* dlc */
		case 0x82:	/* css */
		case 0x83:	/* cns */
		case 0x84:	/* cnp */
		case 0xa1:	/* upu */
		case 0xa2:	/* upa *//* ansi91 only */
		case 0xa3:	/* upt *//* ansi91 only */
		default:
			break;
		}
		break;
	case 1:		/* SNTM */
		switch (tag) {
		case 0x11:	/* sltm */
			goto send_slta;
		case 0x12:	/* slta */
		default:
			break;
		}
		break;
	case 2:		/* SSNTM */
		switch (tag) {
		case 0x11:	/* ssltm */
			goto send_slta;
		case 0x12:	/* sslta */
		default:
			break;
		}
		break;
	      send_slta:
		/* send back */
		ctrl.maxlen = BUFSIZE;
		ctrl.len = sizeof(p->pdu_req);
		ctrl.buf = cbuf;
		p->pdu_req.sl_primitive = SL_PDU_REQ;
		p->pdu_req.sl_mp = 0x3;
		if (putmsg(fd, &ctrl, &data, 0) < 0)
			perror(__FUNCTION__);
		q = dbuf;
		h1 = 2;
		q++;
		*q++ = opc;
		*q++ = opc >> 8;
		*q++ = opc >> 16;
		*q++ = dpc;
		*q++ = dpc >> 8;
		*q++ = dpc >> 16;
		*q++ = sls;
		*q++ = h0 | (h1 << 4);
		printf("Responding: ");
		for (i = 0; i < data.len; i++)
			printf("%02x ", dbuf[i] & 0xff);
		printf("\n");
		printf("DPC = %04x, OPC = %04x\n", dpc, opc);
		fflush(stdout);
		/* send back */
		ctrl.maxlen = BUFSIZE;
		ctrl.len = sizeof(p->pdu_req);
		ctrl.buf = cbuf;
		p->pdu_req.sl_primitive = SL_PDU_REQ;
		p->pdu_req.sl_mp = 0x3;
		if (putmsg(fd, &ctrl, &data, 0) < 0)
			perror(__FUNCTION__);
	}
}

int
get_event(void)
{
	return wait_event(-1);
}

void
get_stats(void)
{
	sdl_stats_t sdl_stats;
	sdt_stats_t sdt_stats;
	sl_stats_t sl_stats;

	ctl.ic_cmd = SDL_IOCGSTATS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdl_stats);
	ctl.ic_dp = (char *) &sdl_stats;
	if (ioctl(fd, I_STR, &ctl) < 0)
		perror(__FUNCTION__);
	else {
		printf("\n");
		printf("rx_octets               = %d\n", (int) sdl_stats.rx_octets);
		printf("tx_octsts               = %d\n", (int) sdl_stats.tx_octets);
		printf("rx_overruns             = %d\n", (int) sdl_stats.rx_overruns);
		printf("tx_underruns            = %d\n", (int) sdl_stats.tx_underruns);
		printf("rx_buffer_overflows     = %d\n", (int) sdl_stats.rx_buffer_overflows);
		printf("tx_buffer_overflows     = %d\n", (int) sdl_stats.tx_buffer_overflows);
		printf("lead_cts_lost           = %d\n", (int) sdl_stats.lead_cts_lost);
		printf("lead_dcd_lost           = %d\n", (int) sdl_stats.lead_dcd_lost);
		printf("carrier_lost            = %d\n", (int) sdl_stats.carrier_lost);
	}
	ctl.ic_cmd = SDT_IOCGSTATS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdt_stats);
	ctl.ic_dp = (char *) &sdt_stats;
	if (ioctl(fd, I_STR, &ctl) < 0)
		perror(__FUNCTION__);
	else {
		printf("tx_bytes                = %d\n", (int) sdt_stats.tx_bytes);
		printf("tx_sus                  = %d\n", (int) sdt_stats.tx_sus);
		printf("tx_sus_repeated         = %d\n", (int) sdt_stats.tx_sus_repeated);
		printf("tx_underruns            = %d\n", (int) sdt_stats.tx_underruns);
		printf("tx_aborts               = %d\n", (int) sdt_stats.tx_aborts);
		printf("tx_buffer_overflows     = %d\n", (int) sdt_stats.tx_buffer_overflows);
		printf("tx_sus_in_error         = %d\n", (int) sdt_stats.tx_sus_in_error);
		printf("rx_bytes                = %d\n", (int) sdt_stats.rx_bytes);
		printf("rx_sus                  = %d\n", (int) sdt_stats.rx_sus);
		printf("rx_sus_compressed       = %d\n", (int) sdt_stats.rx_sus_compressed);
		printf("rx_overruns             = %d\n", (int) sdt_stats.rx_overruns);
		printf("rx_aborts               = %d\n", (int) sdt_stats.rx_aborts);
		printf("rx_buffer_overflows     = %d\n", (int) sdt_stats.rx_buffer_overflows);
		printf("rx_sus_in_error         = %d\n", (int) sdt_stats.rx_sus_in_error);
		printf("rx_sync_transitions     = %d\n", (int) sdt_stats.rx_sync_transitions);
		printf("rx_bits_octet_counted   = %d\n", (int) sdt_stats.rx_bits_octet_counted);
		printf("rx_crc_errors           = %d\n", (int) sdt_stats.rx_crc_errors);
		printf("rx_frame_errors         = %d\n", (int) sdt_stats.rx_frame_errors);
		printf("rx_frame_overflows      = %d\n", (int) sdt_stats.rx_frame_overflows);
		printf("rx_frame_too_long       = %d\n", (int) sdt_stats.rx_frame_too_long);
		printf("rx_frame_too_short      = %d\n", (int) sdt_stats.rx_frame_too_short);
		printf("rx_residue_errors       = %d\n", (int) sdt_stats.rx_residue_errors);
		printf("rx_length_error         = %d\n", (int) sdt_stats.rx_length_error);
		printf("carrier_cts_lost        = %d\n", (int) sdt_stats.carrier_cts_lost);
		printf("carrier_dcd_lost        = %d\n", (int) sdt_stats.carrier_dcd_lost);
		printf("carrier_lost            = %d\n", (int) sdt_stats.carrier_lost);
	}
	ctl.ic_cmd = SL_IOCGSTATS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sl_stats);
	ctl.ic_dp = (char *) &sl_stats;
	if (ioctl(fd, I_STR, &ctl) < 0)
		perror(__FUNCTION__);
	else {
		printf("sl_dur_in_service       = %d\n", (int) sl_stats.sl_dur_in_service);
		printf("sl_fail_align_or_proving= %d\n", (int) sl_stats.sl_fail_align_or_proving);
		printf("sl_nacks_received       = %d\n", (int) sl_stats.sl_nacks_received);
		printf("sl_dur_unavail          = %d\n", (int) sl_stats.sl_dur_unavail);
		printf("sl_dur_unavail_failed   = %d\n", (int) sl_stats.sl_dur_unavail_failed);
		printf("sl_dur_unavail_rpo      = %d\n", (int) sl_stats.sl_dur_unavail_rpo);
		printf("sl_sibs_sent            = %d\n", (int) sl_stats.sl_sibs_sent);
		printf("sl_tran_sio_sif_octets  = %d\n", (int) sl_stats.sl_tran_sio_sif_octets);
		printf("sl_retrans_octets       = %d\n", (int) sl_stats.sl_retrans_octets);
		printf("sl_tran_msus            = %d\n", (int) sl_stats.sl_tran_msus);
		printf("sl_recv_sio_sif_octets  = %d\n", (int) sl_stats.sl_recv_sio_sif_octets);
		printf("sl_recv_msus            = %d\n", (int) sl_stats.sl_recv_msus);
		printf("sl_cong_onset_ind[0]    = %d\n", (int) sl_stats.sl_cong_onset_ind[0]);
		printf("sl_cong_onset_ind[1]    = %d\n", (int) sl_stats.sl_cong_onset_ind[1]);
		printf("sl_cong_onset_ind[2]    = %d\n", (int) sl_stats.sl_cong_onset_ind[2]);
		printf("sl_cong_onset_ind[3]    = %d\n", (int) sl_stats.sl_cong_onset_ind[3]);
		printf("sl_dur_cong_status[0]   = %d\n", (int) sl_stats.sl_dur_cong_status[0]);
		printf("sl_dur_cong_status[1]   = %d\n", (int) sl_stats.sl_dur_cong_status[1]);
		printf("sl_dur_cong_status[2]   = %d\n", (int) sl_stats.sl_dur_cong_status[2]);
		printf("sl_dur_cong_status[3]   = %d\n", (int) sl_stats.sl_dur_cong_status[3]);
		printf("sl_cong_discd_ind[0]    = %d\n", (int) sl_stats.sl_cong_discd_ind[0]);
		printf("sl_cong_discd_ind[1]    = %d\n", (int) sl_stats.sl_cong_discd_ind[1]);
		printf("sl_cong_discd_ind[2]    = %d\n", (int) sl_stats.sl_cong_discd_ind[2]);
		printf("sl_cong_discd_ind[3]    = %d\n", (int) sl_stats.sl_cong_discd_ind[3]);
	}
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdl_conf_read);
	ctl.ic_dp = (char *) &sdl_conf_read;
	bzero(&sdl_conf_read, sizeof(sdl_conf_read));
	if (ioctl(fd, I_STR, &ctl) < 0)
		perror(__FUNCTION__);
	else {
		printf("ifflags                 = %s %s %s\n",
		       (sdl_conf_read.ifflags & SDL_IF_TX_RUNNING) ? "TX" : "tx",
		       (sdl_conf_read.ifflags & SDL_IF_RX_RUNNING) ? "RX" : "rx",
		       (sdl_conf_read.ifflags & SDL_IF_UP) ? "UP" : "up");
		printf("iftype                  = ");
		switch (sdl_conf_read.iftype) {
		case SDL_TYPE_NONE:
			printf("none");
			break;
		case SDL_TYPE_V35:
			printf("V35");
			break;
		case SDL_TYPE_DS0:
			printf("DS0");
			break;
		case SDL_TYPE_DS0A:
			printf("DS0A");
			break;
		case SDL_TYPE_E1:
			printf("E1");
			break;
		case SDL_TYPE_T1:
			printf("T1");
			break;
		case SDL_TYPE_ATM:
			printf("ATM");
			break;
		case SDL_TYPE_PACKET:
			printf("PACKET");
			break;
		default:
			printf("(UNKNOWN %d)", (int) sdl_conf_read.iftype);
			break;
		}
		printf("\n");
		printf("ifrate                  = %d\n", (int) sdl_conf_read.ifrate);
		printf("ifgtype			= ");
		switch (sdl_conf_read.ifgtype) {
		case SDL_GTYPE_NONE:
			printf("none");
			break;
		case SDL_GTYPE_T1:
			printf("T1");
			break;
		case SDL_GTYPE_E1:
			printf("E1");
			break;
		case SDL_GTYPE_J1:
			printf("J1");
			break;
		case SDL_GTYPE_ATM:
			printf("ATM");
			break;
		case SDL_GTYPE_ETH:
			printf("ETHERNET");
			break;
		case SDL_GTYPE_IP:
			printf("IP");
			break;
		case SDL_GTYPE_UDP:
			printf("UDP");
			break;
		case SDL_GTYPE_TCP:
			printf("TCP");
			break;
		case SDL_GTYPE_RTP:
			printf("RTP");
			break;
		case SDL_GTYPE_SCTP:
			printf("SCTP");
			break;
		default:
			printf("(UNKNOWN %d)", (int) sdl_conf_read.ifgtype);
			break;
		}
		printf("\n");
		printf("ifgrate			= %d\n", (int) sdl_conf_read.ifgrate);
		printf("ifmode                  = ");
		switch (sdl_conf_read.ifmode) {
		case SDL_MODE_NONE:
			printf("none");
			break;
		case SDL_MODE_DSU:
			printf("DSU");
			break;
		case SDL_MODE_CSU:
			printf("CSU");
			break;
		case SDL_MODE_DTE:
			printf("DTE");
			break;
		case SDL_MODE_DCE:
			printf("DCE");
			break;
		case SDL_MODE_CLIENT:
			printf("CLIENT");
			break;
		case SDL_MODE_SERVER:
			printf("SERVER");
			break;
		case SDL_MODE_PEER:
			printf("PEER");
			break;
		case SDL_MODE_ECHO:
			printf("ECHO");
			break;
		case SDL_MODE_REM_LB:
			printf("REM_LB");
			break;
		case SDL_MODE_LOC_LB:
			printf("LOC_LB");
			break;
		case SDL_MODE_LB_ECHO:
			printf("LB_ECHO");
			break;
		case SDL_MODE_TEST:
			printf("TEST");
			break;
		default:
			printf("(UNKNOWN %d)", (int) sdl_conf_read.ifmode);
			break;
		}
		printf("\n");
		printf("ifgmode                 = ");
		switch (sdl_conf_read.ifgmode) {
		default:
		case SDL_GMODE_NONE:
			printf("Normal");
			break;
		case SDL_GMODE_LOC_LB:
			printf("Local Loopback");
			break;
		case SDL_GMODE_REM_LB:
			printf("Remove Loopback");
			break;
			printf("(UNKNOWN %d)", (int) sdl_conf_read.ifgmode);
			break;
		}
		printf("\n");
		printf("ifgcrc                  = ");
		switch (sdl_conf_read.ifgcrc) {
		default:
		case SDL_GCRC_NONE:
			printf("NONE");
			break;
		case SDL_GCRC_CRC4:
			printf("CRC4");
			break;
		case SDL_GCRC_CRC5:
			printf("CRC5");
			break;
		case SDL_GCRC_CRC6:
			printf("CRC6");
			break;
			printf("(UNKNOWN %d)", (int) sdl_conf_read.ifgcrc);
			break;
		}
		printf("\n");
		printf("ifclock                 = ");
		switch (sdl_conf_read.ifclock) {
		default:
		case SDL_CLOCK_NONE:
			printf("none");
			break;
		case SDL_CLOCK_INT:
			printf("Internal");
			break;
		case SDL_CLOCK_EXT:
			printf("External");
			break;
		case SDL_CLOCK_LOOP:
			printf("Loop");
			break;
		case SDL_CLOCK_MASTER:
			printf("Master");
			break;
		case SDL_CLOCK_SLAVE:
			printf("Slave");
			break;
		case SDL_CLOCK_DPLL:
			printf("Digital PLL");
			break;
		case SDL_CLOCK_ABR:
			printf("Average Bit Rate");
			break;
		case SDL_CLOCK_SHAPER:
			printf("Traffic Shaper");
			break;
		case SDL_CLOCK_TICK:
			printf("Tick Clock");
			break;
			printf("(UNKNOWN %d)", (int) sdl_conf_read.ifclock);
			break;
		}
		printf("\n");
		printf("ifcoding                = ");
		switch (sdl_conf_read.ifcoding) {
		case SDL_CODING_NONE:
			printf("Normal");
			break;
		case SDL_CODING_NRZ:
			printf("NRZ");
			break;
		case SDL_CODING_NRZI:
			printf("NRZI");
			break;
		case SDL_CODING_AMI:
			printf("AMI");
			break;
		case SDL_CODING_B6ZS:
			printf("B6ZS");
			break;
		case SDL_CODING_B8ZS:
			printf("B8ZS");
			break;
#if 0
		case SDL_CODING_ESF:
			printf("ESF");
			break;
#endif
		case SDL_CODING_AAL1:
			printf("AAL1");
			break;
		case SDL_CODING_AAL2:
			printf("AAL2");
			break;
		case SDL_CODING_AAL5:
			printf("AAL5");
			break;
		case SDL_CODING_HDB3:
			printf("HDB3");
			break;
		default:
			printf("(UNKNOWN %d)", (int) sdl_conf_read.ifcoding);
			break;
		}
		printf("\n");
		printf("ifframing               = ");
		switch (sdl_conf_read.ifframing) {
		case SDL_FRAMING_NONE:
			printf("Default");
			break;
		case SDL_FRAMING_CCS:
			printf("CCS");
			break;
		case SDL_FRAMING_CAS:
			printf("CAS");
			break;
		case SDL_FRAMING_SF:
			printf("SF");
			break;
		case SDL_FRAMING_ESF:
			printf("ESF");
			break;
		default:
			printf("(UNKNOWN %d)", (int) sdl_conf_read.ifframing);
			break;
		}
		printf("\n");
		printf("ifleads                 = %s %s %s %s %s\n",
		       (sdl_conf_read.ifleads & SDL_LEAD_DTR) ? "DTR" : "dtr",
		       (sdl_conf_read.ifleads & SDL_LEAD_RTS) ? "RTS" : "rts",
		       (sdl_conf_read.ifleads & SDL_LEAD_DCD) ? "DCD" : "dcd",
		       (sdl_conf_read.ifleads & SDL_LEAD_CTS) ? "CTS" : "cts",
		       (sdl_conf_read.ifleads & SDL_LEAD_DSR) ? "DSR" : "dsr");
		printf("ifalarms                = %s %s %s %s\n",
		       (sdl_conf_read.ifalarms & SDL_ALARM_RED) ? "RED" : "red",
		       (sdl_conf_read.ifalarms & SDL_ALARM_BLU) ? "BLU" : "blu",
		       (sdl_conf_read.ifalarms & SDL_ALARM_YEL) ? "YEL" : "yel",
		       (sdl_conf_read.ifalarms & SDL_ALARM_REC) ? "REC" : "rec");
		printf("ifrxlevel               = %d\n", (int) sdl_conf_read.ifrxlevel);
		printf("iftxlevel               = %d\n", (int) sdl_conf_read.iftxlevel);
		printf("ifalarms                = 0x%08x\n", (int) sdl_conf_read.ifalarms);
		printf("ifsync                  = %d\n", (int) sdl_conf_read.ifsync);
		fflush(stdout);
		printf("\n");
	}
}

const char *
print_lmi_error(int err)
{
	switch (err) {
	case LMI_UNSPEC:
		return ("Unknown or unspecified");
	case LMI_BADADDRESS:
		return ("Address was invalid");
	case LMI_BADADDRTYPE:
		return ("Invalid address type");
	case LMI_BADDIAL:
		return ("(not used)");
	case LMI_BADDIALTYPE:
		return ("(not used)");
	case LMI_BADDISPOSAL:
		return ("Invalid disposal parameter");
	case LMI_BADFRAME:
		return ("Defective SDU received");
	case LMI_BADPPA:
		return ("Invalid PPA identifier");
	case LMI_BADPRIM:
		return ("Unregognized primitive");
	case LMI_DISC:
		return ("Disconnected");
	case LMI_EVENT:
		return ("Protocol-specific event ocurred");
	case LMI_FATALERR:
		return ("Device has become unusable");
	case LMI_INITFAILED:
		return ("Link initialization failed");
	case LMI_NOTSUPP:
		return ("Primitive not supported by this device");
	case LMI_OUTSTATE:
		return ("Primitive was issued from invalid state");
	case LMI_PROTOSHORT:
		return ("M_PROTO block too short");
	case LMI_SYSERR:
		return ("UNIX system error");
	case LMI_WRITEFAIL:
		return ("Unitdata request failed");
	case LMI_CRCERR:
		return ("CRC or FCS error");
	case LMI_DLE_EOT:
		return ("DLE EOT detected");
	case LMI_FORMAT:
		return ("Format error detected");
	case LMI_HDLC_ABORT:
		return ("Aborted frame detected");
	case LMI_OVERRUN:
		return ("Input overrun");
	case LMI_TOOSHORT:
		return ("Frame too short");
	case LMI_INCOMPLETE:
		return ("Partial frame received");
	case LMI_BUSY:
		return ("Telephone was busy");
	case LMI_NOANSWER:
		return ("Connection went unanswered");
	case LMI_CALLREJECT:
		return ("Connection rejected");
	case LMI_HDLC_IDLE:
		return ("HDLC line went idle");
	case LMI_HDLC_NOTIDLE:
		return ("HDLC link no longer idle");
	case LMI_QUIESCENT:
		return ("Line being reassigned");
	case LMI_RESUMED:
		return ("Line has been reassigned");
	case LMI_DSRTIMEOUT:
		return ("Did not see DSR in time");
	case LMI_LAN_COLLISIONS:
		return ("LAN excessive collisions");
	case LMI_LAN_REFUSED:
		return ("LAN message refused");
	case LMI_LAN_NOSTATION:
		return ("LAN no such station");
	case LMI_LOSTCTS:
		return ("Lost Clear to Send signal");
	case LMI_DEVERR:
		return ("Start of device-specific error codes");
	default:
		return ("(unknown)");
	}
}

void
show_time(void)
{
	time_t t;

	time(&t);
	printf("\n%s", ctime(&t));
}

int
mymain(int argc, char **argv)
{
	int ret, state = 0;

	ppa = ((board - 1) << 12) | ((span - 1) << 8) | (channel << 0);
	fprintf(stderr, "\nRunning %s on board %d, span %d, channel %d.\n\n", argv[0], board, span,
		channel);
	printf("Opening device.\n");
	fflush(stdout);
	if ((fd = open("/dev/x400p-sl", O_NONBLOCK | O_RDWR)) < 0) {
		perror(argv[0]);
		exit(1);
	}
	printf("Ioctl on device.\n");
	fflush(stdout);
	if (ioctl(fd, I_SRDOPT, RMSGD) < 0) {
		perror(argv[0]);
		exit(1);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(m->attach_req) + sizeof(ppa_t);
	ctrl.buf = cbuf;
	m->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	m->attach_req.lmi_ppa_length = sizeof(ppa_t);
	m->attach_req.lmi_ppa_offset = sizeof(m->attach_req);
	bcopy(&ppa, m->attach_req.lmi_ppa, sizeof(ppa_t));
	printf("Attaching device.\n");
	fflush(stdout);
	if (putmsg(fd, &ctrl, NULL, 0) < 0) {
		perror(argv[0]);
		exit(1);
	}
	switch ((ret = wait_event(100))) {
	case LMI_OK_ACK:
		break;
	case LMI_ERROR_ACK:
		fprintf(stderr, "%s: LMI_ERROR_ACK %s\n", argv[0], print_lmi_error(m->error_ack.lmi_reason));
		exit(1);
	default:
		fprintf(stderr, "%s: failed with event %d\n", argv[0], ret);
		exit(1);
	}
	printf("Configuring SL LMI.\n");
	fflush(stdout);
	ctl.ic_cmd = SL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lmi_conf);
	ctl.ic_dp = (char *) &lmi_conf;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	ctl.ic_cmd = SL_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lmi_conf_read);
	ctl.ic_dp = (char *) &lmi_conf_read;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	if (lmi_conf.pvar != lmi_conf_read.pvar || lmi_conf.popt != lmi_conf_read.popt) {
		fprintf(stderr, "%s: warning partial lmi configuration\n", argv[0]);
	}
	printf("Configuring SDT LMI.\n");
	fflush(stdout);
	ctl.ic_cmd = SDT_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lmi_conf);
	ctl.ic_dp = (char *) &lmi_conf;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	ctl.ic_cmd = SDT_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lmi_conf_read);
	ctl.ic_dp = (char *) &lmi_conf_read;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	if (lmi_conf.pvar != lmi_conf_read.pvar || lmi_conf.popt != lmi_conf_read.popt) {
		fprintf(stderr, "%s: warning partial lmi configuration\n", argv[0]);
	}
	printf("Configuring SL LMI.\n");
	fflush(stdout);
	ctl.ic_cmd = SL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lmi_conf);
	ctl.ic_dp = (char *) &lmi_conf;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	ctl.ic_cmd = SL_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(lmi_conf_read);
	ctl.ic_dp = (char *) &lmi_conf_read;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	if (lmi_conf.pvar != lmi_conf_read.pvar || lmi_conf.popt != lmi_conf_read.popt) {
		fprintf(stderr, "%s: warning partial lmi configuration\n", argv[0]);
	}
	printf("Configuring SDL.\n");
	fflush(stdout);
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdl_conf);
	ctl.ic_dp = (char *) &sdl_conf;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
#if 0
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdl_conf_read);
	ctl.ic_dp = (char *) &sdl_conf_read;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	if (sdl_conf != sdl_conf_read) {
		fprintf(stderr, "%s: warning partial sdl configuration\n", argv[0]);
	}
#endif
	printf("Configuring SDT.\n");
	fflush(stdout);
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdt_conf);
	ctl.ic_dp = (char *) &sdt_conf;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
#if 0
	ctl.ic_cmd = SDT_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdt_conf_read);
	ctl.ic_dp = (char *) &sdt_conf_read;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	if (sdt_conf != sdt_conf_read) {
		fprintf(stderr, "%s: warning partial sdt configuration\n", argv[0]);
	}
#endif
	printf("Configuring SL.\n");
	fflush(stdout);
	ctl.ic_cmd = SL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sl_conf);
	ctl.ic_dp = (char *) &sl_conf;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
#if 0
	ctl.ic_cmd = SL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sl_conf_read);
	ctl.ic_dp = (char *) &sl_conf_read;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(argv[0]);
		goto detach;
	}
	if (sl_conf != sl_conf_read) {
		fprintf(stderr, "%s: warning partial sl configuration\n", argv[0]);
	}
#endif
	printf("Enabling.\n");
	fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(m->enable_req);
	ctrl.buf = cbuf;
	m->enable_req.lmi_primitive = LMI_ENABLE_REQ;
	ctrl.len = sizeof(m->enable_req);
	if (putmsg(fd, &ctrl, NULL, 0) < 0) {
		perror(argv[0]);
		goto detach;
	}
	switch ((ret = wait_event(100))) {
	case LMI_ENABLE_CON:
		break;
	default:
		fprintf(stderr, "%s: failed with event %d\n", argv[0], ret);
		goto detach;
	}
	printf("Power on.\n");
	fflush(stdout);
	sleep(1);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->power_on_req);
	ctrl.buf = cbuf;
	p->power_on_req.sl_primitive = SL_POWER_ON_REQ;
	if (putmsg(fd, &ctrl, NULL, 0) < 0) {
		perror(argv[0]);
		goto getout;
	}
	switch ((ret = wait_event(100))) {
	case 0:
		break;
	default:
		fprintf(stderr, "%s: failed with event %d\n", argv[0], ret);
		goto getout;
	}
	printf("Emergency.\n");
	fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->emergency_req);
	ctrl.buf = cbuf;
	p->emergency_req.sl_primitive = SL_EMERGENCY_REQ;
	if (putmsg(fd, &ctrl, NULL, 0) < 0) {
		perror(argv[0]);
		goto getout;
	}
	switch ((ret = wait_event(100))) {
	case 0:
		break;
	default:
		fprintf(stderr, "%s: failed with event %d\n", argv[0], ret);
		goto getout;
	}
	state = 0;
	while (1) {
		switch (state) {
		case 0:
			show_time();
			printf("Start.\n");
			fflush(stdout);
			ctrl.maxlen = BUFSIZE;
			ctrl.len = sizeof(p->start_req);
			ctrl.buf = cbuf;
			p->start_req.sl_primitive = SL_START_REQ;
			if (putmsg(fd, &ctrl, NULL, 0) < 0) {
				perror(argv[0]);
				goto getout;
			}
			get_stats();
			state = 1;
			break;
		case 1:
			switch ((ret = get_event())) {
			case SL_IN_SERVICE_IND:
				show_time();
				printf("In service!\n");
				fflush(stdout);
				state = 2;
				break;
			case SL_OUT_OF_SERVICE_IND:
				show_time();
				printf("Out of service!\n");
				fflush(stdout);
				get_stats();
				start_timer(1000);
				state = 3;
				break;
			default:
				break;
			}
			break;
		case 2:
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				show_time();
				printf("Got pdu.\n");
				fflush(stdout);
				handle_message();
				break;
			case SL_OUT_OF_SERVICE_IND:
				show_time();
				printf("Out of service.\n");
				fflush(stdout);
				get_stats();
				start_timer(1000);
				state = 3;
				break;
			default:
				break;
			}
			break;
		case 3:
			switch ((ret = get_event())) {
			case TIMEOUT:
				state = 0;
				break;
			default:
				break;
			}
			break;
		}
	}
	exit(0);
      getout:
	get_stats();
	show_time();
	printf("Disabling.\n");
	fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(m->disable_req);
	ctrl.buf = cbuf;
	m->disable_req.lmi_primitive = LMI_DISABLE_REQ;
	if (putmsg(fd, &ctrl, NULL, 0) < 0) {
		perror(argv[0]);
		exit(1);
	}
      detach:
	show_time();
	printf("Detaching.\n");
	fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(m->detach_req);
	ctrl.buf = cbuf;
	m->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if (putmsg(fd, &ctrl, NULL, 0) < 0) {
		perror(argv[0]);
		exit(1);
	}
	show_time();
	printf("Done.\n");
	fflush(stdout);
	exit(1);
}

int
main(int argc, char **argv)
{
	int c, val;

	sl_conf = sl_conf_ansi;
	sdt_conf = sdt_conf_ansi;
	sdl_conf = sdl_conf_ansi;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"board",	required_argument,	NULL, 'b'},
			{"span",	required_argument,	NULL, 's'},
			{"channel",	required_argument,	NULL, 'c'},
			{"crc4",	no_argument,		NULL,  1 },
			{"crc5",	no_argument,		NULL,  2 },
			{"crc6",	no_argument,		NULL,  3 },
			{"hdb3",	no_argument,		NULL,  4 },
			{"ami",		no_argument,		NULL,  5 },
			{"b8zs",	no_argument,		NULL,  6 },
			{"cas",		no_argument,		NULL,  7 },
			{"ccs",		no_argument,		NULL,  8 },
			{"sf",		no_argument,		NULL,  9 },
			{"esf",		no_argument,		NULL, 10 },
			{"flags",	required_argument,	NULL, 11 },
			{"t1",		no_argument,		NULL, 12 },
			{"e1",		no_argument,		NULL, 13 },
			{"ds0",		no_argument,		NULL, 14 },
			{"ds0a",	no_argument,		NULL, 15 },
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "b:s:c:qv::hVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
		case 'b':
			board = atoi(optarg);
			break;
		case 's':
			span = atoi(optarg);
			break;
		case 'c':
			channel = atoi(optarg);
			break;
		case 1:	/* crc4 */
			sdl_conf.ifgcrc = SDL_GCRC_CRC4;
			break;
		case 2:	/* crc5 */
			sdl_conf.ifgcrc = SDL_GCRC_CRC5;
			break;
		case 3:	/* crc6 */
			sdl_conf.ifgcrc = SDL_GCRC_CRC6;
			break;
		case 4:	/* hdb3 */
			sdl_conf.ifcoding = SDL_CODING_HDB3;
			break;
		case 5:	/* ami */
			sdl_conf.ifcoding = SDL_CODING_AMI;
			break;
		case 6:	/* b8zs */
			sdl_conf.ifcoding = SDL_CODING_B8ZS;
			break;
		case 7:	/* cas */
			sdl_conf.ifframing = SDL_FRAMING_CAS;
			break;
		case 8:	/* ccs */
			sdl_conf.ifframing = SDL_FRAMING_CCS;
			break;
		case 9:	/* sf */
			sdl_conf.ifframing = SDL_FRAMING_SF;
			break;
		case 10:	/* esf */
			sdl_conf.ifframing = SDL_FRAMING_ESF;
			break;
		case 11:	/* flags */
			sdt_conf.f = atoi(optarg);
			break;
		case 14: /* ds0 */
			sdl_conf.iftype = SDL_TYPE_DS0;
			sdl_conf.ifrate = 64000;
			break;
		case 15: /* ds0a */
			sdl_conf.iftype = SDL_TYPE_DS0A;
			sdl_conf.ifrate = 56000;
			break;
		case 12:	/* t1 */
			sdl_conf.ifgrate = 1540000;
			sdl_conf.ifgcrc = SDL_GCRC_CRC6;
			sdl_conf.ifcoding = SDL_CODING_B8ZS;
			sdl_conf.ifframing = SDL_FRAMING_ESF;
			break;
		case 13:	/* e1 */
			sdl_conf.ifgrate = 2048000;
			sdl_conf.ifgcrc = SDL_GCRC_CRC5;
			sdl_conf.ifcoding = SDL_CODING_HDB3;
			sdl_conf.ifframing = SDL_FRAMING_CCS;
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
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
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
		      syntax_error:
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
	if (optind < argc)
		goto syntax_error;
	return mymain(argc, argv);
}
