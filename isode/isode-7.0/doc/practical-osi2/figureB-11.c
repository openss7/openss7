		/* other errors are handled here... */

		default:
		    adios (NULLCP, "unknown result/error returned (%d)",
				response);
		    /* NOTREACHED */
	    }
	    break;

	case DONE:		/* got RO-END? */
	    adios (NULLCP, "got RO-END.INDICATION");
	    /* NOTREACHED */

	default:
	    adios (NULLCP, "unknown return from RyOperation=%d", result);
	    /* NOTREACHED */
    }

    free_MODULE_argument (arg);

...
