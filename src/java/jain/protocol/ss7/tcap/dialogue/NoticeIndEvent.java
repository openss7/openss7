/*
 @(#) $RCSfile: NoticeIndEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:31 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:31 $ by $Author: brian $
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An event representing a TCAP Notice indication dialogue primitive.
  * This event will be passed from the Provider (TCAP) to the
  * Listener(the TC User) to indicate that the Network Service Provider
  * has been unable to provide the requested service. <br>
  * The mandatory parameters of this primitive are supplied to the
  * constructor. Optional parameters may then be set using the set
  * methods.
  * @version 1.2.2
  * @author Monavacon Limited
  * @see DialogueIndEvent
  */
public final class NoticeIndEvent extends DialogueIndEvent {
    /** Constructs a new NoticeIndEvent, with only the Event Source and
      * the JAIN TCAP Mandatory parameters being supplied to the
      * constructor.
      * @param source
      * The Event Source supplied to the constructor.
      * @param dialogueId
      * The Dialogue Identifier supplied to the constructor.
      * @param reportCause
      * The Report Cause supplied to the constructor.  */
    public NoticeIndEvent(java.lang.Object source, int dialogueId,
            byte[] reportCause) {
        super(source);
        setDialogueId(dialogueId);
        setReportCause(reportCause);
    }
    /** Sets the Destination Address parameter of the Notice indication
      * primitive. Destination Address is an SCCP parameter that is
      * required from the application. Within this method
      * TcapUserAddress has been changed to SccpUserAddress. The method
      * has not been deprecated as a new method using type
      * SccpUserAddress would have to be created with a different method
      * name. This is less desirable than the effective removal of the
      * old method.
      * @param destinationAddress
      * The new Destination Address value.  */
    public void setDestinationAddress(SccpUserAddress destinationAddress) {
        m_destinationAddress = destinationAddress;
        m_destinationAddressPresent = true;
    }
    /** Sets the Originating Address parameter of the Notice indication
      * primitive. Origination Address is an SCCP parameter that is
      * required from the application. Within this method
      * TcapUserAddress has been changed to SccpUserAddress. The method
      * has not been deprecated as a new method using type
      * SccpUserAddress would have to be created with a different method
      * name. This is less desirable than the effective removal of the
      * old method.
      * @param originatingAddress
      * The new Originating Address value.  */
    public void setOriginatingAddress(SccpUserAddress originatingAddress) {
        m_originatingAddress = originatingAddress;
        m_originatingAddressPresent = true;
    }
    /** Sets the Report Cause parameter of this Notice indication
      * primitive.
      * @param reportCause
      * The new Report Cause value.  */
    public void setReportCause(byte[] reportCause) {
        m_reportCause = reportCause;
        m_reportCausePresent = true;
    }
    /** Indicates if the Destination Address parameter is present in
      * this Event.
      * @return
      * True if Destination Address has been set, false otherwise.  */
    public boolean isDestinationAddressPresent() {
        return m_destinationAddressPresent;
    }
    /** Gets the Destination Address parameter of the Notice indication
      * primtive. Destination Address is an SCCP parameter that is
      * required from the application. The return type of this get
      * method has been changed from TcapUserAddress. The
      * TcapUserAddress class has been deprecated in this release
      * (V1.1). This method has not been deprecated as it's replacement
      * would then have to have a different name.
      * @return
      * The SccpUserAddress representing the Destination Address of the
      * Notice indication primtive.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public SccpUserAddress getDestinationAddress()
        throws ParameterNotSetException {
        if (m_destinationAddressPresent)
            return m_destinationAddress;
        throw new ParameterNotSetException("Destination Address: not set.");
    }
    /** Indicates if the Originating Address parameter is present in
      * this Event.
      * @return
      * True if Originating Address has been set, false otherwise.  */
    public boolean isOriginatingAddressPresent() {
        return m_originatingAddressPresent;
    }
    /** Gets the Originating Address parameter of the Notice indication
      * primitive. Origination Address is an SCCP parameter that is
      * required from the application. The return type of this get
      * method has been changed from TcapUserAddress. The
      * TcapUserAddress class has been deprecated in this release
      * (V1.1). This method has not been deprecated as it's replacement
      * would then have to have a different name.
      * @return
      * The SccpUserAddress represnting the Originating Address of the
      * Notice Event.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public SccpUserAddress getOriginatingAddress()
        throws ParameterNotSetException {
        if (m_originatingAddressPresent)
            return m_originatingAddress;
        throw new ParameterNotSetException("Originating Address: not set.");
    }
    /** Indicates if the Report Cause parameter is present in this
      * Event.
      * @return
      * True if Report Cause has been set, false otherwise.  */
    public boolean isReportCausePresent() {
        return m_reportCausePresent;
    }
    /** Gets the Report Cause parameter of this Notice indication
      * primitive. Report Cause contains information indicating the
      * reason that the message was returned by the SCCP
      * @return
      * The report cause of this Notice indication primitive.
      * @exception ParameterNotSetException
      * This exception is thrown if this Mandatory parameter has not yet
      * been set.  */
    public byte[] getReportCause()
        throws ParameterNotSetException {
        if (m_reportCausePresent)
            return m_reportCause;
        throw new ParameterNotSetException("Report Cause: not set.");
    }
    /** This method returns the type of this primitive.
      * @return
      * The Primitive Type value.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_NOTICE;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_dialoguePortionPresent = false;
        m_dialogueIdPresent = false;
        m_destinationAddressPresent = false;
        m_originatingAddressPresent = false;
        m_reportCausePresent = false;
    }
    /** java.lang.String representation of class NoticeIndEvent.
      * @return
      * java.lang.String provides description of class NoticeIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nNoticeIndEvent");
        b.append(super.toString());
        b.append("\n\tm_destinationAddress = ");
        if (m_destinationAddress != null)
            b.append(m_destinationAddress.toString());
        b.append("\n\tm_originatingAddress = ");
        if (m_originatingAddress != null)
            b.append(m_originatingAddress.toString());
        b.append("\n\tm_reportCause = ");
        if (m_reportCause != null)
            b.append(m_reportCause.toString());
        b.append("\n\tm_destinationAddressPresent = " + m_destinationAddressPresent);
        b.append("\n\tm_originatingAddressPresent = " + m_originatingAddressPresent);
        b.append("\n\tm_reportCausePresent = " + m_reportCausePresent);
        return b.toString();
    }
    /** The Destination User Address parameter of the Notice indication
      * dialogue primitive.
      * @serial m_destinationAddress
      * - a default serializable field.  */
    private SccpUserAddress m_destinationAddress = null;
    /** The Originating User Address parameter of the Notice indication
      * dialogue primitive.
      * @serial m_originatingAddress
      * - a default serializable field.  */
    private SccpUserAddress m_originatingAddress = null;
    /** The Report Cause parameter of the Notice indication dialogue
      * primitive.
      * @serial m_reportCause
      * - a default serializable field.  */
    private byte[] m_reportCause = null;
    /** Whether Destination Address is present.
      * @serial m_destinationAddressPresent
      * - a default serializable field.  */
    private boolean m_destinationAddressPresent = false;
    /** Whether Originating Address is present.
      * @serial m_originatingAddressPresent
      * - a default serializable field.  */
    private boolean m_originatingAddressPresent = false;
    /** Whether Report Cause is present.
      * @serial m_reportCausePresent
      * - a default serializable field.  */
    private boolean m_reportCausePresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
