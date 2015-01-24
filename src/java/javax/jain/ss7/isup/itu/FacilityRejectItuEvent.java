/*
 @(#) src/java/javax/jain/ss7/isup/itu/FacilityRejectItuEvent.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The FacilityRejectItuEvent class is a sub class of the IsupEvent class
    and is exchanged between an ISUP Provider and an ISUP Listener for sending and
    receiving the ISUP FacilityReject message.
    Listener would send a FacilityEventReject object to the provider for sending a FRJ
    message to the ISUP stack. ISUP Provider would send a FacilityEventReject object to
    the listener on the reception of a FRJ message from the stack for the user address
    handled by that listener.  The mandatory parameters are supplied to the constructor.
    Optional parameters may then be set using the set methods The primitive field is
    filled as ISUP_PRIMITIVE_FACILITY_REJECT.

    @author Monavacon Limited
    @version 1.2.2
  */
public class FacilityRejectItuEvent extends IsupEvent {
    /** Constructs a new FacilityRejectItuEvent, with only the JAIN ISUP Mandatory
        parameters being supplied to the constructor.
        @param source  The source of this event.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param facilityInd  Facility indicator.
        @param causeIndicator  Cause indicators.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public FacilityRejectItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            short facilityInd, CauseInd causeIndicator)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setFacilityInd(facilityInd);
        this.setCauseInd(causeIndicator);
    }
    /** Gets the ISUP FACILITY REJECT primtive value
        @return The ISUP FACILITY REJECT primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_FACILITY_REJECT;
    }
    /** Checks if the mandatory parameters have been set.
        @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
        is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getCauseInd();
    }
    /** Gets the Facility Indicator parameter of the message.
        @return The FacilityInd parameter of the event, range 0 tp 255.
      */
    public short getFacilityInd() {
        return m_facInd;
    }
    /** Sets the Facility Indicator parameter of the message.
      * @param in_facInd  The Facility Indicator parameter of the event, range 0 to 255.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setFacilityInd(short in_facInd)
        throws ParameterRangeInvalidException {
        if (0 <= in_facInd && in_facInd <= 255) {
            m_facInd = in_facInd;
            return;
        }
        throw new ParameterRangeInvalidException("FacilityInd value " + in_facInd + " out of range.");
    }
    /** Gets the Cause Indicator parameter of the message.
      * Refer to CauseInd parameter for greater details.
      * @return The CauseInd parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when the Mandatory parameter
      * is not set.
      */
    public CauseInd getCauseInd()
        throws MandatoryParameterNotSetException {
        if (m_causeIndicator != null)
            return m_causeIndicator;
        throw new MandatoryParameterNotSetException("CauseIndicator not set.");
    }
    /** Sets the Cause Indicator parameter of the message.
      * @param causeIndicator  The CauseInd parameter of the event.
      */
    public void setCauseInd(CauseInd causeIndicator) {
        m_causeIndicator = causeIndicator;
    }
    /** Gets the UserToUserIndicatorsItu parameter of the message.
      * Refer to UserToUserIndicatorsItu parameter for greater details.
      * @return The UserToUserIndicatorsItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public UserToUserIndicatorsItu getUserToUserIndicators()
        throws ParameterNotSetException {
        if (m_uui != null)
            return m_uui;
        throw new ParameterNotSetException("UserToUserIndicators not set.");
    }
    /** Sets the UserToUserIndicatorsItu parameter of the message.
      * Refer to UserToUserIndicatorsItu parameter for greater details.
      * @param uui  The UserToUserIndicatorsItu parameter of the event.
      */
    public void setUserToUserIndicators(UserToUserIndicatorsItu uui) {
        m_uui = uui;
    }
    /** Indicates if the UserToUserIndicatorsItu parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isUserToUserIndicatorsPresent() {
        return (m_uui != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * FacilityRejectItuEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.FacilityRejectItuEvent");
        b.append("\n\tm_facInd: " + m_facInd);
        b.append("\n\tm_causeIndicator: " + m_causeIndicator);
        b.append("\n\tm_uui: " + m_uui);
        return b.toString();
    }
    protected short m_facInd;
    protected CauseInd m_causeIndicator = null;
    protected UserToUserIndicatorsItu m_uui = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
