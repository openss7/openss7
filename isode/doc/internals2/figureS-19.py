-- insert PDU

RReqPDU ::=
    [APPLICATION 2] IMPLICIT
	SEQUENCE {
	    subtree
		ObjectName,

	    priority    -- the lower the better, "-1" means default
		INTEGER (-1..2147483647),

	    operation
		INTEGER {
		    delete(0),
		    readOnly(1),
		    readWrite(2)
		}
	}
