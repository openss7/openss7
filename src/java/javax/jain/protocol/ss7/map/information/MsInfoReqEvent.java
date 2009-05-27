
package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.information.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MsInfoReqEvent extends InformationEvent {
    public MsInfoReqEvent(Object source, E164Number msId, InfoRequestSpecifier infoRequestSpecifier)
        throws SS7InvalidParamException {
    }
    public MsInfoReqEvent()
        throws SS7InvalidParamException {
    }
    public void setMsId(E164Number msId)
        throws SS7InvalidParamException {
    }
    public E164Number getMsId() {
        return new E164Number();
    }
    public void setInfoRequestSpecifier(InfoRequestSpecifier infoRequestSpecifier)
        throws SS7InvalidParamException {
    }
    public InfoRequestSpecifier getInfoRequestSpecifier() {
        return new InfoRequestSpecifier();
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
