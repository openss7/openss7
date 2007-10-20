/* qos.c - DSA/DIT QoS handling */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/qos.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/qos.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: qos.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
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


/*
	SYNTAX:

		dsaQuality ::=  <keyword> ['#' <description>]

		ditQuality ::=  <keyword> '#' <attrQuality>
					  '#' <listQuality>
					 ['#' <description>]
		attrQuality ::= <keyword> '+' <keyword>
		listQuality ::= <list> '$' <list> <listQuality>
		list ::=        <attribute> '+' <attrQuality>
 */

#include "quipu/util.h"
#include "cmd_srch.h"
#define	cmd_srch	CMD_SRCH
#include "quipu/syntaxes.h"

static int  CMD_SRCH ();

static dsaQoS_free (a)
struct dsaQoS *a;
{
    if (!a)
	return;

    if (a -> dsa_description)
	free (a -> dsa_description);

    free ((char *) a);
}

static struct dsaQoS *dsaQoS_cpy (a)
struct dsaQoS *a;
{
    struct dsaQoS *b;

    b = (struct dsaQoS *) smalloc (sizeof *b);
    bzero ((char *) b, sizeof *b);

    b -> dsa_quality = a -> dsa_quality;
    if (a -> dsa_description)
	b -> dsa_description = strdup (a -> dsa_description);

    return b;
}

static	dsaQoS_cmp (a, b)
struct dsaQoS *a;
struct dsaQoS *b;
{
    int	    res;

    if (!a)
	return (b ? (-1) : 0);
    else
	if (!b)
	    return 1;

    if (res = b -> dsa_quality - a -> dsa_quality)
	return (res > 0 ? 1 : -1);
    return lexequ (a -> dsa_description, b -> dsa_description);
}

static CMD_TABLE dsaQoS_tab[] = {
    "DEFUNCT",		DSA_DEFUNCT,
    "EXPERIMENTAL",	DSA_EXPERIMENTAL,
    "BEST-EFFORT",	DSA_BESTEFFORT,
    "PILOT-SERVICE",	DSA_PILOT,
    "FULL-SERVICE",	DSA_FULL,
    NULL,		-1
};

static	dsaQoS_print (ps, a, format)
PS	ps;
struct dsaQoS *a;
int	format;
{
    char   *ptr = rcmd_srch (a -> dsa_quality, dsaQoS_tab);

    if (format == READOUT)
	    ps_printf (ps, "%s", ptr ? ptr : "UNKNOWN !!!");	
    else
	    ps_printf (ps, "%s", ptr ? ptr : "DEFUNCT");

    if (a -> dsa_description)
	ps_printf (ps, format == READOUT ? " (%s)" : " # %s",
		   a -> dsa_description);
}

static struct dsaQoS *str2dsaQoS (str)
char   *str;
{
    int	    quality;
    char   *ptr;
    struct dsaQoS *a;

    if (ptr = index (str, '#'))
	*ptr = NULL;
    quality = cmd_srch (str, dsaQoS_tab);
    if (ptr)
	*ptr = '#';

    if (quality == -1) {
	parse_error ("Unknown serviceQuality in DSAQualitySyntax %s", str);
	return NULL;
    }

    a = (struct dsaQoS *) smalloc (sizeof *a);
    bzero ((char *) a, sizeof *a);
    a -> dsa_quality = quality;

    if (ptr)
	a -> dsa_description = strdup (SkipSpace (ptr + 1));

    return a;
}

static PE dsaQoS_enc (a)
struct dsaQoS *a;
{
    PE	    pe;

    (void) encode_Thorn_DSAQualitySyntax (&pe, 0, 0, NULLCP, a);

    return pe;
}

static struct dsaQoS *dsaQoS_dec (pe)
PE	pe;
{
    struct dsaQoS *a;

    if (decode_Thorn_DSAQualitySyntax (pe, 1, NULLIP, NULLVP, &a) == NOTOK)
	return NULL;

    return a;
}

static attrQoS_free (a)
struct attrQoS *a;
{
    if (!a)
	return;

    free ((char *) a);
}

static ditQoS_free (a)
struct ditQoS *a;
{
    register struct attrsQoS *p,
			     *q;

    if (!a)
	return;

    if (a -> dit_default)
	attrQoS_free (a -> dit_default);

    for (p = a -> dit_attrs; p; p = q) {
	q = p -> dit_next;

	AttrT_free (p -> dit_type);
	attrQoS_free (p -> dit_quality);
	free ((char *) p);
    }

    if (a -> dit_description)
	free (a -> dit_description);

    free ((char *) a);
}

static struct attrQoS *attrQoS_cpy (a)
struct attrQoS *a;
{
    struct attrQoS *b;

    b = (struct attrQoS *) smalloc (sizeof *b);
    *b = *a;	/* struct copy */

    return b;
}

static struct ditQoS *ditQoS_cpy (a)
struct ditQoS *a;
{
    struct ditQoS *b;
    register struct attrsQoS  *p,
			     **q,
			      *r;

    b = (struct ditQoS *) smalloc (sizeof *b);
    bzero ((char *) b, sizeof *b);

    b -> dit_namespace = a -> dit_namespace;

    b -> dit_default = attrQoS_cpy (a -> dit_default);

    q = &b -> dit_attrs;
    for (r = a -> dit_attrs; r; r = r -> dit_next) {
	p = (struct attrsQoS *) smalloc (sizeof *p);
	*q = p,	q = &p -> dit_next, *q = NULL;

	p -> dit_type = AttrT_cpy (r -> dit_type);
	p -> dit_quality = attrQoS_cpy (r -> dit_quality);
    }

    if (a -> dit_description)
	b -> dit_description = strdup (a -> dit_description);

    return b;
}

static	attrQoS_cmp (a, b)
struct attrQoS *a;
struct attrQoS *b;
{
    int	    res;

    if (!a)
	return (b ? (-1) : 0);
    else
	if (!b)
	    return 1;

    if (res = b -> attr_level - a -> attr_level)
	return (res > 0 ? 1 : -1);
    if (res = b -> attr_completeness - a -> attr_completeness)
	return (res > 0 ? 1 : -1);
    return 0;
}

static	ditQoS_cmp (a, b)
struct ditQoS *a;
struct ditQoS *b;
{
    int	    res;
    struct attrsQoS *p,
		    *r;

    if (!a)
	return (b ? (-1) : 0);
    else
	if (!b)
	    return 1;

    if (res = b -> dit_namespace - a -> dit_namespace)
	return (res > 0 ? 1 : -1);
    if (res = attrQoS_cmp (a -> dit_default, b -> dit_default))
	return res;

    for (p = a -> dit_attrs, r = b -> dit_attrs;
	     p && r;
	     p = p -> dit_next, r = r -> dit_next)
	if ((res = AttrT_cmp (p -> dit_type, r -> dit_type))
		|| (res = attrQoS_cmp (p -> dit_quality, r -> dit_quality)))
	    return res;
    if (!p) {
	if (r)
	    return (-1);
    }
    else
	if (!r)
	    return 1;

    return lexequ (a -> dit_description, b -> dit_description);
}

int	qos_indent = 24;

static CMD_TABLE ditQoS_tab[] = {
    "NONE",		DIT_NONE,
    "SAMPLE",		DIT_SAMPLE,
    "SELECTED",		DIT_SELECTED,
    "SUBSTANTIAL",	DIT_SUBSTANTIAL,
    "FULL",		DIT_FULL,
    NULL,		-1
};

static CMD_TABLE attrQoS_tab[] = {
    "UNKNOWN",		ATTR_UNKNOWN,
    "EXTERNAL",		ATTR_EXTERNAL,
    "SYSTEM-MAINTAINED", ATTR_SYSTEM,
    "USER-SUPPLIED",	ATTR_USER,
    NULL,		-1
};

static	attrQoS_print (ps, a, format)
PS	ps;
struct attrQoS *a;
int	format;
{
    char   *p = rcmd_srch (a -> attr_level, attrQoS_tab),
	   *q = rcmd_srch (a -> attr_completeness, ditQoS_tab);

    ps_printf (ps, format == READOUT
			? "%s, completeness %s"
			: "%s + %s",
	       p ? p : "UNKNOWN !!!", q ? q : "UNKNOWN !!!");
}

static	ditQoS_print (ps, a, format)
PS	ps;
struct ditQoS *a;
int	format;
{
    char   *ptr = rcmd_srch (a -> dit_namespace, ditQoS_tab);
    register struct attrsQoS  *p;

    ps_printf (ps, format == READOUT
	       		? "completeness of namespace: %s\n%*sdefault: "
	       			     : "%s # ",
	       ptr ? ptr : "UNKNOWN !!!", qos_indent, "");
    attrQoS_print (ps, a -> dit_default, format);
    if (format != READOUT)
	ps_printf (ps, " #%s", a -> dit_attrs ? "\\\n\t" : "");
    for (p = a -> dit_attrs; p; p = p -> dit_next) {
	if (format == READOUT) {
	    ps_printf (ps, "\n%*s", qos_indent, "");
	    AttrT_print (ps, p -> dit_type, format);
	    ps_printf (ps, ": ");
	}
	else {
	    if (p != a -> dit_attrs)
		ps_printf (ps, "\\\n\t$ ");
	    AttrT_print (ps, p -> dit_type, format);
	    ps_printf (ps, " + ");
	}
	attrQoS_print (ps, p -> dit_quality, format);
    }
    if (a -> dit_description) {
	if (format == READOUT)
		ps_printf (ps, "\n%*s(%s)", qos_indent, 
		   a -> dit_description);
	else
		ps_printf (ps, " # %s", a -> dit_description);
    }
}

static struct attrQoS *str2attrQoS (str)
char *str;
{
    int	    level,
	    completeness;
    char   *ptr;
    struct attrQoS *a;

    if (!(ptr = index (str, '+'))) {
	parse_error ("Missing '+' in AttributeQuality %s", str);
	return NULL;
    }
    *ptr = NULL;
    level = cmd_srch (str, attrQoS_tab);
    *ptr = '+';

    if (level == -1) {
	parse_error ("unknown maintenance-level in AttributeQuality %s", str);
	return NULL;
    }

    if ((completeness = cmd_srch (SkipSpace (ptr + 1), ditQoS_tab)) == -1) {
	parse_error ("unknown attribute-completeness in AttributeQuality %s",
		     str);
	return NULL;
    }

    a = (struct attrQoS *) smalloc (sizeof *a);
    a -> attr_level = level;
    a -> attr_completeness = completeness;

    return a;
}


static struct ditQoS *str2ditQoS (str)
char   *str;
{
    char   *ptr,
	   *qtr;
    struct ditQoS *a;
    register struct attrsQoS  *p,
			     **q;

    a = (struct ditQoS *) smalloc (sizeof *a);
    bzero ((char *) a, sizeof *a);
    if (!(ptr = index (str, '#'))) {
	parse_error ("missing namespace-completeness in DataQualitySyntax %s",
		     str);
out: ;
	ditQoS_free (a);
	return NULL;
    }
    *ptr = NULL;
    a -> dit_namespace = cmd_srch (str, ditQoS_tab);
    *ptr = '#';
    if (a -> dit_namespace == -1) {
	parse_error ("unknown namespace-completeness in DataQualitySyntax %s",
		     str);
	goto out;
    }

    qtr = SkipSpace (ptr + 1);
    if (!(ptr = index (qtr, '#'))) {
	parse_error ("missing defaultAttributeQuality in DataQualitySyntax %s",
		     str);
	goto out;
    }
    *ptr = NULL;
    a -> dit_default = str2attrQoS (qtr);
    *ptr = '#';
    if (!a -> dit_default)
	goto out;

    qtr = SkipSpace (ptr + 1);
    if (ptr = index (qtr, '#'))
	*ptr = NULL;
    q = &a -> dit_attrs;
    while (*qtr) {
	char	u;
	char   *rtr,
	       *ttr,
	       *utr;

	p = (struct attrsQoS *) smalloc (sizeof *p);
	*q = p,	q = &p -> dit_next, *q = NULL;

	if (ttr = index (qtr, '$'))
	    *ttr = NULL;

	if (!(rtr = index (qtr, '+'))) {
	    parse_error ("missing AttributeQuality in attributeQuality %s",
			 qtr);
out2: ;
	    if (ttr)
		*ttr = '$';
	    if (ptr)
		*ptr = '#';
	    goto out;
	}
	*rtr = NULL;
	for (utr = qtr + strlen (qtr) - 1;
	         utr >= qtr && isspace ((u_char) *utr);
	         utr--)
	    continue;
	utr++;
	if ((u = *utr) == NULL || utr == qtr)
	    p -> dit_type = str2AttrT (qtr);
	else {
	    *utr = NULL;
	    p -> dit_type = str2AttrT (qtr);
	    *utr = u;
	}
	*rtr = '+';
	if (!p -> dit_type) {
	    parse_error ("unknown AttributeType in attributeQuality %s",
			 qtr);
	    goto out2;
	}
	if (!(p -> dit_quality = str2attrQoS (SkipSpace (rtr + 1))))
	    goto out2;

	if (ttr)
	    *ttr = '+', qtr = SkipSpace (ttr + 1);
	else
	    break;
    }

    if (ptr)
	a -> dit_description = strdup (SkipSpace (ptr + 1));

    return a;
}

static PE ditQoS_enc (a)
struct ditQoS *a;
{
    PE	    pe;

    (void) encode_Thorn_DataQualitySyntax (&pe, 0, 0, NULLCP, a);

    return pe;
}

static struct ditQoS *ditQoS_dec (pe)
PE	pe;
{
    struct ditQoS *a;

    if (decode_Thorn_DataQualitySyntax (pe, 1, NULLIP, NULLVP, &a) == NOTOK)
	return NULL;

    return a;
}

QoS_syntax ()
{
    (void) add_attribute_syntax ("DSAQualitySyntax",
				 (IFP) dsaQoS_enc,
				 (IFP) dsaQoS_dec,
				 (IFP) str2dsaQoS,
				 dsaQoS_print,
				 (IFP) dsaQoS_cpy,
				 dsaQoS_cmp,
				 dsaQoS_free,
				 NULLCP, NULLIFP, TRUE);
    (void) add_attribute_syntax ("DataQualitySyntax",
				 (IFP) ditQoS_enc,
				 (IFP) ditQoS_dec,
				 (IFP) str2ditQoS,
				 ditQoS_print,
				 (IFP) ditQoS_cpy,
				 ditQoS_cmp,
				 ditQoS_free,
				 NULLCP, NULLIFP, TRUE);
}


#undef	cmd_srch
static int  CMD_SRCH (str, cmd)
char   *str;
CMD_TABLE *cmd;
{
    int	    result;
    char    c;
    register char *p;

    for (p = str + strlen (str) - 1; p >= str && isspace ((u_char) *p); p--)
	continue;
    p++;
    if ((c = *p) == NULL || p == str)
	return cmd_srch (str, cmd);

    *p = NULL;
    result = cmd_srch (str, cmd);
    *p = c;

    return result;
}
