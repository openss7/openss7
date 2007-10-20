-- open PDU (currently only simple authentication)

SimpleOpen ::=
    [APPLICATION 0] IMPLICIT
	SEQUENCE {
	    version     -- of SMUX protocol
		INTEGER {
		    version-1(0)
		},

	    identity    -- of SMUX initiator, authoritative
		OBJECT IDENTIFIER,

	    description -- of SMUX initiator, implementation-specific
		DisplayString,

	    password    -- zero length indicates no authentication
		OCTET STRING
	}
