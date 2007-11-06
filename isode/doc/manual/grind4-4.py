ENCODER build

Example ::=
	SEQUENCE {
	    -- select maybe element if present
	    Maybe
	    OPTIONAL <<maybe == PRESENT>>,

	    SEQUENCE OF
		-- build elements
		%{ construct_elements(); %}
		-- now produce them
		<<init_elements(); more_elements();>> 
		BizarreEvents,

	    CHOICE
	    %{ choose = leap_year() ? 2 : 1; %}
	    <<choose>>
	    {
		[0] MyProposal,
		[1] YourProposal
	    }

	    INTEGER
	    [[i defvalue]]
	    DEFAULT 4 <<defvalue != 4>>
	}
