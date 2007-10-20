
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
 
