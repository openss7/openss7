...

BEGIN

SECTIONS build unbuild none

Salary ::=
	SEQUENCE {
	    name
		PrintableString [[s parm -> name]]
		%{ printf("name %s ", parm -> name); %},

	    salary
		TheSalary [[i parm -> salary]]
		%{ printf("salary %d\n", parm -> salary); %}
}

TheSalary ::=
	INTEGER

END
