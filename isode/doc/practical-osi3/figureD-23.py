Query-REQ ::=
    [0] IMPLICIT
        SEQUENCE {
            name                        -- e.g., { "cs", "ucl", "gb" }
                SEQUENCE OF
                    IA5String,

            interactive                 -- true IFF allow callbacks
                BOOLEAN,

            envlist                     -- search list
                SEQUENCE OF
                    Environment,

            context                     -- e.g., "iso ftam"
                IA5String,

            userdn                      -- DN for binding
                IA5String
                OPTIONAL,

            passwd                      -- for simple authentication
                IA5String
                OPTIONAL
        }
