...

        if (dlist = search-results) THEN
            IF (i > dnsLevel) THEN
                dnsLevel = i, matches = NULL;
            FOR x IN dlist DO
                dprev = matches;
                matches = dnsMatch (dns, x, matches);
                IF (dprev == matches) THEN
                    matches += x;
                ELSE
                    IF (i < dnsLevel) THEN
                        BREAK;
            DONE
        ELSE
            IF (!(dp = tail (dp)))
                RETURN matches;
    DONE
}
