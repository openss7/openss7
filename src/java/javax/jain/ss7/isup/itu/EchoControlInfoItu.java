/*
 @(#) src/java/javax/jain/ss7/isup/itu/EchoControlInfoItu.java <p>
 
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

/** A class representing the ITU ISUP Echo control information parameter.
    This class provides access methods for setting and getting the sub-fields of this
    parameter.

    @author Monavacon Limited
    @version 1.2.2
  */
public class EchoControlInfoItu implements java.io.Serializable {
    public static final byte ORPI_NO_INFORMATION = 0;
    public static final byte ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = 1;
    public static final byte ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = 2;
    public static final byte IRPI_NO_INFORMATION = 0;
    public static final byte IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = 1;
    public static final byte IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = 2;
    public static final byte ORQI_NO_INFORMATION = 0;
    public static final byte ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_ACTIVATION_REQUEST = 1;
    public static final byte ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST = 2;
    public static final byte IRQI_NO_INFORMATION = 0;
    public static final byte IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_ACTIVATION_REQUEST = 1;
    public static final byte IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST = 2;
    /** Constructs a EchoControlInfoItu class, parameters with default values.  */
    public EchoControlInfoItu() {
    }
    /** Constructs a EchoControlInfoItu class from the input parameters specified.
        @param in_ogHalfEchoControlDevRespInd  The outgoing half echo control device
        response indicator, range 0 to 3. <ul> <li>ORPI_NO_INFORMATION,
        <li>ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED and
        <li>ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED. </ul>
        @param in_icHalfEchoControlDevRespInd  The incoming half echo control device
        response indicator, range 0 to 3. <ul> <li>IRPI_NO_INFORMATION,
        <li>IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED and
        <li>IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED. </ul>
        @param in_ogHalfEchoControlDevReqInd  The outgoing half echo control device
        request indicator, range 0 to 3. <ul> <li>ORQI_NO_INFORMATION,
        <li>ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_ACTIVATION_REQUEST and
        <li>ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST. </ul>
        @param in_icHalfEchoControlDevReqInd  The incoming half echo control device
        request indicator, range 0 to 3.  <li>IRQI_NO_INFORMATION,
        <li>IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_ACTIVATION_REQUEST and
        <li>IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST. </ul>
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public EchoControlInfoItu(
            byte in_ogHalfEchoControlDevRespInd,
            byte in_icHalfEchoControlDevRespInd,
            byte in_ogHalfEchoControlDevReqInd,
            byte in_icHalfEchoControlDevReqInd)
        throws ParameterRangeInvalidException {
        this.setOgHalfEchoControlDevRespInd(in_ogHalfEchoControlDevRespInd);
        this.setIcHalfEchoControlDevRespInd(in_icHalfEchoControlDevRespInd);
        this.setOgHalfEchoControlDevReqInd(in_ogHalfEchoControlDevReqInd);
        this.setIcHalfEchoControlDevReqInd(in_icHalfEchoControlDevReqInd);
    }
    /** Gets the Outgoing Half Echo Control Device Response Indicator field of the
        parameter.
        @return The OgHalfEchoDevRespInd value, range 0 to 3, see EchoControlInfoItu().
      */
    public byte getOgHalfEchoControlDevRespInd() {
        return m_ogHalfEchoDevRespInd;
    }
    /** Sets the Outgoing Half Echo Control Device Response Indicator field of the
        parameter.
        @param aOgHalfEchoDevRespInd  The OgHalfEchoDevRespInd value, range 0 to 3, see
        EchoControlInfoItu().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        specified range.
      */
    public void setOgHalfEchoControlDevRespInd(byte aOgHalfEchoDevRespInd)
        throws ParameterRangeInvalidException {
        if (0 <= aOgHalfEchoDevRespInd && aOgHalfEchoDevRespInd <= 3) {
            m_ogHalfEchoDevRespInd = aOgHalfEchoDevRespInd;
            return;
        }
        throw new ParameterRangeInvalidException("OgHalfEchoControlDevRespInd value " + aOgHalfEchoDevRespInd + " out of range.");
    }
    /** Gets the Incoming Half Echo Control Device Response Indicator field of the
      * parameter.
      * @return The IcHalfEchoDevRespInd value, range 0 to 3, see EchoControlInfoItu().
      */
    public byte getIcHalfEchoControlDevRespInd() {
        return m_icHalfEchoDevRespInd;
    }
    /** Sets the Incoming Half Echo Control Device Response Indicator field of the
        parameter.
        @param aIcHalfEchoDevRespInd  The IcHalfEchoDevRespInd value, range 0 to 3, see
        EchoControlInfoItu().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        specified range.
      */
    public void setIcHalfEchoControlDevRespInd(byte aIcHalfEchoDevRespInd)
        throws ParameterRangeInvalidException {
        if (0 <= aIcHalfEchoDevRespInd && aIcHalfEchoDevRespInd <= 3) {
            m_icHalfEchoDevRespInd =  aIcHalfEchoDevRespInd;
            return;
        }
        throw new ParameterRangeInvalidException("IcHalfEchoControlDevRespInd value " + aIcHalfEchoDevRespInd + " out of range.");
    }
    /** Gets the Outgoing Half Echo Control Device Request Indicator field of the
        parameter.
        @return The Outgoing Half Echo Control Device Request Indicator value, range 0 to
        3, see EchoControlInfoItu().
      */
    public byte getOgHalfEchoControlDevReqInd() {
        return m_ogHalfEchoDevReqInd;
    }
    /** Sets the Outgoing Half Echo Control Device Request Indicator field of the
        parameter.
        @param aOgHalfEchoDevReqInd  The Outgoing Half Echo Control Device Request
        Indicator value, range 0 to 3, see EchoControlInfoItu().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        specified range.
      */
    public void setOgHalfEchoControlDevReqInd(byte aOgHalfEchoDevReqInd)
        throws ParameterRangeInvalidException {
        if (0 <= aOgHalfEchoDevReqInd && aOgHalfEchoDevReqInd <= 3) {
            m_ogHalfEchoDevReqInd = aOgHalfEchoDevReqInd;
            return;
        }
        throw new ParameterRangeInvalidException("OgHalfEchoControlDevReqInd value " + aOgHalfEchoDevReqInd + " out of range.");
    }
    /** Gets the Incoming Half Echo Control Device Request Indicator field of the
        parameter.
        @return The Incoming Half Echo Control Device Request Indicator value, range 0 to
        3, see EchoControlInfoItu().
      */
    public byte getIcHalfEchoControlDevReqInd() {
        return m_icHalfEchoControlDevReqInd;
    }
    /** Sets the Incoming Half Echo Control Device Request Indicator field of the
        parameter.
        @param aIcHalfEchoControlDevReqInd  The Incoming Half Echo Control Device Request
        Indicator value, range 0 to 3, see EchoControlInfoItu().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        specified range.
      */
    public void setIcHalfEchoControlDevReqInd(byte aIcHalfEchoControlDevReqInd)
        throws ParameterRangeInvalidException {
        if (0 <= aIcHalfEchoControlDevReqInd && aIcHalfEchoControlDevReqInd <= 3) {
            m_icHalfEchoControlDevReqInd = aIcHalfEchoControlDevReqInd;
            return;
        }
        throw new ParameterRangeInvalidException("IcHalfEchoControlDevReqInd value " + aIcHalfEchoControlDevReqInd + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        EchoControlInfoItu class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString(){
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.EchoControlInfoItu");
        b.append("\n\tm_ogHalfEchoDevRespInd:" + m_ogHalfEchoDevRespInd);
        b.append("\n\tm_icHalfEchoDevRespInd:" + m_icHalfEchoDevRespInd);
        b.append("\n\t:m_ogHalfEchoDevReqInd" + m_ogHalfEchoDevReqInd);
        b.append("\n\tm_icHalfEchoControlDevReqInd:" + m_icHalfEchoControlDevReqInd);
        return b.toString();
    }
    protected byte m_ogHalfEchoDevRespInd;
    protected byte m_icHalfEchoDevRespInd;
    protected byte m_ogHalfEchoDevReqInd;
    protected byte m_icHalfEchoControlDevReqInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
