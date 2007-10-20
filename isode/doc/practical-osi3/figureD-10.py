intermediateSearch (base: DN; s: String): SET OF DN
{
    IF (present (base, organizationalUnitName)) THEN
        RETURN search (base, oneLevel, s,
                       { organizationalUnitName },
                       { substrings, approximateMatch },
                       { organizationalUnit });
    ELSE
        IF (present (base, OrganizationName)) THEN
            RETURN search (base, oneLevel, s,
                           { organizationalUnitName, localityName },
                           { substrings, approximateMatch },
                           { organizationalUnit, locality });
        ELSE
            IF (present (base, localityName)) THEN
                RETURN search (base, oneLevel, s,
                               { organizationName },
                               { substrings, approximateMatch },
                               { organization });
            ELSE
                RETURN search (base, oneLevel, s,
                               { organizationName, localityName },
                               { substrings, approximateMatch },
                               { organization, locality });
}
