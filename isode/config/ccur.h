/* ccur.h - site configuration for Concurrent RTU 6.0 */

/* 
 * $Header: /xtel/isode/isode/config/RCS/ccur.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: ccur.h,v $
 * Revision 9.0  1992/06/16  12:08:13  isode
 * Release 8.0
 *
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


#ifndef	_CONFIG_
#define	_CONFIG_

#define	DEBUG	2
#define	SYS5
#define	WRITEV
#define	VSPRINTF		/*   varargs */

#define	TCP			/* has TCP/IP (of course) */
#define	TLI_TCP			/*   provided by TLI */
#define	SOCKETS			/*   sockets also available */
#define	BIND

#define X25			/* has X.25 */
#define CCUR_X25		/* provided by RTnet-X25/PLUS R01 */

#define TP4			/* has Transport Class 4 */
#define TLI_TP			/* provided by the TLI */
#define RTnet_R02		/* using RTnet-OSI R02 or later */
#define TSDU_COPY_LIMIT 10240

/* #define TLI_POLL		/* use poll(2) instead of select(2)
				   WARNING: only use this if all
				   network stacks are provided via
				   streams.  With select(2) you can only
				   have 32 file descriptors. Poll(2) will
				   handle as many as are congigured in
				   the kernal but *only* works over
				   streams devices.  To get more than 32
				   descriptors in ISODE you will need to
				   modify the size of fdset in
				   h/manifest.h.
				*/

#define NOGOSIP

#define NFS

#endif /* _CONFIG_ */
