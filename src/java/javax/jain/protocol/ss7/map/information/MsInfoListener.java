
package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public interface MsInfoListener extends java.util.EventListener {
    public void processMsInfoEvent(MsInfoConfEvent event, long transactionId);
    public void processMapError(MapErrorEvent error);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
