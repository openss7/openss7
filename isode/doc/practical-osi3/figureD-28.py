mboxSearch (base: DN; s: String; subsettype: SubsetType): SET OF DN
{
    -- map onto Directory Search
    -- baseObject: `base'
    -- subset: `subsetType'
    -- filter:
    --     build filter-item of `s' using substrings over rfc822Mailbox

    RETURN search-results;
}
