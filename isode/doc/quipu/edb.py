EntryDataBlock ::= SEQUENCE OF RelativeEntry

RelativeEntry ::= SEQUENCE {
	RelativeDistinguishedName,
	SET OF Attribute
}

