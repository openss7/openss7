...

    exact = NULL, good = NULL;
    FOR x IN dlist DO
        IF (last (DN).value == s) THEN
            exact += x;
        ELSE
            FOR y IN cache (x, alist) DO
                IF (y.value == s) THEN
                    good += x;

    IF (exact) THEN
        RETURN exact;
    IF (good)  THEN
        RETURN good;

    -- present dlist to user
    -- return only those that are of interest
}
