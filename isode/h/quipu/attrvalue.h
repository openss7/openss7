/* attrvalue.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/attrvalue.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 *
 * $Log: attrvalue.h,v $
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


#ifndef ATTRVALUE
#define ATTRVALUE
#include "quipu/name.h"

typedef struct avseqcomp {      /* attribute may have multiple values   */
				/* respresents SET OF AttributeValue    */
    attrVal      	avseq_av;
    struct avseqcomp    *avseq_next;
} avseqcomp, *AV_Sequence;

#define NULLAV ((AV_Sequence) 0)
#define avs_comp_alloc()	(AV_Sequence) smalloc(sizeof(avseqcomp))
#define avs_comp_print(x,y,z)	AttrV_print (x,&(y)->avseq_av,z)
#define avs_cmp_comp(x,y)	AttrV_cmp (&x->avseq_av ,&y->avseq_av)

AV_Sequence  avs_comp_new ();
AV_Sequence  avs_comp_cpy ();
AV_Sequence  avs_cpy ();
AV_Sequence  avs_merge ();
AV_Sequence  str2avs ();

typedef struct attrcomp {       /* A sequence of attributes             */
				/* represents Attribute                 */
				/* and SET OF Attribute                 */
    attrType       	attr_type;
    AV_Sequence         attr_value;
    struct attrcomp     *attr_link;
				/* ACL is NOT for use by DUA            */
				/* this must be done by use of ACL      */
				/* attribute                            */
    struct acl_info     *attr_acl;
} attrcomp, *Attr_Sequence;

#define NULLATTR ((Attr_Sequence) 0)
#define as_comp_alloc()          (Attr_Sequence) smalloc(sizeof(attrcomp))
#define as_comp_cmp(x,y)      (((oid_cmp (&x->attr_type ,&y->attr_type) == OK) && (avs_cmp (x->attr_value ,y->attr_value) == OK)) ? OK : NOTOK)

Attr_Sequence  as_comp_new ();
Attr_Sequence  as_comp_cpy ();
Attr_Sequence  as_cpy ();
Attr_Sequence  as_find_type ();
Attr_Sequence  as_merge ();
Attr_Sequence  str2as();


				/* ACL is defined here as it is         */
				/* referenced.   it is only used by     */
				/* DSA                                  */
				/* represents ACLInfo defined by INCA   */
struct acl_info {
    int acl_categories;
#define ACL_NONE        0
#define ACL_DETECT      1
#define ACL_COMPARE     2
#define ACL_READ        3
#define ACL_ADD         4
#define ACL_WRITE       5
    int  acl_selector_type;
#define ACL_ENTRY       1
#define ACL_OTHER       2
#define ACL_PREFIX      3
#define ACL_GROUP       4
    struct dn_seq       *acl_name;    /* prefix and group only         */
    struct acl_info     *acl_next;
};

#define NULLACL_INFO (struct acl_info *) NULL
#define acl_info_alloc()        (struct acl_info *) smalloc (sizeof  (struct acl_info))
#define acl_info_fill(w,x,y,z)    w -> acl_categories = x ;  \
				  w -> acl_selector_type = y; \
				  w -> acl_name = z;
struct acl_info *acl_info_new ();
struct acl_info *acl_info_cpy();
struct acl_info *acl_default();
struct acl_info *acl_dflt();

/*
 * the per-entry authentication policy
 */

typedef struct auth_policy {
	int	ap_mod;	/* mod, modrdn, add, remove */
	int	ap_rnc;	/* read, compare */
	int	ap_lns;	/* list, search */
} authpolicy, *Authpolicy;
#define ap_modification		ap_mod
#define ap_readandcompare	ap_rnc
#define ap_listandsearch	ap_lns

#define AP_TRUST	0
#define AP_SIMPLE	1
#define AP_STRONG	2

#define NULLAUTHP	((Authpolicy) 0)

#define authp_alloc()	((Authpolicy) smalloc( sizeof(authpolicy) ))

/*
 * this next bit is for defining the search and list acls.
 */

typedef struct sacl_info {
	struct acl_info	 sac_info;
#define sac_selector	 sac_info.acl_selector_type
#define sac_name	 sac_info.acl_name
	char		 *sac_tmpbstr;	/* for pepsy */
	int		 sac_tmplen;	/* for pepsy */
	char		 sac_scope;
#define SACL_SUBTREE		0x01
#define SACL_SINGLELEVEL	0x02	
#define SACL_BASEOBJECT		0x04
	int		 sac_access;
#define SACL_UNSEARCHABLE	1
#define SACL_SEARCHABLE		2
	struct oid_seq	*sac_types;
	int		sac_maxresults;
	int		sac_minkeylength;
#define SACL_NOMINLENGTH	0
	char		sac_zeroifexceeded;
} saclinfo, *Saclinfo;
#define NULLSACL	((Saclinfo) 0)

#define Listacl	Saclinfo
#define NULLLISTACL	((Listacl) 0)

struct mailbox {
	char *	mtype;
	char * 	mbox;
};

struct fax {
    char    *number;
    PE     bits;
    char *   fax_bits;
    int	     fax_len;
};

struct postaddr {
	int  addrtype;   /* 1 == T61, 2 == Printstr */
	char * addrcomp;
	struct postaddr * pa_next;
};

struct telex {
	char * telexnumber;
	char * countrycode;
	char * answerback;
};

struct teletex {
	char    *terminal;
	char    *graphic;
	char    *control;
	char    *page;
	char    *misc;
	char    *t_private;
};

struct pref_deliv {
    	int	deliv;
	struct pref_deliv * pd_next;
};


struct Guide {
	OID objectClass;
	struct Criteria * criteria;

	int	subset;	/* >= 0 iff this is an NadfGuide... */
};

struct Criteria {
    int     offset;
#define	Criteria_type	1
#define	Criteria_and	2
#define	Criteria_or	3
#define	Criteria_not	4

    union {
            struct CriteriaItem *type;
            struct and_or_set {
                    struct Criteria *and_or_comp;
                    struct and_or_set *and_or_next;
            } *and_or;
            struct Criteria *not;
    }       un;
};

struct CriteriaItem {
    int  offset;
#define	choice_equality		1
#define	choice_substrings	2
#define	choice_greaterOrEqual	3
#define	choice_lessOrEqual	4
#define	choice_approximateMatch	5
    AttributeType attrib;
};


/* Upper bounds */
#define UB_POSTAL_LINE		6
#define UB_POSTAL_STRING 	30
#define UB_TELETEX_TERMINAL_ID 	1024
#define UB_TELEPHONE_NUMBER	32
#define UB_TELEX_NUMBER		14
#define UB_ANSWERBACK		8
#define UB_COUNTRY_CODE		4

typedef struct _InheritAttr {
	Attr_Sequence 	i_default;	
	Attr_Sequence 	i_always;	
	OID		i_oid;
} * InheritAttr;
#define NULLINHERIT ((InheritAttr)NULL)

struct CIList {
	char *	l_str;
	struct CIList * l_next;
	int 	l_type;   /* 1 == T61, 2 == Printstr */
};
#define NULLCILIST (struct CIList *)NULL

struct documentStore {
    int	    ds_method;
#define	DS_UNK	(-1)
#define	DS_FTP	0
#define	DS_FTAM	1

    char   *ds_host;
    char   *ds_dir;
    char   *ds_file;
};

struct dsaQoS {
    int	dsa_quality;
#define	DSA_DEFUNCT	0
#define	DSA_EXPERIMENTAL 1
#define	DSA_BESTEFFORT	2
#define	DSA_PILOT	3
#define	DSA_FULL	4

    char   *dsa_description;
};

struct attrQoS {
    int	    attr_level;
#define	ATTR_UNKNOWN	1
#define	ATTR_EXTERNAL	2
#define	ATTR_SYSTEM	3
#define	ATTR_USER	4

    int	    attr_completeness;
/* same values as dit_namespace */
};

struct ditQoS {
    int	    dit_namespace;
#define	DIT_NONE	1
#define	DIT_SAMPLE	2
#define	DIT_SELECTED	3
#define	DIT_SUBSTANTIAL	4
#define	DIT_FULL	5

    struct attrQoS  *dit_default;

    struct attrsQoS {
	AttributeType	dit_type;
	struct attrQoS *dit_quality;
	struct attrsQoS *dit_next;
    }		   *dit_attrs;

    char   *dit_description;
};

struct dsa_control
{
	int	dsa_control_option ;
#define CONTROL_LOCKDSA 1
#define CONTROL_SETLOGLEVEL 2
#define CONTROL_REFRESH 3
#define CONTROL_STOPDSA 4
#define CONTROL_UNLOCK 5
#define CONTROL_RESYNCH 6
#define CONTROL_CHANGETAILOR 7
#define CONTROL_UPDATESLAVEEDBS 8

	union
	{
		struct	optional_dn	*lockdsa ;
		struct	qbuf		*setLogLevel ;
		struct	optional_dn	*refresh ;
		char			stopDSA ;
		struct	optional_dn	*unlock ;
		struct	optional_dn	*resynch ;
		struct	qbuf		*changeTailor ;
		struct	qbuf		*updateSlaveEDBs ;
	} un ;
} ;

struct optional_dn
{
  int offset ;
#define EMPTYDN 1
#define DN_PRESENT 2

  union
    {
      char no_dn ;
      DN selectedDN ;
    } un ;
} ;

typedef struct quipu_call
{
	int	protocol ;
#define PROTOCOL_DAP  0
#define PROTOCOL_DSP  1
#define PROTOCOL_QUIPUDSP	2
#define	PROTOCOL_INTERNET_DSP	3
	int	assoc_id ;
struct	auth_level * authorizationLevel ;
	char	initiated_by_dsa ;
	DN	usersDN ;
	char	*net_address ;
	char	*start_time ;
	char	*finish_time ;
struct	op_list	*pending_ops ;
struct	op_list	*invoked_ops ;
} quipu_call ;

struct	auth_level
{
	int	parm ;
#define AUTHLEVEL_NONE        1
#define AUTHLEVEL_IDENTIFIED  2
#define AUTHLEVEL_SIMPLE      3
#define AUTHLEVEL_PROTECTED_SIMPLE   4
#define AUTHLEVEL_STRONG      5
} ;

struct sub_ch_list
{
	int	assoc_id ;
	int	invok_id ;
} ;

struct chain_list
{
	struct	sub_ch_list	*sub_chained_ops ;
	struct	chain_list		*next ;
} ;

struct	ops
{
	int	invoke_id ;
	int	operation_id ;
	DN	base_object ;
	char	*start_time ;
	char	*finish_time ;

struct	chain_list	*chained_ops ;
} ;

struct op_list
{
	struct	ops	*operation_list ;
	struct	op_list *next ;
} ;

#endif
