TreeStructureSyntax ::= SET {
    mandatoryObjectClasses [1] SET OF OBJECT IDENTIFIER,
    optionalObjectClasses [2] SET OF OBJECT IDENTIFIER OPTIONAL,
    permittedRDNs [3] SET OF SET OF AttributeType }
    
TreeStructure ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX TreeStructureSyntax
    MULTI VALUE

