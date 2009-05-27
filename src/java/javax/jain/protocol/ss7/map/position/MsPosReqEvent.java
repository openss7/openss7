

package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.position.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MsPosReqEvent extends PositionEvent {
    public MsPosReqEvent(Object source, PositionTargetId msId, PosRequestSpecifier posRequestSpecifier)
        throws SS7InvalidParamException {
    }
    public MsPosReqEvent()
        throws SS7InvalidParamException {
    }
    public void setMsId(PositionTargetId msId)
        throws SS7InvalidParamException {
    }
    public PositionTargetId getMsId() {
        return null;
    }
    public void setPosRequestSpecifier(PosRequestSpecifier posRequestSpecifier)
        throws SS7InvalidParamException {
    }
    public PosRequestSpecifier getPosRequestSpecifier() {
        return null;
    }
    public void setMsEquipmentId(EquipmentId msEquipmentId)
        throws SS7InvalidParamException {
    }
    public EquipmentId getMsEquipmentId() {
        return null;
    }
    public boolean isMsEquipmentIdPresent() {
        return false;
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
