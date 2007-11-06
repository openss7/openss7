PERSONNEL DEFINITIONS	::=

BEGIN

PersonnelRecord ::=
	[APPLICATION 0]
	    IMPLICIT SET {
		    Name,

		title[0]
		    IA5String,

		    EmployeeNumber,

		dateOfHire[1]
		    Date,

		nameOfSpouse[2]
		    Name,

		[3]
		    IMPLICIT SEQUENCE OF ChildInformation DEFAULT {}
	    }

ChildInformation ::=
	SET {
		Name,

	    dateofBirth[0]
		Date
	}

Name ::=
	[APPLICATION 1]
	    IMPLICIT SEQUENCE {
		givenName
		    IA5String,

		initial
		    IA5String,

		familyName
		    IA5String
	    }

EmployeeNumber ::=
	[APPLICATION 2]
	    IMPLICIT INTEGER

Date ::=
	[APPLICATION 3]
	    IMPLICIT IA5String -- YYYYMMDD --

END
