-- EAN Defined Types

-- $Header: /xtel/isode/isode/pepy/RCS/EAN.py,v 9.0 1992/06/16 12:25:01 isode Rel $
--
--
-- $Log: EAN.py,v $
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


EAN DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/EAN.py,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif
%}

BEGIN

PRINTER	print

-- P1: EAN stores the Content of the message after the MPDU, not inside it

MPDU ::=
	CHOICE {
	    [0]
		IMPLICIT UserMPDU,

		ServiceMPDU
	}

ServiceMPDU ::=
	CHOICE {
	    [1]
		IMPLICIT P1.DeliveryReportMPDU,

	    [2]
		IMPLICIT P1.ProbeMPDU
	}

UserMPDU ::=
	SEQUENCE { P1.UMPDUEnvelope }


-- P2: EAN considers the Body OPTIONAL

UAPDU ::=
	CHOICE {
	    [0]
		IMPLICIT IM-UAPDU,

	    [1]
		IMPLICIT P2.SR-UAPDU
	}

IM-UAPDU ::=
	SEQUENCE {
	    heading
		P2.Heading,

	    body
		P2.Body
		OPTIONAL
	}

END
