InheritedListSyntax ::= SET OF CHOICE {
    AttributeType,	-- Take value from the entry
    Attribute }

InheritedAttributeSyntax ::= SET {
    default [0] InheritedListSyntax OPTIONAL,
        -- default which can be overriden in lower entry.
    always  [1] InheritedListSyntax OPTIONAL,
        -- always present in lower entry.
    objectclass OBJECT IDENTIFIER OPTIONAL 
        -- object class to inherit to.
        -- The null case means the objectclass attribute 
        --    itself is inherited
}

InheritedAttribute ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX InheritedAttributeSyntax
    MULTI VALUE

