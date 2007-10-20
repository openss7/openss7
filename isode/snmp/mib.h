/* mib.h - MIB realization */

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/mib.h,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * $Log: mib.h,v $
 * Revision 9.0  1992/06/16  12:38:11  isode
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


#include <nlist.h>
#ifdef	__NeXT__
#define	n_name	n_un.n_name
#endif
#include "SNMP-types.h"
#include "objects.h"
#include "logger.h"


#define	generr(offset)	((offset) == type_SNMP_PDUs_get__next__request \
				    ? NOTOK : int_SNMP_error__status_genErr)

/*  */

#define	sysDescr	"4BSD/ISODE SNMP"
#define	sysObjectID	"fourBSD-isode.5"

/*  */

extern struct nlist nl[];
#define	N_ARPTAB	0
#define	N_ARPTAB_SIZE	1
#define	N_ICMPSTAT	2
#define	N_IFNET		3
#define	N_IPFORWARDING	4
#define	N_IPSTAT	5
#define	N_RTHASHSIZE	6
#define	N_RTHOST	7
#define	N_RTNET		8
#define	N_TCB		9
#define	N_TCPSTAT	10
#define	N_UDB		11
#define	N_UDPSTAT	12
#define	N_RTSTAT	13
#define	N_MBSTAT	14
#ifdef	BSD44
#define	N_RADIX_NODE_HEAD 15
#define	N_ISO_SYSTYPE	16
#define	N_CLNP_STAT	17
#define	N_ESIS_STAT	18
#endif


int	init_mib (), fin_mib (), set_variable ();
int	getkmem (), setkmem (), chekmem ();

/*  */

extern  int	nd;
extern	int	quantum;

extern	struct timeval
    		my_boottime;

extern	OID	nullSpecific;


void	adios (), advise ();
