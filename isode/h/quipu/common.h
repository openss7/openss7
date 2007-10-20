/* common.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/common.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: common.h,v $
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


#ifndef COMH
#define COMH

#include "quipu/attrvalue.h"
#include "quipu/dsp.h"
#include "quipu/commonarg.h"
#include "quipu/bind.h"
#include "quipu/read.h"
#include "quipu/compare.h"
#include "quipu/abandon.h"
#include "quipu/list.h"
#include "quipu/ds_search.h"
#include "quipu/add.h"
#include "quipu/modify.h"
#include "quipu/modifyrdn.h"
#include "quipu/remove.h"
#include "quipu/ds_error.h"

typedef struct simple_creds
	{
	DN      usc_dn;
	int     usc_passwd_len;
	char    *usc_passwd;
	char    *usc_time1;
        char    *usc_time2;
	} USC;

#endif
