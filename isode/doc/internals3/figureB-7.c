...

	case error_MODULE_error1:
	{
	    register struct type_MODULE_parameter *parm =
			    (struct type_MODULE_parameter *) parameter;

	    /* do something with ``parm'' here... */

	    break;
	}

	/* other errors are handled here... */

	default:
	    adios (NULLCP, "unknown error returned (%d)", error);
	    /* NOTREACHED */
    }
    
    /* ``parameter'' will be automatically free'd when do_error returns */

    return OK;
}
