...

int     OP1 (), ..., OPn ();


/* OPERATIONS are numbered APDU_OPx, where each is a unique integer.  Further,
   APDU_UNKNOWN is used as a tag different than any valid operation.

   ERRORS are numbered ERROR_xyz, where each is a unique integer.
   ERROR_MISTYPED is used to signal an argument error to an operation.
   Further, ERROR_UNKNOWN is used as a tag to indicate that the operation
   succeeded.

   Finally, note that rox -> rox_args is updated in place by these routines.
   If the routine returns ERROR_UNKNOWN, then rox_args contains the results
   of the operation.  If the routine returns ERROR_MISTYPED, then rox_args is
   untouched.  Otherwise, if the routine returns any other value, then
   rox_args contains the parameters of the error which occurred.  Obviously,
   each routine calls ROXFREE prior to setting rox_args to a new value.
 */

static struct dispatch {
    int     ds_operation;
    IFP     ds_vector;
}       dispatches[] = {
    APDU_OP1,   OP1,

...

    APDU_OPn,   OPn,

    APDU_UNKNOWN
};

...
