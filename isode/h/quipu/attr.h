/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __ISODE_QUIPU_ATTR_H__
#define __ISODE_QUIPU_ATTR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* attr.h - directory service interface definition */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/attr.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: attr.h,v
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

#ifndef QUIPUATTR
#define QUIPUATTR

#include "quipu/oid.h"
#include "manifest.h"

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

	/* FIRST SECTION DEFINES STRUCTURES FOR ADDRESSING */

typedef oid_table_attr *attrType;
typedef oid_table_attr *AttributeType;

#define NULLAttrT (AttributeType) NULL
#define AttrT_alloc()
#define AttrT_cmp(x,y)	( x == y ? 0 : ( x > y ? -1 : 1 ))
#define AttrT_decode(x)
#define AttrT_free(x)
#define AttrT_cpy(x) x
#define grab_oid(x)  (x ? x->oa_ot.ot_oid : NULLOID)

AttributeType AttrT_new();

#define str2AttrT(x) AttrT_new(x)

typedef struct {
	short av_syntax;		/* Specifies the syntax of the */
	/* attribute that follows.  */
	caddr_t av_struct;
} attrVal, *AttributeValue;

#define AV_WRITE_FILE		256
#define AV_FILE			128
#define MAX_AV_SYNTAX 		100

#define NULLAttrV (AttributeValue) NULL
#define AttrV_alloc()   (AttributeValue) smalloc (sizeof (attrVal));

AttributeValue AttrV_cpy();
AttributeValue str_at2AttrV();
AttributeValue str2AttrV();
short str2syntax();
short add_attribute_syntax();
short modify_av_printer();

struct file_syntax {
	short fs_real_syntax;
	char *fs_name;
	char fs_mode;
	char fs_ref;
	AttributeValue fs_attr;
};

#define FS_DEFAULT 0x01		/* default file name */
#define FS_CREATE  0x02		/* created thus remove file */
#define FS_TMP     0x04		/* DSA created tmp file */

#define EDBOUT  1
#define FILEOUT 2		/* for writing to files only */
#define READOUT 3
#define DIROUT  4		/* for dn and rdn print only */
#define	RDNOUT	5
#define	UFNOUT	6		/* user-friendly naming */

#define EDB_LINEWRAP	38	/* Wrap very long lines after 38 characters */

#define ps_print(ps,data) (void)ps_write(ps,(PElementData)data,strlen(data))

typedef struct {
	char *s_sntx;			/* String defining syntax */
	IFP s_encode;
	IFP s_decode;
	IFP s_parse;
	IFP s_print;
	IFP s_copy;
	IFP s_compare;
	IFP s_free;
	char *s_pe_print;		/* process to handle raw PE */
	IFP s_approx;			/* approx match routine */
	char s_multiline;		/* if true print each value on new line */
} sntx_table;

extern char quipu_faststart;
extern char *TidyString();
extern char *TidyString2();

#define FAST_TIDY(x) if (quipu_faststart) while (isascii(*x) && isspace (*x)) x++; else x = TidyString(x)

#endif

#endif				/* __ISODE_QUIPU_ATTR_H__ */
