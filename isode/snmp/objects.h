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

#ifndef __SNMP_OBJECTS_H__
#define __SNMP_OBJECTS_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* objects.h - MIB objects */

/* 
 * Header: /xtel/isode/isode/snmp/RCS/objects.h,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * Log: objects.h,v
 * Revision 9.0  1992/06/16  12:38:11  isode
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

#ifndef	PEPYPATH
#include <isode/psap.h>
#else
#include "psap.h"
#endif

typedef struct object_syntax {
	char *os_name;			/* syntax name */

	IFP os_encode;			/* data -> PE */
	IFP os_decode;			/* PE -> data */
	IFP os_free;			/* free data */

	IFP os_parse;			/* str -> data */
	IFP os_print;			/* data -> tty */

	char **os_data1;		/* for moresyntax() in snmpi... */
	int os_data2;			/* .. */
} object_syntax, *OS;

#define	NULLOS	((OS) 0)

int readsyntax(), add_syntax();
OS text2syn();

typedef struct object_type {
	char *ot_text;			/* OBJECT DESCRIPTOR */
	char *ot_id;			/* OBJECT IDENTIFIER */
	OID ot_name;			/* .. */

	OS ot_syntax;			/* SYNTAX */

	int ot_access;			/* ACCESS */
#define	OT_NONE		0x00
#define	OT_RDONLY	0x01
#define	OT_WRONLY	0x02
#define	OT_RDWRITE	(OT_RDONLY | OT_WRONLY)

	u_long ot_views;		/* for views */

	int ot_status;			/* STATUS */
#define	OT_OBSOLETE	0x00
#define	OT_MANDATORY	0x01
#define	OT_OPTIONAL	0x02
#define	OT_DEPRECATED	0x03

	caddr_t ot_info;		/* object information */
	IFP ot_getfnx;			/* get/get-next method */
	IFP ot_setfnx;			/* set method */
#define	type_SNMP_PDUs_commit	(-1)
#define	type_SNMP_PDUs_rollback	(-2)

	caddr_t ot_save;		/* for set method */

	caddr_t ot_smux;		/* for SMUX */

	struct object_type *ot_chain;	/* hash-bucket for text2obj */

	struct object_type *ot_sibling;	/* linked-list for name2obj */
	struct object_type *ot_children;	/* .. */

	struct object_type *ot_next;	/* linked-list for get-next */
} object_type, *OT;

#define	NULLOT	((OT) 0)

int readobjects();
int add_objects();
OT name2obj(), text2obj();
OID text2oid();
char *oid2ode_aux();

typedef struct object_instance {
	OID oi_name;			/* instance OID */

	OT oi_type;			/* prototype */
} object_instance, *OI;

#define	NULLOI	((OI) 0)

OI name2inst(), next2inst(), text2inst();

extern IFP o_advise;

int o_generic(), s_generic();

int o_number();
int o_longword();

#define	o_integer(oi,v,value)	o_longword ((oi), (v), (integer) (value))

int o_string();
int o_qbstring();

int o_specific();

#define	o_ipaddr(oi,v,value)	o_specific ((oi), (v), (caddr_t) (value))
#ifdef	BSD44
#define	o_clnpaddr(oi,v,value)	o_specific ((oi), (v), (caddr_t) (value))
#endif

int mediaddr2oid();

#define	ipaddr2oid(ip,addr) \
	mediaddr2oid ((ip), (u_char*) (addr), sizeof (struct in_addr), 0)
#ifdef	BSD44
#define	clnpaddr2oid(ip,addr) \
	mediaddr2oid ((ip), \
		      (u_char *) (addr) -> isoa_genaddr, \
		      (int) (addr) -> isoa_len, 1)
#endif

OID oid_extend(), oid_normalize();

extern int debug;
extern char PY_pepy[BUFSIZ];

char *strdup();

#endif				/* __SNMP_OBJECTS_H__ */
