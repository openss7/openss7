integer_free (x) integer *x; { free ((char *) x); }

integer_parse (x, s)
integer **x;
char     *s;
{
    long    l;

    if (scanf (s, "%ld", &l) != 1
	    || (*x = (integer *) malloc (sizeof **x)) == NULL)
	return NOTOK;
    **x = (integer) l;

    return OK;
}

integer_print (x, os) integer *x; OS os; { printf ("%d", *x); }
