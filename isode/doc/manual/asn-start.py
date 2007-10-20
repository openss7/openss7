QuipuDirectoryDefinitions
DEFINITIONS ::=

BEGIN

IMPORTS
EVERYTHING
    FROM QuipuNameDefinitions
    
mhs-or-addresses 
    FROM MHSDirectoryObjectsAndAttributes 
            {joint-iso-ccitt mhs-motis(6) arch(5) modules(0) directory(1)}

ABSTRACT-OPERATION, ABSTRACT-ERROR 
    FROM AbstractServiceNotation {joint-iso-ccitt mhs-motis(6) asdc(2) modules(0) notation(1)}

NameError, ServiceError, SecurityError 
    FROM DirctoryAbstractService {joint-iso-ccitt ds(5) modules(1) directoryAbstractService(2)}

DistinguishedName, RelativeDistinguishedName, Attribute, AttributeType, 
ATTRIBUTE, ATTRIBUTE-SYNTAX, OBJECT-CLASS 
    FROM InformationFramework {joint-iso-ccitt ds(5) modules(1) informationFramework(1)}

distinguishedNameSyntax, commonName, description, presentationAddress
    FROM SelectedAttributeTypes {joint-iso-ccitt ds(5) modules(1) selectedAttributeTypes(5)}

top, dsa 
    FROM SelectedObjectClasses {joint-iso-ccitt ds(5) modules(1) selectedObjectClasses(6)}

AccessPoint 
    FROM DistributedOperations {joint-iso-ccitt ds(5) modules(1) distributedOperations(3)} ;
