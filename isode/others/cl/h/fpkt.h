/* fpkt.h - include file for FTAM provider (FS-PROVIDER) */

/* 
 * $Header: /f/iso/h/RCS/fpkt.h,v 5.0 88/07/21 14:38:50 mrose Rel $
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


#ifndef	_FTAM_
#include "ftam.h"		/* definitions for FS-USERs */
#endif
#ifndef	_AcSAP_
#include "acsap.h"		/* definitions for AcS-USERs */
#endif

/*  */

#define	FS_GEN(fsb) \
	((fsb -> fsb_flags & FSB_INIT) ? FS_GEN_INITIATOR : FS_GEN_RESPONDER)

#define	ftamPsig(fsb, sd) \
{ \
    if ((fsb = findfsblk (sd)) == NULL) { \
	(void) sigsetmask (smask); \
	return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "invalid ftam descriptor"); \
    } \
    if (!(fsb -> fsb_flags & FSB_CONN)) { \
	(void) sigsetmask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor not connected"); \
    } \
    if (fsb -> fsb_flags & FSB_FINN) { \
	(void) sigsetmask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor finishing"); \
    } \
}

#define	ftamFsig(fsb, sd) \
{ \
    if ((fsb = findfsblk (sd)) == NULL) { \
	(void) sigsetmask (smask); \
	return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "invalid ftam descriptor"); \
    } \
    if (!(fsb -> fsb_flags & FSB_CONN)) { \
	(void) sigsetmask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor not connected"); \
    } \
    if (!(fsb -> fsb_flags & FSB_FINN)) { \
	(void) sigsetmask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor not finishing"); \
    } \
}

#define toomuchP(b,n,m,p) \
{ \
    if (b == NULL) \
	n = 0; \
    else \
	if (n > m) \
	    return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "too many %ss", p); \
}

#define	missingP(p) \
{ \
    if (p == NULL) \
	return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}

#ifndef	lint
#ifndef	__STDC__
#define	copyFTAMdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/* */_cc = min (i, sizeof d -> d/* */_data)) > 0) \
	bcopy (base, d -> d/* */_data, d -> d/* */_cc); \
}
#else
#define	copyFTAMdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyFTAMdata(base,len,d)	bcopy (base, (char *) d, len)
#endif


#define	pyinit(dtns,diags,chrgs,attrs,passwd) \
{ \
    ftam_state = FSTATE_SUCCESS, ftam_action = FACTION_SUCCESS; \
    ftam_manage = CTXMGMT_DEFAULT; \
    ftam_level = LEVEL_DEFAULT, ftam_class = CLASS_DEFAULT; \
    ftam_units = ftam_attrs = 0; \
    ftam_rollback = ROLLBACK_DEFAULT; \
    ftam_identity = ftam_account = ftam_password = NULL, ftam_passlen = 0; \
    ftam_ndtn = 0, ftam_dtns = dtns; \
    ftam_ndiag = 0, ftam_diags = diags; \
    if (ftam_chrgs = chrgs) \
	ftam_chrgs -> fc_ncharge = 0; \
    ftam_threshold = 0; \
    if (ftam_fa = attrs) \
	bzero ((char *) ftam_fa, sizeof *ftam_fa); \
    ftam_acrequest = 0; \
    ftam_override = CREATE_DEFAULT; \
    ftam_attrnames = 0; \
    if (ftam_fp = passwd) \
	bzero ((char *) ftam_fp, sizeof *ftam_fp); \
    ftam_mode = 0; \
    FCINIT (ftam_fc); \
    ftam_faduop = FA_OPS_READ; \
    ftam_accontext = 0; \
    fadu_nodename = NULL; \
}

#define	pyfree() \
{ \
    register int    PYI; \
    register struct fc_charge  *PYC; \
 \
    if (ftam_identity) \
	free (ftam_identity), ftam_identity = NULL; \
    if (ftam_account) \
	free (ftam_account), ftam_account = NULL; \
    if (ftam_password) \
	free (ftam_password), ftam_password = NULL, ftam_passlen = 0; \
    if (ftam_fp) { \
	FPFREE (ftam_fp); \
	ftam_fp = NULL; \
    } \
 \
    if (ftam_dtns) { \
	for (PYI = ftam_ndtn - 1; PYI >= 0; PYI--) \
	    if (ftam_dtns[PYI]) \
		oid_free (ftam_dtns[PYI]), ftam_dtns[PYI] = NULLOID; \
	ftam_ndtn = 0; \
	ftam_dtns = NULL; \
    } \
 \
    if (ftam_chrgs) { \
	for (PYC = ftam_chrgs -> fc_charges, PYI = ftam_chrgs -> fc_ncharge - 1; \
		PYI >= 0; \
		PYC++, PYI--) { \
	    if (PYC -> fc_resource) \
		free (PYC -> fc_resource), PYC -> fc_resource = NULL; \
	    if (PYC -> fc_unit) \
		free (PYC -> fc_unit), PYC -> fc_unit = NULL; \
	} \
	ftam_chrgs -> fc_ncharge = 0; \
	ftam_chrgs = NULL; \
    } \
    if (ftam_fa) \
	FAFREE (ftam_fa); \
    if (ftam_faduid) \
	FUFREE (ftam_faduid); \
    if (fadu_nodename) {\
	free (fadu_nodename); \
	fadu_nodename = NULL; \
    } \
}

#define	pylose() \
	fpktlose (fsb, fti, FS_PRO_ERR, NULLCP, "%s", PY_pepy)


int	ftamlose (), fpktlose (), ftamoops ();

/*  */

struct ftamblk {
    struct ftamblk *fsb_forw;	/* doubly-linked list */
    struct ftamblk *fsb_back;	/*   .. */

    int	    fsb_fd;		/* association descriptor */

    short   fsb_flags;		/* our state */
#define	FSB_NULL	0x0000
#define	FSB_CONN	0x0001	/* connected */
#define	FSB_FINN	0x0002	/* other side wants to finish */
#define	FSB_INIT	0x0004	/* this side initiated the association */
#define	FSB_ASYN	0x0008	/* asynchronous */
#define	FSB_ROLLBACK	0x0010	/* rollback available */
#define	FSB_DECHARGE	0x0020	/* responder can using charging on deselect */
#define	FSB_CANCEL	0x0040	/* this side started F-CANCEL */
#define	FSB_COLLIDE	0x0080	/* collision */

    short   fsb_state;		/* more state */
#define	FSB_INITIALIZED	0	/*   initialized */
#define	FSB_MANAGEMENT	1	/*   management */
#define	FSB_BULKBEGIN	2	/*   bulk data begin */
#define	FSB_BULKEND	3	/*   bulk data end */
#define	FSB_DATAIDLE	4	/*   data transfer idle */
#define	FSB_LOCATE	5	/*   locate in progress */
#define	FSB_ERASE	6	/*   erase in progress */
#define	FSB_DATAREAD	7	/*   data transfer read */
#define	FSB_DATAWRITE	8	/*   data transfer write */
#define	FSB_DATAFIN1	9	/*   data transfer done */
#define	FSB_DATAFIN2	10	/*     .. */
#define	FSB_DATACANCEL	11	/*   cancel in progress */
    
    int	    fsb_group;		/* group flags */

    int	    fsb_srequirements;	/* session requirements */
    int	    fsb_owned;		/* session tokens we own */
    int	    fsb_avail;		/* session tokens available */
    int	    fsb_settings;	/* initial and resync settings */
    long    fsb_ssn;		/* serial number */
    struct SSAPref fsb_connect;	/* session connection reference */
    int	    fsb_ssdusize;	/* largest atomic SSDU */

    int	    fsb_id;		/* FTAM context id */
    int	    fsb_prequirements;	/* presentation requirements */
    struct PSAPctxlist fsb_contexts;/* presentation contexts */
    struct FTAMcontentlist fsb_contents; /* FTAM document types */

    OID	    fsb_context;	/* application context */

    int	    fsb_level;		/* service-level */
    int	    fsb_class;		/* service-class */
    int	    fsb_units;		/* functional-units */
				/* mandatory functional-units */
#define	FUNITS_TRANSFER	(FUNIT_GROUPING)
#define	FUNITS_ACCESS	(FUNIT_READ | FUNIT_WRITE | FUNIT_ACCESS)
#define	FUNITS_MANAGE	(FUNIT_LIMITED | FUNIT_ENHANCED | FUNIT_GROUPING)
#define	FUNITS_TM	(FUNIT_GROUPING)
#define	FUNITS_UNCONS	(0)

    int	    fsb_attrs;		/* attribute-groups */

    IFP	    fsb_indication;	/* event handler */
    
    int	    fsb_cancelaction;	/* handle CANCEL collisions */
    struct FTAMdiagnostic *fsb_canceldiags;
    int	    fsb_cancelndiag;
    
    IFP	    fsb_trace;		/* user-defined tracing function */
};
#define	NULLFSB		((struct ftamblk *) 0)

int	freefsblk ();
struct ftamblk *newfsblk (), *findfsblk ();

#ifndef	lint
#define	fsbtrace(fsb,a)	if ((fsb) -> fsb_trace) (*((fsb) -> fsb_trace)) a
#else
#define	fsbtrace(fsb,a)	FTraceHook a
#endif

/*  */

struct pair {
    int	    p_mask;
    int	    p_bitno;
};

extern struct pair funit_pairs[],
		   fattr_pairs[],
		   faction_pairs[];

/*  */

					/* Application wide types */
#define	FTAM_AET	0	/* Application-Entity-Title */
#define	FTAM_STATE	1	/* State-Result */
#define	FTAM_ACTION	2	/* Action-Result */
#define	FTAM_DIAG	3	/* Diagnostic */
#define	FTAM_USERID	4	/* User-Identity */
#define	FTAM_ACCOUNT	5	/* Account */
#define	FTAM_PASSWORD	6	/* Password */
#define	FTAM_DTN	7	/* Document-Type-Name */
#define	FTAM_CSN	8	/* Constraint-Set-Name */
#define	FTAM_ASN	9	/* Abstract-Syntax-Name */
#define	FTAM_CHARGING	10	/* Charging */
#define	FTAM_ATTRIBUTES	11	/* Attributes */
#define	FTAM_CTA	12	/* Contents-Type-Attribute */
#define	FTAM_ACREQUEST	13	/* Access-Request */
#define	FTAM_ACPASSWORD	14	/* Access-Passwords */
#define	FTAM_C1CONTROL	15	/* Concurrency-Control */
#define	FTAM_LOCK	16	/* Lock */
#define	FTAM_ACTID	17	/* Activity-Identifier */
#define	FTAM_ACCONTEXT	18	/* Access-Context */
#define	FTAM_FADUID	19	/* FADU-Identity */
#define	FTAM_C2CONTROL	20	/* Commitmenet-Control */


					/* FTAM Regime PDUs */
#define	F_INITIALIZE_REQ	0	/* F-INITIALIZE-Request */
#define	F_INITIALIZE_RSP	1	/* F-INITIALIZE-Response */
#define	  STATE_DEFAULT		FSTATE_SUCCESS
#define	  ACTION_DEFAULT	FACTION_SUCCESS
#define	FINIT_VERSION		0	/* protocol-version */
#define	FINIT_CTXMGMT		1	/* presentation-context-management */
#define	  CTXMGMT_DEFAULT	0
#define	FINIT_LEVEL		2	/* service-level */
#define	  LEVEL_DEFAULT		FLEVEL_RELIABLE
#define	FINIT_CLASS		3	/* service-class */
#define	  CLASS_DEFAULT		FCLASS_TRANSFER
#define	FINIT_UNITS		4	/* functional-units */
#define	FINIT_ATTRS		5	/* attribute-groups */
#define	FINIT_ROLLBACK		6	/* rollback-availability */
#define	  ROLLBACK_DEFAULT	0
#define	FINIT_CONTENTS		7	/* contents-type-list */
#define	  CONTENTS_DOCUMENTS	0	/*   document-types */
#define	  CONTENTS_CONSTRAINTS	0	/*   contraint-sets-and-abstract-syntaxes */

#define	F_TERMINATE_REQ		2
#define	F_TERMINATE_RSP		3
#define	F_U_ABORT_REQ		4
#define	F_P_ABORT_REQ		5

				/* File PDUs */
#define	F_SELECT_REQ		6
#define	F_SELECT_RSP		7
#define	F_DESELECT_REQ		8
#define	F_DESELECT_RSP		9
#define	F_CREATE_REQ		10
#define	F_CREATE_RSP		11
#define	  CREATE_DEFAULT	FOVER_FAIL
#define	F_DELETE_REQ		12
#define	F_DELETE_RSP		13
#define	F_READ_ATTRIB_REQ	14
#define	F_READ_ATTRIB_RSP	15
#define	F_CHANGE_ATTRIB_REQ	16
#define	F_CHANGE_ATTRIB_RSP	17
#define	F_OPEN_REQ		18
#define	F_OPEN_RSP		19
#define	F_CLOSE_REQ		20
#define	F_CLOSE_RSP		21
#define	F_BEGIN_GROUP_REQ	22
#define	F_BEGIN_GROUP_RSP	23
#define	F_END_GROUP_REQ		24
#define	F_END_GROUP_RSP		25
#define	F_RECOVER_REQ		26
#define	F_RECOVER_RSP		27
#define	F_LOCATE_REQ		28
#define	F_LOCATE_RSP		29
#define	F_ERASE_REQ		30
#define	F_ERASE_RSP		31
#define	F_READ_REQ		32
#define	F_WRITE_REQ		33
#define	F_DATA_END_REQ		34
#define	F_TRANSFER_END_REQ	35
#define	F_TRANSFER_END_RSP	36
#define	F_CANCEL_REQ		37
#define	F_CANCEL_RSP		38
#define	F_RESTART_REQ		39
#define	F_RESTART_RSP		40


extern int  ftam_state;
extern int  ftam_action;
extern int  ftam_manage;
extern int  ftam_level;
extern int  ftam_class;
extern int  ftam_units;
extern int  ftam_attrs;
extern int  ftam_rollback;
extern char *ftam_identity;
extern char *ftam_account;
extern char *ftam_password;
extern int  ftam_passlen;

extern struct FTAMpasswords *ftam_fp;

extern struct FTAMconcurrency *ftam_fc;


extern int  ftam_threshold;

extern struct FTAMattributes *ftam_fa;

extern int ftam_acrequest;
extern int ftam_override;
extern int ftam_attrnames;

extern int  ftam_mode;

extern int  ftam_ndtn;
extern OID  *ftam_dtns;

extern int  ftam_ndiag;
extern struct FTAMdiagnostic *ftam_diags;

extern struct FTAMcharging *ftam_chrgs;

extern int ftam_accontext;
extern int ftam_faduop;

extern struct FADUidentity *ftam_faduid;

extern char *fadu_nodename;

/*  */

int	attr2pe (), conctl2pe (), diag2pe (), faduid2pe ();

int	acs2ftamlose (), acs2ftamabort ();
int	ps2ftamlose ();
