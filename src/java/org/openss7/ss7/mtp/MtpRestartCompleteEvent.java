
package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * The MtpRestartCompleteEvent class models the MTP-RESTART-COMPLETE
  * indication primitive of MTP.  This event is passed from the
  * JainMtpProvider implementation object to JainMtpListener objects registered
  * for an MtpUserAddress containing the affectedPointCode. <p>
  *
  * When a JainMtpListener receives this event, it may begin sending
  * MtpTransferEvent events to the JainMtpProvider implementation object from
  * the affectedPointCode.  However, it should not issue MtpTransferEvent
  * events for any of the point codes on the pausedPointCodes list, and should
  * avoid issuing MtpTransferEvent events for any of the point codes on the
  * restrictedPointCodes list. <p>
  *
  * <em>Note to Developers:</em> <br>
  * Some implementations may choose not to indicate paused or restricted
  * signaling points using the corresponding members of this structure, but,
  * rather, by indicating paused or restricted signalling points by issuing
  * separate MtpPauseEvent and MtpRestrictedEvent event indications before the
  * MtpRestartCompleteEvent is issued.
  */
public class MtpRestartCompleteEvent extends MtpEvent {
    public MtpRestartCompleteEvent() {
        super();
    }
    public MtpRestartCompleteEvent(SignalingPointCode affectedPointCode,
            SignalingPointCode[] pausedPointCodes,
            SignalingPointCode[] restrictedPointCodes) {
    }
    public SignalingPointCode getAffectedPointCode() {
    }
    public void setAffectedPointCode(SignalingPointCode aAffectedPointCode) {
    }
    public SignalingPointCode[] getPausedPointCodes() {
    }
    public void setPausedPointCodes(SignalingPointCode[] in_pausedPointCodes) {
    }
    public SignalingPointCode[] getRestrictedPointCodes() {
    }
    public void setRestrictedPointCodes(SignalingPointCode[] in_restrictedPointCodes) {
    }
    protected SignalingPointCode affectedPointCode;
    protected SignalingPointCode[] pausedPointCodes;
    protected SignalingPointCode[] restrictedPointCodes;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
