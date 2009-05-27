
package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

public class MtpPauseEvent extends MtpEvent {
    public MtpPauseEvent() {
        super();
    }
    public MtpPauseEvent(SignalingPointCode in_affectedPointCode) {
        affectedPointCode = in_affectedPointCode;
    }
    protected SignalingPointCode affectedPointCode;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
