/* std_sntx.c - invoke standard syntax handlers */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/std_sntx.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/std_sntx.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: std_sntx.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

standard_syntaxes ()
{
	string_syntaxes ();
	cilist_syntax ();
	dn_syntax ();
	psap_syntax ();
	objectclass_syntax ();
	oid_syntax ();
	time_syntax ();
	boolean_syntax ();
	integer_syntax ();
	fax_syntax ();
	post_syntax ();
	telex_syntax ();
	teletex_syntax ();
	pref_deliv_syntax ();
	guide_syntax ();
	certificate_syntax ();
	certificate_pair_syntax ();
}
