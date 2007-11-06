PurportedName ::=       -- inverted, e.g., { "us", "psi", "mrose" }
    SEQUENCE OF
        String

RDN ::=
    DAV
    -- simplification, as potentially multi-value

DAV ::=
    SEQUENCE {
        type
            OBJECT IDENTIFIER,
        value
            ANY DEFINED BY type
    }
