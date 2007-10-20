QuipuNameDefinitions DEFINITIONS ::=

BEGIN

IMPORTS
ABSTRACT-OPERATION, ABSTRACT-ERROR
    FROM AbstractServiceNotation
	{joint-iso-ccitt mhs-motis(6) asdc(2) modules(0) notation(1)}

NameError, ServiceError, SecurityError
    FROM DirctoryAbstractService
	{joint-iso-ccitt ds(5) modules(1) directoryAbstractService(2)}

DistinguishedName, RelativeDistinguishedName, Attribute,
ATTRIBUTE, ATTRIBUTE-SYNTAX, OBJECT-CLASS
    FROM InformationFramework
	{joint-iso-ccitt ds(5) modules(1) informationFramework(1)}

caseIgnoreStringSyntax
    FROM SelectedAttributeTypes
	{joint-iso-ccitt ds(5) modules(1) selectedAttributeTypes(5)}

TreeStructure, ACL, EDBInfo, MasterDSA, SlaveDSA, DSA, QuipuVersion, 
InheritedAttribute, SubordinateReference, Crossreference, 
NonSpecificSubordinateReference,
QuipuDSA, QuipuObject, QuipuNonLeafObject, ExternalNonLeafObject,
    FROM QuipuDirectoryDefinitions;

quipu OBJECT IDENTIFIER	::= {ccitt data(9) pss(2342) ucl(19200300) quipu(99)}
				-- interim QUIPU OID

attributeType OBJECT IDENTIFIER ::= {quipu attributeType(1)}    

attributeSyntax OBJECT IDENTIFIER ::= {quipu attributeSyntax(2)}

objectClass OBJECT IDENTIFIER ::= {quipu objectClass(3)}

treeStructure TreeStructure ::= {attributeType 1}

acl ACL ::= {attributeType 2}

eDBInfo EDBInfo ::= {attributeType 3}

masterDSA MasterDSA ::= {attributeType 4}

slaveDSA SlaveDSA ::= {attributeType 5}

control DSAControl ::= {attributeType 15}

quipuVersion QuipuVersion ::= {attributeType 16}

protectedPassword ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX    ProtectedPassword
          ::= {attributeType 17}

SecurityPolicy ::= ANY 
	       -- to be defined

entrySecurityPolicy ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX
    SecurityPolicy ::= {attributeType 18}

dsaDefaultSecurityPolicy ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX
    SecurityPolicy ::= {attributeType 19}

dsaPermittedSecurityPolicy ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX
    SecurityPolicy ::= {attributeType 20}

inheritedAttribute InheritedAttribute ::= {attributeType 21}

execVector ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX
    PrintableString ::= {attributeType 22}

relayDSA ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX
    distinguishedNameSyntax ::= {attributeType 23}

subordinateReference SubordinateReference ::= {attributeType 25}

crossReference Crossreference ::= {attributeType 26}

nonSpecificSubordinateReference NonSpecificSubordinateReference 
                              ::= {attributeType 27}

listenAddress ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX
    PresentationAddress ::= {attributeType 28}

cachedEDB ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX
    distinguishedNameSyntax ::= {attributeType 29}

quipuDSA QuipuDSA ::= {objectClass 1}

quipuObject QuipuObject	::= {objectClass 2}	

quipuNonLeafObject QuipuNonLeafObject ::= {objectClass 6}

externalNonLeafObject ExternalNonLeafObject ::= {objectClass 9}

quipuSecurityUser OBJECT-CLASS
    SUBCLASS OF quipuObject
    MUST CONTAIN {protectedPassword}
         ::= {objectClass 7}

iSODEApplicationEntity OBJECT-CLASS
    SUBCLASS OF applicationEntity
    MUST CONTAIN {execVector}
         ::= {objectclass 8}

END
