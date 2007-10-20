friendlyMatch (p: PurportedName; el: EnvironmentList): SET OF DN
{
    e: EnvironmentItem;

    FOR (e = head (el); e; e = head (el = tail (el)))
        IF (length (p) <= e.upper-bound
                && length (p) >= e.lower-bound) THEN
            RETURN envMatch (p, e.environment);

    RETURN NULL;
}


envMatch (p: PurportedName; e: Environment): SET OF DN
{
    x: DN;
    matches: SET OF DN;

    FOR (x = head (e); x; x = head (e = tail (e)))
        IF (matches = purportedMatch (x, p)) THEN
            RETURN matches;
}
