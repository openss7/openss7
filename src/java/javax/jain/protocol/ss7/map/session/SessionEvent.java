

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public abstract class SessionEvent extends JainEvent {
    public static final int SESSION_OPEN_REQ_ID = 0;
    public static final int SESSION_OPEN_IND_ID = 1;
    public static final int SESSION_DATA_REQ_ID = 2;
    public static final int SESSION_DATA_IND_ID = 3;
    public static final int SESSION_CLOSE_REQ_ID = 4;
    public static final int ALERTING_LEVEL_0_SILENT = 5;
    public static final int ALERTING_LEVEL_1_NORMAL = 6;
    public static final int ALERTING_LEVEL_2_URGENT = 7;
    public static final int ALERTING_CATEGORY_1 = 8;
    public static final int ALERTING_CATEGORY_2 = 9;
    public static final int ALERTING_CATEGORY_3 = 10;
    public static final int ALERTING_CATEGORY_4 = 11;
    public static final int ALERTING_CATEGORY_5 = 12;
    public SessionEvent(Object source, int eventId)
        throws SS7InvalidParamException {
    }
    public SessionEvent() {
    }
    public void setEventId(int eventId)
        throws SS7InvalidParamException {
    }
    public int getEventId() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
