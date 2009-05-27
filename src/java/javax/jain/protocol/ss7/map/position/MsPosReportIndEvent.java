

package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.position.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MsPosReportIndEvent extends PositionEvent {
    public static final int EVENT_EMERGENCY_CALL_ORIGINATION = 0;
    public static final int EVENT_EMERGENCY_CALL_RELEASE = 1;
    public static final int EVENT_MO_LOCATION_REQUEST = 2;
    public static final int EVENT_UNDEFINED = 3;
    public MsPosReportIndEvent(Object source, PositionTargetId msId, int triggeringEvent, LcsClientId lcsClientId)
        throws SS7InvalidParamException {
    }
    public MsPosReportIndEvent()
        throws SS7InvalidParamException {
    }
    public void setMsId(PositionTargetId msId)
        throws SS7InvalidParamException {
    }
    public PositionTargetId getMsId() {
    }
    public void setPosition(Position position)
        throws SS7InvalidParamException {
    }
    public Position getPosition() {
    }
    public boolean isPositionPresent() {
    }
    public void setTriggeringEvent(int triggeringEvent)
        throws SS7InvalidParamException {
    }
    public int getTriggeringEvent() {
    }
    public void setLcsClientId(LcsClientId lcsClientId)
        throws SS7InvalidParamException {
    }
    public LcsClientId getLcsClientId() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
