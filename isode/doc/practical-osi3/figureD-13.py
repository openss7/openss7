present (d: DN; t: AttributeType): BOOLEAN
{
    x: RDN;

    FOR x IN d DO
        IF (x.type == t) THEN
            RETURN TRUE;

    RETURN FALSE;
}
