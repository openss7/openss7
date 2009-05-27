

package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public interface MsInfoProvider {
    public long processMsInfoOperation(MsInfoReqEvent event, MsInfoListener listener)
        throws SS7MandatoryParameterNotSetException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
