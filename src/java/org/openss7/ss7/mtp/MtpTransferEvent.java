
package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

public class MtpTransferEvent extends MtpEvent {
    public MtpTransferEvent() {
        super();
    }
    public MtpTransferEvent(SignalingPointCode in_dpc,
            SignalingPointCode in_opc,
            SignalingLinkSelection in_sls,
            MessagePriority in_mp,
            byte[] data) {
    }
    protected SignalingPointCode dpc;
    protected SignalingPointCode opc;
    protected SignalingLinkSelection sls;
    protected MessagePriority mp;
    protected byte[] data;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
