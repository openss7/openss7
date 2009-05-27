

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public interface JainMapMessageSessionStack extends JainMapStack {
    public MessageSessionProvider createMessageSessionProvider()
        throws SS7PeerUnavailableException;
    public void deleteMessageSessionProvider(MessageSessionProvider provider);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
