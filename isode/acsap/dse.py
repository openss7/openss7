-- dse.py - X.500 PresentationAddress syntax

-- $Header: /xtel/isode/isode/acsap/RCS/dse.py,v 9.0 1992/06/16 12:05:59 isode Rel $
--
--
-- $Log: dse.py,v $
-- Revision 9.0  1992/06/16  12:05:59  isode
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


DSE DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/dse.py,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif
%}

BEGIN

PSAPaddr ::=
	SEQUENCE {
	    pSelector[0]
		OCTET STRING
		OPTIONAL,

	    sSelector[1]
		OCTET STRING
		OPTIONAL,

	    tSelector[2]
		OCTET STRING
		OPTIONAL,

	    nAddress[3]
		SET OF
		    OCTET STRING
	}

END
