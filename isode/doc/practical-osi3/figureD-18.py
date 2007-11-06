simpleMatch (kind: ObjectClass; name, qual: String; base: DN): SET OF DN
{
    x: DN;
    alist, qlist: SET OF AttributeType;
    subsettype: SubsetType;

    IF (kind == person) THEN
        alist = { surName, rfc822Mailbox };
    ELSE
        IF (kind == organization) THEN
            alist = { organizationName };
        ELSE
            IF (kind == organizationalUnit) THEN
                alist = { organizationalUnitName };
            ELSE
                IF (kind == organizationalRole) THEN
                    alist = { commonName };
                ELSE
                    IF (kind == locality) THEN
                        alist = { localityName };
                    ELSE
                        alist = { commonName, surName, rfc822Mailbox };

...
