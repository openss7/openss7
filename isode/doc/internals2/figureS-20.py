RRspPDU ::=
    [APPLICATION 3] IMPLICIT
	INTEGER {
	    failure(-1)
			   -- on success the non-negative priority is returned
	}

SOutPDU ::=
    [APPLICATION 4] IMPLICIT
	INTEGER {
	    commit(0),
	    rollback(1)
	}

END
