/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */
#ident "@(#) LiS log.c 2.3 09/13/04 10:12:31 "

/* Streams logging module */

#include <linux/types.h>
#include <linux/time.h>
#include <linux/signal.h>
#include <linux/param.h>
#include <linux/sysmacros.h>
#include <linux/stream.h>
#include <linux/stropts.h>
#include <linux/user.h>
#include <linux/errno.h>
#include "log.h"

#define MAXB 100

static struct module_info log_minfo =
{
		0, "strlog", 0, INFPSZ, 0, 0
};

static int _RP log_open (), _RP log_close ();
static int _RP log_wput (), _RP log_rput ();

static struct qinit log_rinit =
{
		log_rput, NULL, log_open, log_close, NULL, &log_minfo, NULL
};

static struct qinit log_winit =
{
		log_wput, NULL, NULL, NULL, NULL, &log_minfo, NULL
};

struct streamtab log_info =
{&log_rinit, &log_winit, NULL, NULL};

#define NLOG 10
static struct log {
	char flags;
	char nr;
} log_log[NLOG];

static int log_cnt = NLOG;


static int _RP
log_open (queue_t * q, int dev, int flag, int sflag
#ifdef DO_ADDERROR
		,int *err
#endif
)
{
	struct log *log;

#ifdef DO_STREAMDEBUG
	logme ();
#endif
	if (q->q_ptr) {
		if (0)
			printf ("Log: already open?\n");
		return 0;
	}
	for (dev = 0; dev < log_cnt; dev++) {
		if (!(log_log[dev].flags & LOG_INUSE))
			break;
	}

	if (dev >= log_cnt) {
		printf ("log: all %d devices allocated\n", log_cnt);
		return OPENFAIL;
	}
	log = &log_log[dev];
	WR (q)->q_ptr = (char *) log;
	q->q_ptr = (char *) log;

	log->flags = LOG_INUSE | LOG_READ | LOG_WRITE;
	log->nr = minor (dev);
	if (1)
		printf ("Log driver %d opened.\n", dev);

	return 0;
}


#if 0
void
log_printtcp (struct log *log, mblk_t * mp, char wr)
{
	register struct ip *ip;
	register uint_t hlen;
	register struct tcphdr *oth;
	register struct tcphdr *th;

	register mblk_t *mq = dupmsg (mp);

	if (wr)
		wr = 'w';
	else
		wr = 'r';

	mp = pullup (mq, 128);
	if (mp == NULL) {
		freemsg (mq);
		printf ("Log%cN %d ", wr);
		return;
	}
	ip = (struct ip *) mp->b_rptr;
	hlen = ip->ip_hl;


	if (ip->ip_p != IPPROTO_TCP
			|| (ip->ip_off & htons (0x3fff)) || mq->b_wptr - mq->b_rptr < 40) {
		freemsg (mp);
		printf ("Log%cT %x", wr, ip->ip_p);
		return;
	}
	th = (struct tcphdr *) & ((unchar *) ip)[hlen << 2];
	if (wr == 'w') {
		printf ("TCP %x.%x ", th->th_seq, dsize (mp) - th->th_off);
	} else {
		printf ("TCP %x ", th->th_ack);
	}
	freemsg (mp);
}

#endif

void
log_printmsg (struct log *log, const char *text, mblk_t * mp)
{
	int ms = splstr ();

#ifdef DO_STREAMDEBUG
	logme ();
#endif
	if (log != NULL) {
#if 0
		if (log - log_log != 0 && mp->b_datap->db_type == M_DATA)
			return;
#endif
		printf ("Log %d: ", log - log_log);
	}
	printf ("%s", text);

	{
		mblk_t *mp1;
		char *name;
		int nblocks = 0;
		static mblk_t *blocks[MAXB];

		for (mp1 = mp; mp1 != NULL; mp1 = mp1->b_cont) {
			blocks[nblocks++] = mp1;
			switch (mp1->b_datap->db_type) {
			case M_DATA:
				name = "DATA";
				break;
			case M_PROTO:
				name = "PROTO";
				break;
#ifdef M_SPROTO
			case M_SPROTO:
				name = "SPROTO";
				break;
#endif
			case M_BREAK:
				name = "BREAK";
				break;
			case M_SIG:
				name = "SIG";
				break;
			case M_DELAY:
				name = "DELAY";
				break;
			case M_CTL:
				name = "CTL";
				break;
			case M_IOCTL:
				name = "IOCTL";
				break;
			case M_SETOPTS:
				name = "SETOPTS";
				break;
#ifdef M_ADMIN
			case M_ADMIN:
				name = "ADMIN";
				break;
#endif
#ifdef M_EXPROTO
			case M_EXPROTO:
				name = "EXPROTO";
				break;
#endif
#ifdef M_EXDATA
			case M_EXDATA:
				name = "EXDATA";
				break;
#endif
#ifdef M_EXSPROTO
			case M_EXSPROTO:
				name = "EXSPROTO";
				break;
#endif
#ifdef M_EXSIG
			case M_EXSIG:
				name = "EXSIG";
				break;
#endif
#ifdef M_PCPROTO
			case M_PCPROTO:
				name = "PCPROTO";
				break;
#endif
#ifdef M_PKT
			case M_PKT:
				name = "PKT";
				break;
#endif
#ifdef M_PKTSTOP
			case M_PKTSTOP:
				name = "PKTSTOP";
				break;
#endif
			case M_IOCACK:
				name = "IOCACK";
				break;
			case M_IOCNAK:
				name = "IOCNAK";
				break;
			case M_PCSIG:
				name = "PCSIG";
				break;
			case M_FLUSH:
				name = "FLUSH";
				break;
			case M_STOP:
				name = "STOP";
				break;
			case M_START:
				name = "START";
				break;
			case M_HANGUP:
				name = "HANGUP";
				break;
			case M_ERROR:
				name = "ERROR";
				break;
#ifdef _MSG_PROTO_OWN
#ifdef MSG_PROTO
			case MSG_PROTO:
				name = "MSGPROTO";
				break;
#endif
#endif
			default:
				printf (":%d:", mp1->b_datap->db_type);
				name = "unknown"; /* ,mp->b_datap->db_type); */
				break;
			}
			printf ("; %s: %x.%x.%d", name, mp1, mp1->b_datap, mp1->b_wptr - mp1->b_rptr);
			if (nblocks == MAXB) {
				printf ("\n*** Block 0x%x pointed to 0x%x", mp1, mp1->b_cont);
				mp1->b_cont = NULL;
			} else {
				int j;

				for (j = 0; j < nblocks; j++) {
					if (mp1->b_cont == blocks[j]) {
						printf ("\n*** Block 0x%x circled to 0x%x (%d)", mp1, mp1->b_cont, j);
						mp1->b_cont = NULL;
					}
				}
			}
		}
	}
	printf ("\n");
	{
		int j;
		mblk_t *mp1;
		const char ctab[]= "0123456789abcdef";

#define BLOCKSIZE 0x10
		for (j = 0, mp1 = mp; mp1 != NULL; mp1 = mp1->b_cont, j++) {
			int i;
			unchar *dp;
			unchar x;

			for (i = 0, dp = (unchar *) mp1->b_rptr; dp < (unchar *) mp1->b_wptr; dp += BLOCKSIZE) {
				int k;
				int l = (unchar *) mp1->b_wptr - dp;

				printf ("    ");
				for (k = 0; k < BLOCKSIZE && k < l; k++)
					printf ("%c%c ", ctab[dp[k] >> 4], ctab[dp[k] & 0x0F]);
				for (; k < BLOCKSIZE; k++)
					printf ("   ");
				printf (" : ");
				for (k = 0; k < BLOCKSIZE && k < l; k++)
					if (dp[k] > 31 && dp[k] < 127)
						printf ("%c", dp[k]);
					else
						printf (".");
				if (k < l)
					printf (" +\n");
				else if (mp1->b_cont != NULL) {
					for (; k < BLOCKSIZE; k++)
						printf (" ");
					printf (" -\n");
				} else
					printf ("\n");
			}
		}
	}
	splx (ms);
}

static int _RP
log_wput (queue_t * q, mblk_t * mp)
{
	register struct log *log;

#ifdef DO_STREAMDEBUG
	logme ();
#endif
	log = (struct log *) q->q_ptr;

	if (log->flags & LOG_WRITE) {
#if 0
		if (*mp->b_rptr == 0x45)
			log_printtcp (log, mp, 1);
		else
#endif
			log_printmsg (log, "write", mp);
		switch (mp->b_datap->db_type) {
		default:{
				break;
			}
		}
	}
	putnext (q, mp);
	return 0;
}

static int _RP
log_rput (queue_t * q, mblk_t * mp)
{
	register struct log *log;

#ifdef DO_STREAMDEBUG
	logme ();
#endif
	log = (struct log *) q->q_ptr;

	if (log->flags & LOG_READ) {
#if 0
		if (*mp->b_rptr == 0x45)
			log_printtcp (log, mp, 0);
		else
#endif
			log_printmsg (log, "read", mp);
	}
	putnext (q, mp);
	return 0;
}


static int _RP
log_close (queue_t * q)
{
	struct log *log;

#ifdef DO_STREAMDEBUG
	logme ();
#endif
	log = (struct log *) q->q_ptr;

	flushq (q, FLUSHALL);
	flushq (WR (q), FLUSHALL);
	if (1)
		printf ("Log driver %d closed.\n", log->nr);
	log->flags = 0;
	return 0;
}
