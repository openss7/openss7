RRspPDU ::=
    [APPLICATION 3] IMPLICIT
	INTEGER {
	    failure(-1)
			   -- on success the non-negative priority is returned
	}

END
