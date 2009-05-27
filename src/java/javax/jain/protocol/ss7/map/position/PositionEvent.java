
package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public abstract class PositionEvent extends JainEvent {
    public static final int MS_POS_REQ_ID = 0;
    public static final int MS_POS_CONF_ID = 1;
    public static final int MS_POS_IND_ID = 2;
    public static final int MS_POS_RESP_ID = 3;
    public static final int MS_POS_REPORT_IND_ID = 4;
    public static final int MS_POS_REPORT_RESP_ID = 5;
    public PositionEvent(Object source, int eventId)
        throws SS7InvalidParamException {
    }
    public PositionEvent() {
    }
    public void setEventId(int eventId)
        throws SS7InvalidParamException {
    }
    public int getEventId() {
        return 0;
    }
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
