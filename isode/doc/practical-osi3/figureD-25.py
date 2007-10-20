Callback-REQ ::=
    [1] IMPLICIT
        SEQUENCE {
            string                      -- e.g., "smith"
                IA5String,

            choices                     -- list of possible matches
                SEQUENCE OF
                    SEQUENCE {
                        friendly
                            IA5String,
                        complete
                            IA5String
                        }
        }

Callback-RSP ::=
    [2] IMPLICIT
        SEQUENCE OF
            IA5String
