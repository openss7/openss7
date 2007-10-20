#include <isode/ssap.h>
#include <isode/isoservent.h>

...

register struct SSAPaddr *sa;
register struct isoservent *is;

...

if ((is = getisoserventbyname ("presentation", "ssap")) == NULL)
    error ("ssap/presentation");
    
/* RemoteHost is the host we're interested in,
   e.g., ``gremlin.nrtc.northrop.com'' */

if ((sa = is2saddr (RemoteHost, NULLCP, is)) == NULLSA)
    error ("address translation failed");

...
