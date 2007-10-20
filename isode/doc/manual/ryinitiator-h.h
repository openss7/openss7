/* ryinitiator.h - include file for the generic interactive initiator */

#include <isode/rosy.h>


static struct dispatch {
    char   *ds_name;
    int	    ds_operation;

    IFP	    ds_argument;
    IFP	    ds_free;

    IFP	    ds_result;
    IFP	    ds_error;

    char   *ds_help;
};


void	adios (), advise ();
void	acs_adios (), acs_advise ();
void	ros_adios (), ros_advise ();

int	ryinitiator ();
