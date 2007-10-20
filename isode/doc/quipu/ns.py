Query ::= [0] IMPLICIT SEQUENCE {
		request-id[0] IMPLICIT INTEGER,

		name[1] IMPLICIT IA5String,

		attribute[2] IMPLICIT IA5String,

		cache-ok[3] IMPLICIT BOOLEAN DEFAULT TRUE }


Response ::= [1] IMPLICIT SEQUENCE {
		request-id[0] IMPLICIT INTEGER,

		name[1] IMPLICIT ANY OPTIONAL, 
		
		value[2] IMPLICIT  ANY OPTIONAL }

