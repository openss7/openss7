/*
 *  tirdwr module.
 *
 *  Version: 0.1
 *
 *  Copyright (C) 1999 Ole Husgaard (sparre@login.dknet.dk)
 *
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
 *
 *  This is about as simple as a STREAMS module can be.
 */

#include <sys/LiS/module.h>	/* must be VERY first include */

#include <stdarg.h>

#include <sys/stream.h>
#include <sys/tihdr.h>
#include <sys/lislocks.h>
#include <sys/osif.h>

#ifndef STATIC
#define STATIC static
#endif

#define TIRDWR_TRACE 0 /* Verbose level */

/*
 *  The private state of a tirdwr stream
 */
typedef struct {
	int got_ordrel;
#if TIRDWR_TRACE
	int verbose;
#endif
} tirdwr_priv_t;

STATIC int _RP tirdwr_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int _RP tirdwr_close(queue_t *, int, cred_t *);
STATIC int _RP tirdwr_rput(queue_t *, mblk_t *);
STATIC int _RP tirdwr_wput(queue_t *, mblk_t *);


STATIC struct module_info tirdwr_minfo = 
{
	0,		/* Module ID number 				*/
	"tirdwr",	/* Module name					*/
	0,		/* Min packet size accepted			*/
	INFPSZ,		/* Max packet size accepted			*/
	0,		/* Hi water mark ignored, no queue service	*/
	0		/* Low water mark ignored, no queue service	*/
};


STATIC struct qinit tirdwr_rinit = 
{
	tirdwr_rput,	/* Read put			*/
	NULL,		/* No read queue service	*/
	tirdwr_open,	/* Each open			*/
	tirdwr_close,	/* Last close			*/
	NULL,		/* Reserved			*/
	&tirdwr_minfo,	/* Information			*/
	NULL		/* No statistics		*/
};

STATIC struct qinit tirdwr_winit = 
{
	tirdwr_wput,	/* Write put			*/
	NULL,		/* No write queue service	*/
	NULL,		/* Each open			*/
	NULL,		/* Last close			*/
	NULL,		/* Reserved			*/
	&tirdwr_minfo,	/* Information			*/
	NULL		/* No statistics		*/
};

#ifdef MODULE
STATIC
#endif
struct streamtab tirdwr_info = 
{
	&tirdwr_rinit,	/* Read queue			*/
	&tirdwr_winit,	/* Write queue			*/
	NULL,		/* Not a multiplexer		*/
	NULL		/* Not a multiplexer		*/
};

/*
 *  Debugging
 */

#if TIRDWR_TRACE

STATIC void
vtrace(char *func, queue_t *rq, int verbose, char *msg, va_list args)
{
	tirdwr_priv_t *priv = (tirdwr_priv_t *)rq->q_ptr;

	if (verbose <= priv->verbose) {
		char s1[128], s2[1024];

		if (!(rq->q_flag & QREADR))
			rq = RD(rq);

		sprintf(s1, "rq=%p", rq);
		vsprintf(s2, msg, args);
		printk("tirdwr: %s(): (%s): %s\n", func, s1, s2);
	}
}

STATIC void trace(char *func, queue_t *rq, int verbose, char *msg, ...)
	__attribute__ ((format (printf, 4, 5)));

STATIC void trace(char *func, queue_t *rq, int verbose, char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	vtrace(func, rq, verbose, msg, args);
	va_end(args);
}

#define TRACE(rq, verbose, msg, args...) \
	trace(__FUNCTION__, rq, verbose, msg, ## args)

#define FUNC_ENTER(rq)	TRACE(rq, 10, "Entered")
#define FUNC_EXIT(rq)	TRACE(rq, 10, "Exiting")

#else /* No trace logging */

#define TRACE(rq, verbose, msg, args...) do {} while (0)

#define FUNC_ENTER(rq)
#define FUNC_EXIT(rq)

#endif


STATIC int _RP tirdwr_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	tirdwr_priv_t *priv;
	lis_flags_t    psw;
	queue_t *hq;
	mblk_t *mp;

	if (sflag != MODOPEN)
		return ENXIO;
	if (q->q_ptr != NULL)
		return ENXIO;
	if ((priv = ALLOC(sizeof(tirdwr_priv_t))) == NULL)
		return ENOSR;
	priv->got_ordrel = 0;
#if TIRDWR_TRACE
	priv->verbose = TIRDWR_TRACE;
#endif
	q->q_ptr = WR(q)->q_ptr = priv;
	
	/*
	 *  This is Very Ugly
	 */
	hq = q->q_next;
	LIS_QISRLOCK(hq, &psw) ;
	LISASSERT(hq != NULL);
	for (mp = hq->q_first; mp; mp = mp->b_next) {
		if (mp->b_datap->db_type == M_PROTO ||
		    mp->b_datap->db_type == M_PCPROTO) {
			LIS_QISRUNLOCK(hq, &psw) ;
			TRACE(q, 1, "Control message present at head, "
			      "failing open.");
			return EPROTO;
		}
	}
	LIS_QISRUNLOCK(hq, &psw) ;

	MODGET();
	return 0;
}

STATIC int _RP tirdwr_close(queue_t *q, int flag, cred_t *crp)
{
	tirdwr_priv_t *priv = (tirdwr_priv_t *)q->q_ptr;

	LISASSERT(priv != NULL);
	LISASSERT(q->q_ptr == WR(q)->q_ptr);

	q->q_ptr = WR(q)->q_ptr = NULL;
	if (priv->got_ordrel) {
		mblk_t *mp = allocb(sizeof(struct T_ordrel_req), BPRI_HI);

		if (!mp)
			TRACE(q, 1, "T_ordrel_req allocation failed.");
		else {
			struct T_ordrel_req *req;

			req = (struct T_ordrel_req *)mp->b_rptr;
			mp->b_datap->db_type = M_PROTO;
			mp->b_wptr += sizeof(*req);
			req->PRIM_type = T_ORDREL_REQ;
			putnext(WR(q), mp);
		}
	}
	FREE(priv);
	MODPUT();
	return 0;
}

STATIC int _RP tirdwr_rput(queue_t *q, mblk_t *mp)
{
	FUNC_ENTER(q);

	if (mp->b_datap->db_type!=M_PROTO && mp->b_datap->db_type!=M_PCPROTO)
		putnext(q, mp);
	else {
		t_scalar_t prim;

		if (mp->b_wptr - mp->b_rptr < sizeof(t_scalar_t)) {
			TRACE(q, 1, "Short control, erroring.");
			freemsg(mp);
			putctl1(RD(q), M_ERROR, EPROTO);
			goto quit;
		}
		prim = *(t_scalar_t *)mp->b_rptr;
		if (prim == T_DATA_IND) {
			mblk_t *bp;

			TRACE(q, 9, "Data, removing control.");
			bp = unlinkb(mp);
			freemsg(mp);
			putnext(q, bp);
			goto quit;
		}
		if (prim == T_DISCON_IND) {
			TRACE(q, 4, "Disconnect received, erroring.");
			freemsg(mp);
			putctl1(RD(q), M_ERROR, ENXIO);
			goto quit;
		}
		if (prim == T_ORDREL_IND) {
			TRACE(q, 5, "Orderly release received.");
			((tirdwr_priv_t *)q->q_ptr)->got_ordrel = 1;
			freemsg(unlinkb(mp));
			mp->b_wptr = mp->b_rptr;
			mp->b_datap->db_type = M_DATA;
			putnext(q, mp);
			goto quit;
		}
		if (prim == T_EXDATA_IND)
			TRACE(q, 1, "Expedited data, erroring.");
		else
			TRACE(q, 1, "Control portion present, erroring.");
		freemsg(mp);
		putctl1(RD(q), M_ERROR, EPROTO);
	}

quit:	FUNC_EXIT(q);
	return(0) ;
}

STATIC int _RP tirdwr_wput(queue_t *q, mblk_t *mp)
{
	FUNC_ENTER(q);
	if (mp->b_datap->db_type == M_DATA) {
		if (msgdsize(mp) != 0) {
			TRACE(q, 9, "Data msg, doing putnext()");
			putnext(q, mp);
		} else {
			TRACE(q, 4, "Freeing zero length data msg.");
			freemsg(mp);
		}
	} else if (mp->b_datap->db_type == M_PROTO ||
	    mp->b_datap->db_type == M_PCPROTO) {
		TRACE(q, 1, "Control portion present, erroring.");
		freemsg(mp);
		putctl1(RD(q), M_ERROR, EPROTO);
	} else {
		TRACE(q, 7, "Other msg, doing putnext()");
		putnext(q, mp);
	}
	FUNC_EXIT(q);
	return(0) ;
}

#ifdef MODULE

#ifdef KERNEL_2_5
int tirdwr_init_module(void)
#else
int init_module(void)
#endif
{
	int ret = lis_register_strmod(&tirdwr_info, "tirdwr");
	if (ret < 0) {
		printk("Unable to register tirdwr module.\n");
		return ret;
	}
	TRACE(q, 1, "STREAMS TPI read/write interface initialized.");
	return 0;
}

#ifdef KERNEL_2_5
void tirdwr_cleanup_module(void)
#else
void cleanup_module(void)
#endif
{
	if (lis_unregister_strmod(&tirdwr_info) < 0)
		printk("Unable to unregister tirdwr module.\n");
	else
		TRACE(q, 1, "tirdwr module unregistered, ready to unload.");
	return;
}

#ifdef KERNEL_2_5
module_init(tirdwr_init_module) ;
module_exit(tirdwr_cleanup_module) ;
#endif

#if defined(LINUX)			/* linux kernel */
#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL and additional rights");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("Ole Husgaard (sparre@login.dknet.dk");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("STREAMS tirdwr, converts read/write to TLI");
#endif
#endif					/* LINUX */

#endif					/* MODULE */
