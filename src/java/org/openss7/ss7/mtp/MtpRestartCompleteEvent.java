/*
 @(#) $RCSfile: MtpRestartCompleteEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:36:46 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:36:46 $ by $Author: brian $
 */

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * The MtpRestartCompleteEvent class models the MTP-RESTART-COMPLETE
  * indication primitive of MTP.
  * This event is passed from the JainMtpProvider implementation object
  * to JainMtpListener objects registered for an MtpUserAddress
  * containing the affectedPointCode. <p>
  *
  * When a JainMtpListener receives this event, it may begin sending
  * MtpTransferEvent events to the JainMtpProvider implementation object
  * from the affectedPointCode.  However, it should not issue
  * MtpTransferEvent events for any of the point codes on the
  * pausedPointCodes list, and should avoid issuing MtpTransferEvent
  * events for any of the point codes on the restrictedPointCodes list.
  * <p>
  *
  * <h4>Note to Developers:</h4>
  * Some implementations may choose not to indicate paused or restricted
  * signaling points using the corresponding members of this structure,
  * but, rather, by indicating paused or restricted signalling points by
  * issuing separate MtpPauseEvent and MtpRestrictedEvent event
  * indications before the MtpRestartCompleteEvent is issued.
  * @see MtpRestartBeginsEvent
  * @see MtpPauseEvent
  * @see MtpRestrictedEvent
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class MtpRestartCompleteEvent extends MtpRestartEvent {
    /** Private array of paused signalling pint codes.  */
    private javax.jain.ss7.SignalingPointCode[] pausedPointCodes = new javax.jain.ss7.SignalingPointCode[0];
    /** Private array of restricted signalling pint codes.  */
    private javax.jain.ss7.SignalingPointCode[] restrictedPointCodes = new javax.jain.ss7.SignalingPointCode[0];
    /** Constructs a new MtpRestartCompleteEVent object with object
      * source, affected point code, paused point code array and
      * restricted point code array.  Note that signalling points that
      * do not appear in the pausedPointCode nor restrictedPointCodes
      * array and for which no MtpPauseEvent nor MtpRestrictedEvent has
      * been received are considered in the available state.
      * @param source
      * The source object generating the event.
      * @param affectedPointCode
      * The affected point code (the point code of the restarting
      * signalling point).
      * @param pausedPointCodes
      * The paused point codes.  These point codes were in an MTP-PAUSE
      * state at the time the restart completed.
      * @param restrictedPointCodes
      * The restricted point codes.  These point codes were in an
      * MTP-RESTRICTED state at the time the restart completed.
      */
    public MtpRestartCompleteEvent(java.lang.Object source,
            javax.jain.ss7.SignalingPointCode affectedPointCode,
            javax.jain.ss7.SignalingPointCode[] pausedPointCodes,
            javax.jain.ss7.SignalingPointCode[] restrictedPointCodes) {
        super(source, MtpPrimitiveType.RESTART_ENDS, affectedPointCode);
        setPausedPointCodes(pausedPointCodes);
        setRestrictedPointCodes(restrictedPointCodes);
    }
    /** Gets the list of paused point codes.
      * @return
      * An array of signalling point codes that were in an inaccessible
      * state at the time that the MTP restart completed.
      * @see MtpPauseEvent
      */
    public javax.jain.ss7.SignalingPointCode[] getPausedPointCodes() {
        return pausedPointCodes;
    }
    /** Sets the list of paused point codes.
      * @param pausedPointCodes
      * An array of signalling point codes that were in an inaccessible
      * state at the time that the MTP restart completed.
      * @see MtpPauseEvent
      */
    public void setPausedPointCodes(javax.jain.ss7.SignalingPointCode[] pausedPointCodes) {
        this.pausedPointCodes = pausedPointCodes;
    }
    /** Gets the list of restricted point codes.
      * @return
      * An array of signalling point codes that were in a restricted
      * state (for the issuing provider) at the time that the MTP
      * restart completed.
      * @see MtpRestrictedEvent
      */
    public javax.jain.ss7.SignalingPointCode[] getRestrictedPointCodes() {
        return restrictedPointCodes;
    }
    /** Sets the list of restricted point codes.
      * @param restrictedPointCodes
      * An array of signalling point codes that were in a restricted
      * state (for the issuing provider) at the time that the MTP
      * restart completed.
      * @see MtpRestrictedEvent
      */
    public void setRestrictedPointCodes(javax.jain.ss7.SignalingPointCode[] restrictedPointCodes) {
        this.restrictedPointCodes = restrictedPointCodes;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
