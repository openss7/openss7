/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The AnswerItuEvent class is a sub class of the JAIN ISUP Core
    AnswerEvent class and is used for handling of parameters for the ANSI variant of
    AnswerEvent Message.
    @author Monavacon Limited
    @version 1.2.2
  */
public class AnswerItuEvent extends AnswerEvent {
    /** Constructs a new AnswerItuEvent, with only the JAIN ISUP Mandatory parameters
        being supplied to the constructor.
        @param source  The source of this event.
        @param primitive  Primitive value for StartResetInd is ISUP_PRIMITIVE_ANSWER.
        Refer to IsupEvent class for more details.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public AnswerItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
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
    /** Gets the CallHistoryInfo parameter of the message.
        @return The CallHistoryInfo parameter of the event, range 0 to 0xffffffff.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public long getCallHistoryInfo()
        throws ParameterNotSetException {
        if (m_callHistInfoIsSet)
            return m_callHistInfo;
        throw new ParameterNotSetException("CallHistoryInfo not set.");
    }
    /** Sets the Call History Information parameter in the message.
        @param callHistInfo  Call History Information parameter of the event, range 0 to
        0xffffffff.
      */
    public void setCallHistoryInfo(long callHistInfo) {
        m_callHistInfo = callHistInfo;
        m_callHistInfoIsSet = true;
    }
    /** Indicates if the CallHistoryInfo parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isCallHistoryInfoPresent() {
        return m_callHistInfoIsSet;
    }
    /** Gets the ConnectedNumberItu parameter of the message.
        @return The ConnectedNumberItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public ConnectedNumberItu getConnectedNumber()
        throws ParameterNotSetException {
        if (m_connNumItu != null)
            return m_connNumItu;
        throw new ParameterNotSetException("ConnectedNumberItu not set.");
    }
    /** Sets the Call History Information parameter in the message.
        @param connNumItu  Connected Number parameter of the event.
      */
    public void setConnectedNumber(ConnectedNumberItu connNumItu) {
        m_connNumItu = connNumItu;
    }
    /** Indicates if the ConnectedNumberItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isConnectedNumberPresent() {
        return (m_connNumItu != null);
    }
    /** Gets the EchoControlInfoItu parameter of the message.
        @return The EchoControlInfoItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public EchoControlInfoItu getEchoControlInfo()
        throws ParameterNotSetException {
        if (m_eci != null)
            return m_eci;
        throw new ParameterNotSetException("EchoControlInfoItu not set.");
    }
    /** Sets the EchoControlInfoItu parameter of the message.
        @param eci  The EchoControlInfoItu parameter of the event.
      */
    public void setEchoControlInfo(EchoControlInfoItu eci) {
        m_eci = eci;
    }
    /** Indicates if the EchoControlInfoItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isEchoControlInfoPresent() {
        return (m_eci != null);
    }
    /** Gets the GenericNumberItu parameter of the message.
        @return The GenericNumberItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public GenericNumberItu getGenericNumber()
        throws ParameterNotSetException {
        if (m_genNumItu != null)
            return m_genNumItu;
        throw new ParameterNotSetException("GenericNumberItu not set.");
    }
    /** Sets the GenericNumberItu parameter of the message.
        @param genNumItu  Generic Number parameter of the event.
      */
    public void setGenericNumber(GenericNumberItu genNumItu) {
        m_genNumItu = genNumItu;
    }
    /** Indicates if the GenericNumbeItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isGenericNumberPresent() {
        return (m_genNumItu != null);
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
        AnswerItuEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.AnswerItuEvent");
        if (m_adiIsSet)
            b.append("\n\tm_adi: " + m_adi);
        if (m_callHistInfoIsSet)
            b.append("\n\tm_callHistInfo: " + m_callHistInfo);
        b.append("\n\tm_connNumItu: " + m_connNumItu);
        b.append("\n\tm_eci: " + m_eci);
        b.append("\n\tm_getNumItu: " + m_genNumItu);
        b.append("\n\tm_nsf: " + m_nsf);
        b.append("\n\tm_pci: " + m_pci);
        b.append("\n\tm_rn: " + m_rn);
        b.append("\n\tm_rnr: " + m_rnr);
        if (m_rnrIsSet)
            b.append("\n\tm_rnr: " + m_rnr);
        return b.toString();
    }
    protected byte m_adi;
    protected boolean m_adiIsSet = false;
    protected long m_callHistInfo;
    protected boolean m_callHistInfoIsSet = false;
    protected ConnectedNumberItu m_connNumItu = null;
    protected EchoControlInfoItu m_eci = null;
    protected GenericNumberItu m_genNumItu = null;
    protected NwSpecificFacItu m_nsf = null;
    protected ParamCompatibilityInfoItu m_pci = null;
    protected RedirectionNumberItu m_rn = null;
    protected byte m_rnr;
    protected boolean m_rnrIsSet = false;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
