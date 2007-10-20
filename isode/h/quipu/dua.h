/* dua.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/dua.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: dua.h,v $
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


#define default_common_args \
	{ \
		{       /* service controls */ \
			0, \
			SVC_PRIO_MED, \
			SVC_NOTIMELIMIT, \
			SVC_NOSIZELIMIT, \
			SVC_REFSCOPE_NONE \
		}, \
		NULLDN,      /* Common arg - requestor DN */ \
		{       /* op_progress */ \
			OP_PHASE_NOTDEFINED, \
			OP_PHASE_NOTDEFINED, \
		}, \
		CA_NO_ALIASDEREFERENCED, \
		(struct security_parms *) NULL, \
		(struct signature *) NULL, \
		(struct extension *) NULL, \
	}
