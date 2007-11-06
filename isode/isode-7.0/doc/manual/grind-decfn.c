put_int(parm, pe)
struct codedata **parm;
PE      pe;
{
    if (pe)
        (*parm)->cd_int = prim2num(pe);
    else
        (*parm)->cd_int = 1;    /* Default */
    return (OK);
}
