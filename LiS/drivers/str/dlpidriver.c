/*
 * A template for a connectionless DLPI driver; e.g. Ethernet
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

#ident "@(#) LiS dlpidriver.c 2.6 09/13/04 10:12:30 "

#include <linux/types.h>
#include <linux/stream.h>
#include <linux/stropts.h>
#include <linux/cred.h>
#include <linux/dlpi.h>
#include <linux/param.h>
#include <linux/errno.h>
#include <linux/debug.h>
#include <linux/cmn_err.h>
#include <linux/ddi.h>
#include "dlpidriver.h"

struct fr 
{
    queue_t	*rq;
    ulong_t	dlstate;
    ushort_t	idtype;
    ulong_t	id;
};

#define NO_ID	0
#define BUF_ID	1
#define TIME_ID	2

#define ONESEC	1000000

typedef enum retval 
{
    DONE, RETRY, ERR 
}
retval_t;

STATIC int _RP fropen(queue_t *, dev_t *, int, int, cred_t *);
STATIC int _RP frclose(queue_t *q, int cred_t *);
STATIC int _RP frrsrv(queue_t *);
STATIC int _RP frwput(queue_t *, mblk_t *);
STATIC int _RP frwsrv(queue_t *);

STATIC retval_t fr_info(struct fr *);
STATIC retval_t fr_bind(struct fr *, mblk_t *);
STATIC retval_t fr_unbind(struct fr *, mblk_t *);
STATIC retval_t fr_errorack(struct fr *, ulong_t, ulong_t, ulong_t);

STATIC void fr_send(struct fr *, mblk_t *);
STATIC void fr_wsched(struct fr *, int);
STATIC void fr_wcont, struct fr *);
STATIC void fr_free(char *);
STATIC void fr_ioctl(queue_t *, mblk_t *);

void frinit(void);
void frintr(int);

STATIC struct module_info fr_minfo = 
{
    0x7253, "fr", MINDATA, MAXDATA, 10*MAXDATA, MAXDATA
};


STATIC struct qinit fr_rinit = 
{
    NULL, frrsrv, fropen, frclose, NULL, &fr_minfo, NULL
};

STATIC struct qinit fr_winit = 
{
    frwput, frwsrv, NULL, NULL, NULL, &fr_minfo, NULL
};

struct streamtab frinfo = 
{
    &fr_rinit, &fr_winit, NULL, NULL
};

int frdevflag = 0;

void frinit()
{
}


STATIC int _RP fropen(queue_t *q, dev_t *devp, int flag, int sflag, cred_t
		  *crp)
{
    int i;
    if (sflag == MODOPEN)
	return EINVAL;
    if (q->q_ptr != NULL)
	return 0;
    if (sflag == CLONEOPEN)
    {
	for (i = 0; i < MAXFR; i++)
	    if (fr[i].rq == NULL)
		break;
	if (i > MAXFR)
	    return ENXIO;
	*devp = makedevice(getemajor(*devp), i);
    }
    else
    {
	if ((i = getminor(*devp)) >= MAXFR)
	    return ENXIO;
	
    }
    fr[i].rq = q;
    fr[i].idtype = NO_ID;
    fr[i].dlstate = DL_UNBOUND;
    q->q_ptr = (caddr_t)&fr[i];
    WR(q)->q_ptr = (caddr_t)&fr[i];
    return 0;
}

STATIC int _RP frclose(queue_t *q, int flag, cred_t *crp)
{
    struct fr *fr = (struct fr *)q->q_ptr;
    if (fr->idtype==TIME_ID)
	untimeout(fr->id);
    else if (fr->idtype == BUF_ID)
	unbuffcall(fr->id);
    fr->dlstate = DL_UNBOUND;
    fr->rq = NULL;
    q->q_ptr = NULL;
    WR(q)->q_ptr = NULL;
    return 0;
}

STATIC int _RP frwput(queue_t *q, mblk_t *mp)
{
    switch (mp->b_datap->db_type)
    {
    case M_FLUSH:
	if (*mp->b_rptr & FLUSHW)
	{
	    flushq(q, FLUSHALL);
	    *mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR)
	{
	    flushq(RD(q), FLUSHALL);
	    qreply(q, mp);
	}
	else freemsg(mp);
	break;
    case M_IOCTL:
	fr_ioctl(q, mp);
	break;
    case M_PROTO:
    case M_PCPROTO:
	putqf(q, mp);
	break;
    default:
	freemsg(mp);
	break;
    }
    return 0;
}

STATIC int _RP frwsrv(queue_t *q)
{
    mblk_t *mp;
    struct fr *fr;
    int s;
    ulong_t err, prim;
    union DL_primitives *dlp;
    fr = (struct fr *)q->q_ptr;
    if (fr == NULL)
	return 0;
    while ((mp = getq(q)) != NULL)
    {
	LISASSERT(mp->b_datap->db_type == M_PROTO || 
	       mp->b_datap->db_type == M_PCPROTO);
	err = 0;
	dlp = (union DL_primitives *)mp->b_rptr;
	prim = dlp->dl_primitive;
	switch(prim)
	{
	case DL_UNITDATA_REQ:
	    s = splstr();
	    if (frsndbusy)
		putbqf(q, mp);
	    else
		fr_send(fr, mp);
	    splx(s);
	    return 0;
	case DL_INFO_REQ:
	    if (fr_info(fr) != DONE)
		cmn_err(CE_WARN, "fr: can't respond to DL_INFO_REQ");
	    
	    freemsg(mp);
	    break;
	case DL_BIND_REQ:
	    if (fr_bind(fr, mp) == RETRY)
	    {
		putbqf(q, mp);
		return 0;
	    }
	    break;
	case DL_UNBIND_REQ:
	    if (fr_unbind(fr, mp) == RETRY)
	    {
		putbqf(q, mp);
		return 0;
	    }
	    break;
	case DL_ATTACH_REQ:
	case DL_SETACH_REQ:
	case DL_SUBS_BIND_REQ:
	case DL_ENABMULTI_REQ:
	case DL_DISABMULTI_REQ:
	case DL_PROMISCON_REQ:
	case DL_PROMISCOFF_REQ:
	case DL_UDQOS_REQ:
	case DL_CONNECT_REQ:
	case DL_CONNECT_RES:
	case DL_TOKEN_REQ:
	case DL_DISCONNECT_REQ:
	case DL_RESET_REQ:
	case DL_RESET_RES:
	case DL_DATA_ACK_REQ:
	case DL_REPLY_REQ:
	case DL_REPLY_UPDATE_REQ:
	case DL_XID_REQ:
	case DL_TEST_REQ:
	    err = DL_NOTSUPPORTED;
	    /* fall thru... */
	default:
	    if (err == 0) err = DL_BADPRIM;
	    if (fr_errorack(fr, prim, err, 0) == RETRY)
	    {
		if (mp->b_datap->db_type < QPCTL)
		{
		    putbqf(q, mp);
		    return 0;
		}
		cmn_err(CE_WARN, "fr: can't generate DL_ERROR_ACK (%d)", prim);
	    }
	    freemsg(mp);
	    break;
	}
    }
    return 0;
}

STATIC int _RP frrsrv(queue_t *q)
{
    mblk_t *mp;
    while ((mp = getq(q)) != NULL)
    {
	LISASSERT(mp->b_datap->db_type == M_PROTO);
	if (canput(q->q_next)) 
	{
	    putnext(q, mp);
	}
	else
	{
	    putbqf(q, mp);
	    break;
	}
    }
    return 0;
}

STATIC retval_t fr_info(struct fr *fr)
{
    dl_info_ack_t *ackp;
    mblk_t *mp;
    int i = DL_INFO_ACK_SIZE+TOTADDRSZ+MACSZ;
    if ((mp = allocb(i, BPRI_HI))==NULL)
	return ERR;
    mp->b_datap->db_type = M_PCPROTO;
    ackp = (dl_info_ack_t *)mp->b_wptr;
    ackp->dl_primitive = DL_INFO_ACK;
    ackp->dl_max_sdu = MAXDATA;
    ackp->dl_min_sdu = MINDATA;
    ackp->dl_addr_length = TOTADDRSZ;
    ackp->dl_mac_type = DL_??;
    ackp->dl_reserved = 0;
    ackp->dl_current_state = fr->dlstate;
    ackp->dl_sap_length = SAPSZ;
    ackp->dl_service_mode = DL_CLDLS;
    ackp->dl_qos_length = 0;
    ackp->dl_qos_offset = 0;
    ackp->dl_qos_range_length = 0;
    ackp->dl_qos_range_offset = 0;
    ackp->dl_provider_style = DL_STYLE1;
    ackp->dl_version = DL_VERSION_2;
    ackp->dl_brdcst_addr_length = MACSZ;
    ackp->dl_growth = 0;
    mp->b_wptr += DL_INFO_ACK_SIZE;
    if (fr->dlstate == DL_IDLE)
    {
	ackp->dl_addr_offset = DL_INFO_ACK_SIZE;
	???
    }
    else
    {
	???
    }
    ???;
    putnext(fr->rq, mp);
    return DONE;
}

STATIC retval_t fr_bind(struct fr *fr, mblk_t *mp)
{
    int i;
    dl_bind_req_t *reqp;
    dl_bind_ack_t *ackp;
    mblk_t *bp;
    ushort_t prot;
    
    if (fr->dlstate != DL_UNBOUND)
    {
	if (fr_errorack(fr, DL_BIND_REQ, DL_OUTSTATE, 0) == RETRY)
	    return RETRY;
	freemsg(mp);
	return ERR;
    }
    reqp = (dl_bind_req_t *)mp->b_rptr;
    if (reqp->dl_service_mode != DL_CLDLS)
    {
	if (fr_errorack(fr, DL_BIND_REQ, DL_UNSUPPORTED, 0) == RETRY)
	    return RETRY;
	freemsg(mp);
	return ERR;
    }
    if (reqp->dl_xidtest_flg != 0)
    {
	if (fr_errorack(fr, DL_BIND_REQ, DL_NOAUTO, 0) == RETRY)
	    return RETRY;
	freemsg(mp);
	return ERR;
    }
    prot = reqp->dl_sap;
    for (i = 0; i < MAXFRAME; i++)
	if (fr[i].prot == prot && fr[i].state == DL_IDLE)
	{
	    if (fr_errorack(fr, DL_BIND_REQ, DL_NOADDR, 0) == RETRY)
		return RETRY;
	    freemsg(mp);
	    return ERR; 
	}
    if ((bp = allocb(DL_BIND_ACK_SIZE + TOTADDRSZ, BPRI_HI)) == NULL)
    {
	fr_wsched(fr, DL_BIND_ACK_SIZE + TOTADDRSZ);
	return RETRY;
    }
    freemsg(mp);
    bp->b_datap->db_type = M_PCPROTO;
    ackp = (dl_bind_ack_t *)bp->b_wptr;
    ackp->dl_primitive = DL_BIND_ACK;
    ackp->dl_sap = prot;
    ackp->dl_addr_length = TOTADDRSZ;
    ackp->dl_addr_offset = DL_BIND_ACK_SIZE;
    ackp->dl_max_conind = 0;
    ackp->dl_xidtest_flg = 0;
    bp->b_wptr += DL_BIND_ACK_SIZE;
    for (i = 0; i < MACSZ; i++)
	*bp->b_wptr++ = fr_addr[i];
    *bp->b_wptr++ = (prot>>8)&0xff;
    *bp->b_wptr++ = prot & 0xff;
    fr->dlstate = DL_IDLE;
    fr->prot = prot;
    putnext(fr->rq, bp);
    /* enable the receiver */
    if (!frrcvbusy)
        CMD_REG = RECV_ON; /* ??? */
    return DONE;
}

STATIC retval_t fr_unbind(struct fr *fr, mblk_t *mp)
{
    mblk_t *bp;
    dl_ok_ack_t *ackp;
    if (fr->dlstate != DL_IDLE)
    {
	if (fr_errorack(fr, DL_UNBIND_REQ, DL_OUTSTATE, 0) == RETRY)
	    return RETRY;
	freemsg(mp);
	return ERR;
    }    
    if ((bp = allocb(DL_OK_ACK_SIZE, BPRI_HI)) == NULL)
    {
	fr_wsched(fr, DL_OK_ACK_SIZE);
	return RETRY;
    }
    if (putctl1(fr->rq->q_next, M_FLUSH, FLUSHRW) == 0)
	cmn_err(CE_WARN, "fr: can't flush stream on unbind");
    bp->b_datap->db_type = M_PCPROTO;
    ackp = (dl_ok_ack_t *)bp->b_wptr;
    ackp->dl_primitive = DL_OK_ACK;
    ackp->dl_correct_primitive = DL_UNBIND_REQ;
    bp->b_wptr += DL_OK_ACK_SIZE;
    fr->dlstate = DL_UNBOUND;
    fr->prot = 0;
    putnext(fr->rq, bp);
    return DONE;
}


STATIC void fr_send(struct fr *fr, mblk_t *mp)
{
    int i;
    uchar_t *p, *dest;
    dl_unitdata_req_t *reqp;
    struct frhdr f;
    mblk_t *bp;
    long destlen;
    reqp = (dl_unitdata_req_t *)mp->b_rptr;
    p = mp->b_rptr + reqp->dl_dest_addr_offset;
    dest = p;
    destlen = reqp->dl_dest_addr_length;
    if (fr->dlstate != DL_IDLE)
    {
	fr_uderr(fr, dest, destlen, DL_OUTSTATE, 0);
	freemsg(mp);
	return;
    }
    if (reqp->dl_dest_addr_length != TOTADDRSZ)
    {
	fr_uderr(fr, dest, destlen, DL_BADADDR, 0);
	freemsg(mp);
	return;
    }
    for (i= 0 ; i < MACSZ; i++)
    {
	f.dest[i] = *p++;
	f.src[i] = fr_addr[i];
    }
    f.prot.s = *(ushort_t *)p;
    bp = mp;
    mp = mp->b_cont;
    i = msgdsize(mp);
    if ((i > MAXDATA) || (i < MINDATA))
    {
	freemsg(bp);
	fr_uderr(fr, dest, destlen, DL_BADDATA, 0);
	return;
    }
    freeb(bp);
    if (sameaddr(f.dest, fr_addr))
    {
	fr_loop(mp, &f, fr->prot);
	return;
    }
    /* Copy f and mp + plus all continuation blocks to board
       memory, and send the packet */
    frsendbusy = 1;
    if (sameaddr(f.dest, bcast_addr))
    {
	fr_loop(mp, &f, fr->prot);
	return;
    }
    freemsg(mp);
}

STATIC void fr_wsched(struct fr *fr, int size)
{
    fr->id = bufcall(size, BPRI_HI, fr_wcont, (long)fr);
    if (fr->id == 0)
    {
	fr->id = timeout(fr_wcont, (caddr_t)fr, drv_usectohz(ONESEC));
	fr->idtype = TIME_ID;
    }
    else fr->idtype = BUF_ID;
}


STATIC void _RP fr_wcont(struct fr *fr)
{
    if (fr->rq != NULL)
    {
	fr->idtype = NO_ID;
	qenable(WR(fr->rq));
    }
}

STATIC retval_t fr_errorack(struct fr *fr, ulong_t prim, ulong_t err,
			    ulong_t uerr)
{
    dl_error_ack_t *errp;
    mblk_t *bp;
    if ((bp = allocb(DL_ERROR_ACK_SIZE, BPRI_HI)) == NULL)
    {
	fr_wsched(fr, DL_ERROR_ACK_SIZE);
	return RETRY;
    }
    bp->b_datap->db_type = M_PCPROTO;
    errp = (dl_error_ack_t *)bp->b_wptr;
    errp->dl_primitive = DL_ERROR_ACK;
    errp->dl_error_primitive = prim;
    errp->dl_errno = err;
    errp->dl_unix_errno = uerr;
    bp->b_wptr += DL_ERROR_ACK_SIZE;
    putnext(fr->rq, bp);
    return DONE;
}

STATIC void fr_uderr(struct fr *fr, uchar_t *dest, ulong_t destlen,
		     ulong_t err, ulong_t uerr)
{
    dl_uderror_ind_t *errp;
    mblk_t *bp;
    int i;
    
    i = DL_UDERROR_IND_SIZE + destlen;
    if ((bp = allocb(i, BPRI_HI)) == NULL)
	return;
    bp->b_datap->db_type = M_PROTO;
    errp = (dl_uderror_ind_t *)bp->b_wptr;
    errp->dl_primitive = DL_UDERROR_IND;
    errp->dl_errno = err;
    errp->dl_unix_errno = uerr;
    errp->dl_dest_addr_length = destlen;
    errp->dl_dest_addr_offset = DL_UDERROR_IND_SIZE;
    bp->b_wptr += DL_UDERROR_IND_SIZE;
    bcopy((caddr_t)dest, (caddr_t)bp->b_wptr, destlen);
    bp->b_wptr += destlen;
    putnext(fr->rq, bp);
}

void frint(int vec)
{
    dl_unitdata_ind_t *dp;
    uchar_t *p, *protp;
    int i, stat, size;
    mblk_t *mp, *bp;
    ushort_t prot;

    if (DPMem->status != 0)
    {
	/* handle failures */
	return;
    }
    stat = DPMem->ind;
    switch(stat)
    {
    case SEND_DONE:
	frsendbusy = 0;
	for (i = frnext; i < MAXFRAME; i++)
	    if (fr[i].dlstate == DL_IDLE && WR(fr[i].rq)->q_first)
	    {
		qenable(WR(fr[i].rq));
		frnext = i + 1;
		return;
	    }
    case RECV_PKT:
	size = DL_UNITDATA_IND_SIZE + TOTADDRSZ + TOTADDRSZ;
	if ((mp = allocb(size, BPRI_MED)) == NULL)
	{
	    frrcvlost++;
	    return;
	}
	size = ??;
	if ((bp = esballoc(DPMem->??, size, BPRI_MED, &frfrtn)) ==
	    NULL)
	{
	    freeb(mp);
	    frrcvlost++;
	    return;
	}
	mp->b_datap->db_type = M_PROTO;
	dp = (dl_unitdata_ind_t *)mp->b_wptr;
	dp->dl_primitive = DL_UNITDATA_IND;
	dp->dl_dest_addr_length = TOTADDRSZ;
	dp->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE;
	dp->dl_src_addr_length = TOTADDRSZ;
	dp->dl_src_addr_offset = DL_UNITDATA_IND_SIZE + TOTADDRSZ;
	p = bp->b_rptr;
	dp->dl_group_address = isgroupaddr(p);
	mp->b_wptr += DL_UNITDATA_IND_SIZE;
	for (i = 0; i < MACSZ; i++)
	    *mp->b_wptr++ = *p++;
	protp = mp->b_wptr;
	mp->b_wptr += SAPSZ;
	for (i = 0; i < MACSZ; i++)
	    *mp->b_wptr++ = *p++;
	prot = *p << 8;
	*mp->b_wptr++ = *p;
	*protp++ = *p++;
	prot |= *p;
	*mp->b_wptr++ = *p;
	*protp = *p++;
	bp->b_rptr = p;
	mp->b_cont = bp;
	for (i = 0; i < MAXFRAME; i++)
	{
	    if (fr[i].prot == prot && fr[i].dlstate == DL_IDLE)
	    {
		if (canput(fr[i].rq))
		{
		    putqf(fr[i].rq, mp);
		    frrcvbusy = 1;
		    return;
		}
		else
		{
		    freemsg(mp);
		    frrcvlost++;
		}
		break;
	    }
	}
	if (i >= MAXFRAME)
	    freemsg(mp);  /* no destination */
	break;
    default:
	frspurint++;
    }
}


STATIC boid fr_ioctl(queue_t *q, mblk_t *mp)
{
    int i;
    uchar_t *p;
    struct iocblk *iocp;
    struct fr *fr;
    
    iocp = (struct iocblk *)mp->b_rptr;
    fr = (struct fr *)q->q_ptr;
    if (iocp->ioc_count == TRANSPARENT)
	goto nak;
    switch(iocp->ioc_cmd)
    {
    case SETADDR:
	if (iocp->ioc_count != MACSZ)
	    goto nak;
	if (fr->dlstate != DL_UNBOUND)
	{
	    iocp->ioc_error = EBUSY;
	    goto nak;
	}
	iocp->ioc_error = drv_priv(iocp->ioc_cr);
	if (iocp->ioc_error != 0)
	    goto nak;
	p = (uchar_t *)FR_ADDR;
	for (i = 0; i < MACSZ; i++)
	{
	    fr_addr[i] = *mp->b_cont->b_rptr++;
	    *p++ = fr_addr[i];
	}
	mp->b_datap->db_type = M_IOCACK;
	iocp->ioc_count = 0;
	break;
    default:
    nak:
	mp->b_datap->db_type = M_IOCNAK;
    }
    qreply(q, mp);
}

STATIC void fr_loop(mblk_t *mp, struct frhdr *fhp, ushort_t srcsap)
{
    /* Is this necessary at all? */
}

STATIC void fr_free(char *dummy)
{
    frrcvbusy = 0;
}

STATIC int sameaddr(uchar_t *a, uchar_t *b)
{
    int i;
    for (i = 0; i < MACSZ; i++)
	if (*a++ != *b++)
	    return 0;
    return 1;
}

STATIC int isgroupaddr(uchar_t *dest)
{
    return (*dest & 1);
}



      

	    
    
	
 
    


















    


    




