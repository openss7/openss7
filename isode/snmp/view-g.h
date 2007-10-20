/* view-g.h - VIEW group */

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/view-g.h,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 *
 * $Log: view-g.h,v $
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


#include "isoaddrs.h"
#include "internet.h"
#include "psap.h"

/*    VIEWS */

#define	inSubtree(tree,object) \
    	((tree) -> oid_nelem <= (object) -> oid_nelem \
	     && bcmp ((char *) (tree) -> oid_elements, \
		      (char *) (object) -> oid_elements, \
		      (tree) -> oid_nelem \
		          * sizeof ((tree) -> oid_elements[0])) == 0)

struct subtree {
    struct subtree *s_forw;	/* doubly-linked list */
    struct subtree *s_back;	/* doubly-linked list */

    OID	    s_subtree;		/* subtree */
};


struct view {
    struct view *v_forw;	/* doubly-linked list */
    struct view *v_back;	/*   .. */

    OID	    v_name;		/* view name */
    u_long  v_mask;		/* view mask */

    struct subtree v_subtree;	/* list of subtrees */

    struct qbuf *v_community;	/* for proxy, traps... */
    struct sockaddr v_sa;

    unsigned int *v_instance;	/* object instance */
    int	    v_insize;		/*   .. */
};

extern struct view *VHead;

/*    COMMUNITIES */

struct community {
    struct community *c_forw;	/* doubly-linked list */
    struct community *c_back;	/*   .. */

    char   *c_name;		/* community name */
    struct NSAPaddr c_addr;	/* network address */

    int	    c_permission;	/* same as ot_access */
#define	OT_YYY	0x08

    OID	    c_vu;		/* associated view */
    struct view *c_view;	/*   .. */

    unsigned int *c_instance;	/* object instance */
    int	    c_insize;		/*   .. */
    struct community *c_next;	/* next in lexi-order */
};

extern struct community *CHead;

/*    TRAPS */

struct trap {
    struct trap *t_forw;	/* doubly-linked list */
    struct trap *t_back;	/*   .. */

    char   *t_name;		/* trap name */

    struct view  t_vu;		/* associated view */
    struct view *t_view;	/*   .. */

    u_long  t_generics;		/* generic traps enabled */

    unsigned int *t_instance;	/* object instance */
    int	    t_insize;		/*   .. */
};

extern struct trap *UHead;
