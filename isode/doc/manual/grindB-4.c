#include <isode/rtsap.h>
#include <isode/isoservent.h>

...

register struct SSAPaddr *sa;
struct RtSAPaddr rtas;
register struct RtSAPaddr *rta = &rtas;
register struct isoservent *is;

...

if ((is = getisoserventbyname ("p1", "rtsap")) == NULL)
    error ("rtsap/p1");

/* RemoteHost is the host we're interested in,
   e.g., ``gremlin.nrtc.northrop.com'' */

rta -> rta_port = is -> is_port;
if ((is = getisoserventbyname ("rts", "ssap")) == NULL)
    error ("ssap/rts");
if ((sa = is2saddr (RemoteHost, NULLCP, (struct isoservent *) 0))
	== NULLSA)
    error ("address translation failed");
rta -> rta_addr = *sa;

...
