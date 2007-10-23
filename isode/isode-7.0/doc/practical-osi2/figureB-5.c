do_result (sd, id, dummy, result, roi)
int	sd;
int	id,
	dummy;
struct type_MODULE_result *result;
struct RoSAPindication *roi;
{
    /* do something with ``result'' here... */

    /* ``result'' will be automatically free'd when do_result returns */

    return OK;
}
