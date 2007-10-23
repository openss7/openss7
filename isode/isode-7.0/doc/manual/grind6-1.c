#include <isode/tsap.h>
#include <isode/isoservent.h>

...

register struct TSAPaddr *ta;
register struct isoservent *is;

...

if ((is = getisoserventbyname ("session", "tsap")) == NULL)
    error ("tsap/session");

/* RemoteHost is the host we're interested in,
   e.g., ``gremlin.nrtc.northrop.com'' */

if ((ta = is2taddr (RemoteHost, NULLCP, is)) == NULLTA)
    error ("address translation failed");

...
