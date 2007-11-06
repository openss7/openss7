search (base: DN; subsettype: SubsetType; s: string;
        alist: SET OF AttributeType;
        matchtypes: SET OF MatchType;
        objectClasses: SET OF ObjectClass): SET OF DN
{
    x: DN;
    dlist, exact, good: SET OF DN;

    -- map onto Directory Search
    -- baseObject: `base'
    -- subset: `subsetType'
    -- filter:
    --     build OR-conjuction of `s' using `matchtypes' over `alist'
    --     if `objectClasses' isn't empty:
    --         build OR-conjuction of `objectClasses' using equality
    --             over objectClass
    --         AND with original filter
    -- selection: attributes and values of
    --     countryName, friendlyCountryName, organizationName
    --     organizationalUnitName, localityName,
    --     commonName, surName, userId

    -- cache returned entries and associated attributes/values
    dlist = search-results;

...
