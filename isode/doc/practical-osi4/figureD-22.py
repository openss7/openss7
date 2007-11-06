aeSearch (base: DN; sac: OBJECT IDENTIFIER;
          subtreeType: SubtreeType): SET OF DN
{
    -- map onto Directory Search
    -- baseObject: `base'
    -- subset: `subtreeType'
    -- filter:
    --     build filter-item of `sac' using equality over
    --         supportedApplicationContext
    -- selection: all attributes and values

    -- cache returned entries and associated attributes/values
    RETURN search-results;
}
