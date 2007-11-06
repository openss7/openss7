...

    IF (containsType (s)) THEN
        dlist = keyedSearch (base, lhs (s), rhs (s));
    ELSE
        IF (length (base) == 0) THEN
            dlist = rootSearch (s);
        ELSE
            dlist = intermediateSearch (base, s);

    matches = NULL;
    FOR x IN dlist DO
        matches += purportedMatch (x, tail (p));

    RETURN matches;
}
