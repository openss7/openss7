/* quipu_sntx.c - invoke quipu syntax handlers */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/quipu_sntx.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/quipu_sntx.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: quipu_sntx.c,v $
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

quipu_syntaxes ()
{
static char done = 0;

	if (done++) 
	    return;

	standard_syntaxes ();
	acl_syntax ();
	edbinfo_syntax ();
	schema_syntax ();
	photo_syntax ();
	protected_password_syntax();
	inherit_syntax ();
	audio_syntax ();
	ap_syntax ();
	attribute_syntax ();

	/* Thorn syntaxes */
	mailbox_syntax ();
	documentStore_syntax ();
	QoS_syntax ();
	sacl_syntax();
	lacl_syntax();
	authp_syntax();

	/* DSA control */
	dsa_control_syntax();
	quipu_call_syntax();
}
