
package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * The MtpRestartBeginsEvent class models the MTP-RESTART-BEGINS indication
  * primitive of MTP.  This event is passed from the JainMtpProvider
  * implementation object to the JainMtpListener object to indicate that the
  * affected signalling point code is restarting.  The
  * affectedSignalingPointCode will always be the same as a signaling point
  * code associated with the JainMtpListener.  That is, it is the signallng
  * point code associated with an MtpUserAddress for which the JainMtpListener
  * has registered with a JainMtpProvider. <p>
  *
  * When a JainMtpListener receives this event, it should stop requesting
  * MtpTransferEvent events of the JainMtpProvider for the
  * affectedSignalingPointCode otherwise additional MtpRestartBeginsEvent
  * events will be indicated.
  */
public class MtpRestartBeginsEvent extends MtpEvent {
    public MtpRestartBeginsEvent() {
        super();
    }
    public MtpRestartBeginsEvent(SignalingPointCode in_affectedPointCode) {
        affectedPointCode = in_affectedPointCode;
    }
    public SignalingPointCode getAffectedPointCode() {
    }
    public void setAffectedPointCode(SignalingPointCode aAffectedPointCode) {
    }
    protected SignalingPointCode affectedPointCode;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
