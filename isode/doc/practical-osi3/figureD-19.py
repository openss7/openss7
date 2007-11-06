...

    IF (kind == person) THEN
        qlist = { title };
    ELSE
        IF (kind == organization) THEN
            qlist = { businessCategory };
        ELSE
            IF (kind == organizationalUnit) THEN
                qlist = { businessCategory };
            ELSE
                IF (kind == organizationalRole) THEN
                    qlist = { title };
                ELSE
                    IF (kind != locality) THEN
                        qlist = { title };

    IF (kind == organization OR kind == locality) THEN
        subsettype = oneLevel;
    ELSE
        subsettype = wholeSubtree;

...
