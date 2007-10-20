/* smux-g.h - SMUX group */

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/smux-g.h,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 *
 * $Log: smux-g.h,v $
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
#include "psap.h"


struct smuxPeer {
    struct smuxPeer *pb_forw;		/* doubly-linked list */
    struct smuxPeer *pb_back;		/*   .. */

    int	    pb_index;			/* smuxPindex */

    int	    pb_fd;			
    struct sockaddr_in pb_address;
    char    pb_source[30];

    OID	    pb_identity;		/* smuxPidentity */
    char   *pb_description;		/* smuxPdescription */

    PS	    pb_ps;
    int	    pb_sendCoR;

    int	    pb_priority;		/* minimum allowed priority */

    int	    pb_newstatus;		/* for setting smuxPstatus */
    int	    pb_invalid;
};

extern	struct smuxPeer *PHead;


struct smuxTree {
    struct smuxTree *tb_forw;		/* doubly-linked list */
    struct smuxTree *tb_back;		/*   .. */

#define	TB_SIZE	30			/* object instance */
    unsigned int    tb_instance[TB_SIZE + 1];
    int	    tb_insize;

    OT	    tb_subtree;			/* smuxTsubtree */
    int	    tb_priority;		/* smuxTpriority */
    struct smuxPeer *tb_peer;		/* smuxTindex */

    struct smuxTree *tb_next;		/* linked list for ot_smux */

    int	    tb_newstatus;		/* for setting smuxPstatus */
    int	    tb_invalid;
};

extern	struct smuxTree *THead;
