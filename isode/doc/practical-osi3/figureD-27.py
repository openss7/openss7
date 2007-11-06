Domain ::=              -- e.g., "xtel", "co", "uk"
    SEQUENCE OF
        String

mboxLookup (local: String; domain: Domain): SET OF DN
{
    x: DN;
    dlist, matches: SET OF DN;

    if ((dlist = dnsLookup (domain)) == NULL) THEN
        RETURN NULL;

    IF (length (dlist) > 1) THEN
        -- ask user to prune dlist
        ;

    matches = NULL;
    FOR x IN dlist DO
        IF (length (x) >= 2) THEN
            matches += mboxSearch (x, local, wholeSubtree);
        ELSE
            -- warn user that domain did not resolve below country-level
            ;

    RETURN matches;
}
