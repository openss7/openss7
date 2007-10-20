do_error (sd, id, error, parameter, roi)
int	sd;
int	id,
	error;
caddr_t parameter;
struct RoSAPindication *roi;
{
    switch (error) {
	case RY_REJECT:
	{
	    int	    reason = (int) parameter;

	    /* do something with ``reason'' here... */

	    break;
	}
	
...

