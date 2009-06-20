/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */


package jain.protocol.ss7.tcap.dialogue;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/** An event representing a TCAP UserAbort indication dialogue
  * primitive.  This event will be passed from the Provider (TCAP) to
  * the Listener(the TC User) to indicate the abrupt termination of a
  * dialogue.  The mandatory parameters of this primitive are supplied
  * to the constructor. Optional parameters may then be set using the
  * set methods.
  * @version 1.2.2
  * @author Monavacon Limited
  * @see DialogueIndEvent
  */
public final class UserAbortIndEvent extends DialogueIndEvent {
    /** Constructs a new UserAbortIndEvent, with only the Event Source
      * and the JAIN TCAP Mandatory parameters being supplied to the
      * constructor.
      * @param source
      * The Event Source supplied to the constructor.
      * @param dialogueId
      * The Dialogue Identifier supplied to the constructor.  */
    public UserAbortIndEvent(java.lang.Object source, int dialogueId) {
        super(source);
        setDialogueId(dialogueId);
    }
    /** Sets the Quality of Service parameter of the UserAbort
      * indication primitive.
      * @param qualityOfService
      * The new Quality Of Service value.  */
    public void setQualityOfService(byte qualityOfService) {
        m_qualityOfService = qualityOfService;
        m_qualityOfServicePresent = true;
    }
    /** Sets the User Abort Information parameter of the UserAbort
      * indication primitive.
      * @param userAbortInformation
      * The new User Abort Information.  */
    public void setUserAbortInformation(byte[] userAbortInformation) {
        m_userAbortInformation = userAbortInformation;
        m_userAbortInformationPresent = true;
    }
    /** Sets the Abort Reason parameter of the UserAbort indication
      * primitive. Abort Reason indicates whether a dialogue is aborted
      * because the received application context name is not supported
      * and no alternative one can be proposed (abort reason =
      * application context not supported) or because of any other user
      * problem (abort reason = user specific).
      * @param abortReason
      * One of the following: <ul>
      * <li>ABORT_REASON_ACN_NOT_SUPPORTED
      * <li>ABORT_REASON_USER_SPECIFIC </ul> */
    public void setAbortReason(int abortReason) {
        m_abortReason = abortReason;
        m_abortReasonPresent = true;
    }
    /** Indicates if the Quality of Service parameter is present in this
      * Event.
      * @return
      * True if Quality of Service has been set, false otherwise.  */
    public boolean isQualityOfServicePresent() {
        return m_qualityOfServicePresent;
    }
    /** Gets the Quality of Service parameter of the UserAbort
      * indication primitive. Quality of Service is an SCCP parameter
      * that is required from the application.
      * @return
      * The Quality of Service parameter of the UserAbortEvent.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public byte getQualityOfService()
        throws ParameterNotSetException {
        if (m_qualityOfServicePresent)
            return m_qualityOfService;
        throw new ParameterNotSetException("Quality of Service: not set.");
    }
    /** Indicates if the User Abort Information parameter is present in
      * this Event.
      * @return
      * True if User Abort Information has been set, false otherwise.
      * */
    public boolean isUserAbortInformationPresent() {
        return m_userAbortInformationPresent;
    }
    /** Gets the User Abort Information parameter of the UserAbort
      * indication primitive. User Information is the information, which
      * can be exchanged between TCAP applications independently from
      * the remote operation service.
      * @return
      * The User Abort Information of the UserAbortEvent.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public byte[] getUserAbortInformation()
        throws ParameterNotSetException {
        if (m_userAbortInformationPresent)
            return m_userAbortInformation;
        throw new ParameterNotSetException("User Abort Information: not set.");
    }
    /** Indicates if the abort reason parameter is present in this
      * Event.
      * @return
      * True if abort reason has been set, false otherwise.  */
    public boolean isAbortReasonPresent() {
        return m_abortReasonPresent;
    }
    /** Gets the abort reason parameter of the UserAbort indication
      * primitive. Abort Reason indicates whether a dialogue is aborted
      * because the received application context name is not supported
      * and no alternative one can be proposed (abort reason =
      * application context not supported) or because of any other user
      * problem (abort reason = user specific).
      * @return
      * One of the following: <ul>
      * <li>ABORT_REASON_ACN_NOT_SUPPORTED
      * <li>ABORT_REASON_USER_SPECIFIC </ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public int getAbortReason()
        throws ParameterNotSetException {
        if (m_abortReasonPresent)
            return m_abortReason;
        throw new ParameterNotSetException("Abort Reason: not set.");
    }
    /** This method returns the type of this primitive.
      * @return
      * The Primitive Type of this Event.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_USER_ABORT;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_dialoguePortionPresent = false;
        m_dialogueIdPresent = false;
        m_qualityOfServicePresent = false;
        m_userAbortInformationPresent = false;
        m_abortReasonPresent = false;
    }
    /** java.lang.String representation of class UserAbortIndEvent.
      * @return
      * java.lang.String provides description of class UserAbortIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(500);
        b.append("\n\nUserAbortIndEvent");
        b.append(super.toString());
        b.append("\n\tm_abortReason = " + m_abortReason);
        b.append("\n\tm_qualityOfService = " + m_qualityOfService);
        b.append("\n\tm_userAbortInformation = ");
        if (m_userAbortInformation != null)
            for(int ptr=0; ptr < m_userAbortInformation.length; ptr++) {
                b.append(m_userAbortInformation[ptr]);
                b.append(" ");
            }
        b.append("\n\tm_qualityOfServicePresent = " + m_qualityOfServicePresent);
        b.append("\n\tm_userAbortInformationPresent = " + m_userAbortInformationPresent);
        b.append("\n\tm_abortReasonPresent = " + m_abortReasonPresent);
        return b.toString();
    }
    /** The Abort Reason parameter of the UserAbort indication dialogue
      * primitive.
      * @serial m_abortReason
      * - a default serializable field.  */
    private int m_abortReason = 0;
    /** The Quality of Service parameter of the UserAbort indication
      * dialogue primitive.
      * @serial m_qualityOfService
      * - a default serializable field.  */
    private byte m_qualityOfService = 0;
    /** The User Abort Information parameter of the UserAbort indication
      * dialogue primitive.
      * @serial m_userAbortInformation
      * - a default serializable field.  */
    private byte[] m_userAbortInformation = null;
    /** Whether Quality of Service is present.
      * @serial m_qualityOfServicePresent
      * - a default serializable field.  */
    private boolean m_qualityOfServicePresent = false;
    /** Whether User Abort Information is present.
      * @serial m_userAbortInformationPresent
      * - a default serializable field.   */
    private boolean m_userAbortInformationPresent = false;
    /** Whether Abort Reason is present.
      * @serial m_abortReasonPresent
      * - a default serializable field.  */
    private boolean m_abortReasonPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
