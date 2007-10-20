Table ::=
    SET OF
        SEQUENCE {
            kind
                ObjectClass,
            base
                DN
        }
DN ::=
    SEQUENCE OF
	RDN

RDN ::=
    SET OF
	DAV

DAV ::=
    SEQUENCE {
	type
	    OBJECT IDENTIFIER,

	value
	    ANY DEFINED BY type
    }
