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

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The CircuitValidationRespAnsiEvent class is a subclass of the IsupEvent
  * class and is used for exchanging between an ISUP Provider and an ISUP Listener for
  * sending the ANSI ISUP Circuit Validation Response message (CVR).
  * @author Monavacon
  * @version 1.2.2
  */
public class CircuitValidationRespAnsiEvent extends IsupEvent {
    /** Constructs a new EventCircuitValidationResp for ANSI variant, with only the JAIN
      * ISUP Mandatory parameters using the super class constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param in_circuitValResp  Circuit Validation Response, 0 validation successful ;
      * 1 validation failure.
      * @param in_circuitGrpCharacteristicsInd  Circuit Group Characteristics Indicators.
      * Refer to CircuitGrpCharacteristicsInd class for more details.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CircuitValidationRespAnsiEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            short in_circuitValResp,
            CircuitGrpCharacteristicsIndAnsi in_circuitGrpCharacteristicsIndAnsi)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setCircuitValidationResp(in_circuitValResp);
        this.setCircuitGrpCharacteristicsInd(in_circuitGrpCharacteristicsIndAnsi);
    }
    /** Gets the ISUP CIRCUIT VALIDATION RESPONSE primtive value.
      * @return The ISUP CIRCUIT VALIDATION RESPONSE primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_RESP;
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
      * is not set.  */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getCircuitValidationResp();
        getCircuitGrpCharacteristicsInd();
    }
    /** Gets the Circuit Validation Response parameter of the message.
      * @return The Circuit Validation Response parameter of the event, 0 for validation
      * successful and 1 for validation failure.  */
    public short getCircuitValidationResp() {
        return m_cktValResp;
    }
    /** Sets the Circuit Validation Response parameter of the message.
      * @param in_cktValResp  The Circuit Validation Response parameter of the event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setCircuitValidationResp(short in_cktValResp)
        throws ParameterRangeInvalidException {
        switch (in_cktValResp) { case 0: case 1: m_cktValResp = in_cktValResp; return; }
        throw new ParameterRangeInvalidException("cktValResp value " + in_cktValResp + " out of range.");
    }
    /** Gets the Circuit Group Characteristics indicators parameter of the message.
      * Refer to CircuitGrpCharacteristicsInd parameter class for more information.
      * @return The Circuit Group Characteristics Indicator parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when mandatory ISUP
      * parameter is not set.  */
    public CircuitGrpCharacteristicsIndAnsi getCircuitGrpCharacteristicsInd()
        throws MandatoryParameterNotSetException {
        if (m_circuitGrpCharacteristicsIndAnsi != null)
            return m_circuitGrpCharacteristicsIndAnsi;
        throw new MandatoryParameterNotSetException("CircuitGrpCharacteristicsIndAnsi not set.");
    }
    /** Sets the Circuit Group Characteristics Indicator parameter of the message.
      * Refer to CircuitGrpCharacteristicsInd parameter class for more information.
      * @param in_natureConnIndthe  Circuit Group Characteristics Indicator parameter of
      * the event.  */
    public void setCircuitGrpCharacteristicsInd(CircuitGrpCharacteristicsIndAnsi in_circuitGrpCharacteristicsIndAnsi) {
        m_circuitGrpCharacteristicsIndAnsi = in_circuitGrpCharacteristicsIndAnsi;
    }
    /** Gets the CircuitIdNameAnsi parameter of the message.
      * @return The CircuitIdNameAnsi parameter of the event Refer to Circuit
      * Identification Name class for more details.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public CircuitIdNameAnsi getCircuitIdName()
        throws ParameterNotSetException {
        if (m_circuitIdName != null)
            return m_circuitIdName;
        throw new ParameterNotSetException("CircuitIdNameAnsi not set.");
    }
    /** Sets the CircuitIdNameAnsi parameter of the message.
      * @param in_circuitIdName  The CircuitIdName parameter of the event Refer to
      * Circuit Identification Name class for more details.  */
    public void setCircuitIdName(CircuitIdNameAnsi in_circuitIdName) {
        m_circuitIdName = in_circuitIdName;
    }
    /** Indicates if the CircuitIdNameAnsi parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isCircuitIdNamePresent() {
        return (m_circuitIdName != null);
    }
    /** Gets the CommonLangLocationIdAnsi parameter of the event.
      * @return The CommonLangLocationIdAnsi parameter of the event.  Refer to Common
      * Language Location Id class for more details.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public CommonLangLocationIdAnsi getCommonLangLocationId()
        throws ParameterNotSetException {
        if (m_commonLangLocationIdAnsi != null)
            return m_commonLangLocationIdAnsi;
        throw new ParameterNotSetException("CommonLangLocationIdAnsi not set.");
    }
    /** Sets the CommonLangLocationIdAnsi parameter of the message.
      * @param in_commonLangLocationIdAnsi  The CommonLangLocationIdAnsi parameter of the
      * event.  Refer to Common Language Location Id class for more details.  */
    public void setCommonLangLocationId(CommonLangLocationIdAnsi in_commonLangLocationIdAnsi) {
        m_commonLangLocationIdAnsi = in_commonLangLocationIdAnsi;
    }
    /** Indicates if the CommonLangLocationIdAnsi parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isCommonLangLocationIdPresent() {
        return (m_commonLangLocationIdAnsi != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * CircuitValidationRespAnsiEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CircuitValidationRespAnsiEvent");
        b.append("\n\tm_cktValResp : " + m_cktValResp );
        b.append("\n\tm_circuitGrpCharacteristicsIndAnsi : " + m_circuitGrpCharacteristicsIndAnsi );
        b.append("\n\tm_circuitIdName : " + m_circuitIdName );
        b.append("\n\tm_commonLangLocationIdAnsi : " + m_commonLangLocationIdAnsi );
        return b.toString();
    }
    protected short m_cktValResp;
    protected CircuitGrpCharacteristicsIndAnsi m_circuitGrpCharacteristicsIndAnsi = null;
    protected CircuitIdNameAnsi m_circuitIdName = null;
    protected CommonLangLocationIdAnsi m_commonLangLocationIdAnsi = null;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
