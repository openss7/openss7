dnsLevel: INTEGER;

dnsLookup (dns: Domain): SET OF DN
{
    dnsLevel = 0;
    RETURN dnsMatch (dns, NULL, NULL);
}
