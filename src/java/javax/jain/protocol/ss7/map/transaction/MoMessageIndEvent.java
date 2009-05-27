

package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MoMessageIndEvent extends TransactionEvent {
    public MoMessageIndEvent(Object source, MsNumber msId, Message message, SS7Number destinationAddress)
        throws SS7InvalidParamException {
    }
    public MoMessageIndEvent()
        throws SS7InvalidParamException {
    }
    public void setMsId(MsNumber msId)
        throws SS7InvalidParamException {
    }
    public MsNumber getMsId() {
    }
    public void setMessage(Message message)
        throws SS7InvalidParamException {
    }
    public Message getMessage() {
    }
    public void setDestinationAddress(SS7Number destinationAddress)
        throws SS7InvalidParamException {
    }
    public SS7Number getDestinationAddress() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
