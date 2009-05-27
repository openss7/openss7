

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;
import java.util.EventListener;

public interface MessageSessionListener extends EventListener {
    public void processMessageSessionEvent(SessionOpenIndEvent event, long sessionId);
    public void processMessageSessionEvent(SessionDataIndEvent event, long sessionId);
    public void processMapError(MapErrorEvent error);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
