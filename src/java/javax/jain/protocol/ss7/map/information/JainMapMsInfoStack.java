

package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public interface JainMapMsInfoStack extends JainMapStack {
    public MsInfoProvider createMsInfoProvider()
        throws SS7PeerUnavailableException;
    public void deleteMsInfoProvider(MsInfoProvider provider);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
