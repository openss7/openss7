QuipuDirectoryDefinitions
DEFINITIONS ::=
BEGIN

			-- give the module an identifier in QUIPU 6.0

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

QuipuObject ::= OBJECT-CLASS
	SUBCLASS OF top
	MUST CONTAIN {aCL}

QuipuNonLeafObject ::= OBJECTCLASS
	SUBCLASS OF quipuObject
	MUST CONTAIN {masterDSA}
	MAY CONTAIN {slaveDSA,treeStructure}

QuipuDSA ::= OBJECT-CLASS 
    SUBCLASS OF dsa
				-- No Attributes, just object class
				-- to tell you what it is

QuipuDSAInfo ::= OBJECT-CLASS	-- always has common name "Info"
    SUBCLASS OF top
    MUST CONTAIN { commonName, edbInfo, quipuVersion, dSAControl }
    MAY CONTAIN { presentationAddress, mhs-or-addresses, description }


EDBInfoSyntax ::= SEQUENCE {
	edb          DistinguishedName,
	getFromDSA   DistinguishedName OPTIONAL,
				-- If omitted DSA is master
				-- Determine mode of update from this DSA
	sendToDSAs   NameList,
				-- Send these DSAs incremental updates
				-- Namelist is defined with the ACLs
	getEDBAllowed NameList
				-- List of DSAs allowed to pull EDB
	}
    
EdbInfo ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX EDBInfo
    MULTI VALUE

MasterDSA ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX distinguishedNameSyntax
    			-- Master QSR
    			-- Usually, but not necessarily single valued

SlaveDSA ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX distinguishedNameSyntax
    			-- Slave QSR

SubordinateReference ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX AccessPoint
    SINGLE VALUE
 
CrossReference ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX AccessPoint
    SINGLE VALUE
 
NonSpecificSubordinateReference ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX AccessPoint

QuipuVersion ::= ATTRIBUTE
   WITH ATTRIBUTE-SYNTAX caseIgnoreStringSyntax

DSAControl ::= ATTRIBUTE
   WITH ATTRIBUTE-SYNTAX caseIgnoreStringSyntax
 
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

TreeStructureSyntax ::= SET {
    mandatoryObjectClasses [1] SET OF OBJECT IDENTIFIER,
    optionalObjectClasses [2] SET OF OBJECT IDENTIFIER OPTIONAL,
    permittedRDNs [3] SET OF SET OF AttributeType }
    
TreeStructure ::= ATTRIBUTE
    WITH ATTRIBUTE-SYNTAX TreeStructureSyntax
    MULTI VALUE

EntryDataBlock ::= SEQUENCE OF RelativeEntry

RelativeEntry ::= SEQUENCE {
	RelativeDistinguishedName,
	SET OF Attribute
}

ReliableROSData ::= SEQUENCE {
    rosOperation INTEGER,
			-- the operation being applied
    data ANY,
			-- the Operation Argument
    oldVersion ANY
			-- data version to which operation should be applied
			-- version syntax will be defined by the
			-- Reliable ROS user
    newVersion ANY
			-- version number which results from operation
    }

GetEntryDataBlock ABSTRACT-OPERATION
	ARGUMENT GetEntryDataBlockArgument
	RESULT GetEntryDataBlockResult
	ERRORS {NameError,ServiceError,SecurityError}

getEntryDataBlock GetEntryDataBlock ::= 10

				-- will make this an OBJECT IDENTIFER
				-- when ISODE can support this form 
				-- of operation code

GetEntryDataBlockArgument ::= SET {
	entry [0] DistinguishedName,
	sendIfMoreRecentThan [1] EDBVersion OPTIONAL
			-- if omitted, just return version held
			-- To force send, specify old version
}

GetEntryDataBlockResult ::= SEQUENCE {
		versionHeld [0] EDBVersion
		[1] EntryDataBlock OPTIONAL
	}
}


EDBVersion ::= UTCTime
END
