...

Date ::=
	[APPLICATION 3]
	    IMPLICIT IA5String -- YYYYMMDD --
	    [[s Date]]
	    %{
		int year, month, day;
		static char buffer[BUFSIZ];

		if (sscanf (Date, "%4d%2d%2d", &year, &month, &day) != 3
			|| year < 1900 || month < 1 || month > 12
			|| day < 1 || day > 31)
		    adios (NULLCP, "bad date format: %s", Date);
		(void) sprintf (buffer, "%02d %s %4d", day,
			    months[month - 1], year);
		Date = buffer;
	    %}

END

...
