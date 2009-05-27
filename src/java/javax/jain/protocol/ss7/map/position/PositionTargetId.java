
package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class PositionTargetId extends MapParameter {
    public PositionTargetId()
        throws SS7InvalidParamException {
    }
    public void setMsIsdn(E164Number msIsdn)
        throws SS7InvalidParamException {
    }
    public E164Number getMsIsdn() {
        return null;
    }
    public boolean isMsIsdnPresent() {
        return false;
    }
    public void setImsi(Imsi imsi)
        throws SS7InvalidParamException {
    }
    public Imsi getImsi() {
        return null;
    }
    public boolean isImsiPresent() {
        return false;
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
