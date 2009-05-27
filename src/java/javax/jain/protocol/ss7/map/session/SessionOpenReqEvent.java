

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.session.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class SessionOpenReqEvent extends SessionEvent {
    public SessionOpenReqEvent(Object source, E164Number msId, ServiceData serviceData, boolean notify)
        throws SS7InvalidParamException {
    }
    public SessionOpenReqEvent()
        throws SS7InvalidParamException {
    }
    public void setMsId(E164Number msId)
        throws SS7InvalidParamException {
    }
    public E164Number getMsId() {
    }
    public void setServiceData(ServiceData serviceData)
        throws SS7InvalidParamException {
    }
    public ServiceData getServiceData() {
    }
    public void setNotify(boolean notify)
        throws SS7InvalidParamException {
    }
    public boolean getNotify() {
    }
    public void setAlerting(int alerting)
        throws SS7InvalidParamException {
    }
    public int getAlerting() {
    }
    public boolean isAlertingPresent() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
