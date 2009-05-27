

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.session.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class SessionOpenIndEvent extends SessionEvent {
    public SessionOpenIndEvent(Object source, ServiceData serviceData)
        throws SS7InvalidParamException {
    }
    public SessionOpenIndEvent()
        throws SS7InvalidParamException {
    }
    public void setMsId(E164Number msId)
        throws SS7InvalidParamException {
    }
    public E164Number getMsId() {
    }
    public boolean isMsIdPresent() {
    }
    public void setServiceData(ServiceData serviceData)
        throws SS7InvalidParamException {
    }
    public ServiceData getServiceData() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
