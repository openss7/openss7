-- T73 Defined Types (stub)

-- $Header: /xtel/isode/isode/pepy/RCS/T73.py,v 9.0 1992/06/16 12:25:01 isode Rel $
--
--
-- $Log: T73.py,v $
-- Revision 9.0  1992/06/16  12:25:01  isode
-- Release 8.0
--
--

--
--				  NOTICE
--
--    Acquisition, use, and distribution of this module and related
--    materials are subject to the restrictions of a license agreement.
--    Consult the Preface in the User's Manual for the full terms of
--    this agreement.
--
--


T73 DEFINITIONS ::=


%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/T73.py,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif
%}

BEGIN

PRINTER	print

ProtocolElement ::=
	ANY

PresentationCapabilities ::=
	SET { --unimportant-- }

END
