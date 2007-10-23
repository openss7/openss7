#include <isode/psap2.h>
#include <isode/isoservent.h>

...

register struct PSAPaddr *pa;
register struct isoservent *is;

...

if ((is = getisoserventbyname ("loop", "psap")) == NULL)
    error ("psap/loop");
    
/* RemoteHost is the host we're interested in,
   e.g., ``gremlin.nrtc.northrop.com'' */

if ((pa = is2paddr (RemoteHost, NULLCP, is)) == NULLPA)
    error ("address translation failed");

...
