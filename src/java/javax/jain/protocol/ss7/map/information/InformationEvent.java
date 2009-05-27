
package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public abstract class InformationEvent extends JainEvent {
    public static final int MS_INFO_REQ_ID = 0;
    public static final int MS_INFO_CONF_ID = 1;
    public InformationEvent(Object source, int eventId)
            throws SS7InvalidParamException {
            super(source);
    }
    public InformationEvent() {
    }
    public void setEventId(int eventId)
            throws SS7InvalidParamException {
    }
    public int getEventId() {
        return 0;
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
