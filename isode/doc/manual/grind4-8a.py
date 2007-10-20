...

ChildInformation ::=
	SET {
		Name
		%{
		    printf ("\nChild Information\nName: %s %s. %s\n",
			givenName, initial, familyName);
		%},

	    dateofBirth[0]
		Date
		%{ printf ("Date of Birth: %s\n", Date); %}
	}

Name ::=
	[APPLICATION 1]
	    IMPLICIT SEQUENCE {
		givenName
		    IA5String [[s givenName]],

		initial
		    IA5String [[s initial]],

		familyName
		    IA5String [[s familyName]]
	    }

EmployeeNumber ::=
	[APPLICATION 2]
	    IMPLICIT INTEGER
	    %{ printf ("Employee Number: %d\n", $$); %}

...
