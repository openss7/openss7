EnvironmentList ::=
    SEQUENCE OF
        EnvironmentItem

EnvironmentItem ::=
    SEQUENCE {
        lower-bound
            INTEGER,
        upper-bound
            INTEGER,
        environment
            Environment
    }

Environment ::=
    SEQUENCE OF
        DN

DN ::=
    SEQUENCE OF
        RDN
