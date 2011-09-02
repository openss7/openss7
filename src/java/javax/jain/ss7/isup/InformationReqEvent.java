/*
 @(#) $RCSfile: InformationReqEvent.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:47 $ <p>
 
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
 
 Last Modified $Date: 2011-09-02 08:46:47 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The InformationReqEvent class is a sub class of the IsupEvent class and
    is exchanged between an ISUP Provider and an ISUP Listener for sending and receiving
    the ISUP Initial Address Message.
    Listener would send a InformationReqEvent object to the provider for sending a INR
    message to the ISUP stack. ISUP Provider would send a InformationReqEvent object to
    the listener on the reception of a INR message from the stack for the user address
    handled by that listener

    @author Monavacon Limited
    @version 1.2.2
  */
public abstract class InformationReqEvent extends IsupEvent {
    /** Constructs a new InformationReqEvent, with only the JAIN ISUP Mandatory parameters
        being supplied to the constructor.
        @param source  The source of this event.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param in_infoReqInd  Information Request Indicators.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    protected InformationReqEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            InfoReqInd in_infoReqInd)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setInfoReqInd(in_infoReqInd);
    }
    /** Gets the ISUP INFORMATION REQUEST primtive value.
        @return The ISUP INFORMATION REQUEST primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_INFORMATION_REQ;
    }
    /** Checks if the mandatory parameters have been set.
        @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
        is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getInfoReqInd();
    }
    /** Gets the Information Request Indicators parameter of the message.
        Refer to InfoReqInd parameter for greater details.
        @return The Information Request Indicators parameter of the event.
        @exception MandatoryParameterException  Thrown if the Mandatory Parameter is not
        set.
      */
    public InfoReqInd getInfoReqInd()
        throws MandatoryParameterNotSetException {
        if (m_informationReqInd != null)
            return m_informationReqInd;
        throw new MandatoryParameterNotSetException("InfoReqInd not set.");
    }
    /** Sets the Information Request Indicators parameter of the message.
        Refer to InfoReqInd parameter for greater details.
        @param informationReqInd  Information Request Indicators parameter.
      */
    public void setInfoReqInd(InfoReqInd informationReqInd) {
        m_informationReqInd = informationReqInd;
    }
    /** Gets the CallReference parameter of the message.
        Refer to CallReference parameter for greater details.
        @return The CallReference parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public CallReference getCallReference()
        throws ParameterNotSetException {
        if (m_callRef != null)
            return m_callRef;
        throw new ParameterNotSetException("CallReference not set.");
    }
    /** Sets the CallReference parameter of the message.
        Refer to CallReference parameter for greater details.
        @param callRef  The CallReference parameter of the event.
      */
    public void setCallReference(CallReference callRef) {
        m_callRef = callRef;
    }
    /** Indicates if the CallReference parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isCallReferencePresent() {
        return (m_callRef != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
        InformationReqEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.InformationReqEvent");
        b.append("\n\tm_informationReqInd: " + m_informationReqInd);
        b.append("\n\tm_callRef: " + m_callRef);
        return b.toString();
    }
    protected InfoReqInd m_informationReqInd = null;
    protected CallReference m_callRef = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
