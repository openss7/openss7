/* pepy_undo.c - PE parser (yacc-based) building routines */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/pepy_undo.c,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/pepy/RCS/pepy_undo.c,v 9.0 1992/06/16 12:25:01 isode Rel $
 *
 *
 * $Log: pepy_undo.c,v $
 * Revision 9.0  1992/06/16  12:25:01  isode
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
#include <ctype.h>
#include "pepy.h"

extern struct tuple tuples[];
extern int	rflag, hflag;

char   *gensym (), *modsym ();

YP	lookup_type (), lookup_binding ();
YT	lookup_tag ();
char	*add_point ();

static	undo_type_element ();
static	undo_type_member ();
static int  undo_type_choice ();
static undo_components_seq ();
static	undo_components_set ();

/*  */

undo_type (yp, level, id, arg, Vflag)
register YP	yp;
register int	level;
register char  *id,
	       *arg;
int	Vflag;
{
    register int    i,
		    j;
    register char  *narg;
    register struct tuple *t;
    register YP	    y;
    register YV	    yv;

    if (yp -> yp_flags & YP_COMPONENTS) {
	yyerror_aux ("oops, I shouldn't be here!");
        print_type (yp, 0);
        return;
    }

    if (level == 1) {
	(void) printf ("(pe, explicit, len, buffer, parm)\n");
	(void) printf ("%sPE\tpe;\nint\texplicit;\n",
		yp -> yp_code != YP_ANY
		    && yp -> yp_code != YP_NULL
		    && (yp -> yp_code != YP_CHOICE
			|| (yp -> yp_flags & YP_CONTROLLED))
		? "register " : "");
	(void) printf ("int    *len;\nchar  **buffer;\n%s parm;\n{\n",
		yp -> yp_param_type ? yp -> yp_param_type : "PEPYPARM");

	if (yp -> yp_action0) {
	    if (!Pflag && *sysin)
		(void) printf ("# line %d \"%s\"\n", yp -> yp_act0_lineno, sysin);
	    (void) printf ("%*s%s\n", level * 4, "", yp -> yp_action0);
	}
    }

    switch (yp -> yp_code) {
	case YP_BOOL:
	case YP_INT:
	    if (!Vflag && (dflag || !((level == 1) || yp -> yp_action2
						   || yp -> yp_intexp)))
		break;		/* else fall */
	case YP_INTLIST:
	case YP_ENUMLIST:
	    (void) printf ("%*sregister integer %s;\n\n", level * 4, "",
		    narg = gensym ());
	    break;

	case YP_BIT:
	    if (!Vflag && (dflag || !((level == 1) || yp -> yp_action2
						   || yp -> yp_strexp)))
		break;		/* else fall */
	case YP_BITLIST:
	    (void) printf ("%*sregister PE %s;\n\n", level * 4, "",
		    narg = gensym ());
	    break;

	case YP_OCT:
	    if (!dflag && ((level == 1) || yp -> yp_action2
					|| yp -> yp_strexp)) {
		narg = gensym ();
		if (!Vflag && yp -> yp_prfexp == 'q')
		    (void) printf ("%*sregister struct qbuf *%s;\n\n",
			    level * 4, "", narg);
		else
		    (void) printf ("%*sregister char *%s;\n%*sint %s_len;\n\n",
			    level * 4, "", narg, level * 4, "", narg);
	    }
	    break;

	case YP_REAL:
	    if (!dflag && ((level == 1) || yp -> yp_action2
			  || yp -> yp_strexp)) {
		narg = gensym ();
		(void) printf ("%*sregister double %s;\n\n", level * 4, "", narg);
	    }
	    break;

	case YP_NULL:
	case YP_CHOICE:
	case YP_ANY:
	case YP_IDEFINED:
	    narg = NULL;
	    break;

	case YP_OID:
	    if (!Vflag && (dflag || (!yp -> yp_action2 && !yp -> yp_strexp
				     && level != 1)))
		break;		/* else fall */
	    (void) printf ("%*sregister OID %s;\n\n", level * 4, "",
		    narg = gensym ());
	    break;

	case YP_SEQ:
	case YP_SEQTYPE:
	case YP_SEQLIST:
	case YP_SET:
	case YP_SETTYPE:
	case YP_SETLIST:
	    narg = gensym ();
	    if (yp -> yp_code == YP_SETLIST)
		(void) printf ("%*sint %s_count = 0;\n", level * 4, "", narg);
	    (void) printf ("%*sregister PE %s;\n\n", level * 4, "", narg);
	    break;

	default:
	    myyerror ("unknown type: %d", yp -> yp_code);
    }

    if (!Vflag) {
	(void) printf ("#ifdef DEBUG\n%*s(void) testdebug (%s, \"",
		level * 4, "", arg);
	if (level == 1)
	    (void) printf ("%s.", mymodule);
	(void) printf ("%s\");\n#endif\n\n", id);
    }

    if (level == 1 && (yp -> yp_flags & YP_TAG)) {
	(void) printf ("%*sif (explicit\n%*s&& PE_ID (%s -> pe_class, %s -> pe_id)\n",
		level * 4, "", (level + 2) * 4, "", arg, arg);
	(void) printf ("%*s!= PE_ID (PE_CLASS_%s, %d)) {\n",
		(level + 4) * 4, "", pe_classlist[yp -> yp_tag -> yt_class],
		val2int (yp -> yp_tag -> yt_value));
	(void) printf ("%*sadvise (NULLCP, \"%s %%s%%s/0x%%x\", PEPY_ERR_BAD_CLASS,\n",
		(level + 1) * 4, "", id);
	(void) printf ("%*spe_classlist[%s -> pe_class], %s -> pe_id);\n",
		(level + 3) * 4, "", arg, arg);
	(void) printf ("%*sreturn NOTOK;\n%*s}\n",
		(level + 1) * 4, "", (level * 4), "");
    }
    else
	if (!(yp -> yp_flags & YP_IMPLICIT)) {
	    for (t = tuples; t -> t_type != YP_UNDF; t++)
		if (t -> t_type == yp -> yp_code) {
		    check_type (id, level, t -> t_class, t -> t_form,
			    t -> t_id, arg);
		    break;
		}
	}

    if (level == 1 && yp -> yp_code != YP_CHOICE &&
	(yp -> yp_flags & YP_TAG) == YP_TAG) {
	if ((yp -> yp_flags & YP_IMPLICIT) == 0 ||
	    is_nonimplicit_type (yp))
	    tag_pullup (yp, level, arg, "element");
    }

    if (Vflag) {
	if (yp -> yp_flags & YP_ID)
	    (void) printf ("%*svname (\"%s\");\n", level * 4, "", yp -> yp_id);
	else {
	    if (hflag && yp -> yp_code == YP_IDEFINED)
		(void) printf ("%*svname (\"%s\");\n", level * 4, "",
			yp -> yp_identifier);
	    else
		if ((yp -> yp_flags & YP_TAG)
			&& (yp -> yp_flags & (YP_OPTIONAL | YP_DEFAULT)))
		    (void) printf ("%*svtag (%d, %d);\n", level * 4, "",
			    yp -> yp_tag -> yt_class,
			    val2int (yp -> yp_tag -> yt_value));
	}
    }
    if (!dflag && yp -> yp_action05)
	do_action (yp -> yp_action05, level, arg, yp -> yp_act05_lineno);
    if (!dflag && yp -> yp_action1)
	do_action (yp -> yp_action1, level, arg, yp -> yp_act1_lineno);

    switch (yp -> yp_code) {
	case YP_BOOL:
	    if (Vflag || (!dflag && ((level == 1) || yp -> yp_action2
						  || yp -> yp_intexp)))
		(void) printf ("%*sif ((%s = prim2flag (%s)) == NOTOK) {\n",
			level * 4, "", narg, arg);
	    else
		(void) printf ("%*sif (prim2flag (%s) == NOTOK) {\n",
			level * 4, "", arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_BOOLEAN,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    if (!dflag && yp -> yp_intexp)
		(void) printf ("%*s%s = %s;\n", level * 4, "", yp -> yp_intexp, narg);
	    if (!dflag && (level == 1))
		(void) printf ("%*sif (len)\n%*s*len = %s;\n", level * 4, "",
			(level + 1) * 4, "", narg);
	    if (Vflag)
		(void) printf ("%*svprint (%s ? \"TRUE\" : \"FALSE\");\n",
			level * 4, "", narg);
	    break;

	case YP_INT:
	    if (Vflag || (!dflag && ((level == 1) || yp -> yp_action2
						  || yp -> yp_intexp)))
		(void) printf ("%*sif ((%s = prim2num (%s)) == NOTOK\n",
			level * 4, "", narg, arg);
	    else
		(void) printf ("%*sif (prim2num (%s) == NOTOK\n",
			level * 4, "", arg);
	    (void) printf ("%*s&& %s -> pe_errno != PE_ERR_NONE) {\n",
		    (level + 2) * 4, "", arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_INTEGER,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    if (!dflag && yp -> yp_intexp)
		(void) printf ("%*s%s = %s;\n", level * 4, "", yp -> yp_intexp, narg);
	    if (!dflag && (level == 1))
		(void) printf ("%*sif (len)\n%*s*len = %s;\n", level * 4, "",
			(level + 1) * 4, "", narg);
	    if (Vflag)
		(void) printf ("%*svprint (\"%%d\", %s);\n", level * 4, "", narg);
	    break;

	case YP_REAL:
	    if (Vflag || (!dflag && ((level == 1) || yp -> yp_action2
						  || yp -> yp_strexp)))
		(void) printf ("%*sif ((%s = prim2real (%s)) == NOTOK\n",
			level * 4, "", narg, arg);
	    else
		(void) printf ("%*sif (prim2real (%s) == NOTOK\n",
			level * 4, "", arg);
	    (void) printf ("%*s&& %s -> pe_errno != PE_ERR_NONE) {\n",
		    (level + 2) * 4, "", arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_REAL,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    if (!dflag && yp -> yp_strexp)
		    (void) printf ("%*s%s = %s;\n", level * 4, "",
			    yp -> yp_strexp, narg);
	    if (Vflag)
		(void) printf ("%*svprint (\"%%g\", %s);\n", level * 4, "", narg);
	    break;

	case YP_INTLIST:
	case YP_ENUMLIST:
	    (void) printf ("%*sif ((%s = prim2%snum (%s)) == NOTOK\n",
		    level * 4, "", narg,
		    yp->yp_code == YP_ENUMLIST ? "e" : "",
		    arg);
	    (void) printf ("%*s&& %s -> pe_errno != PE_ERR_NONE) {\n",
		    (level + 2) * 4, "", arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_INTEGER,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    if (!dflag && yp -> yp_intexp)
		(void) printf ("%*s%s = %s;\n", level * 4, "", yp -> yp_intexp, narg);
	    if (!dflag && (level == 1))
		(void) printf ("%*sif (len)\n%*s*len = %s;\n", level * 4, "",
			(level + 1) * 4, "", narg);
	    uniqint (yp -> yp_value);
	    (void) printf ("%*sswitch (%s) {\n", level * 4, "", narg);
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
		(void) printf ("%*scase %d:", (level + 1) * 4, "", val2int (yv));
		if (yv -> yv_flags & YV_NAMED)
		    (void) printf ("\t/* %s */", yv -> yv_named);
		(void) printf ("\n");
		if (Vflag) {
		    if (yv -> yv_flags & YV_NAMED)
			(void) printf ("%*svprint (\"%s\");\n", (level + 2) * 4, "",
				yv -> yv_named);
		    else
			(void) printf ("%*svprint (\"%%d\", %s);\n", (level + 2) * 4,
				"", narg);
		}
		if (!dflag && yv -> yv_action)
		    do_action (yv -> yv_action, level + 2, narg,
				yv -> yv_act_lineno);
		(void) printf ("%*sbreak;\n", (level + 2) * 4, "");
	    }
	    if (!rflag && yp -> yp_code == YP_ENUMLIST) {
		(void) printf ("%*sdefault:\n", (level + 1) * 4, "");
		(void) printf ("%*sadvise (NULLCP, \"%s %%s%%d\", PEPY_ERR_UNK_COMP, %s);\n",
			(level + 2) * 4, "", id, narg);
		(void) printf ("%*sreturn NOTOK;\n", (level + 2) * 4, "");
	    }
	    else
		if (Vflag) {
		    (void) printf ("%*sdefault:\n", (level + 1) * 4, "");
		    (void) printf ("%*svprint (\"%%d\", %s);\n", (level + 2) * 4, "",
			    narg);
		    (void) printf ("%*sbreak;\n", (level + 2) * 4, "");
		}
	    (void) printf ("%*s}\n", level * 4, "");
	    break;

	case YP_BIT:
	    if (Vflag || (!dflag && ((level == 1) || yp -> yp_action2
						  || yp -> yp_strexp)))
		(void) printf ("%*sif ((%s = prim2bit (%s)) == NULLPE) {\n",
			level * 4, "", narg, arg);
	    else
		(void) printf ("%*sif (prim2bit (%s) == NULLPE) {\n",
			level * 4, "", arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_BITS,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    if (!dflag && yp -> yp_strexp)
		(void) printf ("%*s%s = bitstr2strb (%s, &(%s));\n",
			level * 4, "", yp -> yp_strexp, arg, yp -> yp_intexp);
	    if (!dflag && (level == 1)) {
		(void) printf ("%*sif (buffer && len)\n", level * 4, "");
		if (yp -> yp_strexp)
		    (void) printf ("%*s*buffer = %s, *len = %s;\n",
			(level + 1) * 4, "", yp -> yp_strexp, yp -> yp_intexp);
		else
		    (void) printf ("%*s*buffer = bitstr2strb (%s, len);\n",
			(level + 1) * 4, "", arg);
	    }
	    if (Vflag) {
		(void) printf ("%*sif (%s -> pe_nbits < 128)\n",
			level * 4, "", narg);
		(void) printf ("%*svprint (\"%%s\", bit2str (%s, \"\\020\"));\n",
			(level + 1) * 4, "", narg);
		(void) printf ("%*selse\n%*svunknown (%s);\n",
			level * 4, "", (level + 1) * 4, "", narg);
	    }
	    break;

	case YP_BITLIST:
	    (void) printf ("%*sif ((%s = prim2bit (%s)) == NULLPE) {\n",
		    level * 4, "", narg, arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_BITS,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    if (!dflag && yp -> yp_strexp)
		(void) printf ("%*s%s = bitstr2strb (%s, &(%s));\n",
			level * 4, "", yp -> yp_strexp, arg, yp -> yp_intexp);
	    if (!dflag && (level == 1)) {
		(void) printf ("%*sif (buffer && len)\n", level * 4, "");
		if (yp -> yp_strexp)
		    (void) printf ("%*s*buffer = %s, *len = %s;\n",
			(level + 1) * 4, "", yp -> yp_strexp, yp -> yp_intexp);
		else
		    (void) printf ("%*s*buffer = bitstr2strb (%s, len);\n",
			(level + 1) * 4, "", arg);
	    }
#ifdef	notdef
	    if (!rflag) {
		register int	j;

		for (yv = yp -> yp_value, i = 0; yv; yv = yv -> yv_next)
		    if ((j = val2int (yv)) > i)
			i = j;
		i++;
		(void) printf ("%*sif (%s -> pe_nbits > %d) {\n",
			level * 4, "", narg, i);
		(void) printf ("%*sadvise (NULLCP, \"%s %%s(%d): %%d\", PEPY_ERR_TOO_MANY_BITS,\n",
			(level + 1) * 4, "", id, i);
		(void) printf ("%*s%s -> pe_nbits);\n", (level + 3) * 4, "", narg);
		(void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
			level * 4, "");
	    }
#endif
	    i = -1;
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
		if ((j = val2int (yv)) < 0)
		    pyyerror (yp, "invalid bit number in BIT STRING");
		else
		    if (j > i)
			i = j;
	    (void) printf ("#define\tBITS\t\"\\020");
	    if (i < sizeof (int) * 8) {		/* NBBY */
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
		    if (yv -> yv_flags & YV_NAMED)
			(void) printf ("\\0%o%s", val2int (yv) + 1, yv -> yv_named);
		    else
			(void) printf ("\\0%oBIT%d", val2int (yv) + 1, val2int (yv));
	    }
	    (void) printf ("\"\n");
	    uniqint (yp -> yp_value);
	    if (!dflag)
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
		    if (!yv -> yv_action)
			continue;
		    (void) printf ("%*sif (bit_test (%s, %d) > OK) {",
			    level * 4, "", narg, val2int (yv));
		    if (yv -> yv_flags & YV_NAMED)
			(void) printf ("\t/* %s */", yv -> yv_named);
		    (void) printf ("\n");
		    do_action (yv -> yv_action, level + 1, narg,
			       yv -> yv_act_lineno);
		    (void) printf ("%*s}\n", level * 4, "");
		}
	    if (Vflag)
		(void) printf ("%*svprint (\"%%s\", bit2str (%s, BITS));\n",
			level * 4, "", narg);
	    break;

	case YP_OCT:
	    if (!dflag && ((level == 1) || yp -> yp_action2
					|| yp -> yp_strexp)) {
		(void) printf ("%*sif ((%s = ", level * 4, "", narg);
		if (!Vflag && yp -> yp_prfexp == 'q')
		    (void) printf ("prim2qb (%s)) == (struct qbuf *)0) {\n", arg);
		else
		    (void) printf ("prim2str (%s, &%s_len)) == NULLCP) {\n",
			    arg, narg);
		(void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_OCTET,\n",
			(level + 1) * 4, "", id);
		(void) printf ("%*spe_error (%s -> pe_errno));\n",
			(level + 3) * 4, "", arg);
		(void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
			level * 4, "");
	    }
	    if (!dflag && yp -> yp_strexp) {
		if (! (yp -> yp_prfexp == 'q' && Vflag))
		    (void) printf ("%*s%s = %s;\n",
			    level * 4, "", yp -> yp_strexp, narg);
	    }
	    if (!dflag && yp -> yp_intexp && yp -> yp_prfexp != 'q')
		(void) printf ("%*s%s = %s_len;\n",
			level * 4, "", yp -> yp_intexp, narg);
	    if (Vflag)
		(void) printf ("%*svstring (%s);\n", level * 4, "", arg);
	    break;

	case YP_ANY:
	    if (!dflag && yp -> yp_strexp)
		(void) printf ("%*s(%s = %s) -> pe_refcnt++;\n",
			level * 4, "", yp -> yp_strexp, arg);
	    if (Vflag)
		(void) printf ("%*svunknown (%s);\n", level * 4, "", arg);
	    break;

	case YP_NULL:
	    if (Vflag)
		(void) printf ("%*svprint (\"NULL\");\n", level * 4, "");
	    break;

	case YP_OID:
	    if (Vflag || (!dflag && (yp -> yp_action2 || yp -> yp_strexp
				     || level == 1)))
		(void) printf ("%*sif ((%s = prim2oid (%s)) == NULLOID) {\n",
			level * 4, "", narg, arg);
	    else
		(void) printf ("%*sif (prim2oid (%s) == NULLOID) {\n",
			level * 4, "", arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_OID,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    if (!dflag && level == 1) {
		(void) printf ("%*sif (buffer)\n", level * 4, "");
		(void) printf ("%*s*buffer = sprintoid (%s);\n",
			(level + 1) * 4, "", narg);
	    }
	    if(!dflag && yp -> yp_strexp)
		(void) printf ("%*s%s = oid_cpy (%s);\n", level * 4, "",
			yp -> yp_strexp, narg);
	    if (Vflag)
		(void) printf ("%*svprint (\"%%s\", oid2ode (%s));\n", level * 4,
		    "", narg);
	    break;

	case YP_SEQ:
	    (void) printf ("%*sif ((%s = prim2seq (%s)) == NULLPE) {\n",
		    level * 4, "", narg, arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_SEQ,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    (void) printf ("%*s%s = %s;\n\n", level * 4, "", arg, narg);
	    if (!dflag && yp -> yp_strexp)
		(void) printf ("%*s(%s = %s) -> pe_refcnt++;\n",
			level * 4, "", yp -> yp_strexp, narg);
	    if (Vflag)
		(void) printf ("%*svunknown (%s);\n", level * 4, "", narg);
	    break;

	case YP_SEQTYPE:
	    (void) printf ("%*sif ((%s = prim2seq (%s)) == NULLPE) {\n",
		    level * 4, "", narg, arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_SEQ,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    (void) printf ("%*s%s = %s;\n\n", level * 4, "", arg, narg);
	    if (Vflag)
		(void) printf ("%*svpush ();\n", level * 4, "");
	    if (yp -> yp_type) {
		(void) printf ("%*sfor (%s = first_member (%s); %s; %s = next_member (%s, %s)) {\n",
			level * 4, "", narg, arg, narg, narg, arg, narg);
		if (!dflag && yp -> yp_action3) {
		    do_action (yp -> yp_action3, ++level, arg,
				yp -> yp_act3_lineno);
		    (void) printf ("%*s{\n", level * 4, "");
		}
		undo_type (yp -> yp_type, level + 1, "element", narg, Vflag);
		if (!dflag && yp -> yp_action3 && yp -> yp_action2 )
		    do_action (yp -> yp_action2, level + 1, arg,
				yp -> yp_act2_lineno);
		if (!dflag && yp -> yp_action3)
		    (void) printf ("%*s}\n", level-- * 4, "");
		(void) printf ("%*s}\n", level * 4, "");
	    }
	    if (Vflag)
		(void) printf ("%*svpop ();\n", level * 4, "");
	    break;

	case YP_SEQLIST:
	    (void) printf ("%*sif ((%s = prim2seq (%s)) == NULLPE) {\n",
		    level * 4, "", narg, arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_SEQ,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    (void) printf ("%*s%s = %s;\n\n", level * 4, "", arg, narg);
	    if (Vflag)
		(void) printf ("%*svpush ();\n", level * 4, "");
	    for (y = yp -> yp_type, i = 0; y; y = y -> yp_next)
		if (y -> yp_flags & YP_COMPONENTS)
		    i += undo_components_seq (y, level, y == yp -> yp_type,
			y -> yp_next == NULLYP, id, arg, narg, Vflag);
		else {
		    undo_type_element (y, level, y == yp -> yp_type,
			y -> yp_next == NULLYP, id, arg, narg, Vflag);
		    i++;
		}
	    if (Vflag)
		(void) printf ("%*svpop ();\n", level * 4, "");
	    for (y = yp -> yp_type; y; y = y -> yp_next) {
		register YP	z;

		if (!(y -> yp_flags & (YP_OPTIONAL | YP_DEFAULT))
			|| lookup_tag (y) == NULLYT)
		    continue;
		for (z = y -> yp_next; z; z = z -> yp_next)
		    if (!(z -> yp_flags & (YP_OPTIONAL | YP_DEFAULT))
			    || lookup_tag (z) == NULLYT)
			break;
		uniqtag (y, z);
		if (z == NULLYP)
		    break;
		y = z;
	    }
	    if (!rflag) {
		(void) printf ("\n%*sif (%s -> pe_cardinal > %d) {\n",
			level * 4, "", arg, i);
		(void) printf ("%*sadvise (NULLCP, \"%s %%s(%d): %%d\", PEPY_ERR_TOO_MANY_ELEMENTS,\n",
			(level + 1) * 4, "", id, i);
		(void) printf ("%*s%s -> pe_cardinal);\n", (level + 3) * 4, "", arg);
		(void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
			level * 4, "");
	    }
	    break;

	case YP_SET:
	    (void) printf ("%*sif ((%s = prim2set (%s)) == NULLPE) {\n",
		    level * 4, "", narg, arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_SET,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    (void) printf ("%*s%s = %s;\n\n", level * 4, "", arg, narg);
	    if (!dflag && yp -> yp_strexp)
		(void) printf ("%*s(%s = %s) -> pe_refcnt++;\n",
			level * 4, "", yp -> yp_strexp, narg);
	    if (Vflag)
		(void) printf ("%*svunknown (%s);\n", level * 4, "", narg);
	    break;

	case YP_SETTYPE:
	    (void) printf ("%*sif ((%s = prim2set (%s)) == NULLPE) {\n",
		    level * 4, "", narg, arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_SET,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    (void) printf ("%*s%s = %s;\n\n", level * 4, "", arg, narg);
	    if (Vflag)
		(void) printf ("%*svpush ();\n", level * 4, "");
	    if (yp -> yp_type) {
		(void) printf ("%*sfor (%s = first_member (%s); %s; %s = next_member (%s, %s)) {\n",
			level * 4, "", narg, arg, narg, narg, arg, narg);
		if (!dflag && yp -> yp_action3) {
		    do_action (yp -> yp_action3, ++level, arg,
				yp -> yp_act3_lineno);
		    (void) printf ("%*s{\n", level * 4, "");
		}
		undo_type (yp -> yp_type, level + 1, "member", narg, Vflag);
		if (!dflag && yp -> yp_action3 && yp -> yp_action2 )
		    do_action (yp -> yp_action2, level + 1, arg,
				yp -> yp_act2_lineno);
		if (!dflag && yp -> yp_action3)
		    (void) printf ("%*s}\n", level-- * 4, "");
		(void) printf ("%*s}\n", level * 4, "");
	    }
	    if (Vflag)
		(void) printf ("%*svpop ();\n", level * 4, "");
	    break;

	case YP_SETLIST:
	    (void) printf ("%*sif ((%s = prim2set (%s)) == NULLPE) {\n",
		    level * 4, "", narg, arg);
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s\", PEPY_ERR_BAD_SET,\n",
		    (level + 1) * 4, "", id);
	    (void) printf ("%*spe_error (%s -> pe_errno));\n", (level + 3) * 4, "",
		    arg);
	    (void) printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
		    level * 4, "");
	    (void) printf ("%*s%s = %s;\n\n", level * 4, "", arg, narg);
	    if (Vflag)
		(void) printf ("%*svpush ();\n", level * 4, "");
	    if (yp -> yp_type) {
		for (y = yp -> yp_type; y; y = y -> yp_next)
		    if (y -> yp_flags & YP_COMPONENTS)
			undo_components_set (y, level, arg, narg, Vflag);
		    else
			undo_type_member (y, level, arg, narg, Vflag);
		choice_pullup (y = copy_type (yp), CH_FULLY);
		uniqtag (y -> yp_type, NULLYP);
		if (!rflag) {
		    (void) printf ("%*sif (%s_count != %s -> pe_cardinal)\n",
			    level * 4, "", narg, arg);
		    (void) printf ("%*sadvise (NULLCP, \"%%s\", PEPY_ERR_EXTRA_MEMBERS);\n",
			    (level + 1) * 4, "");
		}
	    }
	    if (Vflag)
		(void) printf ("%*svpop ();\n", level * 4, "");
	    break;

	case YP_CHOICE:
	    if (Vflag)
		(void) printf ("%*svpush ();\n", level * 4, "");
	    if (yp -> yp_type) {
		int	didefault;
		
		if ((yp -> yp_flags & YP_TAG)
			&& !(yp -> yp_flags & YP_PULLEDUP))
		    tag_pullup (yp, level, arg, "choice");
		(void) printf ("%*sswitch (PE_ID (%s -> pe_class, %s -> pe_id)) {\n",
			level * 4, "", arg, arg);
		choice_pullup (yp, CH_PARTIAL);
		didefault = 0;
		for (y = yp -> yp_type; y; y = y -> yp_next)
		    didefault += undo_type_choice (y, level + 1, arg, Vflag);
		if (didefault > 1)
		    yyerror_aux ("multiple non-tagged ANYs in CHOICE");
		uniqtag (yp -> yp_type, NULLYP);
		if (!didefault && !rflag) {
		    (void) printf ("\n%*sdefault:\n", (level + 1) * 4, "");
		    (void) printf ("%*sadvise (NULLCP, \"%s %%s%%s/%%d/0x%%x\", PEPY_ERR_UNKNOWN_CHOICE,\n",
			    (level + 2) * 4, "", id);
		    (void) printf ("%*spe_classlist[%s -> pe_class], %s -> pe_form, %s -> pe_id);\n",
			    (level + 4) * 4, "", arg, arg, arg);
		    (void) printf ("%*sreturn NOTOK;\n", (level + 2) * 4, "");
		}
		(void) printf ("%*s}\n", level * 4, "");
	    }
	    if (Vflag)
		(void) printf ("%*svpop ();\n", level * 4, "");
	    break;

	case YP_IDEFINED:
	    (void) printf ("%*sif (%s (", level * 4, "", modsym (yp -> yp_module,
		    yp -> yp_identifier, Vflag ? YP_PRINTER : YP_DECODER));
	    (void) printf ("%s, ", arg);
	    if (level != 1 || (yp -> yp_flags & YP_IMPLICIT))
		(void) printf ("%d, ", (yp -> yp_flags & YP_IMPLICIT) ? 0 : 1);
	    else
		(void) printf ("explicit, ");
	    if (yp -> yp_intexp)
		(void) printf ("&(%s), ", yp -> yp_intexp);
	    else if (level == 1)
		(void) printf ("len, ");
	    else
		(void) printf ("NULLIP, ");
	    if (yp -> yp_strexp)
		(void) printf ("&(%s)", yp -> yp_strexp);
	    else if (level == 1)
		(void) printf ("buffer");
	    else
		(void) printf ("NULLVP");
	    if (yp -> yp_flags & YP_PARMVAL)
		(void) printf (", %s", yp -> yp_parm);
	    else
		(void) printf (", NullParm");
	    (void) printf (") == NOTOK)\n%*sreturn NOTOK;\n", (level + 1) * 4, "");
	    break;

	default:
	    myyerror ("unknown type: %d", yp -> yp_code);
    }

    if (!dflag && yp -> yp_action2 && !yp -> yp_action3)
	do_action (yp -> yp_action2, level, narg ? narg : arg,
				yp -> yp_act2_lineno);

    switch (yp -> yp_code) {
	case YP_BITLIST:
	    (void) printf ("#undef\tBITS\n");
	    break;

	case YP_OCT:
	    if (!dflag && yp -> yp_prfexp != 'q' &&
		((level == 1) || yp -> yp_action2)) {
		if (level == 1) {
		    (void) printf ("%*sif (len)\n", level * 4, "");
		    (void) printf ("%*s*len = %s_len;\n", (level + 1) * 4, "", narg);
		    (void) printf ("%*sif (buffer)\n", level * 4, "");
		    (void) printf ("%*s*buffer = %s;\n", (level + 1) * 4, "", narg);
		    (void) printf ("%*selse\n", level * 4, "");
		}
		(void) printf ("%*s", (level + 1) * 4, "");
		if (yp -> yp_strexp)
		    (void) printf ("/* do nothing */;\n");
		else
		    (void) printf ("if (%s)\n%*sfree (%s);\n", narg, (level + 2) * 4,
			    "", narg);
	    }
	    break;

	default:
	    break;
    }
}

/*  */

static	undo_type_element (yp, level, first, last, id, arg, narg, Vflag)
register YP	yp;
register int	level;
int	first,
	last;
register char  *id,
	       *arg,
	       *narg;
int	Vflag;
{
    register char  *narg2;
    register YT yt;

    (void) printf ("%*s{\n%*sregister PE %s;\n\n",
	    level * 4, "", (level + 1) * 4, "", narg2 = gensym ());
    level++;

    if ((yp -> yp_flags & (YP_OPTIONAL | YP_DEFAULT)) && !last) {
	YP yp2 = copy_type (yp);

	if (!(yp2 -> yp_flags & YP_TAG)) {
	    switch (yp2 -> yp_code) {
	    case YP_CHOICE:
		break;
	    case YP_IDEFINED:
		if (lookup_tag (yp2) == NULLYT)
		    break;
	    default:
		tag_type (yp2);
		break;
	    }
	}
	(void) printf ("%*sif ((%s = ", level * 4, "", narg2);
	if (first)
	    (void) printf ("first_member (%s)) != NULLPE", arg);
	else {
	    (void) printf ("(%s != %s ? next_member (%s, %s) : first_member (%s))",
		    arg, narg, arg, narg, arg);
	    (void) printf (") \n%*s!= NULLPE", (level + 3) * 4, "");
	}
	if (yp2 -> yp_flags & YP_TAG && !last) {
	    yt = yp2 -> yp_tag;
	    (void) printf ("\n%*s&& PE_ID (%s -> pe_class, %s -> pe_id)\n",
		    (level + 2) * 4, "", narg2, narg2);
	    (void) printf ("%*s!= PE_ID (PE_CLASS_%s, %d))\n%*s%s = NULLPE;\n",
		    (level + 4) * 4, "", pe_classlist[yt -> yt_class],
		    val2int (yt -> yt_value), (level + 1) * 4, "", narg2);
	}
	else {
	    ype zy;
	    register YP y = &zy;

	    y -> yp_type = copy_type (yp2); /* XXX */
	    y -> yp_type -> yp_next = NULLYP;
	    choice_pullup (y, CH_FULLY); /* XXX */
	    for (y = y -> yp_type; y; y = y -> yp_next) {
		if (!(y -> yp_flags & YP_TAG))
		    tag_type (y);
		(void) printf ("\n%*s&& PE_ID (%s -> pe_class, %s -> pe_id)\n",
			(level + 2) * 4, "", narg2, narg2);
		(void) printf ("%*s!= PE_ID (PE_CLASS_%s, %d)",
			(level + 4) * 4, "",
			pe_classlist[y -> yp_tag -> yt_class],
			val2int (y -> yp_tag -> yt_value));
	    }
	    (void) printf (")\n%*s%s = NULLPE;\n", (level + 1) * 4, "", narg2);
	}
	(void) printf ("%*sif (%s != NULLPE", level * 4, "", narg2);
    }
    else {
	(void) printf ("%*sif ((%s = ", level * 4, "", narg2);
	if (first)
	    (void) printf ("first_member (%s)", arg);
	else
	    (void) printf ("(%s != %s ? next_member (%s, %s) : first_member (%s))",
		    arg, narg, arg, narg, arg);
	(void) printf (") != NULLPE");
    }
    (void) printf (") {\n%*s%s = %s;\n\n", (level + 1) * 4, "", narg, narg2);
    level++;

    if (yp -> yp_code != YP_CHOICE && (yp -> yp_flags & YP_TAG)) {
	if ((yp -> yp_flags & YP_IMPLICIT) == 0 ||
	    is_nonimplicit_type (yp))
	    tag_pullup (yp, level, narg2, "element");
    }
    (void) printf ("%*s{", level * 4, "");
    level++;
    if (yp -> yp_flags & YP_ID)
	(void) printf ("\t/* %s */", yp -> yp_id);
    (void) printf ("\n");

    undo_type (yp, level, yp -> yp_flags & YP_ID ? yp -> yp_id : "element",
	    narg2, Vflag);

    level--;
    (void) printf ("%*s}\n", level * 4, "");

    level--;
    (void) printf ("%*s}\n", level * 4, "");

    if ((yp -> yp_flags & YP_DEFAULT) || !(yp -> yp_flags & YP_OPTIONAL)) {
	(void) printf ("%*selse {\n", level * 4, "");

	if (yp -> yp_flags & YP_DEFAULT)
	    (void) printf ("%*s/* set default here using yp -> yp_default */\n",
		    (level + 1) * 4, "");
	else {
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s",
		    (level + 1) * 4, "", id);
	    if (yp -> yp_flags & YP_ID)
		(void) printf ("%s ", yp -> yp_id);
	    (void) printf ("element\", PEPY_ERR_MISSING);\n%*sreturn NOTOK;\n", (level + 1) * 4, "");
	}

	(void) printf ("%*s}\n\n", level * 4, "");
    }

    level--;
    (void) printf ("%*s}\n\n", level * 4, "");
}

/*  */

static	undo_type_member (yp, level, arg, narg, Vflag)
register YP	yp;
register int	level;
char   *arg,
       *narg;
int	Vflag;
{
    int	    pullup = 0;
    char   *id = yp -> yp_flags & YP_ID ? yp -> yp_id : "member";
    char   *narg2;

    if (!(yp -> yp_flags & YP_TAG)) {
	switch (yp -> yp_code) {
	case YP_CHOICE:
	    break;
	case YP_IDEFINED:
	    if (lookup_tag (yp) == NULLYT)
		break;
	default:
	    tag_type (yp);
	}
    }
    if (yp -> yp_flags & YP_TAG)
	(void) printf ("%*sif (%s = set_find (%s, PE_CLASS_%s, %d)) {\n",
		level * 4, "", narg, arg,
		pe_classlist[yp -> yp_tag -> yt_class],
		val2int (yp -> yp_tag -> yt_value));
    else {
	ype zy;
	register YP y = &zy;

	y -> yp_type = copy_type(yp);	/* XXXX !!! */
	y -> yp_type -> yp_next = NULLYP;
	choice_pullup (y, CH_FULLY);
	/* this is dependant on choice_pullup coding... */
	y = y -> yp_type;
	if (y) {
	    if (!(y -> yp_flags & YP_TAG))
		tag_type (y);
	    (void) printf ("%*sif ( (%s = set_find (%s, PE_CLASS_%s, %d))",
		    level * 4, "", narg, arg,
		    pe_classlist[y->yp_tag->yt_class],
		    val2int (y -> yp_tag -> yt_value));
	    for (y = y -> yp_next; y; y = y -> yp_next) {
		if (!(y -> yp_flags & YP_TAG))
		    tag_type (y);
		(void) printf ("\n%*s|| (%s = set_find (%s, PE_CLASS_%s, %d))",
			(level + 1) * 4, "", narg, arg,
			pe_classlist[y -> yp_tag -> yt_class],
			val2int (y -> yp_tag -> yt_value));
	    }
	    (void) printf (" ) {\n");
	}
    }
    level ++;

    if (yp -> yp_flags & YP_TAG) {
	if ((yp -> yp_flags & YP_IMPLICIT) == 0 ||
	    is_nonimplicit_type (yp))
	    pullup = 1;
    }

    if (pullup) {
	(void) printf ("%*sregister PE %s = %s;\n\n", level * 4, "",
		narg2 = gensym (), narg);
	tag_pullup (yp, level, narg2, id);
	(void) printf ("%*s{\n", level * 4, "");
	level++;
	yp -> yp_flags |= YP_PULLEDUP;
    }
    else
	narg2 = narg;

    undo_type (yp, level, id, narg2, Vflag);

    if (pullup) {
	level--;
	(void) printf ("%*s}\n", level * 4, "");
    }

    (void) printf ("%*s%s_count ++;\n", level * 4, "", narg); 
    level--;
    (void) printf ("%*s}\n", level * 4, "");

    if ((yp -> yp_flags & YP_DEFAULT) || !(yp -> yp_flags & YP_OPTIONAL)) {
	(void) printf ("%*selse {\n", level * 4, "");

	if (yp -> yp_flags & YP_DEFAULT)
	    (void) printf ("%*s/* set default here using yp -> yp_default */\n",
		    (level + 1) * 4, "");
	else {
	    (void) printf ("%*sadvise (NULLCP, \"%s %%s ",
		    (level + 1) * 4, "", id);
	    if (yp -> yp_flags & YP_ID)
		(void) printf ("%s ", yp -> yp_id);
	    (void) printf ("member\", PEPY_ERR_MISSING);\n%*sreturn NOTOK;\n", (level + 1) * 4, "");
	}

	(void) printf ("%*s}\n\n", level * 4, "");
    }
}
/*  */

static int  undo_type_choice (yp, level, narg, Vflag)
register YP	yp;
register int	level;
register char  *narg;
int	Vflag;
{
    int	    pullup = 0;
    int	    result;
    char   *id = yp -> yp_flags & YP_ID ? yp -> yp_id : "member";
    char   *narg2;

    if (is_any_type (yp)) {
	(void) printf ("%*sdefault:", level * 4, "");

	result = 1;	
    }
    else if (!(yp -> yp_flags & YP_TAG) && yp->yp_code == YP_IDEFINED) {
	ype zy;
	register YP y = &zy;

	result = 0;
	y -> yp_type = copy_type(yp);	/* XXXX !!! */
	y -> yp_type -> yp_next = NULL;
	choice_pullup (y, CH_FULLY);
	    /* this is dependant on choice_pullup coding..*/
	for (y = y -> yp_type; y; y = y -> yp_next) {
	    if (is_any_type (y)) {
		(void) printf ("%*sdefault:%s", level * 4, "",
			y -> yp_next ? "\n" : "");
		result ++;
	    }
	    else {
		if (!(y -> yp_flags & YP_TAG))
		    tag_type(y);
		(void) printf("%*scase PE_ID (PE_CLASS_%s, %d):%s", level * 4, "",
		       pe_classlist [y -> yp_tag -> yt_class],
		       val2int (y -> yp_tag -> yt_value),
		       y -> yp_next ? "\n" : "");
	    }
	}
    }
    else {
	if (!(yp -> yp_flags & YP_TAG))
	    tag_type (yp);
	(void) printf ("%*scase PE_ID (PE_CLASS_%s, %d):", level * 4, "",
		pe_classlist [yp -> yp_tag -> yt_class],
		val2int (yp -> yp_tag -> yt_value));

	result = 0;
    }
    if (yp -> yp_flags & YP_ID)
	(void) printf ("\t/* %s */", yp -> yp_id);
    (void) printf ("\n");
    level++;

    (void) printf ("%*s{\n", level * 4, "");
    level++;

    if (yp -> yp_flags & YP_TAG) {
	if ((yp -> yp_flags & YP_IMPLICIT) == 0 ||
	    is_nonimplicit_type (yp))
	    pullup = 1;
    }
    if (pullup) {
	(void) printf ("%*sregister PE %s = %s;\n\n", level * 4, "",
		narg2 = gensym (), narg);
	tag_pullup (yp, level, narg2, id);
	(void) printf ("%*s{\n", level * 4, "");
	level++;
	yp -> yp_flags |= YP_PULLEDUP;
    }
    else
	narg2 = narg;

    undo_type (yp, level, id, narg2, Vflag);

    if (pullup) {
	level--;
	(void) printf ("%*s}\n", level * 4, "");
    }

    level--;
    (void) printf ("%*s}\n%*sbreak;\n", level * 4, "", level * 4, "");

    return result;
}

static undo_components_seq (yp, level, first, last, id, arg, narg, Vflag)
YP	yp;
register int level, first, last;
register char	*id,
    		*arg,
    		*narg;
int	Vflag;
{
    YP	newyp;
    YP	y;
    int	i = 0;

    if (yp -> yp_module) {
	pyyerror (yp, "Can't do COMPONENTS OF with external types for %s",
		  yp -> yp_identifier);
	return i;
    }

    if (!(newyp = lookup_type (yp->yp_module, yp -> yp_identifier))) {
	pyyerror (yp, "Can't find referenced COMPONENTS OF %s",
		  yp->yp_identifier);
	return i;
    }

    for (y = newyp -> yp_type; y; y = y -> yp_next) {
	if (y -> yp_flags & YP_COMPONENTS)
	    i += undo_components_seq (y, level, first && y == yp -> yp_type,
					last && y -> yp_next == NULLYP,
					id, arg, narg, Vflag);
	else {
	    undo_type_element (y, level, first && y == newyp -> yp_type,
			       last && y -> yp_next == NULLYP, id,
			       arg, narg, Vflag);
	    i ++;
	}
    }
    return i;
}

static	undo_components_set (yp, level, arg, narg, Vflag)
register YP	yp;
register int	level;
char   *arg,
       *narg;
int	Vflag;
{
    YP	newyp, y;

    if (yp -> yp_module) {
	pyyerror (yp, "Can't do COMPONENTS OF with external types for %s",
		  yp -> yp_identifier);
	return;
    }

    if (!(newyp = lookup_type (yp->yp_module, yp -> yp_identifier))) {
	pyyerror (yp, "Can't find referenced COMPONENTS OF %s",
		  yp->yp_identifier);
	return;
    }
    if (newyp -> yp_code != YP_SETLIST) {
	yyerror_aux ("COMPONENTS OF type is not a SET");
	print_type (newyp, 0);
	return;
    }

    choice_pullup (newyp, CH_PARTIAL);
    for (y = newyp -> yp_type; y; y = y ->yp_next)
	if (y -> yp_flags & YP_COMPONENTS)
	    undo_components_set (y, level, arg, narg, Vflag);
	else
	    undo_type_member (y, level, arg, narg, Vflag);
    choice_pullup (newyp, CH_FULLY);
}
