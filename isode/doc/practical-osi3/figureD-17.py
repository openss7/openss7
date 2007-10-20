twoLevelMatch (kind: ObjectClass; name, qual, location: String;
               bases: Table): SET OF DN
{
    x: DN;
    dlist, matches: SET OF DN;

    matches = NULL;
    IF (location) THEN
        IF (dlist = simpleMatch ("organization", location, NULL,
                                 findTable ("organization", bases))) THEN
            IF (length (dlist) > 1) THEN
                FOR x IN dlist DO
                    IF (userSaysSo (x)) THEN
                        matches += simpleMatch (kind, name, qual, x);
                RETURN matches;
            ELSE
                RETURN simpleMatch (kind, name, qual, head (dlist));
        ELSE
            -- tell user nowhere to look
            RETURN NULL;
    ELSE
        RETURN simpleMatch (kind, name, qual, findTable (kind, bases));
}
