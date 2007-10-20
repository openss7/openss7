aeLookup (p: PurportedName; sac: OBJECT IDENTIFIER;
          el: EnvironmentList): SET OF DN
{
    x: DN;
    dlist, matches: SET OF DN;

    IF ((dlist = friendlyMatch (p, el)) == NULL) THEN
        RETURN NULL;

    matches = NULL;
    FOR x IN dlist DO
        matches += aeSearch (x, sac, oneLevel);

    IF (matches == NULL) THEN
        FOR x IN dlist DO
            IF (length (x) >= 2)
                matches += aeSearch (x, sac, wholeSubtree);

    RETURN matches;
}
