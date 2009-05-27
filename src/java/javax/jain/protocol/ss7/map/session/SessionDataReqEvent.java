

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.session.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class SessionDataReqEvent extends SessionEvent {
    public SessionDataReqEvent(Object source, ServiceData serviceData, boolean notify)
        throws SS7InvalidParamException {
    }
    public SessionDataReqEvent()
        throws SS7InvalidParamException {
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
