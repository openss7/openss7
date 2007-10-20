...

BEGIN

PersonnelRecord
	%{ char *Title; %}
	::=
	[APPLICATION 0]
	    IMPLICIT SET {
		    Name
		    %{
			printf ("Name: %s %s. %s\n",
				givenName, initial, familyName);
		    %},

		title[0]
		    IA5String
		    [[s Title]]
		    %{ printf ("Title: %s\n", Title); %},

		    EmployeeNumber,

		dateOfHire[1]
		    Date
		    %{ printf ("Date of Hire: %s\n", Date); %},

		nameOfSpouse[2]
		    Name
		    %{
			printf ("Name of Spouse: %s %s. %s\n",
				givenName, initial, familyName);
		    %},

		[3]
		    %{
			PE	cc = prim2seq ($$);
			if (cc)
			    printf ("Number of Children: %d\n",
				    cc -> pe_cardinal);
		    %}
		    IMPLICIT SEQUENCE OF ChildInformation
		    DEFAULT {}
	    }
	    %{ printf ("--------\n"); %}
...
