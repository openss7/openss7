GetEntryDataBlock ABSTRACT-OPERATION
	ARGUMENT GetEntryDataBlockArgument
	RESULT GetEntryDataBlockResult
	ERRORS {NameError,ServiceError,SecurityError}

getEntryDataBlock GetEntryDataBlock ::= 10
				-- will make this an OBJECT IDENTIFER
				-- when ISODE can support this form 
				-- of operation code

GetEntryDataBlockArgument ::= SET {
	entry
		[0] DistinguishedName [[p ga_entry]] ,
	CHOICE {
                    -- Can only use CHOICE [1] if remote DSA is 
                    -- version is QUIPU 6.8 or less.
	       sendIfMoreRecentThan [1] EDBVersion,
	       getVersionNumber [2] NULL,
	       getEDB [3] NULL,
	       continuation [4] SEQUENCE {
                        EDBVersion,
                        nextEntryPosition INTEGER }
                },
	maxEntries [5] INTEGER OPTIONAL 
                    -- Must omit if remote DSA is QUIPU 6.8 or less
}

GetEntryDataBlockResult ::= SEQUENCE {
		versionHeld [0] EDBVersion,
		[1] EntryDataBlock OPTIONAL,
		nextEntryPostion INTEGER OPTIONAL
                    -- Must omit if remote DSA is QUIPU 6.8 or less
	}
}

EDBVersion ::= UTCTime
