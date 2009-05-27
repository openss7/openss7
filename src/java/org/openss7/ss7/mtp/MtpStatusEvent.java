
package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

public class MtpStatusEvent extends MtpEvent {
    public MtpStatusEvent() {
        super();
    }
    public MtpStatusEvent(CongestionStatus in_congStatus) {
        congStatus = in_congStatus;
    }
    protected CongestionStatus congStatus;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
