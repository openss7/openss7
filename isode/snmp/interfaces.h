/* interfaces.h - support for MIB realization of the Interfaces group */

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/interfaces.h,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * $Log: interfaces.h,v $
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


#include "internet.h"
#include <net/if.h>
#include <netinet/if_ether.h>		/* to get struct arpcom */
#include "clns.h"

/*  */

extern	int	ifNumber;

struct interface {
    int	    ifn_index;			/* 1..ifNumber */
    int	    ifn_indexmask;		/* 1 << (index - 1) */

    int	    ifn_ready;			/* has an address associated with it */

    struct arpcom ifn_interface;	/* ifnet+physaddr */
    unsigned long ifn_offset;		/* where in kmem */

    char    ifn_descr[IFNAMSIZ];	/* e.g., "lo0" */

    int	    ifn_type;			/* ifType */
    u_long  ifn_speed;			/* ifSpeed */

    int	    ifn_admin;			/* ifAdminStatus */
    int	    ifn_touched;		/*   .. new value */

    struct interface *ifn_next;
};

extern struct interface *ifs;


int	set_interface (), sort_interface ();

/*  */

union sockaddr_un {		/* 'cause sizeof (struct sockaddr_iso) == 32 */
    struct sockaddr	    sa;

    struct sockaddr_in	    un_in;
#ifdef	BSD44
    struct sockaddr_iso	    un_iso;
#endif
};


struct address {
#define	ADR_SIZE	(20 + 1 + 1)	/* object instance */
    unsigned int    adr_instance[ADR_SIZE];
    int	    adr_insize;

    union sockaddr_un adr_address;	/* address */
    union sockaddr_un adr_broadaddr;	/*   broadcast, only if AF_INET */
    union sockaddr_un adr_netmask;	/*   network mask */

    int	    adr_indexmask;		/* mask of interfaces with address */

    struct address *adr_next;
};

extern struct address *afs_inet;
#ifdef	BSD44
extern struct address *afs_iso;
#endif


struct address *find_address (), *get_addrent ();


#if	defined(BSD44) || defined(BSD43_Tahoe) || defined(RT) || defined(MIPS) || defined(ultrix) || defined(__NeXT__)
#define	NEW_AT
#else
#undef	NEW_AT
#endif
