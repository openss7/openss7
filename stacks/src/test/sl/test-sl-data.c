/*****************************************************************************

 @(#) $RCSfile: test-sl-data.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002 OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/09/06 10:57:48 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sl-data.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $"

static char const ident[] =
    "$RCSfile: test-sl-data.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $";

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

static int board = 1;
static int span = 1;
static int channel = 16;

lmi_option_t lmi_conf = {
	pvar:SS7_PVAR_ITUT_96,
	popt:0,
}, lmi_conf_read;

#ifndef HZ
#define HZ 100
#endif

sl_config_t sl_conf = {
	t1:45 * HZ,			/* jiffies */
	t2:5 * HZ,			/* jiffies */
	t2l:20 * HZ,			/* jiffies */
	t2h:100 * HZ,			/* jiffies */
	t3:1 * HZ,			/* jiffies */
	t4n:8 * HZ,			/* jiffies */
	t4e:500 * HZ / 1000,		/* jiffies */
	t5:100 * HZ / 1000,		/* jiffies */
	t6:4 * HZ,			/* jiffies */
	t7:1 * HZ,			/* jiffies */
	rb_abate:3,			/* messages */
	rb_accept:6,			/* messages */
	rb_discard:9,			/* messages */
	tb_abate_1:128 * 272,		/* octets */
	tb_onset_1:256 * 272,		/* octets */
	tb_discd_1:384 * 272,		/* octets */
	tb_abate_2:512 * 272,		/* octets */
	tb_onset_2:640 * 272,		/* octets */
	tb_discd_2:768 * 272,		/* octets */
	tb_abate_3:896 * 272,		/* octets */
	tb_onset_3:1024 * 272,		/* octets */
	tb_discd_3:1152 * 272,		/* octets */
	N1:127,				/* messages */
	N2:8192,			/* octets */
	M:5,
}, sl_conf_read;

sdt_config_t sdt_conf = {
	t8:100 * HZ / 1000,
	Tin:4,
	Tie:1,
	T:64,
	D:256,
	Te:577169,
	De:9308000,
	Ue:144292000,
	N:16,
	m:272,
	b:8,
	f:SDT_FLAGS_ONE,
#if 0
	f:SDT_FLAGS_SHARED,
	f:SDT_FLAGS_TWO,
	f:SDT_FLAGS_THREE,
#endif
}, sdt_conf_read;

sdl_config_t sdl_conf = {
	ifflags:0,
	iftype:SDL_TYPE_DS0,
	ifrate:64000,
	ifgtype:SDL_GTYPE_NONE,
	ifgrate:2048000,
	ifmode:SDL_MODE_PEER,
	ifgmode:SDL_GMODE_NONE,
	ifgcrc:SDL_GCRC_CRC5,		/* E1 only */
	ifclock:SDL_CLOCK_SLAVE,
	ifcoding:SDL_CODING_HDB3,	/* E1 only */
	ifframing:SDL_FRAMING_CCS,	/* E1 only */
	ifleads:0,
	ifalarms:0,
	ifrxlevel:0,
	iftxlevel:0,
	ifsync:0,
	ifsyncsrc:{1, 2, 0, 0}
}, sdl_conf_read;

static void usage(int argc, char **argv)
{
	fprintf(stderr, "\n\
Usage: %s [options]\n\
  -h, --help\n\
	prints this usage information and exits\n\
  -b, --board board				(default: %d)\n\
	board specifies the board number to open\n\
  -s, --span span				(default: %d)\n\
	span specifies the span on the card to open\n\
  -c, --channel channel				(default: %d)\n\
	channel specifies the time slot in the span to open\n\
  --flags number				(default: %lu)\n\
	number specifies the number of flags (0 means 1)\n\
  --crc4, --crc5, --crc6			(default: crc5)\n\
	specifies the crc to use\n\
  --hdb3, --ami, --b8zs				(default: hdb3)\n\
	specifies the line coding to use\n\
  --cas, --ccs, --sf, --esf			(default: ccs)\n\
	specifies the framing to use\n\
  --t1, --e1					(default: e1)\n\
	sets default settings for T1 or E1 operation\n\
\n", argv[0], board, span, channel, sdt_conf.f);
};

static void help(int argc, char **argv)
{
	usage(argc, argv);
	fprintf(stderr, "\
This program opens and attaches a Signalling Link channel, activates the\n\
links, and reads and writes SLTM/SLTA to and from the channel.  The program\n\
prints continuous information to stdout.\n\
\n");
};

int fd;
int mux;

typedef unsigned short ppa_t;
ppa_t ppa;

#ifndef HZ
#define HZ 100
#endif

#define BUFSIZE 300

static char cbuf[BUFSIZE];
static char dbuf[BUFSIZE];

struct strioctl ctl;
struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
struct strbuf data = { sizeof(*dbuf), 0, dbuf };
union LMI_primitives *m = (union LMI_primitives *) cbuf;
union SL_primitives *p = (union SL_primitives *) cbuf;

static int timer_timeout = 0;

static void timer_handler(int signum)
{
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

static int timer_sethandler(void)
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
static int start_timer(long duration)
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

static int stop_timer(void)
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

static int decode_msg(void)
{
	if (ctrl.len >= sizeof(ulong))
		return *(ulong *) cbuf;
	return -1;
}
static int decode_data(void)
{
	if (data.len > 5)
		return SL_PDU_IND;
	return -1;
}

static int wait_event(int wait)
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

static void handle_message(void)
{
	/* turn around SLTM into SLTA only */
	unsigned char *q = dbuf;
	uint sio, dpc, opc, sls, h0, h1, b;
	int i;
	sio = *q++;
	printf("Received:   ");
	for (i = 0; i < data.len; i++)
		printf("%02x ", dbuf[i]);
	printf("\n");
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
		b = *q++ & 0x00ff;
		h0 = *q & 0x0f;
		h1 = *q++ >> 4;
		if (h0 == 1 && h1 == 1) {
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
				printf("%02x ", dbuf[i]);
			printf("\n");
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

static int get_event(void)
{
	return wait_event(-1);
}

static void get_stats(void)
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
		printf("rx_octets               = %lu\n", sdl_stats.rx_octets);
		printf("tx_octsts               = %lu\n", sdl_stats.tx_octets);
		printf("rx_overruns             = %lu\n", sdl_stats.rx_overruns);
		printf("tx_underruns            = %lu\n", sdl_stats.tx_underruns);
		printf("rx_buffer_overflows     = %lu\n", sdl_stats.rx_buffer_overflows);
		printf("tx_buffer_overflows     = %lu\n", sdl_stats.tx_buffer_overflows);
		printf("lead_cts_lost           = %lu\n", sdl_stats.lead_cts_lost);
		printf("lead_dcd_lost           = %lu\n", sdl_stats.lead_dcd_lost);
		printf("carrier_lost            = %lu\n", sdl_stats.carrier_lost);
	}
	ctl.ic_cmd = SDT_IOCGSTATS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sdt_stats);
	ctl.ic_dp = (char *) &sdt_stats;
	if (ioctl(fd, I_STR, &ctl) < 0)
		perror(__FUNCTION__);
	else {
		printf("tx_bytes                = %lu\n", sdt_stats.tx_bytes);
		printf("tx_sus                  = %lu\n", sdt_stats.tx_sus);
		printf("tx_sus_repeated         = %lu\n", sdt_stats.tx_sus_repeated);
		printf("tx_underruns            = %lu\n", sdt_stats.tx_underruns);
		printf("tx_aborts               = %lu\n", sdt_stats.tx_aborts);
		printf("tx_buffer_overflows     = %lu\n", sdt_stats.tx_buffer_overflows);
		printf("tx_sus_in_error         = %lu\n", sdt_stats.tx_sus_in_error);
		printf("rx_bytes                = %lu\n", sdt_stats.rx_bytes);
		printf("rx_sus                  = %lu\n", sdt_stats.rx_sus);
		printf("rx_sus_compressed       = %lu\n", sdt_stats.rx_sus_compressed);
		printf("rx_overruns             = %lu\n", sdt_stats.rx_overruns);
		printf("rx_aborts               = %lu\n", sdt_stats.rx_aborts);
		printf("rx_buffer_overflows     = %lu\n", sdt_stats.rx_buffer_overflows);
		printf("rx_sus_in_error         = %lu\n", sdt_stats.rx_sus_in_error);
		printf("rx_sync_transitions     = %lu\n", sdt_stats.rx_sync_transitions);
		printf("rx_bits_octet_counted   = %lu\n", sdt_stats.rx_bits_octet_counted);
		printf("rx_crc_errors           = %lu\n", sdt_stats.rx_crc_errors);
		printf("rx_frame_errors         = %lu\n", sdt_stats.rx_frame_errors);
		printf("rx_frame_overflows      = %lu\n", sdt_stats.rx_frame_overflows);
		printf("rx_frame_too_long       = %lu\n", sdt_stats.rx_frame_too_long);
		printf("rx_frame_too_short      = %lu\n", sdt_stats.rx_frame_too_short);
		printf("rx_residue_errors       = %lu\n", sdt_stats.rx_residue_errors);
		printf("rx_length_error         = %lu\n", sdt_stats.rx_length_error);
		printf("carrier_cts_lost        = %lu\n", sdt_stats.carrier_cts_lost);
		printf("carrier_dcd_lost        = %lu\n", sdt_stats.carrier_dcd_lost);
		printf("carrier_lost            = %lu\n", sdt_stats.carrier_lost);
	}
	ctl.ic_cmd = SL_IOCGSTATS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sl_stats);
	ctl.ic_dp = (char *) &sl_stats;
	if (ioctl(fd, I_STR, &ctl) < 0)
		perror(__FUNCTION__);
	else {
		printf("sl_dur_in_service       = %lu\n", sl_stats.sl_dur_in_service);
		printf("sl_fail_align_or_proving= %lu\n", sl_stats.sl_fail_align_or_proving);
		printf("sl_nacks_received       = %lu\n", sl_stats.sl_nacks_received);
		printf("sl_dur_unavail          = %lu\n", sl_stats.sl_dur_unavail);
		printf("sl_dur_unavail_failed   = %lu\n", sl_stats.sl_dur_unavail_failed);
		printf("sl_dur_unavail_rpo      = %lu\n", sl_stats.sl_dur_unavail_rpo);
		printf("sl_sibs_sent            = %lu\n", sl_stats.sl_sibs_sent);
		printf("sl_tran_sio_sif_octets  = %lu\n", sl_stats.sl_tran_sio_sif_octets);
		printf("sl_retrans_octets       = %lu\n", sl_stats.sl_retrans_octets);
		printf("sl_tran_msus            = %lu\n", sl_stats.sl_tran_msus);
		printf("sl_recv_sio_sif_octets  = %lu\n", sl_stats.sl_recv_sio_sif_octets);
		printf("sl_recv_msus            = %lu\n", sl_stats.sl_recv_msus);
		printf("sl_cong_onset_ind[0]    = %lu\n", sl_stats.sl_cong_onset_ind[0]);
		printf("sl_cong_onset_ind[1]    = %lu\n", sl_stats.sl_cong_onset_ind[1]);
		printf("sl_cong_onset_ind[2]    = %lu\n", sl_stats.sl_cong_onset_ind[2]);
		printf("sl_cong_onset_ind[3]    = %lu\n", sl_stats.sl_cong_onset_ind[3]);
		printf("sl_dur_cong_status[0]   = %lu\n", sl_stats.sl_dur_cong_status[0]);
		printf("sl_dur_cong_status[1]   = %lu\n", sl_stats.sl_dur_cong_status[1]);
		printf("sl_dur_cong_status[2]   = %lu\n", sl_stats.sl_dur_cong_status[2]);
		printf("sl_dur_cong_status[3]   = %lu\n", sl_stats.sl_dur_cong_status[3]);
		printf("sl_cong_discd_ind[0]    = %lu\n", sl_stats.sl_cong_discd_ind[0]);
		printf("sl_cong_discd_ind[1]    = %lu\n", sl_stats.sl_cong_discd_ind[1]);
		printf("sl_cong_discd_ind[2]    = %lu\n", sl_stats.sl_cong_discd_ind[2]);
		printf("sl_cong_discd_ind[3]    = %lu\n", sl_stats.sl_cong_discd_ind[3]);
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
			printf("(UNKNOWN %lu)", sdl_conf_read.iftype);
			break;
		}
		printf("\n");
		printf("ifrate                  = %lu\n", sdl_conf_read.ifrate);
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
			printf("(UNKNOWN %lu)", sdl_conf_read.ifgtype);
			break;
		}
		printf("\n");
		printf("ifgrate			= %lu\n", sdl_conf_read.ifgrate);
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
			printf("(UNKNOWN %lu)", sdl_conf_read.ifmode);
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
			printf("(UNKNOWN %lu)", sdl_conf_read.ifgmode);
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
			printf("(UNKNOWN %lu)", sdl_conf_read.ifgcrc);
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
			printf("(UNKNOWN %lu)", sdl_conf_read.ifclock);
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
		case SDL_CODING_ESF:
			printf("ESF");
			break;
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
			printf("(UNKNOWN %lu)", sdl_conf_read.ifcoding);
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
			printf("(UNKNOWN %lu)", sdl_conf_read.ifframing);
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
		printf("ifrxlevel               = %lu\n", sdl_conf_read.ifrxlevel);
		printf("iftxlevel               = %lu\n", sdl_conf_read.iftxlevel);
		printf("ifalarms                = 0x%08lx\n", sdl_conf_read.ifalarms);
		printf("ifsync                  = %lu\n", sdl_conf_read.ifsync);
		fflush(stdout);
		printf("\n");
	}
}

static void show_time(void)
{
	time_t t;
	time(&t);
	printf("\n%s", ctime(&t));
}

static int mymain(int argc, char **argv)
{
	int ret, state = 0;
	unsigned char msg[] = { 0x01, 0xae, 0x11, 0x76, 0x04, 0x11, 0x70, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97 };
	ppa = ((board - 1) << 12) | ((span - 1) << 8) | (channel << 0);
	fprintf(stderr, "\nRunning %s on board %d, span %d, channel %d.\n\n", argv[0], board, span,
		channel);
	printf("Opening mux.\n");
	fflush(stdout);
	if ((mux = open("/dev/sl-mux", O_NONBLOCK | O_RDWR)) < 0) {
		perror(argv[0]);
		exit(1);
	}
	fd = mux;
	printf("Ioctl on mux.\n");
	fflush(stdout);
	if (ioctl(mux, I_SRDOPT, RMSGD) < 0) {
		perror(argv[0]);
		exit(1);
	}
	printf("Start.\n");
	fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->start_req);
	ctrl.buf = cbuf;
	p->start_req.sl_primitive = SL_START_REQ;
	if (putmsg(fd, &ctrl, NULL, 0) < 0) {
		perror(argv[0]);
		exit(1);
	}
	printf("Writing data\n");
	write(mux, msg, sizeof(msg));
	write(mux, msg, sizeof(msg));
	write(mux, msg, sizeof(msg));
	get_stats();
	close(mux);
	exit(0);
}

int main(int argc, char **argv)
{
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"board", 1, 0, 'b'},
			{"span", 1, 0, 's'},
			{"channel", 1, 0, 'c'},
			{"crc4", 0, 0, 1},
			{"crc5", 0, 0, 2},
			{"crc6", 0, 0, 3},
			{"hdb3", 0, 0, 4},
			{"ami", 0, 0, 5},
			{"b8zs", 0, 0, 6},
			{"cas", 0, 0, 7},
			{"ccs", 0, 0, 8},
			{"sf", 0, 0, 9},
			{"esf", 0, 0, 10},
			{"flags", 1, 0, 11},
			{"t1", 0, 0, 12},
			{"e1", 0, 0, 13}
		};
		c = getopt_long(argc, argv, "hb:s:c:", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:	/* -h */
				help(argc, argv);
				exit(0);
			case 1:	/* -b board */
				board = atoi(optarg);
				break;
			case 2:	/* -s span */
				span = atoi(optarg);
				break;
			case 3:	/* -c channel */
				channel = atoi(optarg);
				break;
			default:
				usage(argc, argv);
				exit(1);
			}
		case 'h':
			help(argc, argv);
			exit(0);
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
		case 12:	/* t1 */
			sdl_conf.ifgrate = 1540000;
			sdl_conf.ifgcrc = SDL_GCRC_CRC6;
			sdl_conf.ifcoding = SDL_CODING_B8ZS;
			sdl_conf.ifframing = SDL_FRAMING_SF;
			break;
		case 13:	/* e1 */
			sdl_conf.ifgrate = 2048000;
			sdl_conf.ifgcrc = SDL_GCRC_CRC5;
			sdl_conf.ifcoding = SDL_CODING_HDB3;
			sdl_conf.ifframing = SDL_FRAMING_CCS;
			break;
		default:
			usage(argc, argv);
			exit(1);
		}
	}
	if (optind < argc) {
		fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
		for (; optind < argc; optind++)
			fprintf(stderr, "%s ", argv[optind]);
		fprintf(stderr, "\n");
		usage(argc, argv);
		exit(1);
	}
	return mymain(argc, argv);
}
