-- P3 Defined Types (stub)

-- $Header: /xtel/isode/isode/pepy/RCS/P3.py,v 9.0 1992/06/16 12:25:01 isode Rel $
--
--
-- $Log: P3.py,v $
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


P3 DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/P3.py,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif
%}

BEGIN

PRINTER	print

DeliverEnvelope ::=
	SET {
	    [0]
		IMPLICIT P1.ContentType,

	    originator
		P1.ORName,

	    original[1]
		IMPLICIT P1.EncodedInformationTypes,

		P1.Priority DEFAULT normal,

	    [2]
		IMPLICIT DeliveryFlags,

	    otherRecipients[3]
		IMPLICIT SEQUENCE OF P1.ORName
		OPTIONAL,

	    thisRecipient[4]
		IMPLICIT P1.ORName,

	    intendedRecipient[5]
		IMPLICIT P1.ORName
		OPTIONAL,

	    converted[6]
		IMPLICIT P1.EncodedInformationTypes,

	    submission[7]
		IMPLICIT P1.Time
	}

DeliveryFlags ::=
	BITSTRING { conversionProhibited(2) }

END
