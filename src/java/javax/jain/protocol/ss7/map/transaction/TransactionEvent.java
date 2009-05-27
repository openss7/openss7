

package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public abstract class TransactionEvent extends JainEvent {
    public static final int MT_MESSAGE_REQ_ID = 0;
    public static final int MT_MESSAGE_CONF_ID = 1;
    public static final int MO_MESSAGE_IND_ID = 2;
    public static final int MO_MESSAGE_RESP_ID = 3;
    public static final int MT_MESSAGE_REC__AVAIL_IND_ID = 4;
    public static final int MT_MESSAGE_REC__AVAIL_RESP_ID = 5;
    public TransactionEvent(Object source, int eventId)
        throws SS7InvalidParamException {
    }
    public TransactionEvent() {
    }
    public void setEventId(int eventId)
        throws SS7InvalidParamException {
    }
    public int getEventId() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
