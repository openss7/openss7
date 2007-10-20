-- dase.py - co-resident DASE

-- $Header: /xtel/isode/isode/acsap/RCS/dase.py,v 9.0 1992/06/16 12:05:59 isode Rel $
--
--
-- $Log: dase.py,v $
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


DASE DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/dase.py,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif
%}

BEGIN

Query-REQ ::=
    [0] IMPLICIT
	SEQUENCE {
	    name			-- e.g., "cs, ucl, gb"
		SEQUENCE OF
		    IA5String,

	    interactive			-- true IFF allow callbacks
		BOOLEAN,

	    envlist			-- search list
		SEQUENCE OF
		    Environment,

	    context			-- e.g., "iso ftam"
		IA5String,

	    userdn			-- DN for binding
		IA5String
		OPTIONAL,

	    passwd			-- for simple authentication
		IA5String
		OPTIONAL
	}

Environment ::=
	SEQUENCE {
	    upper
		INTEGER,

	    lower
		INTEGER,

	    path
		SEQUENCE OF
		    IA5String
	}

Callback-REQ ::=
    [1] IMPLICIT
	SEQUENCE {
	    string			-- e.g., "smith"
		IA5String,

	    choices			-- list of possible matches
		SEQUENCE OF
		    Pair
	}

Pair ::=
	SEQUENCE {
	    friendly
		IA5String,

	    complete
		IA5String
	}

Callback-RSP ::=
    [2] IMPLICIT
	SEQUENCE OF
	    IA5String

Query-RSP ::=
    [3] IMPLICIT
	SEQUENCE {
	    friendly[0]			-- friendly name
		IA5String
		OPTIONAL,

	    name[1]			-- a DN in Directory ASN
		ANY
		OPTIONAL,

	    value[2]			-- a PSAPaddr in Directory ASN
		ANY
		OPTIONAL,

	    diagnostic[3]		-- in case of error
		IA5String
		OPTIONAL
	}


-- auxiliary types to make coding easier

Message ::=
	CHOICE {
	    query-request
		Query-REQ,

	    callback-request
		Callback-REQ,

	    callback-response
		Callback-RSP,

	    query-response
		Query-RSP
	}

Provider-RSP ::=
	CHOICE {
	    callback
		Callback-REQ,

	    answer
		Query-RSP
	}

END
