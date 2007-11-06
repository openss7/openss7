purportedMatch (base: DN; p: PurportedName): SET OF DN
{
    s = head (p): String;
    dlist, matches: SET OF DN;

    IF (length (p) == 1) THEN
        IF (containsType (s)) THEN
            RETURN keyedSearch (base, lhs (s), rhs (s));
        ELSE
            IF (length (base) == 0) THEN
                IF (matches = rootSearch (s)) THEN
                    RETURN matches;
                ELSE
                    RETURN leafSearch (base, s, oneLevel);
            ELSE
                IF (length (base) == 1) THEN
                    IF (matches = intermediateSearch (base, s)) THEN
                        RETURN matches;
                    ELSE
                        RETURN leafSearch (base, s, oneLevel);
                ELSE
                    IF (matches = leafSearch (base, s, wholeSubtree)) THEN
                        RETURN matches;
                    ELSE
                        RETURN intermediateSearch (base, s);

...
