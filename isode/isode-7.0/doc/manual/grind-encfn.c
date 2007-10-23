get_int(parm, ppe)
struct codedata *parm;
PE      *ppe;
{
    if (ppe == (PE *)0) {
        if (parm->cd_int == 1)  /* Default case */
            return (OK + 1);
        return (OK);
    }

    *ppe = int2prim(parm->cd_int);
    return (OK);
}
