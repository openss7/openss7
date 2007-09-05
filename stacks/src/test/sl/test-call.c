/*****************************************************************************

 @(#) $RCSfile: test-call.c,v $ $Name:  $($Revision: 0.8.2.9 $) $Date: 2002/11/06 14:50:33 $

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

 Last Modified $Date: 2002/11/06 14:50:33 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $RCSfile: test-call.c,v $ $Name:  $($Revision: 0.8.2.9 $) $Date: 2002/11/06 14:50:33 $"

static char const ident[] =
    "$RCSfile: test-call.c,v $ $Name:  $($Revision: 0.8.2.9 $) $Date: 2002/11/06 14:50:33 $";

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
#include <ss7/sli.h>

static int state = 0;
static unsigned char called[64] = "491755915022";
static unsigned char calling[64] = "6677111222";
static uint my_cic = 1;
static uint my_dpc = 0x11ae;
static uint my_opc = 0x11d8;

static void usage(int argc, char **argv)
{
	fprintf(stderr, "\n\
Usage: %s [options]\n\
  -h, --help\n\
	prints this usage information and exits\n\
  -d, --called number   (max 64 digits)         (default: %s)\n\
	number specifies the called party number\n\
  -g, --calling number  (max 64 digits)         (default: %s)\n\
	number specifies the calling party number\n\
  -c, --cic cic                                 (default: %d)\n\
	cic specified the circuit identification code to use\n\
  -n, --nocall\n\
	don't generate a call\n\
\n", argv[0], called, calling, my_cic);
};

static void help(int argc, char **argv)
{
	usage(argc, argv);
	fprintf(stderr, "\
This program opens the MTP mini-mux and initiates a telephone call to the\n\
designated cic using the called and calling party numbers supplied.\n\
\n");
};

int fd;

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
static inline int start_timer(long duration)
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
	if (data.len > 0)
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

static int get_event(void)
{
	return wait_event(-1);
}

static void show_time(void)
{
	time_t t;
	time(&t);
	printf("\n%s", ctime(&t));
}

#define ISUP_MT_IAM	1	/* 0x01 - 0b00000001 - Initial address */
#define ISUP_MT_SAM	2	/* 0x02 - 0b00000010 - Subsequent address */
#define ISUP_MT_INR	3	/* 0x03 - 0b00000011 - Information request */
#define ISUP_MT_INF	4	/* 0x04 - 0b00000100 - Information */
#define ISUP_MT_COT	5	/* 0x05 - 0b00000101 - Continuity */
#define ISUP_MT_ACM	6	/* 0x06 - 0b00000110 - Address complete */
#define ISUP_MT_CON	7	/* 0x07 - 0b00000111 - Connect (not old ANSI) */
#define ISUP_MT_FOT	8	/* 0x08 - 0b00001000 - Forward transfer */
#define ISUP_MT_ANM	9	/* 0x09 - 0b00001001 - Answer */
#define ISUP_MT_REL	12	/* 0x0c - 0b00001100 - Release */
#define ISUP_MT_SUS	13	/* 0x0d - 0b00001101 - Suspend */
#define ISUP_MT_RES	14	/* 0x0e - 0b00001110 - Resume */
#define ISUP_MT_RLC	16	/* 0x10 - 0b00010000 - Release complete */
#define ISUP_MT_CCR	17	/* 0x11 - 0b00010001 - Continuity check request */
#define ISUP_MT_RSC	18	/* 0x12 - 0b00010010 - Reset circuit */
#define ISUP_MT_BLO	19	/* 0x13 - 0b00010011 - Blocking */
#define ISUP_MT_UBL	20	/* 0x14 - 0b00010100 - Unblcoking */
#define ISUP_MT_BLA	21	/* 0x15 - 0b00010101 - Blocking acknowledgement */
#define ISUP_MT_UBA	22	/* 0x16 - 0b00010110 - Unblocking acknowledgement */
#define ISUP_MT_GRS	23	/* 0x17 - 0b00010111 - Circuit group reset */
#define ISUP_MT_CGB	24	/* 0x18 - 0b00011000 - Circuit group blocking */
#define ISUP_MT_CGU	25	/* 0x19 - 0b00011001 - Circuit group unblocking */
#define ISUP_MT_CGBA	26	/* 0x1a - 0b00011010 - Circuit group blocking acknowledgement */
#define ISUP_MT_CGUA	27	/* 0x1b - 0b00011011 - Circuit group unblocking acknowledgement */
#define ISUP_MT_CMR	28	/* 0x1c - 0b00011100 - Call Modification Request (not old ANSI) */
#define ISUP_MT_CMC	29	/* 0x1d - 0b00011101 - Call Modification Completed (not old ANSI) */
#define ISUP_MT_CMRJ	30	/* 0x1e - 0b00011110 - Call Modification Reject (not old ANSI) */
#define ISUP_MT_FAR	31	/* 0x1f - 0b00011111 - Facility request */
#define ISUP_MT_FAA	32	/* 0x20 - 0b00100000 - Facility accepted */
#define ISUP_MT_FRJ	33	/* 0x21 - 0b00100001 - Facility reject */
#define ISUP_MT_FAD	34	/* 0x22 - 0b00100010 - Facility Deactivated (old Bellcore only) */
#define ISUP_MT_FAI	35	/* 0x23 - 0b00100011 - Facility Information (old Bellcore only) */
#define ISUP_MT_LPA	36	/* 0x24 - 0b00100100 - Loop back acknowledgement */
#define ISUP_MT_DRS	39	/* 0x27 - 0b00100111 - Delayed release (not old ANSI) */
#define ISUP_MT_PAM	40	/* 0x28 - 0b00101000 - Pass along */
#define ISUP_MT_GRA	41	/* 0x29 - 0b00101001 - Circuit group reset acknowledgement */
#define ISUP_MT_CQM	42	/* 0x2a - 0b00101010 - Circuit group query */
#define ISUP_MT_CQR	43	/* 0x2b - 0b00101011 - Circuit group query response */
#define ISUP_MT_CPG	44	/* 0x2c - 0b00101100 - Call progress */
#define ISUP_MT_USR	45	/* 0x2d - 0b00101101 - User-to-user information */
#define ISUP_MT_UCIC	46	/* 0x2e - 0b00101110 - Unequipped circuit identification code */
#define ISUP_MT_CFN	47	/* 0x2f - 0b00101111 - Confusion */
#define ISUP_MT_OLM	48	/* 0x30 - 0b00110000 - Overload */
#define ISUP_MT_CRG	49	/* 0x31 - 0b00110001 - Charge information */
#define ISUP_MT_NRM	50	/* 0x32 - 0b00110010 - Network resource management */
#define ISUP_MT_FAC	51	/* 0x33 - 0b00110011 - Facility */
#define ISUP_MT_UPT	52	/* 0x34 - 0b00110100 - User part test */
#define ISUP_MT_UPA	53	/* 0x35 - 0b00110101 - User part available */
#define ISUP_MT_IDR	54	/* 0x36 - 0b00110110 - Identification request */
#define ISUP_MT_IRS	55	/* 0x37 - 0b00110111 - Identification response */
#define ISUP_MT_SGM	56	/* 0x38 - 0b00111000 - Segmentation */

static void send_msg(void)
{
	/* data portion already ready */
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->pdu_req);
	ctrl.buf = cbuf;
	p->pdu_req.sl_primitive = SL_PDU_REQ;
	p->pdu_req.sl_mp = 0x0;
	if (putmsg(fd, &ctrl, &data, 0) < 0)
		perror(__FUNCTION__);
	return;
}

static void show_msg(const char *label)
{
	int i;
	printf("%s", label);
	for (i = 0; i < data.len; i++)
		printf("%02x ", data.buf[i] & 0x0ff);
	printf("\n");
	fflush(stdout);
}

static void send_iam(void)
{
	unsigned char *h = data.buf;
	uint dsig_len = strlen(called) + 1;
	uint gsig_len = strlen(calling);
	int i;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_IAM;
	/* F nci */
	*h++ = 0x00;
	/* F fci */
	*h++ = 0x21;
	*h++ = 0x01;
	/* F cpc */
	*h++ = 0x0a;		/* 0x0a ordinary calling subscriber; 0x0d test call */
	/* F tmr */
	*h++ = 0x00;		/* speech */
	/* V cdpn */
	*h++ = 0x02;		/* pointer to cdpn */
	*h++ = 4 + ((dsig_len + 1) >> 1);	/* pointer to optional parms */
	*h++ = 2 + ((dsig_len + 1) >> 1);	/* CDPN length */
	*h++ = 0x04 | ((dsig_len & 0x1) ? 0x80 : 0);	/* 0x01 subscriber number 0x03 national
							   0x04 international */
	*h++ = 0x10;		/* inn allowed, E.164 */
	for (i = 0; i < dsig_len; i += 2)
		*h++ = (called[i] & 0x0f) | ((called[i + 1] & 0x0f) << 4);
	if (dsig_len & 0x1)
		h[-1] |= 0xff;	/* add ST and filler */
	else
		h[-1] |= 0xf0;	/* just add ST */
	/* O cgpn */
	*h++ = 0x0a;		/* ISUP_PT_CGPN */
	*h++ = 2 + ((gsig_len + 1) >> 1);	/* CGPN length */
	*h++ = 0x04 | ((gsig_len & 0x1) ? 0x80 : 0);	/* subscriber number 0x03 national 0x04
							   international */
	*h++ = 0x13;		/* E.164, presentation allowed, user prov verified */
	for (i = 0; i < gsig_len; i += 2)
		*h++ = (calling[i] & 0x0f) | ((calling[i + 1] & 0x0f) << 4);
	if (gsig_len & 0x1)
		h[-1] |= 0xf0;
	/* end of optional */
	*h++ = 0x00;
	data.len = ((char *) h - data.buf);
	show_msg("-> IAM: ");
	send_msg();
	return;
}

static void send_rel(void)
{
	unsigned char *h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_REL;
	*h++ = 2;		/* offset */
	*h++ = 0x00;		/* end of optional */
	*h++ = 2;		/* length */
	*h++ = 0x80;		/* Q.763 = User */
	*h++ = 0x90;		/* Normal call clearing */
	data.len = ((char *) h - data.buf);
	show_msg("-> REL: ");
	send_msg();
	return;
}

static void send_rsc(void)
{
	unsigned char *h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_RSC;
	data.len = ((char *) h - data.buf);
	show_msg("-> RSC: ");
	send_msg();
	return;
}

static void send_anm(void)
{
	unsigned char *h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_ANM;
	*h++ = 0x00;		/* end of optional */
	data.len = ((char *) h - data.buf);
	show_msg("-> ANM: ");
	send_msg();
	return;
}

static void handle_message(void)
{
	unsigned char *h, *d;
	uint si, dpc, opc, sls, cic, mt, b;
	d = data.buf;
	si = *d++;
	if ((si & 0x0f) != 0x05) {
		printf("SI is %02x\n", si & 0x0f);
		return;
	}
	b = *d++ & 0x00ff;
	dpc = ((b << 0) & 0xff);
	b = *d++ & 0x00ff;
	dpc |= ((b << 8) & 0x3f00);
	opc = ((b >> 6) & 0x03);
	b = *d++ & 0x00ff;
	opc |= ((b << 2) & 0x3fc);
	b = *d++ & 0x00ff;
	opc |= ((b << 10) & 0x3c00);
	sls = (b >> 4) & 0x0f;
	b = *d++ & 0x00ff;
	cic = ((b << 0) & 0xff);
	b = *d++ & 0x00ff;
	cic |= ((b << 8) & 0x0f00);
	mt = (*d++ & 0x0ff);
	if (cic != my_cic)
		printf("CIC is %04x\n", cic & 0x0fff);
	printf("dpc = %04x, opc = %04x, sls = %02x, cic = %04x\n", dpc, opc, sls, cic);
	switch (mt) {
	case ISUP_MT_ACM:	/* 0x06 - 0b00000110 - Address complete */
		show_msg("<- ACM: ");
		if (state == 1) {	/* wait for ACN/CON */
			start_timer(120000);	/* timer t9 2 minutes */
			state = 2;
			break;
		}
		goto unexpected;
	case ISUP_MT_USR:	/* 0x2d - 0b00101101 - User-to-user information */
		show_msg("<- USR: ");
		goto unexpected;
	case ISUP_MT_CPG:	/* 0x2c - 0b00101100 - Call progress */
		show_msg("<- CPG: ");
		if (state == 2) {	/* wait for ANM */
			start_timer(120000);	/* timer t9 2 minutes */
			state = 2;
		}
		goto unexpected;
	case ISUP_MT_CON:	/* 0x07 - 0b00000111 - Connect (not old ANSI) */
		show_msg("<- CON: ");
		if (state == 1) {	/* wait for ACN/CON */
			start_timer(15000);
			state = 5;	/* connected */
			break;
		}
		goto unexpected;
	case ISUP_MT_ANM:	/* 0x09 - 0b00001001 - Answer */
		show_msg("<- ANM: ");
		if (state == 2) {	/* wait for ANM */
			start_timer(15000);
			state = 5;	/* connected */
			break;
		}
		goto unexpected;
	case ISUP_MT_REL:	/* 0x0c - 0b00001100 - Release */
		show_msg("<- REL: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_RLC;
		*h++ = 0x00;
		data.len = ((char *) h - data.buf);
		show_msg("-> RLC: ");
		send_msg();
		start_timer(100);
		state = 6;
		return;
	case ISUP_MT_BLO:	/* 0x13 - 0b00010011 - Blocking */
		show_msg("<- BLO: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_BLA;
		show_msg("-> BLA: ");
		send_msg();
		if (state == 0 || state == 1) {
			start_timer(100);
			state = 6;
		}
		return;
	case ISUP_MT_UBL:	/* 0x14 - 0b00010100 - Unblcoking */
		show_msg("<- UBL: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_UBA;
		show_msg("-> UBA: ");
		send_msg();
		return;
	case ISUP_MT_UPT:	/* 0x34 - 0b00110100 - User part test */
		show_msg("<- UPT: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_UPA;
		show_msg("-> UPA: ");
		send_msg();
		break;
		/* ignore */
	case ISUP_MT_RLC:	/* 0x10 - 0b00010000 - Release complete */
		show_msg("<- RLC: ");
		start_timer(100);
		state = 6;
		return;
	case ISUP_MT_RSC:	/* 0x12 - 0b00010010 - Reset circuit */
		show_msg("<- RSC: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_RLC;
		*h++ = 0x00;
		data.len = ((char *) h - data.buf);
		show_msg("-> RLC: ");
		send_msg();
		start_timer(100);
		state = 6;
		return;
	case ISUP_MT_UCIC:	/* 0x2e - 0b00101110 - Unequipped circuit identification code */
		show_msg("<- UCIC: ");
		start_timer(100);
		state = 6;
		return;
	case ISUP_MT_UPA:	/* 0x35 - 0b00110101 - User part available */
		show_msg("<- UPA: ");
		return;
	case ISUP_MT_IAM:	/* 0x01 - 0b00000001 - Initial address */
		show_msg("<- IAM: ");
		my_cic = cic;
		if (((*d >> 2) & 0x03) != 0x01) {
			/* rewrite the header swap the point codes */
			h = data.buf;
			*h++ = si;
			*h++ = (opc & 0x0ff);
			*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
			*h++ = ((dpc >> 2) & 0x0ff);
			*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
			*h++ = (cic & 0x0ff);
			*h++ = ((cic >> 8) & 0x0f);
			*h++ = ISUP_MT_ACM;
			*h++ = 0x00;	/* no indication */
			*h++ = 0x00;	/* no indication */
			*h++ = 0x00;
			data.len = ((char *) h - data.buf);
			show_msg("-> ACM: ");
			send_msg();
			state = 8;
			start_timer(3000);
			return;
		}
		state = 7;
		start_timer(10000);	/* t8 = 10 seconds */
		break;
	case ISUP_MT_SAM:	/* 0x02 - 0b00000010 - Subsequent address */
		show_msg("<- SAM: ");
		if (state == 7) {
			start_timer(10000);	/* t8 = 10 seconds */
			return;
		}
		goto unexpected;
	case ISUP_MT_INR:	/* 0x03 - 0b00000011 - Information request */
		show_msg("<- INR: ");
		goto unexpected;
	case ISUP_MT_INF:	/* 0x04 - 0b00000100 - Information */
		show_msg("<- INF: ");
		goto unexpected;
	case ISUP_MT_COT:	/* 0x05 - 0b00000101 - Continuity */
		show_msg("<- COT: ");
		if (*d & 0x1 && state == 7) {	/* success */
			/* rewrite the header swap the point codes */
			h = data.buf;
			*h++ = si;
			*h++ = (opc & 0x0ff);
			*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
			*h++ = ((dpc >> 2) & 0x0ff);
			*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
			*h++ = (cic & 0x0ff);
			*h++ = ((cic >> 8) & 0x0f);
			*h++ = ISUP_MT_ACM;
			*h++ = 0x00;	/* no indication */
			*h++ = 0x00;	/* no indication */
			*h++ = 0x00;
			data.len = ((char *) h - data.buf);
			show_msg("-> ACM: ");
			send_msg();
			state = 8;
			start_timer(3000);
			return;
		}
		start_timer(100);
		state = 6;
		return;
	case ISUP_MT_FOT:	/* 0x08 - 0b00001000 - Forward transfer */
		show_msg("<- FOT: ");
		goto unexpected;
	case ISUP_MT_SUS:	/* 0x0d - 0b00001101 - Suspend */
		show_msg("<- SUS: ");
		if (state == 5) {
			start_timer(15000);
			return;
		}
		goto unexpected;
	case ISUP_MT_RES:	/* 0x0e - 0b00001110 - Resume */
		show_msg("<- RES: ");
		if (state == 5) {
			start_timer(15000);
			return;
		}
		goto unexpected;
	case ISUP_MT_CCR:	/* 0x11 - 0b00010001 - Continuity check request */
		show_msg("<- CCR: ");
		if (0) {
			/* rewrite the header swap the point codes */
			h = data.buf;
			*h++ = si;
			*h++ = (opc & 0x0ff);
			*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
			*h++ = ((dpc >> 2) & 0x0ff);
			*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
			*h++ = (cic & 0x0ff);
			*h++ = ((cic >> 8) & 0x0f);
			*h++ = ISUP_MT_LPA;
			data.len = ((char *) h - data.buf);
			show_msg("-> LPA: ");
			send_msg();
		}
		start_timer(100);
		state = 6;
		break;
	case ISUP_MT_BLA:	/* 0x15 - 0b00010101 - Blocking acknowledgement */
		show_msg("<- BLA: ");
		return;
	case ISUP_MT_UBA:	/* 0x16 - 0b00010110 - Unblocking acknowledgement */
		show_msg("<- UBA: ");
		return;
	case ISUP_MT_CMR:	/* 0x1c - 0b00011100 - Call Modification Request (not old ANSI) */
		show_msg("<- CMR: ");
		goto unexpected;
	case ISUP_MT_CMC:	/* 0x1d - 0b00011101 - Call Modification Completed (not old ANSI) */
		show_msg("<- CMC: ");
		goto unexpected;
	case ISUP_MT_CMRJ:	/* 0x1e - 0b00011110 - Call Modification Reject (not old ANSI) */
		show_msg("<- CMRJ: ");
		goto unexpected;
	case ISUP_MT_FAR:	/* 0x1f - 0b00011111 - Facility request */
		show_msg("<- FAR: ");
		goto unexpected;
	case ISUP_MT_FAA:	/* 0x20 - 0b00100000 - Facility accepted */
		show_msg("<- FAA: ");
		goto unexpected;
	case ISUP_MT_FRJ:	/* 0x21 - 0b00100001 - Facility reject */
		show_msg("<- FRJ: ");
		goto unexpected;
	case ISUP_MT_FAD:	/* 0x22 - 0b00100010 - Facility Deactivated (old Bellcore only) */
		show_msg("<- FAD: ");
		goto unexpected;
	case ISUP_MT_FAI:	/* 0x23 - 0b00100011 - Facility Information (old Bellcore only) */
		show_msg("<- FAI: ");
		goto unexpected;
	case ISUP_MT_LPA:	/* 0x24 - 0b00100100 - Loop back acknowledgement */
		show_msg("<- LPA: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_COT;
		*h++ = 0x01;	/* success */
		data.len = ((char *) h - data.buf);
		sleep(1);
		show_msg("-> COT: ");
		send_msg();
		start_timer(100);
		state = 6;
		break;
	case ISUP_MT_DRS:	/* 0x27 - 0b00100111 - Delayed release (not old ANSI) */
		show_msg("<- DRS: ");
		goto unexpected;
	case ISUP_MT_PAM:	/* 0x28 - 0b00101000 - Pass along */
		show_msg("<- PAM: ");
		goto unexpected;
	case ISUP_MT_CFN:	/* 0x2f - 0b00101111 - Confusion */
		show_msg("<- CFN: ");
		goto unexpected;
	case ISUP_MT_OLM:	/* 0x30 - 0b00110000 - Overload */
		show_msg("<- OLM: ");
		goto unexpected;
	case ISUP_MT_CRG:	/* 0x31 - 0b00110001 - Charge information */
		show_msg("<- CRG: ");
		goto unexpected;
	case ISUP_MT_NRM:	/* 0x32 - 0b00110010 - Network resource management */
		show_msg("<- NRM: ");
		goto unexpected;
	case ISUP_MT_FAC:	/* 0x33 - 0b00110011 - Facility */
		show_msg("<- FAC: ");
		goto unexpected;
	case ISUP_MT_IDR:	/* 0x36 - 0b00110110 - Identification request */
		show_msg("<- IDR: ");
		goto unexpected;
	case ISUP_MT_IRS:	/* 0x37 - 0b00110111 - Identification response */
		show_msg("<- IRS: ");
		goto unexpected;
	case ISUP_MT_SGM:	/* 0x38 - 0b00111000 - Segmentation */
		show_msg("<- SGM: ");
		return;
	case ISUP_MT_GRS:	/* 0x17 - 0b00010111 - Circuit group reset */
		show_msg("<- GRS: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_GRA;
		show_msg("-> GRA: ");
		send_msg();
		return;
	case ISUP_MT_CGB:	/* 0x18 - 0b00011000 - Circuit group blocking */
		show_msg("<- CGB: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_CGBA;
		show_msg("-> CGBA: ");
		send_msg();
		if (state == 0 || state == 1) {
			start_timer(100);
			state = 6;
		}
		return;
	case ISUP_MT_CGU:	/* 0x19 - 0b00011001 - Circuit group unblocking */
		show_msg("<- CGU: ");
		/* rewrite the header swap the point codes */
		h = data.buf;
		*h++ = si;
		*h++ = (opc & 0x0ff);
		*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
		*h++ = ((dpc >> 2) & 0x0ff);
		*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
		*h++ = (cic & 0x0ff);
		*h++ = ((cic >> 8) & 0x0f);
		*h++ = ISUP_MT_CGUA;
		show_msg("-> CGUA: ");
		send_msg();
		return;
	case ISUP_MT_CGBA:	/* 0x1a - 0b00011010 - Circuit group blocking acknowledgement */
		show_msg("<- CGBA: ");
		return;
	case ISUP_MT_CGUA:	/* 0x1b - 0b00011011 - Circuit group unblocking acknowledgement */
		show_msg("<- CGUA: ");
		return;
	case ISUP_MT_GRA:	/* 0x29 - 0b00101001 - Circuit group reset acknowledgement */
		show_msg("<- GRA: ");
		return;
	case ISUP_MT_CQM:	/* 0x2a - 0b00101010 - Circuit group query */
		show_msg("<- CQM: ");
		goto unexpected;
	case ISUP_MT_CQR:	/* 0x2b - 0b00101011 - Circuit group query response */
		show_msg("<- CQR: ");
		return;
	default:
		show_msg("???: ");
		break;
	}
	return;
	goto release;
      release:
	/* rewrite the header swap the point codes */
	h = data.buf;
	*h++ = si;
	*h++ = (opc & 0x0ff);
	*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
	*h++ = ((dpc >> 2) & 0x0ff);
	*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
	*h++ = (cic & 0x0ff);
	*h++ = ((cic >> 8) & 0x0f);
	*h++ = ISUP_MT_REL;
	*h++ = 2;		/* offset */
	*h++ = 0x00;		/* end of optional */
	*h++ = 2;		/* length */
	*h++ = 0x00;		/* Q.763 = User */
	*h++ = 0x10;		/* Normal call clearing */
	data.len = ((char *) h - data.buf);
	show_msg("-> REL: ");
	send_msg();
	return;
      unexpected:
	if (state == 1) {
		state = 6;
		return;
	}
	if (state != 0 && state != 6)
		return;
	/* rewrite the header swap the point codes */
	h = data.buf;
	*h++ = si;
	*h++ = (opc & 0x0ff);
	*h++ = ((opc >> 8) & 0x03f) | ((dpc & 0x03) << 6);
	*h++ = ((dpc >> 2) & 0x0ff);
	*h++ = ((dpc >> 10) & 0x0f) | ((sls & 0x0f) << 4);
	*h++ = (cic & 0x0ff);
	*h++ = ((cic >> 8) & 0x0f);
	*h++ = ISUP_MT_RSC;
	data.len = ((char *) h - data.buf);
	show_msg("-> RSC: ");
	send_msg();
	return;
}

static int release_attempts = 0;

static int mymain(int argc, char **argv)
{
	int ret;
	printf("Opening mux.\n");
	fflush(stdout);
	if ((fd = open("/dev/sl-mux", O_NONBLOCK | O_RDWR)) < 0) {
		perror(argv[0]);
		exit(1);
	}
	printf("Ioctl on mux.\n");
	fflush(stdout);
	if (ioctl(fd, I_SRDOPT, RMSGD) < 0) {
		perror(argv[0]);
		exit(1);
	}
	if (state != 6)
		start_timer(100);	/* wait a bit */
	for (;;) {
		switch (state) {
		case 0:
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				start_timer(20000);	/* timer t7 = 20 seconds */
				send_iam();
				state = 1;
				break;
			default:
				break;
			}
			break;
		}
		case 1:	/* waiting for ACM/CON */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				release_attempts = 0;
				send_rel();
				start_timer(60000);	/* timer t1 = 60 seconds */
				state = 3;
				break;
			default:
				break;
			}
			break;
		}
		case 2:	/* waiting for ANM */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				send_rel();
				start_timer(60000);	/* timer t1 = 60 seconds */
				state = 3;
				break;
			default:
				break;
			}
			break;
		}
		case 3:	/* wait for RLC */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				if (++release_attempts > 4) {
					send_rsc();
					start_timer(300000);	/* timer t17 */
					state = 4;
				} else {
					send_rel();
					start_timer(60000);	/* timer t1 = 60 seconds */
					state = 3;
				}
				break;
			default:
				break;
			}
			break;
		}
		case 4:	/* wait for RLC */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				send_rsc();
				start_timer(300000);	/* timer t17 */
				state = 4;
				break;
			default:
				break;
			}
			break;
		}
		case 5:	/* connected */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				send_rel();
				start_timer(60000);	/* timer t1 = 60 seconds */
				state = 3;
				break;
			}
			break;
		}
		case 6:	/* got RLC done, exit */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				break;
			}
			break;
		}
		case 7:	/* got IAM, wait COT t8 = 10 seconds */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				send_rel();
				start_timer(60000);	/* timer t1 = 60 seconds */
				state = 3;
				break;
			}
			break;
		}
		case 8:	/* send ACM, waiting to send ANM */
		{
			switch ((ret = get_event())) {
			case SL_PDU_IND:
				handle_message();
				break;
			case TIMEOUT:
				send_anm();
				start_timer(15000);
				state = 5;	/* connected */
				break;
			}
			break;
		}
		default:
			fprintf(stderr, "Software error.\n");
			exit(1);
		}
	}
	close(fd);
	exit(0);
	return (0);
}

int main(int argc, char **argv)
{
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"called", 1, 0, 'd'},
			{"calling", 1, 0, 'g'},
			{"cic", 1, 0, 'c'},
			{"nocall", 0, 0, 'n'}
		};
		c = getopt_long(argc, argv, "hd:g:c:n", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:	/* -h */
				help(argc, argv);
				exit(0);
			case 1:	/* -d called */
				strncpy(called, optarg, sizeof(called) - 1);
				break;
			case 2:	/* -g calling */
				strncpy(calling, optarg, sizeof(calling) - 1);
				break;
			case 3:	/* -c cic */
				my_cic = atoi(optarg);
				break;
			case 4:	/* -n */
				state = 6;
				break;
			default:
				usage(argc, argv);
				exit(1);
			}
		case 'h':
			help(argc, argv);
			exit(0);
		case 'd':
			strncpy(called, optarg, sizeof(called) - 1);
			break;
		case 'g':
			strncpy(calling, optarg, sizeof(calling) - 1);
			break;
		case 'c':
			my_cic = atoi(optarg);
			break;
		case 'n':
			state = 6;
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
