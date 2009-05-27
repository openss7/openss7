

package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.position.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MsPosIndEvent extends PositionEvent {
    public MsPosIndEvent(Object source, PositionTargetId msId)
        throws SS7InvalidParamException {
    }
    public MsPosIndEvent()
        throws SS7InvalidParamException {
    }
    public void setMsId(PositionTargetId msId)
        throws SS7InvalidParamException {
    }
    public PositionTargetId getMsId() {
    }
    public void setPosRequestSpecifier(PosRequestSpecifier posRequestSpecifier)
        throws SS7InvalidParamException {
    }
    public PosRequestSpecifier getPosRequestSpecifier() {
    }
    public boolean isPosRequestSpecifierPresent() {
    }
    public void setMsEquipmentId(EquipmentId msEquipmentId)
        throws SS7InvalidParamException {
    }
    public EquipmentId getMsEquipmentId() {
    }
    public boolean isMsEquipmentIdPresent() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
