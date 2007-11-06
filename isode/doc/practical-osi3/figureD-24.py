Query-RSP ::=
    [3] IMPLICIT
        SEQUENCE {
            friendly[0]                 -- friendly name
                IA5String
                OPTIONAL,

            name[1]                     -- a DN in Directory ASN
                ANY
                OPTIONAL,

            value[2]                    -- a PSAPaddr in Directory ASN
                ANY
                OPTIONAL,

            diagnostic[3]               -- in case of error
                IA5String
                OPTIONAL
        }
