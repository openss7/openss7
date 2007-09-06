/*****************************************************************************

 @(#) $RCSfile: test-mgmt.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $

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

#ident "@(#) $RCSfile: test-mgmt.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $"

static char const ident[] =
    "$RCSfile: test-mgmt.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $";

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

static int command = 0;
static uint my_cic = 1;
static uint my_dpc = 0x11ae;
static uint my_opc = 0x11d8;

static void usage(int argc, char **argv)
{
	fprintf(stderr, "\n\
Usage: %s [options]\n\
  -h, --help\n\
	prints this usage information and exits\n\
  -c, --cic cic                                 (default: %d)\n\
	cic specified the circuit identification code to use\n\
  -b, --blo\n\
	block the specified cic\n\
  -u, --ubl\n\
	unblock the specified cic\n\
  -r, --rsc\n\
	reset the specified cic\n\
  -t, --ccr\n\
	continuity test the specified cic\n\
  -g, --group\n\
	perform action on entire group\n\
\n", argv[0], my_cic);
};

static void help(int argc, char **argv)
{
	usage(argc, argv);
	fprintf(stderr, "\
This program opens the MTP mini-mux and initiates a generates a\n\
management command as specified by command options.\n\
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

static void send_blo(void)
{
	unsigned char *h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_BLO;
	data.len = ((char *) h - data.buf);
	show_msg("-> BLO: ");
	send_msg();
	return;
}

static void send_ubl(void)
{
	unsigned char *h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_UBL;
	data.len = ((char *) h - data.buf);
	show_msg("-> UBL: ");
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

static void send_ccr(void)
{
	unsigned char *h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_CCR;
	data.len = ((char *) h - data.buf);
	show_msg("-> CCR: ");
	send_msg();
	return;
}

static void send_cgb(void)
{
	unsigned char *h;
	my_cic = 1;
	h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_CGB;
	*h++ = 0x00;		/* 0x00 maintenance, 0x01 hardware */
	*h++ = 0x01;		/* offset */
	*h++ = 5;		/* length */
	*h++ = 30;		/* 30+1 circuits affected */
	*h++ = 0xff;		/* status - 31 circuits */
	*h++ = 0x7f;
	*h++ = 0xff;
	*h++ = 0x7f;
	data.len = ((char *) h - data.buf);
	show_msg("-> CGB: ");
	send_msg();
	my_cic = 33;
	h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_CGB;
	*h++ = 0x00;		/* 0x00 maintenance, 0x01 hardware */
	*h++ = 0x01;		/* offset */
	*h++ = 5;		/* length */
	*h++ = 30;		/* 30+1 circuits affected */
	*h++ = 0xff;		/* status - 31 circuits */
	*h++ = 0xff;
	*h++ = 0xff;
	*h++ = 0x7f;
	data.len = ((char *) h - data.buf);
	show_msg("-> CGB: ");
	send_msg();
	return;
}

static void send_cgu(void)
{
	unsigned char *h;
	my_cic = 1;
	h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_CGU;
	*h++ = 0x00;		/* 0x00 maintenance, 0x01 hardware */
	*h++ = 0x01;		/* offset */
	*h++ = 5;		/* length */
	*h++ = 30;		/* 30+1 circuits affected */
	*h++ = 0xff;		/* status - 31 circuits */
	*h++ = 0x7f;
	*h++ = 0xff;
	*h++ = 0x7f;
	data.len = ((char *) h - data.buf);
	show_msg("-> CGU: ");
	send_msg();
	my_cic = 33;
	h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_CGU;
	*h++ = 0x00;		/* 0x00 maintenance, 0x01 hardware */
	*h++ = 0x01;		/* offset */
	*h++ = 5;		/* length */
	*h++ = 30;		/* 30+1 circuits affected */
	*h++ = 0xff;		/* status - 31 circuits */
	*h++ = 0xff;
	*h++ = 0xff;
	*h++ = 0x7f;
	data.len = ((char *) h - data.buf);
	show_msg("-> CGU: ");
	send_msg();
	return;
}

static void send_grs(void)
{
	unsigned char *h;
	my_cic = 1;
	h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_GRS;
	*h++ = 0x00;		/* 0x00 maintenance, 0x01 hardware */
	*h++ = 0x01;		/* offset */
	*h++ = 5;		/* length */
	*h++ = 62;		/* 30+1 circuits affected */
	*h++ = 0xff;		/* status - 31 circuits */
	*h++ = 0x7f;
	*h++ = 0xff;
	*h++ = 0x7f;
	data.len = ((char *) h - data.buf);
	show_msg("-> GRS: ");
	send_msg();
	my_cic = 33;
	h = data.buf;
	*h++ = 0x05;		/* ISUP */
	*h++ = (my_opc & 0x0ff);
	*h++ = ((my_opc >> 8) & 0x3f) | ((my_dpc & 0x3) << 6);
	*h++ = ((my_dpc >> 2) & 0x0ff);
	*h++ = ((my_dpc >> 10) & 0x0f) | ((my_cic & 0x0f) << 4);
	*h++ = (my_cic & 0x0ff);
	*h++ = ((my_cic >> 8) & 0x0f);
	*h++ = ISUP_MT_GRS;
	*h++ = 0x00;		/* 0x00 maintenance, 0x01 hardware */
	*h++ = 0x01;		/* offset */
	*h++ = 5;		/* length */
	*h++ = 62;		/* 30+1 circuits affected */
	*h++ = 0xff;		/* status - 31 circuits */
	*h++ = 0xff;
	*h++ = 0xff;
	*h++ = 0x7f;
	data.len = ((char *) h - data.buf);
	show_msg("-> GRS: ");
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

static int mymain(int argc, char **argv)
{
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
	switch (command) {
	default:
	case 0:		/* block */
		send_blo();
		break;
	case 1:		/* unblock */
		send_ubl();
		break;
	case 2:		/* reset */
		send_rsc();
		break;
	case 3:		/* cont test */
		send_ccr();
		break;
	case 4:		/* group blocking */
		send_cgb();
		break;
	case 5:		/* group unblocking */
		send_cgu();
		break;
	case 6:		/* group reset */
		send_grs();
		break;
	case 7:		/* release */
		send_rel();
		break;
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
			{"cic", 1, 0, 'c'},
			{"blo", 0, 0, 'b'},
			{"ubl", 0, 0, 'u'},
			{"rsc", 0, 0, 'r'},
			{"ccr", 0, 0, 't'},
			{"group", 0, 0, 'g'}
		};
		c = getopt_long(argc, argv, "hc:burtg", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:	/* -h */
				help(argc, argv);
				exit(0);
			case 1:	/* -c cic */
				my_cic = atoi(optarg);
				break;
			case 2:	/* -b */
				command = 0;
				break;
			case 3:	/* -u */
				command = 1;
				break;
			case 4:	/* -r */
				command = 2;
				break;
			case 5:	/* -t */
				command = 3;
				break;
			case 6:	/* -g */
				command += 4;
				break;
			default:
				usage(argc, argv);
				exit(1);
			}
		case 'h':
			help(argc, argv);
			exit(0);
		case 'c':
			my_cic = atoi(optarg);
			break;
		case 'b':
			command = 0;
			break;
		case 'u':
			command = 1;
			break;
		case 'r':
			command = 2;
			break;
		case 't':
			command = 3;
			break;
		case 'g':
			command += 4;
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
