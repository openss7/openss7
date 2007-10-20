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
