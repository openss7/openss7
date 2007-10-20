/* sector5.h - VTPM: FSM sector 5 definitions */

/* 
 * $Header: /xtel/isode/isode/vt/RCS/sector5.h,v 9.0 1992/06/16 12:41:08 isode Rel $
 *
 *
 * $Log: sector5.h,v $
 * Revision 9.0  1992/06/16  12:41:08  isode
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


typedef struct expl_ptr {
#define NULLCOORD  -1
	int xval;	/* if they don't exist = NULLCOORD */
	int yval;
	int zval;
} EXPL_PTR;

#define NOBKTOK	   -1  		/* for token in S mode */

typedef struct bkq_content {
	int token_val; /* 0 initiator, 1 acceptor, 2 accChoice or nobktok */
	EXPL_PTR ExplPtr;
} BKQ_content;


typedef struct bkr_content {
	int token_val; /* 0 initiator, 1 acceptor or nobktok */
	EXPL_PTR ExplPtr;
} BKR_content;

typedef struct br_cnt {
	BKQ_content	BKQcont;
	BKR_content	BKRcont;
	EXPL_PTR 	ExPtr;
} BRcnt;

