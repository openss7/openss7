-- ry.ry - ROSY remote operations definitions

-- $Header: /xtel/isode/isode/ronot/RCS/ry.py,v 9.0 1992/06/16 12:36:36 isode Rel $
--
--
-- $Log: ry.py,v $
-- Revision 9.0  1992/06/16  12:36:36  isode
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


Remote-Operations-Notation {joint-iso-ccitt remote-operations(4) notation(0)}

DEFINITIONS ::=

BEGIN

SECTIONS build parse print

BindArgumentValue ::= 
	[16] ANY

BindResultValue ::=
	[17] ANY

BindErrorValue ::= 
	[18] ANY

UnBindArgumentValue ::=
	[19] ANY

UnBindResultValue ::=
	[20] ANY

UnBindErrorValue ::=
	[21] ANY

END
