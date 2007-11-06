-- th-enc.py -  Thorn and RARE defined attribute sytaxes
--		(Now COSINE/Internet !)

-- $Header: /xtel/isode/isode/dsap/x500as/RCS/th.py,v 9.0 1992/06/16 12:14:33 isode Rel $
--
--
-- $Log: th.py,v $
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


Thorn
	{
	ccitt
	data(9)
	pss(2342)
	ucl(19200300)
	thorn(100)
	directoryDefinitions(1)
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

MailBox [[P struct mailbox *]]
        ::=
        SEQUENCE
        {
	mailboxType
		PrintableString [[ s mtype ]],
	mailbox
		IA5String	[[ s mbox ]]
        }

DocumentStoreSyntax [[P struct documentStore *]]
	::=
	SEQUENCE {
	    method
		INTEGER		[[ i ds_method ]]
		{ ftp(0), ftam(1) },

	    textEncodedHostName
		IA5String	[[ s ds_host ]],

	    directoryName[0]
		IA5String	[[ s ds_dir ]]
		OPTIONAL,

	    fileFsName
		IA5String	[[ s ds_file ]]
	}

DSAQualitySyntax [[P struct dsaQoS *]]
	::=
	SEQUENCE {
	    serviceQuality
		ENUMERATED	[[ i dsa_quality ]]
		{ defunct       (0),
		  experimental  (1),
		  best-effort   (2),
		  pilot-service (3),
		  full-service  (4) },

	    description
		PrintableString [[ s dsa_description ]]
		OPTIONAL
	}

DataQualitySyntax [[P struct ditQoS *]]
	::=
	SEQUENCE {
	    namespace-completeness
		ENUMERATED	[[ i dit_namespace ]]
		{ none        (1),
		  sample      (2),
		  selected    (3),
		  substantial (4),
		  full        (5) },

	    defaultAttributeQuality
		AttributeQuality [[ p dit_default ]],

	    attributeQuality
		SET OF [[ T struct attrsQoS * $ dit_attrs ]] << dit_next >>
		    SEQUENCE [[ T struct attrsQoS * $ *]] {
			AttributeType [[ p dit_type ]],

			AttributeQuality [[ p dit_quality ]]
		    },

	    description
		PrintableString [[ s dit_description ]]
		OPTIONAL
	}

AttributeQuality [[P struct attrQoS *]]
	::=
	SEQUENCE {
	%D{ (*parm) -> attr_completeness = 5; %}
	    maintenance-level
		ENUMERATED	[[ i attr_level ]]
		{ unknown           (1),
		  external          (2),
		  system-maintained (3),
		  user-supplied     (4)
		},

	    attribute-completeness
		ENUMERATED	[[ i attr_completeness ]]
		{ none        (1),
		  sample      (2),
		  selected    (3),
		  substantial (4),
		  full        (5) }
		-- DEFAULT full
		OPTIONAL <E<parm -> attr_completeness != 5>> <D<0>>
	}

END
