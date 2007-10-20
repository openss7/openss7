-- tags for SMUX-specific PDUs are application-wide to
-- avoid conflict with tags for current (and future) SNMP-generic PDUs

SMUX-PDUs ::=
    CHOICE {
	open            -- SMUX peer uses
	    OpenPDU,    -- immediately after TCP open

	close           -- either uses immediately before TCP close
	    ClosePDU,

	registerRequest -- SMUX peer uses
	    RReqPDU,

	registerResponse -- SNMP agent uses
	    RRspPDU,

	PDUs            -- note that roles are reversed
    }
