integer_encode (x, pe)
integer *x;
PE      *pe;
{
    return ((*pe = int2prim (*x)) == NULLPE ? NOTOK : OK);
}

integer_decode (x, pe)
integer **x;
PE        pe;
{
    integer    i = prim2num (pe);

    if ((i == NOTOK && pe -> pe_errno != PE_ERR_NONE)
	    || (*x = (integer *) malloc (sizeof **x)) == NULL)
	return NOTOK;
    **x = i;

    return OK;
}
