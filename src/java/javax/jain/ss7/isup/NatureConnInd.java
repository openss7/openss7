/*
 @(#) $RCSfile: NatureConnInd.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:05 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:05 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ITU and ANSI ISUP Nature Connection Indicator.
  * This class provides the access methods for the different sub-fields of this
  * parameter. This is a common class for both ITU and ANSI variants.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class NatureConnInd implements java.io.Serializable {
    public static final byte SI_NO_SATELLITE_CIRCUIT = 0;
    public static final byte SI_ONE_SATELLITE_CIRCUIT = 1;
    public static final byte SI_TWO_SATELLITE_CIRCUIT = 2;
    public static final byte CCI_CONTINUITY_CHECK_NOT_REQUIRED = 0;
    public static final byte CCI_CONTINUITY_CHECK_REQUIRED = 1;
    public static final byte CCI_CONTINUITY_CHECK_PERFORMED_ON_PREVIOUS_CIRCUIT = 2;
    public static final boolean ECDI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = false;
    public static final boolean ECDI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = true;
    /** Constructs a new NatureConnInd class, parameters with default values.  */
    public NatureConnInd() {
    }
    /** Constructs a NatureConnInd class from the input parameters specified.
      * @param in_satInd  The satellite indicator, range 0 to 3; <ul>
      * <li>SI_NO_SATELLITE_CIRCUIT <li>SI_ONE_SATELLITE_CIRCUIT and
      * <li>SI_TWO_SATELLITE_CIRCUIT. </ul>
      * @param in_contCheckInd  The continuity check indicator, range 0 to 3; <ul>
      * <li>CCI_CONTINUITY_CHECK_NOT_REQUIRED, <li>CCI_CONTINUITY_CHECK_REQUIRED and
      * <li>CCI_CONTINUITY_CHECK_PERFORMED_ON_PREVIOUS_CIRCUIT. </ul>
      * @param in_echoControlDevInd  The echo control device indicator; <ul>
      * <li>ECDI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED and
      * <li>ECDI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public NatureConnInd(byte in_satInd, byte in_contCheckInd,
            boolean in_echoControlDevInd)
        throws ParameterRangeInvalidException {
        this.setSatelliteInd(in_satInd);
        this.setContCheckInd(in_contCheckInd);
        this.setEchoControlDevInd(in_echoControlDevInd);
    }
    /** Gets the Satellite Indicator field of the parameter.
      * @return byte The satellite indicator value, range 0 to 3, see NatureConnInd().  */
    public byte getSatelliteInd() {
        return m_satInd;
    }
    /** Sets the Satellite Indicator field of the parameter.
      * @param satelliteInd  The Satellite Indicator value, range 0 to 3, see
      * NatureConnInd().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setSatelliteInd(byte satelliteInd)
        throws ParameterRangeInvalidException {
        switch (satelliteInd) {
            case SI_NO_SATELLITE_CIRCUIT:
            case SI_ONE_SATELLITE_CIRCUIT:
            case SI_TWO_SATELLITE_CIRCUIT:
                break;
            default:
                throw new ParameterRangeInvalidException("satelliteInd value " + satelliteInd + " out of range.");
        }
        m_satInd = satelliteInd;
    }
    /** Gets the Continuity Check Indicator field of the parameter.
      * @return The continuity check indicator value, range 0 - 3, see
      * NatureConnInd().  */
    public byte getContCheckInd() {
        return m_contCheckInd;
    }
    /** Sets the Continuity Check Indicator field of the parameter.
      * @param continuityCheckInd  The Continuity Check Indicator value, range 0 to 3,
      * see NatureConnInd().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setContCheckInd(byte continuityCheckInd)
        throws ParameterRangeInvalidException {
        switch (continuityCheckInd) {
            case CCI_CONTINUITY_CHECK_NOT_REQUIRED:
            case CCI_CONTINUITY_CHECK_REQUIRED:
            case CCI_CONTINUITY_CHECK_PERFORMED_ON_PREVIOUS_CIRCUIT:
                break;
            default:
                throw new ParameterRangeInvalidException("continuityCheckInd value " + continuityCheckInd + " out of range.");
        }
        m_contCheckInd = continuityCheckInd;
    }
    /** Gets the Echo Control Device Indicator field of the parameter.
      * @return Boolean the echo control device indicator, see NatureConnInd().  */
    public boolean getEchoControlDevInd() {
        return m_echoControlDevInd;
    }
    /** Sets the Echo Control Device Indicator field of the parameter.
      * @param echoDevInd  The Echo Control Device Indicator value, see
      * NatureConnInd().  */
    public void setEchoControlDevInd(boolean echoDevInd) {
        m_echoControlDevInd = echoDevInd;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the NatureConnInd class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.NatureConnInd");
        b.append("\n\tm_satInd: " + m_satInd);
        b.append("\n\tm_contCheckInd: " + m_contCheckInd);
        b.append("\n\tm_echoControlDevInd: " + m_echoControlDevInd);
        return b.toString();
    }
    protected byte m_satInd;
    protected byte m_contCheckInd;
    protected boolean m_echoControlDevInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
