dnsMatch (dns: Domain; base: DN; matches: SET OF DN): SET OF DN
{
    i: INTEGER;
    x: DN;
    dlist, dprev: SET OF DN;
    dp: Domain;

    dp = dns;

    FOR (; TRUE; )
        IF ((i = length (dp)) < dnsLevel)
            RETURN matches;

        -- map onto Directory Search
        -- baseObject: `base'
        -- subset: oneLevel
        -- filter:
        --     build filter-item of `dp' using equality over associatedDomain

...
