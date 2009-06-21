/*
 @(#) $RCSfile: CallProgressItuEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:08 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:08 $ by $Author: brian $
 */

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The CallProgressItuEvent class is a sub class of the JAIN ISUP Core
    CallProgressEvent class and is used for handling of parameters for the ITU variant of
    CPG message.

    @author Monavacon Limited
    @version 1.2.2
  */
public class CallProgressItuEvent extends CallProgressEvent {
    /** Constructs a new CallProgressItuEvent, with only the JAIN ISUP Mandatory
        parameters by calling the super class constructor.
        @param source  The source of this event.
        @param primitive  Primitive value is ISUP_PRIMITIVE_ALERT.  Refer to IsupEvent
        class for more details.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param eventInformation  Event Information.
        @exception ParameterRangeInvalidException  Thrown when value is out of range
      */
    public CallProgressItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            EventInformation eventInformation)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, eventInformation);
    }
    /** Gets the AccessDeliveryInfo parameter of the message.
        @return The AccessDeliveryInfo parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public byte getAccessDeliveryInfo()
        throws ParameterNotSetException {
        if (m_adiIsSet)
            return m_adi;
        throw new ParameterNotSetException("AccessDeliveryInfo not set.");
    }
    /** Sets the Access Delivery Information parameter of the message.
        @param adi  Access delivery information parameter of the event.
      */
    public void setAccessDeliveryInfo(byte adi) {
        m_adi = adi;
        m_adiIsSet = true;
    }
    /** Indicates if the AccessDeliveryInfo parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isAccessDeliveryInfoPresent() {
        return m_adiIsSet;
    }
    /** Gets the CallDiversionInfoItu parameter of the message.
        @return The CallDiversionInfoItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public CallDiversionInfoItu getCallDiversionInfo()
        throws ParameterNotSetException {
        if (m_cdi != null)
            return m_cdi;
        throw new ParameterNotSetException("CalLDiversionInfoItu not set.");
    }
    /** Sets the CallDiversionInfoItu parameter of the message.
        @param cdi  The CallDiversionInfoItu parameter of the event.
      */
    public void setCallDiversionInfo(CallDiversionInfoItu cdi) {
        m_cdi = cdi;
    }
    /** Indicates if the CallDiversionInfoItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isCallDiversionInfoPresent() {
        return (m_cdi != null);
    }
    /** Gets the NwSpecificFacItu parameter of the message.
        @return The NwSpecificFacItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public NwSpecificFacItu getNwSpecificFac()
        throws ParameterNotSetException {
        if (m_nsf != null)
            return m_nsf;
        throw new ParameterNotSetException("NwSpecificFacItu not set.");
    }
    /** Sets the NwSpecificFacItu parameter of the message.
        @param nsf  The NwSpecificFacItu parameter of the event.
      */
    public void setNwSpecificFac(NwSpecificFacItu nsf) {
        m_nsf = nsf;
    }
    /** Indicates if the NwSpecificFacItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isNwSpecificFacPresent() {
        return (m_nsf != null);
    }
    /** Gets the ParamCompatibilityInfoItu parameter of the message.
        @return The ParamCompatibilityInfoItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public ParamCompatibilityInfoItu getParamCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_pci != null)
            return m_pci;
        throw new ParameterNotSetException("ParamCompatibilityInfoItu not set.");
    }
    /** Sets the ParamCompatibilityInfoItu parameter of the message.
        @param pci  The ParamCompatibilityInfoItu parameter of the event.
      */
    public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci) {
        m_pci = pci;
    }
    /** Indicates if the ParamCompatibilityInfoItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isParamCompatibilityInfoPresent() {
        return (m_pci != null);
    }
    /** Gets the RedirectionNumberItu parameter of the message.
        @return The RedirectionNumberItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public RedirectionNumberItu getRedirectionNumber()
        throws ParameterNotSetException {
        if (m_rn != null)
            return m_rn;
        throw new ParameterNotSetException("RedirectionNumberItu not set.");
    }
    /** Sets the RedirectionNumberItu parameter of the message.
        @param rn  The RedirectionNumberItu parameter of the event.
      */
    public void setRedirectionNumber(RedirectionNumberItu rn) {
        m_rn = rn;
    }
    /** Indicates if the RedirectionNumberItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isRedirectionNumberPresent() {
        return (m_rn != null);
    }
    /** Gets the RedirectionNumberRest parameter of the message.
        @return The RedirectionNumberRest parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public byte getRedirectionNumberRest()
        throws ParameterNotSetException {
        if (m_rnrIsSet)
            return m_rnr;
        throw new ParameterNotSetException("RedirectionNumberRest not set.");
    }
    /** Sets the RedirectionNumberRest parameter of the message.
        @param rnr  The RedirectionNumberRest parameter of the event.
      */
    public void setRedirectionNumberRest(byte rnr) {
        m_rnr = rnr;
        m_rnrIsSet = true;
    }
    /** Indicates if the RedirectionNumberRest parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isRedirectionNumberRestPresent() {
        return m_rnrIsSet;
    }
    /** The toString method retrieves a string containing the values of the members of the
        CallProgressItuEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.CallProgressItuEvent");
        if (m_adiIsSet)
            b.append("\n\tm_adi: " + m_adi);
        b.append("\n\tm_cdi: " + m_cdi);
        b.append("\n\tm_nsf: " + m_nsf);
        b.append("\n\tm_pci: " + m_pci);
        b.append("\n\tm_rn: " + m_rn);
        if (m_rnrIsSet)
            b.append("\n\tm_rnr: " + m_rnr);
        return b.toString();
    }
    protected byte m_adi;
    protected boolean m_adiIsSet = false;
    protected CallDiversionInfoItu m_cdi = null;
    protected NwSpecificFacItu m_nsf = null;
    protected ParamCompatibilityInfoItu m_pci = null;
    protected RedirectionNumberItu m_rn = null;
    protected byte m_rnr;
    protected boolean m_rnrIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
