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

#ifndef __QUIPU_UIPS_FRED_FRED_H__
#define __QUIPU_UIPS_FRED_FRED_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* fred.h - definitions for fred */

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/fred.h,v 9.0 1992/06/16 12:44:30 isode Rel
 *
 *
 * Log: fred.h,v
 * Revision 9.0  1992/06/16  12:44:30  isode
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

#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

/* MAIN */

extern int interrupted;
extern int oneshot;

extern SFP istat;
extern SFP qstat;

extern LLog _fred_log, *fred_log;

void adios(), advise();
int ask(), getline();
char *strdup();

/* DATA */

extern int bflag;
extern int boundP;
extern int debug;
extern int fflag;
extern int kflag;
extern int mail;
extern int nametype;
extern int network;
extern int phone;
extern int query;
extern int readonly;
extern int soundex;
extern int timelimit;
extern int ufn_options;
extern int verbose;
extern int watch;

extern int didbind;
extern int dontpage;
extern int rcmode;
extern int runcom;
extern int runsys;

extern char *manager;
extern char *pager;
extern char *server;

extern char *myarea;
extern char *mydn;
extern char *myhome;
extern char *myuser;

extern FILE *stdfp;
extern FILE *errfp;

#define	EOLN	(network && !mail ? "\r\n" : "\n")

/* DISPATCH */

struct dispatch {
	char *ds_name;
	IFP ds_fnx;

	int ds_flags;
#define	DS_NULL	0x00
#define	DS_USER	0x01
#define	DS_SYOK	0x02

	char *ds_help;
};

/* MISCELLANY */

struct area_guide {
	int ag_record;
#define	W_ORGANIZATION	0x01
#define	W_UNIT		0x02
#define	W_LOCALITY	0x03
#define	W_PERSON	0x04
#define	W_DSA		0x05
#define	W_ROLE		0x06
	char *ag_key;

	char *ag_search;

	char *ag_class;
	char *ag_rdn;

	char *ag_area;
};

extern int area_quantum;
extern struct area_guide areas[];

/* WHOIS */

extern char *whois_help[];

extern int errno;

#endif				/* __QUIPU_UIPS_FRED_FRED_H__ */
