

package org.openss7.javax.jain.protocol.ss7.map.information;

import org.openss7.javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class JainMapMsInfoStackImpl implements JainMapMsInfoStack {
    public native MsInfoProvider createMsInfoProvider()
        throws SS7PeerUnavailableException;
    public native void deleteMsInfoProvider(MsInfoProvider provider);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
