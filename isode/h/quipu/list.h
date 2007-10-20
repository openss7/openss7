/* list.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/list.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: list.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
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


#ifndef QUIPULIST
#define QUIPULIST

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_list_arg {
    CommonArgs lsa_common;
    DN lsa_object;
};

/*
.VE
.VS
*/
struct subordinate {
    RDN sub_rdn;
    char sub_aliasentry;        /* set to TRUE or FALSE                 */
    char sub_copy;              /* set to TRUE or FALSE                 */
    struct subordinate *sub_next;
};
#define NULLSUBORD (struct subordinate *) NULL
struct subordinate	* subord_comp_new();

/*
.VE
.VS
*/
struct ds_list_result {
    CommonResults lsr_common;
    DN lsr_object;
    time_t lsr_age;
    struct subordinate *lsr_subordinates;
    POQ	   lsr_poq;
#define lsr_limitproblem lsr_poq.poq_limitproblem
#define lsr_cr lsr_poq.poq_cref
    struct ds_list_result * lsr_next;
};
				/* Note uncorrelated will need to be    */
				/* added in to do the secure stuff      */
				/* in a distributed manner              */
				/* this also applies to search          */


struct list_cache {
	DN              list_dn;
	struct subordinate *list_subs;
	struct subordinate *list_sub_top;
	int             list_count;
	int             list_problem;
	struct list_cache *list_next;
};

#define NULLCACHE (struct list_cache *) NULL

struct list_cache *find_list_cache ();

#endif
