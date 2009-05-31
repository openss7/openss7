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

/** An ISUP EVENT: The SuspendResumeEvent class is a sub class of the IsupEvent class
  * and is exchanged between an ISUP Provider and an ISUP Listener for sending and
  * receiving the ISUP Suspend/Resume message.
  * Listener would send a SuspendResumeEvent object to the provider for sending a
  * SUS/RES message to the ISUP stack. ISUP Provider would send a SuspendResumeEvent
  * object to the listener on the reception of a SUS/RES message from the stack for
  * the user address handled by that listener. The mandatory parameters are supplied
  * to the constructor.  Optional parameters may then be set using the set methods
  * Note:
  * If the message to be sent to the network is SUS, then the primitive field
  * is filled as ISUP_PRIMITIVE_SUSPEND and if the message to be sent to the network
  * is RES, then the primitive is filled as ISUP_PRIMITIVE_RESUME
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SuspendResumeEvent extends IsupEvent {
    /** Constructs a new SuspendResumeEvent, with only the JAIN ISUP Mandatory
      * parameters being supplied to the constructor.
      * @param source  The source of this event.
      * @param primitive  Primitive value for SuspendResume Event is
      * ISUP_PRIMITIVE_SUSPEND or ISUP_PRIMITIVE_RESUME depending on whether the
      * message to be sent is SUS or RES.
      * @param dpc  The destination point code
      * @param opc  The origination point code
      * @param sls  The signaling link selection
      * @param cic  The CIC on which the call has been established
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * suspendResumeInd suspend resume indicators 1 if network initiated and 0 if
      * subscriber initiated.
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * set correctly.  */
    public SuspendResumeEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            int primitive, byte suspendResumeInd)
        throws PrimitiveInvalidException, ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setIsupPrimitive(primitive);
        this.setSuspendResumeInd(suspendResumeInd);
    }
    /** Gets the ISUP primtive value.
      * @return One of the following ISUP primitive values. If ISUP_PRIMITIVE_UNSET is
      * returned, it indicates the primitive value is not set, hence this is an
      * invalid event. <ul> <li>ISUP_PRIMITIVE_UNSET, <li>ISUP_PRIMITIVE_SUSPEND and
      * <li>ISUP_PRIMITIVE_RESUME. </ul> */
    public int getIsupPrimitive() {
        return m_primitive;
    }
    /** Sets the ISUP primtive value.
      * @param i_isupPrimitive  The value of isup primitive from; <ul>
      * <li>ISUP_PRIMITIVE_SUSPEND and <li>ISUP_PRIMITIVE_RESUME. </ul>
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * set correctly.  */
    public void setIsupPrimitive(int i_isupPrimitive)
        throws PrimitiveInvalidException {
        switch (i_isupPrimitive) {
            case IsupConstants.ISUP_PRIMITIVE_SUSPEND:
            case IsupConstants.ISUP_PRIMITIVE_RESUME:
                m_primitive = i_isupPrimitive;
                return;
        }
        throw new PrimitiveInvalidException("IsupPrimitive value " + i_isupPrimitive + " out of range.");
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException - Thrown when the mandatory
      * parameter is not set.  */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        if (m_primitive == IsupConstants.ISUP_PRIMITIVE_UNSET)
            throw new MandatoryParameterNotSetException("IsupPrimitve not set.");
        if (m_suspendResumeIndIsSet)
            throw new MandatoryParameterNotSetException("SuspendResumeInd not set.");
    }
    /** Gets the Suspend Resume Indicator parameter of the message.
      * @return The SuspendResumeInd parameter of the event, 1 if network initiated
      * and 0 if subscriber initiated.  */
    public byte getSuspendResumeInd() {
        return m_suspendResumeInd;
    }
    /** Sets the Suspend Resume Indicator parameter of the message.
      * @param suspendResumeInd  The SuspendResumeInd parameter of the event, 1 if
      * network initiated and 0 if subscriber initiated.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setSuspendResumeInd(byte suspendResumeInd)
        throws ParameterRangeInvalidException {
        switch (suspendResumeInd) {
        case 0:
        case 1:
            m_suspendResumeInd = suspendResumeInd;
            m_suspendResumeIndIsSet = true;
            return;
        }
        throw new ParameterRangeInvalidException("SuspendResumeInd value " + suspendResumeInd + " out of range.");
    }
    /** Gets the CallReference parameter of the message.
      * Refer to CallReference parameter for greater details.
      * @return The CallReference parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public CallReference getCallReference()
        throws ParameterNotSetException {
        if (m_callRef != null)
            return m_callRef;
        throw new ParameterNotSetException("CallReference not set.");
    }
    /** Sets the CallReference parameter of the message.
      * Refer to CallReference parameter for greater details.
      * @param callRef  The CallReference parameter of the event.  */
    public void setCallReference(CallReference callRef) {
        m_callRef = callRef;
    }
    /** Indicates if the CallReference parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isCallReferencePresent() {
        return (m_callRef != null);
    }
    /** The toString method retrieves a string containing the values of the members of
      * the SuspendResumeEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.SuspendResumeEvent");
        b.append("\n\tm_primitive : " + m_primitive );
        if (m_suspendResumeIndIsSet)
        b.append("\n\tm_suspendResumeInd: " + m_suspendResumeInd);
        b.append("\n\tm_callRef : " + m_callRef );
        return b.toString();
    }
    protected int m_primitive = IsupConstants.ISUP_PRIMITIVE_UNSET;
    protected byte m_suspendResumeInd;
    protected boolean m_suspendResumeIndIsSet = false;
    protected CallReference m_callRef = null;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
