/*
 @(#) src/java/javax/jain/ss7/isup/ansi/CircuitGrpCharacteristicsIndAnsi.java <p>
 
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

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ansi ISUP CircuitGrpCharacteristicsInd parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CircuitGrpCharacteristicsIndAnsi implements java.io.Serializable {
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

    /** Constructs a new ANSI CircuitGrpCharacteristicsInd class, parameters
      * with default values.  */
    public CircuitGrpCharacteristicsIndAnsi() {
    }
    /** Constructs an ANSI CircuitGrpCharacteristicsInd class from the input parameters
      * specified.
      * @param in_cktGrpCarrierInd  The circuit group carrier indicator, range 0-3;
      * <ul><li>CGCI_UNKNOWN, <li>CGCI_ANALOG, <li>CGCI_DIGITAL and
      * <li>CGCI_ANALOG_AND_DIGITAL.</ul>
      * @param in_doubleSeizingControlInd  The double seizing control indicator, range
      * 0-3; <ul><li>DSCI_NO_CIC_CONTROL, <li>DSCI_ODD_CIC_CONTROL,
      * <li>DSCI_EVEN_CIC_CONTROL and <li>DSCI_ALL_CIC_CONTROL.</ul>
      * @param in_alarmCarrierInd  The alarm carrier indicator value, range 0-3;
      * <ul><li>ACI_UNKNOWN, <li>ACI_SOFTWARE_CARRIER_HANDELING and
      * <li>ACI_HARDWARE_CARRIER_HANDELING.</ul>
      * @param in_continuityCheckRequirementsInd  The continuity check
      * requirements indicator, range 0-3; <ul><li>CCRI_UNKNOWN, <li>CCRI_NONE,
      * <li>CCRI_STATISTICAL and <li>CCRI_PER_CALL.</ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
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
    /** Gets the CircuitGrpCarrierInd field of CircuitGrpCharacteristicsInd parameter.
      * @return CircuitGrpCarrierInd, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().  */
    public byte getCircuitGrpCarrierInd() {
        return m_cktGrpCarrierInd;
    }
    /** Sets the CircuitGrpCarrierInd field of CircuitGrpCharacteristicsInd parameter.
      * @param in_cktGrpCarrierInd  The CircuitGrpCarrierInd value, range 0-3,
      * see CircuitGrpCharacteristicsIndAnsi().  */
    public void setCircuitGrpCarrierInd(byte in_cktGrpCarrierInd) throws ParameterRangeInvalidException {
        if (0 > in_cktGrpCarrierInd || in_cktGrpCarrierInd > 3)
            throw new ParameterRangeInvalidException("CircuitGroupCarrierInd " + in_cktGrpCarrierInd + " is out of range.");
        m_cktGrpCarrierInd = in_cktGrpCarrierInd;
    }
    /** Gets the DoubleSeizingControlInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @return DoubleSeizingControlInd, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().  */
    public byte getDoubleSeizingControlInd() {
        return m_doubleSeizingControlInd;
    }
    /** Sets the DoubleSeizingControlInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @param in_doubleSeizingControlInd  The DoubleSeizingControlInd value, range
      * 0-3, see CircuitGrpCharacteristicsIndAnsi().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setDoubleSeizingControlInd(byte in_doubleSeizingControlInd) throws ParameterRangeInvalidException {
        if (0 > in_doubleSeizingControlInd || in_doubleSeizingControlInd > 3)
            throw new ParameterRangeInvalidException("DoubleSeizingControlInd " + in_doubleSeizingControlInd + " is out of range.");
        m_doubleSeizingControlInd = in_doubleSeizingControlInd;
    }
    /** Gets the AlarmCarrierInd field of CircuitGrpCharacteristicsInd parameter.
      * @return AlarmCarrierInd, range 0-3, see CircuitGrpCharacteristicsIndAnsi().  */
    public byte getAlarmCarrierInd() {
        return m_alarmCarrierInd;
    }
    /** Sets the AlarmCarrierInd field of CircuitGrpCharacteristicsInd parameter.
      * @param in_alarmCarrierInd   The AlarmCarrierInd value, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setAlarmCarrierInd(byte in_alarmCarrierInd) throws ParameterRangeInvalidException {
        if (0 > in_alarmCarrierInd || in_alarmCarrierInd > 3)
            throw new ParameterRangeInvalidException("AlarmCarrierInd " + in_alarmCarrierInd + " is out of range.");
        m_alarmCarrierInd = in_alarmCarrierInd;
    }
    /** Gets the ContinuityCheckRequirementsInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @return ContinuityCheckRequirementsInd, range 0-3, see
      * CircuitGrpCharacteristicsIndAnsi().  */
    public byte getContinuityCheckRequirementsInd() {
        return m_continuityCheckRequirementsInd;
    }
    /** Sets the ContinuityCheckRequirementsInd field of CircuitGrpCharacteristicsInd
      * parameter.
      * @param in_continuityCheckRequirementsInd  The ContinuityCheckRequirementsInd
      * value, range 0-3.
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setContinuityCheckRequirementsInd(byte in_continuityCheckRequirementsInd)
        throws ParameterRangeInvalidException {
        if (0 > in_continuityCheckRequirementsInd || in_continuityCheckRequirementsInd > 3)
            throw new ParameterRangeInvalidException("ContinuityCheckRequirementsInd " + in_continuityCheckRequirementsInd + " is out of range.");
        m_continuityCheckRequirementsInd = in_continuityCheckRequirementsInd;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CircuitGrpCharacteristicsIndAnsi class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\nCircuitGrpCharacteristicsIndAnsi:");
        b.append("\n\tCktGrpCarrierInd: " + m_cktGrpCarrierInd);
        b.append("\n\tDoubleSeizingControlInd: " + m_doubleSeizingControlInd);
        b.append("\n\tAlarmCarrierInd: " + m_alarmCarrierInd);
        b.append("\n\tContinuityCheckRequirementsInd: " + m_continuityCheckRequirementsInd);
        return b.toString();
    }
    private byte m_cktGrpCarrierInd;
    private byte m_doubleSeizingControlInd;
    private byte m_alarmCarrierInd;
    private byte m_continuityCheckRequirementsInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
