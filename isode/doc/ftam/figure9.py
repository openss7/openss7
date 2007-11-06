FileDirectory ::=
	[PRIVATE 2]
	    IMPLICIT SEQUENCE {
		fileName
		    GraphicString,

		    ContentsType
	    }

ContentsType ::=
	CHOICE {
	    document-type-Name[0]
		IMPLICIT OBJECT IDENTIFIER,
	    
	    constraint-set-and-abstract-syntax[1]
		IMPLICIT SEQUENCE {
		    constraint-set-Name[0]
			IMPLICIT OBJECT IDENTIFIER,

		    abstract-syntax-Name[1]
			IMPLICIT OBJECT IDENTIFIER
		}
	}
