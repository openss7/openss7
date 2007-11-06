-- close PDU

ClosePDU ::=
    [APPLICATION 1] IMPLICIT
	INTEGER {
	    goingDown(0),
	    unsupportedVersion(1),
	    packetFormat(2),
	    protocolError(3),
	    internalError(4),
	    authenticationFailure(5)
	}
