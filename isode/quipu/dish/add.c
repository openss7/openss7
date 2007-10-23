/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* add.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/add.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/add.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: add.c,v
 * Revision 9.0  1992/06/16  12:35:39  isode
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

#include "quipu/util.h"
#include "quipu/dua.h"
#include "quipu/add.h"
#include "quipu/entry.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

#include "osisec-stub.h"

#define ORG_PERSON "newPilotPerson & quipuObject"
	/* this should probably go elsewhere !!! */

extern DN dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS opt, rps;

extern char dad_flag;

char fname[128];
static char new_draft;

call_add(argc, argv)
	int argc;
	char **argv;
{

	Entry entry_ptr;
	FILE *fd;
	struct ds_addentry_arg add_arg;
	struct DSError error;

#ifdef TURBO_DISK
	Attr_Sequence fget_attributes();
#else
	Attr_Sequence get_attributes();
#endif
	extern int parse_status;

	int x;
	int draft_flag = 0;
	char *O_class = NULLCP;
	char noedit_flag = FALSE;
	DN moddn;
	char *home;

	if (home = getenv("DISHDRAFT"))
		(void) strcpy(fname, home);
	else if (dad_flag) {
		(void) strcpy(fname, "/tmp/dishXXXXXX");
		(void) unlink(mktemp(fname));
	} else if (home = getenv("HOME"))
		(void) sprintf(fname, "%s/.dishdraft", home);
	else
		(void) strcpy(fname, "./.dishdraft");
	new_draft = FALSE;

	if ((argc = service_control(OPT, argc, argv, &add_arg.ada_common)) == -1)
		return;

	for (x = 1; x < argc; x++) {
		if (test_arg(argv[x], "-template", 1)) {
			int i;
			FILE *in, *out;
			extern int errno;

			draft_flag = 1;
			if (++x == argc) {
				ps_printf(OPT, "template file name missing\n");
				Usage(argv[0]);
				return;
			}
			if ((in = fopen(argv[x], "r")) == NULL) {
				ps_printf(OPT, "unable to open template %s: %s\n",
					  argv[x], sys_errname(errno));
				return;
			}
			i = umask(0177);
			out = fopen(fname, "w");
			(void) umask(i);
			if (out == NULL) {
				ps_printf(OPT, "unable to write draft %s: %s\n",
					  fname, sys_errname(errno));
				(void) fclose(in);
				return;
			}
			while ((i = getc(in)) != EOF)
				if (putc(i, out) == EOF) {
					ps_printf(OPT, "error writing draft %s: %s\n",
						  fname, sys_errname(errno));
					(void) fclose(in);
					(void) fclose(out);
					return;
				}
			(void) fclose(in);
			(void) fclose(out);
		} else if (test_arg(argv[x], "-draft", 1)) {
			draft_flag = 1;
			if (++x == argc) {
				ps_printf(OPT, "Draft file name missing\n");
				Usage(argv[0]);
				return;
			}
			(void) strcpy(fname, argv[x]);
		} else if (test_arg(argv[x], "-objectclass", 1)) {
			if (++x == argc) {
				ps_printf(OPT, "Object Class missing\n");
				Usage(argv[0]);
				return;
			}
			O_class = argv[x];
		} else if (test_arg(argv[x], "-newdraft", 2))
			new_draft = TRUE;
		else if (test_arg(argv[x], "-noedit", 3))
			noedit_flag = TRUE;
		else if (move(argv[x]) == OK)
			continue;
		else {
			ps_printf(OPT, "Unknown option %s\n", argv[x]);
			Usage(argv[0]);
			return;
		}
	}

	if (dad_flag && (draft_flag || noedit_flag)) {
		ps_printf(OPT, "operation not allowed when using directory assistance server!\n");
		return;
	}

	if ((!noedit_flag) && (draft_flag != 1)) {	/* if no draft - create a template */
		if (add_template(fname, O_class) != OK)
			return;
	}

	if (!noedit_flag)
		if (editentry(1, argv) != OK) {
			make_old(fname, draft_flag);
			return;
		}

	/* now parse the files */

	if ((fd = fopen(fname, "r")) == (FILE *) NULL) {
		ps_printf(OPT, "Can't open draft entry %s\n", fname);
		return;
	}

	entry_ptr = get_default_entry(NULLENTRY);
#ifdef TURBO_DISK
	entry_ptr->e_attributes = fget_attributes(fd);
#else
	entry_ptr->e_attributes = get_attributes(fd);
#endif

	(void) fclose(fd);
	if (parse_status != 0)
		return;

	add_arg.ada_object = dn;

	for (moddn = dn; moddn->dn_parent != NULLDN; moddn = moddn->dn_parent) ;
	entry_ptr->e_name = rdn_cpy(moddn->dn_rdn);

	add_arg.ada_entry = entry_ptr->e_attributes;

	if (rebind() != OK) {
		entry_free(entry_ptr);
		return;
	}

	/* Strong authentication */
	if (add_arg.ada_common.ca_security != (struct security_parms *) 0) {
		extern struct SecurityServices *dsap_security;

		add_arg.ada_common.ca_sig =
		    (dsap_security->serv_sign) ((caddr_t) &add_arg, _ZAddEntryArgumentDataDAS,
						&_ZDAS_mod);
	}

	while (ds_addentry(&add_arg, &error) != DS_OK) {
		if (dish_error(OPT, &error) == 0) {
			entry_free(entry_ptr);
			return;
		}
		add_arg.ada_object = error.ERR_REFERRAL.DSE_ref_candidates->cr_name;
	}
	ps_print(RPS, "Added ");
	dn_print(RPS, dn, EDBOUT);
	ps_print(RPS, "\n");

	delete_list_cache(dn);
	entry_free(entry_ptr);

	make_old(fname, draft_flag);

}

make_old(file, commit)
	char *file;
	char commit;
{
	char newname[LINESIZE];

	if (dad_flag) {
		(void) unlink(file);
		return;
	}

	if (commit == 0) {
		(void) sprintf(newname, "%s.old", file);
		(void) rename(file, newname);
	}
}

Attr_Sequence
make_template_as(oc)
	AV_Sequence oc;
{
	AV_Sequence avs;
	Attr_Sequence newas;
	Attr_Sequence as = NULLATTR;
	table_seq optr;
	AttributeType at;
	objectclass *ocp;

	for (avs = oc; avs != NULLAV; avs = avs->avseq_next) {
		ocp = (objectclass *) avs->avseq_av.av_struct;
		for (optr = ocp->oc_must; optr != NULLTABLE_SEQ; optr = optr->ts_next) {
			at = optr->ts_oa;
			newas = as_comp_new(at, NULLAV, NULLACL_INFO);
			as = as_merge(as, newas);
		}
	}

	for (avs = oc; avs != NULLAV; avs = avs->avseq_next) {
		ocp = (objectclass *) avs->avseq_av.av_struct;
		for (optr = ocp->oc_may; optr != NULLTABLE_SEQ; optr = optr->ts_next) {
			at = optr->ts_oa;
			newas = as_comp_new(at, NULLAV, NULLACL_INFO);
			as = as_merge(as, newas);
		}
	}

	return (as);
}

add_template(name, objclass)
	char *name;
	char *objclass;
{
	FILE *fptr;
	PS ps;
	char obuf[LINESIZE];
	Attr_Sequence as;
	Attr_Sequence ocas;
	int um;

	if (objclass == NULLCP)
		objclass = ORG_PERSON;

	if (!new_draft)
		if ((fptr = fopen(name, "r")) != NULL) {
			(void) fclose(fptr);
			if (!yesno("Use existing draft file ? "))
				return OK;
			else
				make_old(fname, FALSE);
		}
	um = umask(0177);
	if ((fptr = fopen(name, "w")) == NULL) {
		ps_printf(OPT, "Can't open template entry %s\n", name);
		return (-1);
	}
	(void) umask(um);
	if ((ps = ps_alloc(std_open)) == NULLPS) {
		return (-1);
	}
	if (std_setup(ps, fptr) == NOTOK) {
		return (-1);
	}

	(void) sprintf(obuf, "objectClass=%s", objclass);
	if ((ocas = str2as(obuf)) == NULLATTR)
		return (-1);

	as = make_template_as(ocas->attr_value);
	as = as_merge(as, ocas);

	as_print(ps, as, EDBOUT);

	as_free(as);
	ps_free(ps);
	(void) fclose(fptr);

	return (OK);

}
