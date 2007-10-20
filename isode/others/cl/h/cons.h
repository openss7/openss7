/* cons.h - CONS abstractions */

/* 
 * $Header: /f/iso/h/RCS/cons.h,v 5.0 88/07/21 14:38:47 mrose Rel $
 *
 * Contributed by Keith Ruttle, CAMTEC Electronics Ltd
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


#ifndef	_CONS_
#define	_CONS_

/* 
 *
 * #defines for generic addressing & TSEL encoded addresses.
 *
 */

#ifndef _INTERNET_
#include <sys/socket.h>
#endif
#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/socketvar.h>
#include <sys/errno.h>
#include <net/if.h>

#ifdef CAMTEC_CCL
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netccl/ccl.h>

typedef struct ccliovec CONN_DB;

#define close_cons_socket     close
#define	select_cons_socket    selsocket
#endif

int	start_cons_client ();
int	start_cons_server ();
int	join_cons_client ();
int	join_cons_server ();
int	read_cons_socket ();
int	write_cons_socket ();
int	close_cons_socket ();
int	select_cons_socket ();

struct NSAPaddr *if2gen();
CONN_DB *gen2if();


#define	ADDR_LOCAL	0
#define	ADDR_REMOTE	1
#define	ADDR_LISTEN	2
#define SEPARATOR ':'


#define MAXNSDU	1024
#endif
