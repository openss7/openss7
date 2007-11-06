-- sa.py

-- $Header: /xtel/isode/isode/dsap/x500as/RCS/sa.py,v 9.0 1992/06/16 12:14:33 isode Rel $
--
--
-- $Log: sa.py,v $
-- Revision 9.0  1992/06/16  12:14:33  isode
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


SA
	{
	joint-iso-ccitt
	ds(5)
	modules(1)
	selectedAttributeTypes(5)
	}

DEFINITIONS ::=

PREFIXES encode decode print

BEGIN

IMPORTS
	AttributeType
		FROM IF
			{
			joint-iso-ccitt
			ds(5)
			modules(1)
			informationFramework(1)
			};

FacsimileTelephoneNumber [[P struct fax *]]
        ::=
        SEQUENCE
        {
	telephoneNumber
		PrintableString [[s number]],
	parameters
		BIT STRING [[x fax_bits $ fax_len ]] 
                OPTIONAL

        }

PostalAddress [[P struct postaddr *]]
	::=
	SEQUENCE OF [[T struct postaddr * $ *]] <<pa_next>>
	CHOICE [[T struct postaddr * $ *]] << addrtype >>
		{
		T61String [[s addrcomp]],
		PrintableString [[s addrcomp]]
		}

TelexNumber [[P struct telex *]]
	::=
	SEQUENCE
	{
		telexNumber	PrintableString [[s telexnumber]],
		countryCode	PrintableString [[s countrycode]],
		answerback	PrintableString [[s answerback]]
	}

TeletexTerminalIdentifier [[P struct teletex *]]
	::=
	SEQUENCE
	{
	teletexTerminal
		PrintableString [[s terminal]],
	parameters SET [[T struct teletex * $ *]] {
	        graphic-character-sets [0] TeletexString
			[[s parm->graphic]] OPTIONAL,
	        control-character-sets [1] TeletexString
			[[s parm->control]] OPTIONAL,
	        page-formats [2] OCTET STRING 
			[[s parm->page]] OPTIONAL,
	        miscellaneous-terminal-capabilities [3] TeletexString
			[[s parm->misc]] OPTIONAL,
	        private-use [4] OCTET STRING 
			[[s parm->t_private]] OPTIONAL
		} OPTIONAL <E<
		    ((parm->graphic != NULLCP) || 
		     (parm->control != NULLCP) ||
		     (parm->page != NULLCP) || 
		     (parm->misc != NULLCP) ||
		     (parm->t_private != NULLCP)) 
		     >>
		     <D<0>>
	}

PreferredDeliveryMethod [[P struct pref_deliv *]]
	::=
	SEQUENCE OF [[T struct pref_deliv * $ * ]] <<pd_next>>
	INTEGER [[i deliv ]]
	{
	any-delivery-method	(0),
	mhs-delivery		(1),
	physical-delivery	(2),
	telex-delivery		(3),
	teletex-delivery	(4),
	g3-facsimilie-delivery	(5),
	g4-facsimilie-delivery	(6),
	ia5-terminal-delivery	(7),
	videotex-delivery	(8),
	telephone-delivery	(9)
	}

CriteriaItem [[P struct CriteriaItem *]]
    ::=
         CHOICE <<offset>> {
                    equality[0]
                        AttributeType
                        [[p attrib ]],

                    substrings[1]
                        AttributeType
                        [[p attrib ]],

                    greaterOrEqual[2]
                        AttributeType
                        [[p attrib ]],

                    lessOrEqual[3]
                        AttributeType
                        [[p attrib ]],

                    approximateMatch[4]
                        AttributeType
                        [[p attrib ]]
                
        }


Criteria [[P struct Criteria *]]
    ::=
        CHOICE <<offset>>
        {
            type[0]
                CriteriaItem [[p un.type ]],

            and[1]
                SET OF [[T struct and_or_set * $ un.and_or ]] << and_or_next >>
                    Criteria [[p and_or_comp]],

            or[2]
                SET OF [[T struct and_or_set * $ un.and_or ]] << and_or_next >>
                    Criteria [[p and_or_comp]],

            not[3]
                Criteria [[p parm -> un.not ]]
        }



Guide [[ P struct Guide *]]
    ::=
    SET {
        objectClass [0]
                OBJECT IDENTIFIER [[O objectClass ]] OPTIONAL,
	criteria [1]
		Criteria [[p criteria]]
    }

NadfGuide [[ P struct Guide *]]
	::=
	SET {
	%D{ (*parm) -> subset = 1; %}
            objectClass [0]
                OBJECT IDENTIFIER [[O objectClass ]],
	    criteria [1]
		Criteria [[p criteria]],
	    subset [2]
		INTEGER [[i subset]] {
		    baseObject(0), oneLevel(1), wholeSubtree(2)
		}
		-- DEFAULT oneLevel
		OPTIONAL <E<parm -> subset != 1>> <D<0>>
	}

CaseIgnoreList [[P struct CIList *]]
	::=
	SEQUENCE OF [[T struct CIList * $ *]] << l_next>>
	CHOICE [[T struct CIList * $ *]] << l_type >>
		{
		T61String [[s l_str]],
		PrintableString [[s l_str]]
		}

END
