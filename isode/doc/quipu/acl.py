ACLInfo ::= SET OF SEQUENCE {
	AccessSelector,
	AccessCategories }

AccessCategories ::= ENUMERATED {
	none (0),	
	detect (1),
	compare (2),
	read (3),
	add (4),
	write (5) }

AccessSelector ::= CHOICE {
	entry [0] NULL,
		-- DUA identified by the entry
	other [2] NULL,
		-- This indicates 'public' rights
	prefix [3] NameList,
		-- This identifies a prefix name for specified DUAs
		-- e.g. anyone in the UK
	group [4] NameList
		-- For specifying group rights
	}

NameList ::= SET OF DistinguishedName	

ACLSyntax ::= SEQUENCE { 
    childACL            [0] ACLInfo DEFAULT {{other, read}},
    entryACL		[1] ACLInfo DEFAULT {{other, read}},
    defaultAttributeACL [2] ACLInfo DEFAULT {{other, read}},
    [3] SET OF AttributeACL }
				-- Defaults to a publicly readable
				-- read only directory
    
AttributeACL ::= SEQUENCE {
    SET OF AttributeType,
    ACLInfo }    

ACL ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX ACLSyntax
    SINGLE VALUE

