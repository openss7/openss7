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

#ifndef __FTAM2_FTAMSBR_H__
#define __FTAM2_FTAMSBR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ftamsbr.h - include file for FTAM initiator/responder subroutines */

/* 
 * Header: /xtel/isode/isode/ftam2/RCS/ftamsbr.h,v 9.0 1992/06/16 12:15:43 isode Rel
 *
 *
 * Log: ftamsbr.h,v
 * Revision 9.0  1992/06/16  12:15:43  isode
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

#include "ftam.h"		/* definitions for FS-USERs */
#include "DOCS-types.h"
#ifdef	NULL
#undef	NULL
#endif
#include <sys/param.h>
#ifndef	NULL
#define	NULL	0
#endif
#ifndef	SYS5
#include <sys/file.h>
#else
#define	L_SET		0	/* absolute offset */
#define	L_INCR		1	/* relative to current offset */
#define	L_XTND		2	/* relative to end of file */

#define	F_OK		0	/* file exists */
#define	X_OK		1	/* executable by caller */
#define	W_OK		2	/* writable by caller */
#define	R_OK		4	/* readable by caller */

#if	!defined(_AIX) && !defined(HPUX) && !defined(AUX) && !defined(masscomp)
#include <sys/fcntl.h>
#else
#include <fcntl.h>
#endif
#endif
#include <sys/stat.h>
#include "usr.dirent.h"

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	MAXNAMLEN
#endif

#ifdef	MAXBSIZE
#define	BLKSIZE	MAXBSIZE
#else
#define	BLKSIZE	BUFSIZ
#endif

/*
   Used to calculate the estimated integral FADU size:

   FTAM-3 transfers -

	An FADU maps onto a single PSDU with the P-DATA service.  Because the
	DCS is non-empty and the FADU is not in the default context, the
	Fully-encoded-data encoding is used.  Further, since only one PSDU is
	present, the single-ASN1-type option is used.  Hence, the outer ASN.1
	wrapper consists of

	    [APPLICATION 1] IMPLICIT
		SEQUENCE OF {	-- 4 octets for the outer structure
		    SEQUENCE {	-- 4 octets for the one and only PDV-list

				-- 3 octets for the PCI
		        presentation-context-identifier
		            INTEGER,

		        presentation-data-values {
				-- 4 octets for the single-ASN1-type wrapper
		            single-ASN1-type[0]
				-- 4 octets for the id/length of the FADU
				-- n octets for the data in the FADU
			        ANY
			}
		    }
		}

     4 + 4 + 3 + 4 + 4 = 19

     For each structure, 4 octets is used for non-data encodings
		1 octet for the ID
		1 octet for an indefinite form
		2 octets for the EOC
    If the data portion is smaller, then the definite form might be used which
    requires 3 octets, not 4.

   FTAM-1 transfers -

	FADUs are batched to the P-DATA service.  This means that the
	octet-aligned option is used.  Hence, the outer ASN.1 wrapper
	consists of  

	    [APPLICATION 1] IMPLICIT
		SEQUENCE OF {	-- 4 octets for the outer structure

-- this sequence is repeated for each member of the batch

		    SEQUENCE {	-- 4 octets for the one and only PDV-list

				-- 3 octets for the PCI
		        presentation-context-identifier
		            INTEGER,

		        presentation-data-values {
				-- 4 octets for the octet-aligned wrapper
		            octet-aligned[1]
				-- 4 octets for the id/length of the FADU
				-- n octets for the data in the FADU
			        IMPLICIT OCTET STRING
			}
		    }

		}

     4 + N*(4 + 3 + 4 + 4)
 */
#define	MAGIC_SINGLE	19
#define	MAGIC_OCTET1	4
#define	MAGIC_OCTET2	15

struct vfsmap {
	char *vf_entry;			/* document entry */
	OID vf_oid;			/* object identifier */
	caddr_t vf_parameter;		/* parameter, filled-in by vf_peek */

	int vf_flags;			/* flags */
#define	VF_NULL	0x00
#define	VF_OK	0x01			/* negotiated */
#define	VF_WARN	0x02			/* warn if loses */
#define	VF_PARM	0x04			/* parameter dynamically allocated */
#ifdef COMPAT_OLD_NBS9OID
#define VF_ALIASED  0x08		/* document type may be aliased */
#endif					/* COMPAT_OLD_NBS9OID */

	int vf_id;			/* presentation context */

	int vf_mode;			/* st.st_mode & S_IFMT bits */
	IFP vf_peek;			/* sees if really this type of file */
	char vf_stat;			/* stat character for 'ls' */

	int vf_simplify;		/* the next document type to try */
#define	VFS_XXX	(-1)

	int vf_context;			/* access context */
	/* really should have entire constraint set */

	int vf_mandatory;		/* > 0 parameter required < 0 parameter optional == 0
					   parameter illegal */
	IFP vf_check;			/* .. check */
	int vf_number;			/* encode/decode index */

	char *vf_text;			/* textual description */
};

struct vfsmap *st2vfs();

/* WATCHP is one pepsy people should use as the macro which is
 * not expansion order dependant
 */

#ifndef DEBUG
#define	WATCH(fnx, pe, rw)
#define	WATCHP(args, pe, rw)
#else
#ifdef __STDC__
#define	WATCHP(args, pe, rw) \
    pvpdu (ftam_log, print_##args##_P, pe, \
	rw ? "F-DATA.INDICATION" : "F-DATA.REQUEST", rw)
#define	WATCH(fnx, pe, rw) \
    pvpdu (ftam_log, fnx/**/_P, pe, \
	rw ? "F-DATA.INDICATION" : "F-DATA.REQUEST", rw)
#else
#define	WATCHP(args, pe, rw) \
    pvpdu (ftam_log, print_/**/args/**/_P, pe, \
	rw ? "F-DATA.INDICATION" : "F-DATA.REQUEST", rw)
#define	WATCH(fnx, pe, rw) \
    pvpdu (ftam_log, fnx/**/_P, pe, \
	rw ? "F-DATA.INDICATION" : "F-DATA.REQUEST", rw)
#endif
#endif

int binarypeek(), textpeek(), fdfpeek();

int binarycheck(), textcheck();

#define	FA_RDATTR \
    (FA_FILENAME | FA_ACTIONS | FA_CONTENTS | FA_ACCOUNT | FA_DATE_CREATE \
	| FA_DATE_MODIFY | FA_DATE_READ | FA_DATE_ATTR | FA_ID_CREATE \
	| FA_ID_MODIFY | FA_ID_READ | FA_ID_ATTR | FA_AVAILABILITY \
	| FA_FILESIZE)

#ifdef	BRIDGE
extern int ftp_default;
extern int ftp_directory;
#endif

int de2fd();

int compath();

#endif				/* __FTAM2_FTAMSBR_H__ */
