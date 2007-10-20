ProtectedPassword ::=
        SEQUENCE {
        algorithm [0] AlgorithmIdentifier OPTIONAL,
        salt [1] SET {
                time1 [0] UTCTime OPTIONAL,
                time2 [1] UTCTime OPTIONAL,
                random1 [2] BIT STRING OPTIONAL,
                random2 [3] BIT STRING OPTIONAL}
                OPTIONAL,
        password [2] OCTET STRING}
