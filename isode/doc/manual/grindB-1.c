#include <isode/rosap.h>
#include <isode/isoservent.h>

...

register struct SSAPaddr *sa;
struct RoSAPaddr roas;
register struct RoSAPaddr *roa = &roas;
register struct isoservent *is;

...

if ((is = getisoserventbyname ("directory", "rosap")) == NULL)
    error ("rosap/directory");

/* RemoteHost is the host we're interested in,
   e.g., ``gremlin.nrtc.northrop.com'' */

roa -> roa_port = is -> is_port;
if ((is = getisoserventbyname ("ros", "ssap")) == NULL)
    error ("ssap/ros");
if ((sa = is2saddr (RemoteHost, NULLCP, (struct isoservent *) is))
	== NULLSA)
    error ("address translation failed");
roa -> roa_addr = *sa;

...
