/* tp4.h - TP4 abstractions */

/* 
 * $Header: /f/iso/h/RCS/tp4.h,v 5.0 88/07/21 14:39:38 mrose Rel $
 *
 *
 * $Log$
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef	_TP4_
#define	_TP4_

/*    TLI */

#if	defined (WIN)
#define	TLI
#endif

#ifdef	TLI
#include <tiuser.h>

				/* why aren't these in <tiuser.h>? */

#define	T_NULL	0x00

extern int   t_errno;

int	t_close ();


#define	TLIMAXCONN	1
#endif

/*    SunLink OSI */

#ifdef SUN_TP4
#include <sys/ieee802.h>
#ifndef	SOCK_STREAM
#include <sys/socket.h>
#endif
#include <netosi/osi.h>
#include <netosi/osi_addr.h>
#include <netosi/osi_error.h>
#include <netosi/tp_event.h>


#define	MSG_OOB         0x1	/* process out-of-band data */


struct tp4pkt {
    union {
	TP_MSG		  tp_msg;
	TP_MSG_CONNECT	  tp_connect;
	TP_MSG_DATA	  tp_data;
	TP_MSG_X_DATA	  tp_x_data;
	TP_MSG_DISCONNECT tp_disconnect;
    } tp_un;
#define tp4_event	tp_un.tp_msg.tp_event
#define tp4_called 	tp_un.tp_connect.dst_address
#define tp4_calling 	tp_un.tp_connect.src_address
#define tp4_expedited	tp_un.tp_connect.expedited_selected
#define tp4_qos		tp_un.tp_connect.tp_qos
#define tp4_eot		tp_un.tp_data.eot
#define tp4_reason	tp_un.tp_disconnect.reason
};

struct tp4pkt *newtp4pkt ();
#define	freetp4pkt(tp)	cfree ((char *) (tp))

int	gen2tp4 (), tp42gen ();
#endif

/*  */

#ifdef	TLI
#define	close_tp4_socket	t_close
#define	select_tp4_socket	selsocket
#endif

#ifdef	SUN_TP4
#define	close_tp4_socket	close
#define	select_tp4_socket	selsocket
#endif

int	close_tp4_socket ();
int	select_tp4_socket ();

#endif

