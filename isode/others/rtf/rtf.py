-- rtf.py - RTF definitions

-- $Header: /xtel/isode/isode/others/rtf/RCS/rtf.py,v 9.0 1992/06/16 12:48:07 isode Rel $
--
--
-- $Log: rtf.py,v $
-- Revision 9.0  1992/06/16  12:48:07  isode
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

RTF DEFINITIONS ::=

BEGIN

Request ::=
    SEQUENCE {
	user[0]
	    IMPLICIT IA5String,

	password[1]
	    IMPLICIT IA5String,

	file[2]
	    IMPLICIT IA5String
    }

END
