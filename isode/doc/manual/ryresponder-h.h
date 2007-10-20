/* ryresponder.h - include file for the generic idempotent responder */

#include <isode/rosy.h>
#include <isode/logger.h>


static struct dispatch {
    char   *ds_name;
    int     ds_operation;

    IFP     ds_vector;
};


extern int  debug;


void    adios (), advise ();
void    acs_advise ();
void    ros_adios (), ros_advise ();

int     ryresponder ();
