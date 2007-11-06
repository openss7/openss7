...

    IF (x = findCache (base, { aliasedObjectname })
        base = x;

    -- map onto Directory Search
    -- baseObject: `base'
    -- subset: `subsetType'
    -- filter:
    --     build OR-conjuction of `s' using approximateMatch over `alist'
    --     if `qual' isn't empty:
    --         build filter-item of `qual' using equality over `qlist'
    --         AND with original filter
    -- selection: attributes and values of
    --     telephoneNumber, rfc822Mailbox

    -- cache returned entries and associated attributes/values

    RETURN search-results;
}
