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

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;


/**
  * A class representing the ansi ISUP CircuitGrpCharacteristicsInd parameter.
  */
public class CircuitGrpCharacteristicsIndAnsi extends java.lang.Object implements java.io.Serializable {
    public static final byte CGCI_UNKNOWN = 0;
    public static final byte CGCI_ANALOG = 1;
    public static final byte CGCI_DIGITAL = 2;
    public static final byte CGCI_ANALOG_AND_DIGITAL = 3;

    public static final byte DSCI_NO_CIC_CONTROL = 0;
    public static final byte DSCI_ODD_CIC_CONTROL = 1;
    public static final byte DSCI_EVEN_CIC_CONTROL = 2;
    public static final byte DSCI_ALL_CIC_CONTROL = 3;

    public static final byte ACI_UNKNOWN = 0;
    public static final byte ACI_SOFTWARE_CARRIER_HANDELING = 1;
    public static final byte ACI_HARDWARE_CARRIER_HANDELING = 2;

    public static final byte CCRI_UNKNOWN = 0;
    public static final byte CCRI_NONE = 1;
    public static final byte CCRI_STATISTICAL = 2;
    public static final byte CCRI_PER_CALL = 3;

    /**
      * Constructs a new ANSI CircuitGrpCharacteristicsInd class, parameters
      * with default values.
      */
    public CircuitGrpCharacteristicsIndAnsi() {
    }
    /**
      * Constructs an ANSI CircuitGrpCharacteristicsInd class from the input
      * parameters specified.
      * @param in_cktGrpCarrierInd  The circuit group carrier indicator, range 0-3.
      * <li>CGCI_UNKNOWN
      * <li>CGCI_ANALOG
      * <li>CGCI_DIGITAL
      * <li>CGCI_ANALOG_AND_DIGITAL
      * @param in_doubleSeizingControlInd  The double seizing control indicator,
      * range 0-3.
      * <li>DSCI_NO_CIC_CONTROL
      * <li>DSCI_ODD_CIC_CONTROL
      * <li>DSCI_EVEN_CIC_CONTROL
      * <li>DSCI_ALL_CIC_CONTROL
      * @param in_alarmCarrierInd  The alarm carrier indicator value, range 0-3.
      * <li>ACI_UNKNOWN
      * <li>ACI_SOFTWARE_CARRIER_HANDELING
      * <li>ACI_HARDWARE_CARRIER_HANDELING
      * @param in_continuityCheckRequirementsInd  The continuity check
      * requirements indicator, range 0-3.
      * <li>CCRI_UNKNOWN
      * <li>CCRI_NONE
      * <li>CCRI_STATISTICAL
      * <li>CCRI_PER_CALL
      * @exception ParameterRangeInvalidException  Thrown when value is out of
      * range.
      */
    public CircuitGrpCharacteristicsIndAnsi(byte in_cktGrpCarrierInd,
            byte in_doubleSeizingControlInd, byte in_alarmCarrierInd,
            byte in_continuityCheckRequirementsInd)
        throws ParameterRangeInvalidException {
        this();
        this.setCircuitGrpCarrierInd(in_cktGrpCarrierInd);
        this.setDoubleSeizingControlInd(in_doubleSeizingControlInd);
        this.setAlarmCarrierInd(in_alarmCarrierInd);
        this.setContinuityCheckRequirementsInd(in_continuityCheckRequirementsInd);
    }
    /**
      * Gets the CircuitGrpCarrierInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @return CircuitGrpCarrierInd, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().
      */
    public byte getCircuitGrpCarrierInd() {
        return m_cktGrpCarrierInd;
    }
    /**
      * Sets the CircuitGrpCarrierInd field of CircuitGrpCharacteristicsInd
      * parameter
      * @param in_cktGrpCarrierInd  The CircuitGrpCarrierInd value, range 0-3,
      * see CircuitGrpCharacteristicsIndAnsi().
      */
    public void setCircuitGrpCarrierInd(byte in_cktGrpCarrierInd) throws ParameterRangeInvalidException {
        if (0 > in_cktGrpCarrierInd || in_cktGrpCarrierInd > 3)
            throw ParameterRangeInvalidException("CircuitGroupCarrierInd " + in_cktGrpCarrierInd + " is out of range.");
        m_cktGrpCarrierInd = in_cktGrpCarrierInd;
    }
    /**
      * Gets the DoubleSeizingControlInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @return DoubleSeizingControlInd, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().
      */
    public byte getDoubleSeizingControlInd() {
        return m_doubleSeizingControlInd;
    }
    /**
      * Sets the DoubleSeizingControlInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @param in_doubleSeizingControlInd,  The DoubleSeizingControlInd value,
      * range 0-3, see CircuitGrpCharacteristicsIndAnsi().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is
      * out of the range specified.
      */
    public void setDoubleSeizingControlInd(byte in_doubleSeizingControlInd) throws ParameterRangeInvalidException {
        if (0 > in_doubleSeizingControlInd || in_doubleSeizingControlInd > 3)
            throw ParameterRangeInvalidException("DoubleSeizingControlInd " + in_doubleSeizingControlInd + " is out of range.");
        m_doubleSeizingControlInd = in_doubleSeizingControlInd;
    }
    /**
      * Gets the AlarmCarrierInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @return AlarmCarrierInd, range 0-3, see CircuitGrpCharacteristicsIndAnsi().
      */
    public byte getAlarmCarrierInd() {
        return m_alarmCarrierInd;
    }
    /**
      * Sets the AlarmCarrierInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @param in_alarmCarrierInd   The AlarmCarrierInd value, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is
      * out of the range specified.
      */
    public void setAlarmCarrierInd(byte in_alarmCarrierInd) throws ParameterRangeInvalidException {
        if (0 > in_alarmCarrierInd || in_alarmCarrierInd > 3)
            throw ParameterRangeInvalidException("AlarmCarrierInd " + in_alarmCarrierInd + " is out of range.");
        m_alarmCarrierInd = in_alarmCarrierInd;
    }
    /**
      * Gets the ContinuityCheckRequirementsInd field of
      * CircuitGrpCharacteristicsInd parameter.
      * @return ContinuityCheckRequirementsInd, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().
      */
    public byte getContinuityCheckRequirementsInd() {
        return m_continuityCheckRequirementsInd;
    }
    /**
      * Sets the ContinuityCheckRequirementsInd field of
      * CircuitGrpCharacteristicsInd parameter.
      * @param in_continuityCheckRequirementsInd  The
      * ContinuityCheckRequirementsInd value, range 0-3.
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is
      * out of the range specified.
      */
    public void setContinuityCheckRequirementsInd(byte in_continuityCheckRequirementsInd)
        throws ParameterRangeInvalidException {
        if (0 > in_continuityCheckRequirementsInd || in_continuityCheckRequirementsInd > 3)
            throw ParameterRangeInvalidException("ContinuityCheckRequirementsInd " + in_continuityCheckRequirementsInd + " is out of range.");
        m_continuityCheckRequirementsInd = in_continuityCheckRequirementsInd;
    }
    /**
      * The toString method retrieves a string containing the values of the
      * members of the CircuitGrpCharacteristicsIndAnsi class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\nCircuitGrpCharacteristicsIndAnsi:");
        if (m_cktGrpCarrierInd != null)
            b.append("\n\tCktGrpCarrierInd: " + m_cktGrpCarrierInd);
        if (m_doubleSeizingControlInd != null)
            b.append("\n\tDoubleSeizingControlInd: " + m_doubleSeizingControlInd);
        if (m_alarmCarrierInd != null)
            b.append("\n\tAlarmCarrierInd: " + m_alarmCarrierInd);
        if (m_continuityCheckRequirementsInd != null)
            b.append("\n\tContinuityCheckRequirementsInd: " + m_continuityCheckRequirementsInd);
        return b.toString();
    }
    private byte m_cktGrpCarrierInd = null;
    private byte m_doubleSeizingControlInd = null;
    private byte m_alarmCarrierInd = null;
    private byte m_continuityCheckRequirementsInd = null;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
