

package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public interface MessageTransactionListener extends java.util.EventListener {
    public void processMessageTransactionEvent(MtMessageConfEvent event, long transactionId);
    public void processMessageTransactionEvent(MoMessageIndEvent event, long transactionId);
    public void processMessageTransactionEvent(MtMessageRecAvailIndEvent event, long transactionId);
    public void processMapError(MapErrorEvent error);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
