ProtocolElement ::=
	CHOICE {
	    textUnit[8]
		IMPLICIT TextUnit
		%{ action1 %},

	    specificLogicalDescriptor[5]
		%{ action2 %}
		IMPLICIT LogicalDescriptor
	}
