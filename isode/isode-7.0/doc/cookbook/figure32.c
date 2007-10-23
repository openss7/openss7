...

	case DONE:		/* got RO-END? */
	    adios (NULLCP, "got RO-END.INDICATION");
	    /* NOTREACHED */

	default:
	    adios (NULLCP, "unknown return from RyStub=%d", result);
	    /* NOTREACHED */
    }

    free_MODULE_argument (arg);

...
