

package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.information.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MsInfoConfEvent extends InformationEvent {
    public static final int MS_STATE_IDLE = 0;
    public static final int MS_STATE_BUSY = 1;
    public static final int MS_STATE_NOT_REACHABLE = 2;
    public static final int MS_STATE_NOT_PROVIDED = 3;
    public MsInfoConfEvent(Object source)
        throws SS7InvalidParamException {
        super(source);
    }
    public MsInfoConfEvent()
        throws SS7InvalidParamException {
    }
    public void setLocation(Location location)
        throws SS7InvalidParamException {
    }
    public Location getLocation() {
        return new Location();
    }
    public boolean isLocationPresent() {
        return true;
    }
    public void setState(int state)
        throws SS7InvalidParamException {
    }
    public int getState() {
        return 0;
    }
    public boolean isStatePresent() {
        return true;
    }
    public void setResult(Result result)
        throws SS7InvalidParamException {
    }
    public Result getResult() {
        return new Result();
    }
    public boolean isResultPresent() {
        return true;
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
