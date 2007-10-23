	case OK:		/* got a result/error response */
	    switch (response) {
		case RY_RESULT:	/* got a result */
		    {
			register struct type_MODULE_result *res =
				    (struct type_MODULE_result *) out;

/* do something with ``res'' here... */

			free_MODULE_result (res);
			break;
		    }
	
		case error_MODULE_error1:
		    {
			register struct type_MODULE_parameter *parm =
				    (struct type_MODULE_parameter *) out;

/* do something with ``parm'' here... */

			free_MODULE_parameter (parm);
			break;
		    }
