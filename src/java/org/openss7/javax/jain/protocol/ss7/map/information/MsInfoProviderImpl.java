
package org.openss7.javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class MsInfoProviderImpl implements java.rmi.Remote, MsInfoProvider {
    public native long processMsInfoOperation(MsInfoReqEvent event, MsInfoListener listener)
        throws SS7MandatoryParameterNotSetException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
