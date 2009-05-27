
package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

public class MtpResumeEvent extends MtpEvent {
    public MtpResumeEvent() {
        super();
    }
    public MtpResumeEvent(SignalingPointCode in_affectedPointCode,
            SignalingPointCode in_concernedPointCode) {
        affectedPointCode = in_affectedPointCode;
        concernedPointCode = in_concernedPointCode;
    }
    protected SignalingPointCode concernedPointCode;
    protected SignalingPointCode affectedPointCode;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
