/*
	filename:	tokqnx.c
	purpose :	test program for raw rx with 4.21 net

        Copyright (C) 1997  Mikel Matthews, Gcom, Inc <mikel@gcom.com>
*/
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

#ident "@(#) LiS tokqnx.c 2.2 3/19/01 22:03:40 "

/********************************************************** INCLUDES ******/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <conio.h>			/* for inp & outp (smc) */
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/seginfo.h>
#include <sys/irqinfo.h>
#include <sys/osinfo.h>
#include <sys/psinfo.h>
#include <sys/proxy.h>
#include <sys/sched.h>
#include <sys/sys_msg.h>
#include <sys/sendmx.h>
#include "../sendmx32_16.h"
#include <sys/inline.h>
#include <sys/net_nq.h>
#include <sys/net41msg.h>
#include <sys/net_drvr.h>

/********************************************************** DEFINES *******/

#define MSG_LEN		1000		

/********************************************************** STRUCTS *******/

union _drvr_msg {
	short unsigned int		type;
	short unsigned int		status;
	char				buf[MSG_LEN];
	struct _net_raw_reg		reg;
	struct _net_raw_reg_reply	reg_reply;
	struct _net_raw_dreg		dreg;
	struct _net_raw_dreg_reply	dreg_reply;
};

/********************************************************** GLOBAL VARS ***/

union _drvr_msg				 msg;
union _drvr_msg				 rmsg;
pid_t						 mid;
int							 handle;
short unsigned int			 p2nq_seg;
int							 net_mid;
int							 raw_seg;
int							 buf_in_use;
short unsigned int			 lan_id;
short unsigned int			 pnid1;
short unsigned int			 pnid2;
short unsigned int			 pnid3;
short unsigned int			 net_my_cs;
short unsigned int			 net_my_ds;
void					far *net_work_q;
void					far *net_free_q;
struct _net_q				 txd_q;
int							 alloc_rx_len;
int count = 0;
int bufrdy = 0;

/********************************************************** ALLOC_BUF *****/
/*
 *	This function is far called by the network manager when a raw packet
 *	of our type has arrived.  We allocate a buffer in our space & return
 *	a far pointer to it so Net can put the received packet in it
 */
void far *		far alloc_buf( length )
int length;
{
	/* save to print out later */
	alloc_rx_len = length;	
count++;
	return(MK_FP(net_my_ds,&rmsg));

} /* alloc_buf */


/********************************************************** BUF_READY *****/

int		far buf_ready( p_lan_id, p_pnid1, p_pnid2, p_pnid3 ) 
short unsigned int p_lan_id;
short unsigned int p_pnid1;
short unsigned int p_pnid2;
short unsigned int p_pnid3;
{
	/* save info about packet to globals */
	lan_id = p_lan_id;
	pnid1  = p_pnid1;
	pnid2  = p_pnid2;
	pnid3  = p_pnid3;
bufrdy++;
	/* always have net kick our proxy */
	return(mid);

} /* buf_ready */


/********************************************************** TX_BUF_DONE ***/

int		far tx_buf_done ( void )
{
	/* always have net kick our proxy */
	return(mid);

} /* tx_buf_done */


/********************************************************** NET_PID *******/

pid_t net_pid()
{
	/*
	 *	locate network manager process, return his pid.  A return
	 *	value of -1 means can't we find him.
	 */
	return(qnx_name_locate(getnid(),"qnx/net",0,NULL));

} /* net_pid */


/******************************************************* NET_DREG *******/				
/*
 *	This is the shutdown routine called when a signal is set on
 *	process.
 */

void net_dreg ( qnx_tok )
{
#if 0
	qnx_display_msg("Raw App: Terminating from signal 0x");
	qnx_display_hex(sig);
	qnx_display_msg("   ");
#endif
	/* tell net we're history */
	msg.dreg.type		= _NET_RAW_DREG;
	msg.dreg.handle		= qnx_tok->handle;
	Send(net_pid(), &msg, &msg, sizeof(msg.dreg), sizeof(msg.dreg_reply));
	
	/* bye bye */
	exit(0);

} /* net_dreg */


/********************************************************** NET_REG *******/

int net_reg( int net_id )
{
	int		I;
	pid_t	net;

	/*
	 *	if register fails, print error msg
	 */

	/* locate network manager */
	if ((net=net_pid()) == -1) {
		printf("Raw: could not locate net\n");
		return(-1);
	}

	/* allow network manager to alias our segments */
	if (qnx_segment_arm(net, -1, 0) == -1) {
		printf("Raw: seg_arm failed\n");
		return(-1);
	}

	/* construct register message to send to net */
	memset(&msg.reg, 0, sizeof(msg.reg));
	msg.reg.type		= _NET_RAW_REG; 
	msg.reg.subtype		= 1; 
	msg.reg.rpt_len      	= -1;
	msg.reg.rpt[0]       	= 0x0c;
	msg.reg.rpt[1]       	= 0x0c;
	msg.reg.rx_lan_addr    	= net_id;
	msg.reg.rx_alloc_fn_seg	= my_cs();
	msg.reg.rx_alloc_fn_ofs	= FP_OFF(&alloc_buf);
	msg.reg.rx_done_fn_seg	= my_cs();
	msg.reg.rx_done_fn_ofs	= FP_OFF(&buf_ready);
	msg.reg.tx_done_fn_seg	= my_cs();
	msg.reg.tx_done_fn_ofs 	= FP_OFF(&tx_buf_done);
	msg.reg.ds		= my_ds();
	msg.reg.txd_q_seg	= my_ds();
	msg.reg.txd_q_ofs	= FP_OFF(&txd_q);

	if (Send(net,&msg.reg,&msg.reg_reply,
	  sizeof(msg.reg),sizeof(msg.reg_reply)) == -1) {
		printf("Raw: send to net failed\n");
		return(-1);
	}

	if (msg.reg_reply.status != EOK) {
		printf("Raw: bad status on reg from net = %d\n",
		  msg.reg_reply.status);
		return(-1);
	}
	handle   = msg.reg_reply.handle;
	net_my_cs = msg.reg_reply.cs;
	net_my_ds = msg.reg_reply.ds;
	p2nq_seg = msg.reg_reply.net_qseg;
	net_mid  = msg.reg_reply.net_mid;		
	net_free_q = MK_FP(p2nq_seg,msg.reg_reply.free_q);
	net_work_q = MK_FP(p2nq_seg,msg.reg_reply.work_q);

	rmsg = msg;

	printf("Raw: reg reply: handle = %d, cs = %X, ds = %X\n",
	  handle, msg.reg_reply.cs, msg.reg_reply.ds);

	printf("Raw: qseg = %X, fr_q = %X, wr_q = %X, net_p = %d\n",
	  p2nq_seg, FP_OFF(net_free_q), FP_OFF(net_work_q), net_mid);

	/* set up signal handler for shutting down */
	for(I=1; I<=_SIGMAX; ++I){
		signal(I, &net_dreg);
	}

	return(0);

} /* net_reg */

int
net_find(void)
{
 	int num_d,size;
	struct _net_drvr_info net_send;
	struct _net_drvr_info_reply net_reply,*hardware_reply;
	pid_t pid;
	
	pid=qnx_name_locate(getnid(),"qnx/net",0,NULL);

	memset(&net_send,0,sizeof(net_send));

	net_send.type=_NET_DRVR_INFO;

	Send(pid,&net_send,&net_reply,sizeof(net_send),sizeof(net_reply));

	size=(sizeof(net_reply))+(net_reply.num_drvrs*sizeof(struct _net_drvr_info_struct));
	hardware_reply=malloc(size); 

	Send(pid,&net_send,hardware_reply,sizeof(net_send),size);

 	if (hardware_reply->status==EOK)
		for (num_d=0;num_d<hardware_reply->num_drvrs;num_d++){
			switch (hardware_reply->di[num_d].hw_type){
				case _HWT_ARCNET:
					printf("This is an arcnet card\n");
					break;
				case _HWT_ETHERNET:
					printf("This is an ethernet card\n");
					printf("Subtype: 0x%x\n",
					    hardware_reply->di[num_d].hw_subtype);
					printf("Lan addr: 0x%x\n",
					    hardware_reply->di[num_d].lan_addr);
					printf("pid: 0x%x\n",
					    hardware_reply->di[num_d].pid);
					break;
				case _HWT_TOKEN_RING:
					printf("This is a token ring card\n");
					printf("Subtype: 0x%x\n",
					    hardware_reply->di[num_d].hw_subtype);
					printf("Lan addr: 0x%x\n",
					    hardware_reply->di[num_d].lan_addr);
					printf("pid: 0x%x\n",
					    hardware_reply->di[num_d].pid);
					return(hardware_reply->di[num_d].lan_addr);
				default:
					printf("Unknown type (0x%x) or not present\n", hardware_reply->di[num_d].hw_type);
					break;
				}
			}
	else
		printf("Send to net failed\n");
	return(-1);
}		

/********************************************************** MAIN **********/

STATIC  int tok_attach_lpa(queue_t *q, unsigned long id)
{
	struct qnx_tok      *minor_ptr = q->q_ptr;

	minor_ptr->dlstate = DL_ATTACH_PENDING;

	if ( (net_id = net_find()) == -1 )
	{
	    q->q_ptr->dlstate = DL_UNATTACHED ;
	    return(1);
	}

	if (net_reg(id) == -1)
	{
	    q->q_ptr->dlstate = DL_UNATTACHED ;
	    return(1);
	}

	q->q_ptr->dlstate = DL_ATTACHED ;
	q->q_ptr->prot    = id ;

	return(0);

}

{
	char			  far	*p;
	struct _mxfer_entry32_16  far	*mx_ptr;
	struct _net_nq_pkt 	  far	*qpkt;
	int				 cur_qlen;
	int				 net_id;
	int				 num_qpkts;
	int				 i;

	if ( (net_id = net_find()) == -1 )
	{
		exit(1);
	}

	if (net_reg(net_id) == -1) {
		exit(1);
	}

qnx_tok_bind_it()
{

	if ((mid = qnx_proxy_attach(0,0,0,-1)) == -1) {
		printf("Raw: can't get a proxy\n");
		net_dreg(SIGINT);
	}

	if ((raw_seg = qnx_segment_alloc_flags(2048L, _PMF_GLOBAL)) == -1) {
		printf("Raw: unable to get segment in GDT\n");
		net_dreg(SIGINT);
	}
}
	printf("Raw: gdt segment = %X\n",raw_seg);

	/* load up segment in gdt */
	p = MK_FP(raw_seg, sizeof(*mx_ptr));

	mx_ptr = MK_FP(raw_seg,0);
	_setmx32_16(mx_ptr, p, 60);

	*p++ = 0x0c;	/* set type */
	*p++ = 0x0c;

	*p++ = 0xf3;	/* set data */
#if 0
	*p++ = 0x0c;
	*p++ = 0x03;
	*p++ = 0x04;
	*p++ = 0x05;
	*p++ = 0x06;
	*p++ = 0x07;
	*p++ = 0x08;
#endif


	for(num_qpkts = 0;;) {

		/* allocate a q pkt */
		if ((qpkt = q_get_first(net_free_q,IRQ_OFF,p2nq_seg)) == 0) {
			printf("Raw: unable to alloc qpkt\n");
			net_dreg(SIGINT);
		}
		num_qpkts++;

		/* stuff the q pkt */
		_fmemset(qpkt, 0, sizeof (*qpkt));  /* this is important */
		qpkt->type		= _VC_RAW;
		qpkt->dst_vid		= net_id; /* lan_addr */
		qpkt->mx_seg		= raw_seg;
		qpkt->mx_off		= 0;
		qpkt->mx_parts		= 1;
#if 0
qpkt->vid_seq = 1;	/* we will supply all the frames */
#endif
		qpkt->src_vid			= handle;
		qpkt->phys_dst_nid[0]	= 0x08;
		qpkt->phys_dst_nid[1]	= 0x00;
		qpkt->phys_dst_nid[2]	= 0x5a;
		qpkt->phys_dst_nid[3]	= 0x68;
		qpkt->phys_dst_nid[4]	= 0x34;
		qpkt->phys_dst_nid[5]	= 0xf7;

		/* add q pkt to end of net's work queue - will never fail */
		cur_qlen = q_put_last(net_work_q,qpkt,IRQ_OFF,p2nq_seg);

		/* return net's msgr pid if was queue empty before */
		if (cur_qlen == 1) {
			Trigger(net_mid);
		}

		sleep(2);

		if (num_qpkts >= 2) {
			Receive(0,0,0);	
		}

		if (qpkt = q_get_first(&txd_q,IRQ_OFF,p2nq_seg)) {
			if (qpkt->type == EOK) {
				printf("Raw: txd ok\n");
			} else if (qpkt->type == EIO) {
				printf("Raw: tx error\n");
			} else {
				printf("Raw: unknown tx return code\n");
			}

			/* MUST FREE!! */
			q_put_first(net_free_q,qpkt,IRQ_OFF,p2nq_seg);	

			num_qpkts--;
        	}
printf("count: %d\n", count);
printf("bufrdy: %d\n", bufrdy);
printf("pnid1 = 0x%x\n", pnid1);
printf("pnid2 = 0x%x\n", pnid2);
printf("pnid3 = 0x%x\n", pnid3);
printf("alloc_rx_len  = 0x%x\n", alloc_rx_len);
for ( i = 0; i < alloc_rx_len; i++)
  {
    if ( (i % 30 ) == 0 )
	printf("\n");
    printf("%02x ", rmsg.buf[i]);
  }
printf("\n");
	} /* for */

	/* won't ever reach here, but if we did, we'd dregister from net */
	/* before we exit */
	net_dreg(SIGINT);

} /* main */


