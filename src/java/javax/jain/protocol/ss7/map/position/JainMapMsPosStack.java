
package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public interface JainMapMsPosStack extends JainMapStack {
    public MsPosProvider createMsPosProvider()
        throws SS7PeerUnavailableException;
    public void deleteMsPosProvider(MsPosProvider provider);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
