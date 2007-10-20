rootSearch (s: String): SET OF DN
{
    IF (is3166 (s)) THEN
        RETURN search (NULL, oneLevel, s,
                       { countryName, friendlyCountryName, organizationName },
                       { equality },
                       NULL);
    ELSE
        RETURN search (NULL, oneLevel, s,
                       { friendlyCountryName, organizationName, localityName },
                       { substrings, approximateMatch },
                       NULL);
}
