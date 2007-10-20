leafSearch (base: DN; s: String; subsettype: SubsetType): SET OF DN
{
    RETURN search (base, subsettype, s,
                   { commonName, surName, userId},
                   { substrings, approximateMatch },
                   NULL);
}


keyedSearch (base: DN; type, value: String): SET OF DN
{
    RETURN search (base,
                   length (base) > 1 ? wholeSubtree : oneLevel,
                   value,
                   { str2Attr (type) },
                   { substrings, approximateMatch },
                   NULL);
}
