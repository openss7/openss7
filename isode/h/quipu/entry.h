/* entry.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/entry.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: entry.h,v $
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


#ifndef QUIPUENTRY
#define QUIPUENTRY

#include "quipu/config.h"
#include "quipu/attrvalue.h"
#include "quipu/dsp.h"
#include "quipu/turbo.h"

struct acl_attr {
    struct oid_seq   *aa_types;
    union {
	struct acl_info *aa_un_acl;
	Saclinfo	aa_un_sacl;
    } aa_un;
#define aa_acl	aa_un.aa_un_acl
#define aa_sacl	aa_un.aa_un_sacl
    struct acl_attr  *aa_next;
};

#define NULLACL_ATTR ((struct acl_attr *)0)
#define acl_attr_alloc()  (struct acl_attr *) smalloc (sizeof (struct acl_attr));

struct acl_attr * acl_attr_cpy();

struct acl {                    /* represents ACL                       */
    struct acl_info  *ac_child;
    struct acl_info  *ac_entry;
    struct acl_info  *ac_default;
    struct acl_attr  *ac_attributes;
};

#define acl_alloc()  (struct acl *) smalloc (sizeof (struct acl));
#define NULLACL (struct acl *)NULL

struct acl * acl_cpy();
struct acl * str2acl();

/* Entry is the structure which is used to hold the DIT in core         */

typedef struct entry {
				/* First components refer to the entry  */
				/* itself                               */
    RDN         e_name;

    Attr_Sequence e_attributes; /* the attributes of the entry          */
				/* ALL attributes held, including the   */
				/* special ones                         */
    InheritAttr e_iattr;	/* Attributes to inherit into the entry */
				/* MAYBE specials			*/

    char        e_leaf;         /* TRUE if entry is leaf                */
    char        e_complete;     /* TRUE if all attributes present       */

    char        e_data;         /* Info on data in entry                */
#define E_DATA_MASTER             1
#define E_TYPE_SLAVE              2
#define E_TYPE_CACHE_FROM_MASTER  3
#define E_TYPE_CONSTRUCTOR        4
#define E_TYPE_NEGATIVE		  5

    char        e_allchildrenpresent;
				/* set  FALSE if none                   */
				/* else 1 if children at onelevel	*/
				/* > 1 imples subtree			*/

				/* structures referring to special      */
				/* attributes in the entry              */

    struct acl  *e_acl;         /* The ACL for the entry                */

    AV_Sequence	e_lacl;		/* the list ACL for the entry		*/
    AV_Sequence	e_sacl;		/* the search ACL for the entry		*/

    Authpolicy	e_authp;	/* authentication policy for the entry	*/

    DN          e_alias;        /* if present, entry is alias           */

    struct dsa_info *e_dsainfo; /* only present if entry represents DSA */


				/* Second block refer to children of    */
				/* entry                                */

    char        *e_edbversion;
				/* only present for non leaf - non-cache */
				/* this refers to all the child entries  */
				/* NULL if edb is not held               */

    AV_Sequence e_oc;		/* Objectclass attribute		*/
    AV_Sequence e_inherit;	/* COPY of inherit attribute		*/

				/* Final block is the linkage of the    */
				/* structure to the tree                */
				/* across the tree                      */
    struct entry *e_parent;
    Avlnode	 *e_children;

    time_t	e_age;		/* time entry created (for cache use only) */

    char        e_lock;         /* TRUE if EDB locked -> no write to disk */

    char	e_external;	/* 0 -> Quipu, 1 -> External */
    union  {
       struct {
       AV_Sequence un_master;	/* Master DSA(s) of EDB                 */
       AV_Sequence un_slave;	/* Slave DSAs of EDB                    */
       } un_in;
       struct {
       int         un_reftype;
       AV_Sequence un_reference;
       } un_out;
       Attr_Sequence un_dua;	/* For DUAs negative cache */
       struct DSError *un_dse;
    } e_un;

    int		e_refcount;	/* How many things pointing to it */

#define e_master 	e_un.un_in.un_master
#define e_slave 	e_un.un_in.un_slave
#define e_reference 	e_un.un_out.un_reference
#define e_reftype 	e_un.un_out.un_reftype
#define e_noattrs	e_un.un_dua	/* NYI */
#define e_error		e_un.un_dse
} entry, *Entry;

#define NULLENTRY ((Entry)0)
#define entry_alloc()           (Entry) calloc (1,sizeof(entry));
Avlnode *getentry_block();
Entry directory_load();
int find_entry ();
int find_master_entry ();
int really_find_entry ();
Entry local_find_entry ();
Entry get_default_entry ();

#define isleaf(x) 	((x)->e_leaf || \
		((x)->e_children == NULLAVL && \
		(x)->e_allchildrenpresent != FALSE))

struct oid_seq {
   OID  oid_oid;
   struct oid_seq *oid_next;
};
#define NULLOIDSEQ ((struct oid_seq*) 0)

#define oid_seq_alloc()         (struct oid_seq  *) smalloc (sizeof (struct oid_seq))
struct oid_seq * oid_seq_cpy();
struct oid_seq * oid_seq_merge();

struct tree_struct {            /* represents TreeStructure             */
    objectclass        *tree_object;
};
#define NULLTREE ((struct tree_struct *)0)
#define tree_struct_alloc()     (struct tree_struct *) smalloc (sizeof (struct tree_struct))

struct tree_struct * tree_struct_cpy ();
struct tree_struct * str2schema ();

struct dn_seq {
    DN  dns_dn;
    struct dn_seq *dns_next;
};
#define NULLDNSEQ ((struct dn_seq *)  0)
#define dn_seq_alloc()          (struct dn_seq *) smalloc (sizeof (struct dn_seq))
struct dn_seq *dn_seq_cpy();
struct dn_seq *str2dnseq();

struct dsa_info {               /* represents DSA information           */
				/* UpdateInfoSyntax                     */
    struct PSAPaddr *dsa_addr;  /* type from ISODE                      */
				/* might need to allow for multiple     */
				/* addresses, but not now               */
				/* also OR Address                      */
				/* leave for now                        */

				/* List of EDBs handled                 */
				/* by this dsa                          */
    AV_Sequence	     dsa_attr;
    char *	     dsa_version;
				/* info to assess reliability of a DSA */		
    time_t	     dsa_last_success;
    time_t	     dsa_last_attempt;
    int		     dsa_failures;
};

#define NULLDSA ((struct dsa_info *)0)

#define dsa_info_alloc()   (struct dsa_info *) smalloc (sizeof (struct dsa_info));

struct edb_info {               /* represent EDBInfo                    */
    DN          edb_name;
    DN          edb_getfrom;    /* DSA I get EDBupdates from            */
    struct dn_seq  *edb_sendto;  /* where I send EDB updates to          */
    struct dn_seq  *edb_allowed;  /* Who is allowed updates */
};
#define NULLEDB ((struct edb_info *) 0)

#define edb_info_alloc()   (struct edb_info *) smalloc (sizeof (struct edb_info));
struct edb_info * edb_info_cpy ();
struct edb_info * str2update ();

DN get_copy_dn ();

struct getedb_type {
  char _gr_encoded;
  union edb_result {
        PE _gr_pe;
        Avlnode	*_gr_edb;
  } un_gr;
};

struct getedb_result {
	char * gr_version;
	struct getedb_type gr_type;
#define gr_pe gr_type.un_gr._gr_pe
#define gr_encoded gr_type._gr_encoded
#define gr_edb gr_type.un_gr._gr_edb

	struct getedb_result * gr_next;
	int    gr_nextEntryPos;
};
#define NULL_GETRESULT (struct getedb_result *) NULL

struct getedb_arg {
	DN 	ga_entry;
	char * 	ga_version;
	struct getedb_arg * get_next;
	int     ga_nextEntryPos;	
	int	ga_maxEntries;
	int	ga_type;
#define	GA_SENDIFMORERECENT 	1
#define GA_GETVERSION 		2
#define GA_GETEDB		3
#define GA_CONTINUE		4
	struct getedb_result * ga_previous;	/* previous results */
};

#define NULL_GETARG (struct getedb_arg *) NULL

	
#endif
