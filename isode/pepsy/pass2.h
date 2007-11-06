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

#ifndef __PEPSY_PASS2_H__
#define __PEPSY_PASS2_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* pass2.h */

/* 
 * Header: /xtel/isode/isode/pepsy/RCS/pass2.h,v 9.0 1992/06/16 12:24:03 isode Rel
 *
 *
 * Log: pass2.h,v
 * Revision 9.0  1992/06/16  12:24:03  isode
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

/* Change the version number only important features change - so people can
 * #ifdef on the version number. Also used to provide backwards compatible
 * macro definitions for posy/pepy.
 */
#define PEPSY_VERSION_NUMBER	2
#define NBPC	8		/* Number of Bits per character - machine dependant */
#define NBPI	sizeof (int)*NBPC	/* Number of bits per integer */

#define PSAP_DOT_H	"\"psap.h\""
#define I_PSAP_DOT_H	"<isode/psap.h>"
#define UNIV_TYPES_DOT_H	"\"UNIV-types.h\""
#define I_UNIV_TYPES_DOT_H	"<isode/pepsy/UNIV-types.h>"
#define	HFILE1	"_defs.h"
#define HFILE2	"_pre_defs.h"
#define ACTIONDEFS	"_action.h"

#define GENTYPES	"-types.h"
#define INCFILE1	"pepsy.h"
/* #define INCFILE2	"pepdefs.h" - not used any more */

#define	ACT1	"_act1"
#define	ACT2	"_act2"
#define	ACT3	"_act3"
#define	ACT4	"_act4"
#define	ACT05	"_act05"

#define PREFIX	"_Z"

#define ETABLE	"et_"
#define DTABLE	"dt_"
#define PTABLE	"pt_"

#define	ENCFNCNAME	"enc_f"
#define	DECFNCNAME	"dec_f"
#define	PRNTFNCNAME	"prnt_f"

#define	ENC_FNCNAME	"enc_f_"
#define	DEC_FNCNAME	"dec_f_"
#define	PRNT_FNCNAME	"prnt_f_"

#define	ENCFILENAME	"_enc.c"
#define	DECFILENAME	"_dec.c"
#define	PRNTFILENAME	"_prnt.c"

#define TBLNAME		"_tables.c"
#define MODTYP_SUFFIX	"_mod"

#define MAXPTRS		200	/* maximum number of pointers in pointer table */

#define PTR_TABNAME	"ptrtab"
extern int p_debug;

#define DEB 1

#ifdef DEB
#define Printf(x, y) if (x <= p_debug) printf y
#else
#define Printf(x, y)
#endif

/*
 * info for handling a Universal type
 */
struct univ_typ {
	char *univ_name;		/* Name of this Universal type */
	char *univ_data;		/* type to generate for it */
	char *univ_tab;			/* type of table entry it needs */
	PElementID univ_id;		/* tag of the type */
	PElementClass univ_class;	/* class - probably Universal primative */
	char *univ_mod;			/* Name of its module if it has one */
	int univ_flags;			/* Information about entry */
	int univ_type;			/* Type we can use for its contents */
#define UNF_EXTMOD	1		/* Use an external module reference */
#define UNF_HASDATA	2		/* Has data structure - allocate data for it */
};

extern struct univ_typ *univtyp();

/* How many entries in an array */
#define NENTRIES(x)	(sizeof (x)/sizeof ((x)[0]))

/* used to specify which tables a routine is to generate */
#define G_ENC	0		/* encoding */
#define G_DEC	1		/* decoding */
#define G_PNT	2		/* printing */

#define	hflag	(options[0])
#define	Hflag	(options[1])
#define h2flag	(options[2])
#define	NOPTIONS	3
extern int options[];

#define STRSIZE 128		/* general buffer size */

extern char *proc_name();

extern char *getfield(), *getfldbit();
extern char *class2str();

extern int gen_ventry();		/* generate a Value Passing Entry */
extern int gen_fnentry();		/* generate a function calling entry */

/* extern Action	start_action, final_action; */
extern char *int2tstr();		/* integer to temporary string */

extern char *getfield(), *getfldbit();
extern char *class2str();

extern int gen_ventry();		/* generate a Value Passing Entry */
extern int gen_fnentry();		/* generate a function calling entry */

/* extern Action	start_action, final_action; */

#endif				/* __PEPSY_PASS2_H__ */
