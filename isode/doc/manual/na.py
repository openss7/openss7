
QuipuObject ::= OBJECT-CLASS
	SUBCLASS OF top
	MUST CONTAIN {aCL}
	MAY CONTAIN {lastModifiedBy, lastModifiedTime, entrySecurityPolicy}

QuipuNonLeafObject ::= OBJECTCLASS
	SUBCLASS OF quipuObject
	MUST CONTAIN {masterDSA}
	MAY CONTAIN {slaveDSA, treeStructure, inheritedAttribute}

ExternalNonLeafObject ::= OBJECTCLASS
	SUBCLASS OF quipuObject
	MAY CONTAIN {subordinateReference, crossReference, 
                     nonSpecificSubordinateReference}

QuipuDSA ::= OBJECT-CLASS 
    SUBCLASS OF dsa
    MUST CONTAIN { aCL, edbInfo, userPassword, manager, quipuVersion}
    MAY CONTAIN { description, lastModifiedBy, lastModifiedTime, 
	dsaDefaultSecurityPolicy, dsaPermittedSecurityPolicy, relayDSA, 
	listenAddress, info } 

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
 
