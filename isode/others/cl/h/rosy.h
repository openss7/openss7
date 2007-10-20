/* rosy.h - include file for ROSY users */

/* 
 * $Header: /f/iso/h/RCS/rosy.h,v 5.0 88/07/21 14:39:25 mrose Rel $
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


#ifndef	_ROSY_
#define	_ROSY_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#include "rosap.h"		/* definitions for RoS-USERs */

/*  */

struct RyOperation {
    char   *ryo_name;		/* operation name */
    int	    ryo_op;		/* operation code */

    IFP	    ryo_arg_encode;	/* encodes argument */
    IFP	    ryo_arg_decode;	/* decodes   .. */
    IFP	    ryo_arg_free;	/* frees     .. */

    int	    ryo_result;		/* result possible */
    IFP	    ryo_res_encode;	/* encodes result */
    IFP	    ryo_res_decode;	/* decodes   .. */
    IFP	    ryo_res_free;	/* frees     .. */

    struct RyError **ryo_errors;/* errors possible */
};

struct RyOperation *findopbyop (), *findopbyname ();


struct RyError {
    char   *rye_name;		/* error name */
    int	    rye_err;		/* error code */

    IFP	    rye_param_encode;	/* encodes parameter */
    IFP	    rye_param_decode;	/* decodes   .. */
    IFP	    rye_param_free;	/* frees     .. */
};

struct RyError *finderrbyerr (), *finderrbyname ();

/*  */

struct opsblk {
    struct opsblk *opb_forw;	/* doubly-linked list */
    struct opsblk *opb_back;	/*   .. */

    short	opb_flags;	/* our state */
#define	OPB_NULL	0x0000
#define	OPB_EVENT	0x0001	/* event present */
#define	OPB_INITIATOR	0x0002	/* initiator block */
#define	OPB_RESPONDER	0x0004	/* responder block */

    int	    opb_fd;		/* association descriptor */
    int	    opb_id;		/* invoke ID */

    IFP	    opb_resfnx;		/* result vector */
    IFP	    opb_errfnx;		/* error vector */

    struct RyOperation *opb_ryo;/* entry in operation table */

    struct RoSAPindication opb_event;	/* resulting event */
    caddr_t opb_out;			/* event parameter */
    IFP	    opb_free;			/* free routine for event parameter */

    PE	    opb_pe;		/* for Simon */
};
#define	NULLOPB		((struct opsblk *) 0)


int	freeopblk (), loseopblk ();
struct opsblk *newopblk (), *findopblk (), *firstopblk ();


struct dspblk {
    struct dspblk *dsb_forw;	/* doubly-linked list */
    struct dspblk *dsb_back;	/*   .. */

    int	    dsb_fd;		/* association descriptor */
				/* NOTOK-valued is wildcard for RyWait */

    struct RyOperation *dsb_ryo;/* entry in operation table */
    
    IFP	    dsb_vector;		/* dispatch vector */
};
#define	NULLDSB		((struct dspblk *) 0)


int	freedsblk (), losedsblk ();
struct dspblk *newdsblk (), *finddsblk ();


#define	RY_RESULT	(-1)	/* distinguishes RESULTs from ERRORs */
#define	RY_REJECT	(-2)	/* distinguishes REJECTs from ERRORs */

/*  */

int	RyWait ();		/* WAIT */

				/* Initiator */
int	RyStub ();		/* stub */
#define	ROS_INTR	2	/*   invoke stub but return on interrupt */
int	RyOperation ();		/* OPERATION */
int	RyOpInvoke ();		/* INVOKE */
int	RyGenID ();		/* generate unique invoke ID */

				/* Responder */
int	RyDispatch ();		/* DISPATCH */
int	RyDsResult ();		/* RESULT */
int	RyDsError ();		/* ERROR */
int	RyDsUReject ();		/* U-REJECT */

int	RyLose ();		/* clean-up after association termination */
#endif
