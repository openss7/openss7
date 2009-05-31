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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;
import javax.*;

/** The IsupEvent class is the supercalss of all ISUP primitives that can be exchanged
  * between an object implementing the JainIsupProvider interface and an object
  * implementing the JainIsupListener interface.  This consists of the primitive for
  * ISUP protocol messages, primitives for ISUP error indications, primitives for
  * timeout indications and primitives for network status indications.  These event
  * classes corresponding to each kind of primitives extend the IsupEvent class.  The
  * IsupEvent class provides methods for getting and setting the ISUP primitive, the
  * Destination Point Code, the Originating Point Code, the Signalling Link Selection
  * and the Circuit Identification Code associated with an ISUP Event.  This class
  * implements the <i>java.lang.Cloneable</i> interface, therefore all message that
  * extend to this Event can be cloned using the clone() method inherited from
  * <i>java.lang.Object</i>.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class IsupEvent extends java.util.EventObject implements java.lang.Cloneable {
    /** Constructs a new IsupEvent and sets the source of the event.
      * @param source The source of this event.  */
    protected IsupEvent(java.lang.Object source) {
        super(source);
    }
    /** Constructs a new IsupEvent and sets the source, dpc, opc, sls and cic.
      * @param source The source of this event that is either the JainIsupListener
      * object or the JainIsupProvider object.
      * @param dpc The destination point code in the event.
      * @param opc The originating point code in the event.
      * @param sls The signaling link selection in the event.
      * @param cic The circuit identification code in the event.
      * @param congestionPriority The congestion priority of the event.
      * @exception ParameterRangeInvalidException Thrown when a value is out of range.  */
    protected IsupEvent(java.lang.Object source, SignalingPointCode dpc, SignalingPointCode opc, byte sls, int cic, byte congestionPriority) throws ParameterRangeInvalidException {
        this(source);
        this.setDpc(dpc);
        this.setOpc(opc);
        this.setSls(sls);
        this.setCic(cic);
        this.setCongestionPriority(congestionPriority);
    }
    /** Gets the Isup Primitive value.  This method should be implemented by each event
      * class.  The primitive value gives more information on the event being
      * exchanged; e.g., MainControl event is sent from JainIsupProvider object ot
      * JainIsupListener object to indicate that a maintenance event has occurred on
      * the stack.  The ISUP primitive field indicates the kind of maintenance event,
      * for example, whether it is a Blocking or Unblocking event.
      * @return The ISUP primtiive value in an ISUP event.
      * @exception MandatoryParameterNotSetException Thrown when mandatory ISUP
      * parameter is not set.  */
    public abstract int getIsupPrimitive();
    /** Gets the destination point code.  The destination point code is a signaling
      * point code, the format of which is described in the IsupUserAddress class.
      * Refer SignalingPointCode for a full description of destination point code.
      * @return The destination point code in an ISUP event.
      * @exception MandatoryParameterNotSetException Thrown when mandatory ISUP
      * parameter is not set.  */
    public SignalingPointCode getDpc() throws MandatoryParameterNotSetException {
        if (m_dpc != null)
            return m_dpc;
        throw new MandatoryParameterNotSetException("DestinationPointCode not set.");
    }
    /** Gets the origination point code. The origination point code is a signalling
      * point code, the format for the signaling point code is described in the
      * IsupUserAddress class.  Refer to SignalingPointCode for a full description of
      * origination point code.
      * @return The origination point code.
      * @exception MandatoryParameterNotSetException Thrown when mandatory ISUP
      * parameter is not set.  */
    public SignalingPointCode getOpc() throws MandatoryParameterNotSetException {
        if (m_opc != null)
            return m_opc;
        throw new MandatoryParameterNotSetException("OriginatingPointCode not set.");
    }
    /** Gest the signalling link selection field in the ISUP event.  The signaling link
      * selection has a range of 0 to 15 in ITU and 0-255 in ANSI.
      * @return The signaling link selection in an ISUP event.  */
    public byte getSls() {
        return m_sls;
    }
    /** Gets the circuit identification code.  The circuit identification code
      * identifies a voice circuit between two nodes in the SS7 network.  Refer to
      * CICrange for a description.
      * @return The circuit ideentification code in an ISUP event.  */
    public int getCic() {
        return m_cic;
    }
    /** Gets the congestion priority value of the message.  The congestion priority
      * indicates the priority with which the ISUP message is to be passed to MTP in
      * the MTP-TRANSFER-Request service primitive.  The congestion priority field will
      * be used by MTP during congestion control procedures wherein only those User
      * Part Messages that have greater congestion priority than the congestion level
      * reported for a link or signalling point code, are sent to the network; while
      * the ISUP messages with a priority less than the congestion status are discarded
      * by MTP.  The congestion control procedure is only specified for national
      * networks and it is an optional procedure.  Hence, this field may be used only
      * when the Network Indicator indicates a national network.  That is, when the
      * network indicator is 2 or 3.  For international network indicator values, the
      * congestion priority field is to be coded as zero (0).  This field also does not
      * have any significance at the incoming end.  The priority field held in an
      * IsupEvent from the JainIsupProvider ot JainIsupListener object has no relevance
      * for international networks. <p>
      *
      * For national networks the value of 3 is reserved for MTP management messages
      * and is invalid in any IsupEvent message.  Some IsupEvent messages restrict
      * further the setting of the congestion priority and some messages only permit
      * one value.  Other permit selection only between 0 or 1.  When this value is not
      * set for events passed from the JainIsupListener object to the JainIsupProvider
      * object, the JainIsupProvider (or the underlying ISUP stack implementation) will
      * assign a value.  The value of the received message will be set in all
      * IsupEvents passed from the JainIsupProvider object to the
      * JainIsupListenerObject.  For international networks, the value will not be set.
      *
      * @return The congestion priority of the message being sent to MTP.  The value
      * ranges from 0 to 3, 0 being the lowest priorty and 3 the highest.  Priority
      * level 3 is reserved for MTP management messages and can neither be sent nor
      * returned.  */
    public byte getCongestionPriority() {
        return m_mp;
    }
    /** Sets the destination point code. The destination point code is a signaling
      * point code, the format of which is described in the IsupUserAddress class.
      * Refer to SignalingPointCode for a description of destination point code.
      * @param destpc The destination point code in an ISUP event.
      * @exception ParameterRangeInvalidException Thrown when a value is out of range.  */
    public void setDpc(SignalingPointCode destpc) throws ParameterRangeInvalidException {
        try {
            int[] pc = new int[3];
            pc[0] = destpc.getMember();
            pc[1] = destpc.getCluster();
            pc[2] = destpc.getZone();
        } catch (MandatoryParameterNotSetException e) {
            throw new ParameterRangeInvalidException("Destination Point Code out of range.");
        }
        m_dpc = destpc;
    }
    /** Sets the originating point code.  The originating point code is a signaling
      * point code, the format of which is described in the IsupUserAddress class.
      * Refer to SignalingPointCode for a description of originating point code.
      * @param origpc The origination point code in an ISUP event.
      * @exception ParameterRangeInvalidException Thrown when a value is out of range.  */
    public void setOpc(SignalingPointCode origpc) throws ParameterRangeInvalidException {
        try {
            int[] pc = new int[3];
            pc[0] = origpc.getMember();
            pc[1] = origpc.getCluster();
            pc[2] = origpc.getZone();
        } catch (MandatoryParameterNotSetException e) {
            throw new ParameterRangeInvalidException("Origination Point Code out of range.");
        }
        m_opc = origpc;
    }
    /** Sets the signalling link selection field in the ISUP event.  The signalling
      * link selection has a range of 0 to 15 for ITU and a range of 0 to 31 for 5-bit
      * ANSI SLS and 0 to 255 for 8-bit ANSI SLS.
      * @param isls The SLS value in the ISUP event.  */
    public void setSls(byte isls) {
        m_sls = isls;
    }
    /** Sets the circuit identification code.  The circuit identification code
      * identifies a voice circuit between two nodes in the SS7 network.  Refern to
      * CICrange for a description.  The range of the value is between 0 to 4095 for
      * ITU and 0 to 4095 for 12-bit CIC ANSI and 0 to 16383 for 14-bit CIC ANSI.
      * @param cic The circuit identification code in an ISUP event.  */
    public void setCic(int cic) throws ParameterRangeInvalidException {
        if (0 > cic || cic > 16383)
            throw new ParameterRangeInvalidException("Circuit Identification Code " + cic + " is out of range.");
        m_cic = cic;
    }
    /** Sets the congestion priority value of the ISUP message being sent to MTP.
      * Refere to getMessagePriority for more detail on the congestion priority field
      * of the IsupEvent class.
      * @param congestionPriority  The congestion priority of the ISUP message.  Values
      * range from 0 to 3, 0 being the lower priority and 3 being the highest.  The
      * value 3 is reserved for MTP management messages and will never appear in an
      * IsupEvent.  */
    public void setCongestionPriority(byte congestionPriority) throws ParameterRangeInvalidException {
        if (0 > congestionPriority || congestionPriority > 2)
            throw new ParameterRangeInvalidException("Congestion Prioirty value " + congestionPriority + " is out of range.");
        m_mp = congestionPriority;
    }

    /** Checks if all mandatory parameters in an ISUP event are set.  This method can
      * be used to check if all the Mandatory Parameters have been set in an ISUP Event
      * or not.  */
    public void checkMandatoryParameters() throws MandatoryParameterNotSetException {
        if (m_dpc == null || m_opc == null)
            throw new MandatoryParameterNotSetException("Point code not set.");
    }
    /** Gets the IsupParameter parameter of the messsage.  The IsupParameter cna be
      * used to send and receive extension parameter information.  Refer to
      * IsupParameter for details.
      * @return The IsupParameter parameter of the event.
      * @exception ParameterNotSetException Thrown when the optional parameter is not
      * set.  */
    public IsupParameter getIsupParameter() throws ParameterNotSetException {
        if (isIsupParameterPresent() == false)
            throw new ParameterNotSetException("Isup Parameter is not set.");
        return m_oparms;
    }
    /** Sets the IsupParameter parameter of the message.  The IsupParameter can be used
      * to send and receive extension parameter information.  Refer to IsupParameter
      * for details.
      * @param isupParam The IsupParameter parameter for the event.  */
    public void setIsupParameter(IsupParameter isupParam) {
        m_oparms = isupParam;
    }
    /** Indicates if the IsupParameter parameter is present in this Event.  */
    public boolean isIsupParameterPresent() {
        if (m_oparms != null)
            return true;
        return false;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the IsupEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\nIsupEvent:");
        buffer.append("\n\tdpc =" + m_dpc);
        buffer.append("\n\topc =" + m_opc);
        buffer.append("\n\tsls =" + m_sls);
        buffer.append("\n\tcic =" + m_cic);
        buffer.append("\n\tpri =" + m_mp);
        buffer.append("\n\tparm =" + m_oparms);
        return buffer.toString();
    }
    private SignalingPointCode m_dpc = null;
    private SignalingPointCode m_opc = null;
    private byte m_sls;
    private int m_cic;
    private byte m_mp;
    private IsupParameter m_oparms = null;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
