/*
 @(#) $RCSfile: UserToUserIndicatorsItu.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:09 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:09 $ by $Author: brian $
 */

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ITU ISUP User To User Indicators parameter.
  * This class inherits from the base UserToUserIndicators parameter class for adding the
  * ITU sub-fields to the parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class UserToUserIndicatorsItu extends UserToUserIndicators {
    public static final byte S1RQ_NO_INFORMATION = 0;
    public static final byte S1RQ_REQUEST_NOT_ESSENTIAL = 2;
    public static final byte S1RQ_REQUEST_ESSENTIAL = 3;
    public static final byte S2RQ_NO_INFORMATION = 0;
    public static final byte S2RQ_REQUEST_NOT_ESSENTIAL = 2;
    public static final byte S2RQ_REQUEST_ESSENTIAL = 3;
    public static final byte S3RQ_NO_INFORMATION = 0;
    public static final byte S3RQ_REQUEST_NOT_ESSENTIAL = 2;
    public static final byte S3RQ_REQUEST_ESSENTIAL = 3;
    public static final byte S1RP_NO_INFORMATION = 0;
    public static final byte S1RP_NOT_PROVIDED = 1;
    public static final byte S1RP_PROVIDED = 2;
    public static final byte S2RP_NO_INFORMATION = 0;
    public static final byte S2RP_NOT_PROVIDED = 1;
    public static final byte S2RP_PROVIDED = 2;
    public static final byte S3RP_NO_INFORMATION = 0;
    public static final byte S3RP_NOT_PROVIDED = 1;
    public static final byte S3RP_PROVIDED = 2;
    /** Constructs a new UserToUserIndicatorsItu class, parameters with default values.  */
    public UserToUserIndicatorsItu() {
    }
    /** Constructs a UserToUserIndicatorsItu class from the input parameters specified.
      * @param in_type  The type, see UserToUserIndicators().
      * @param in_nwDiscardInd  The network discard indicator, see UserToUserIndicators().
      * @param in_service1  The service1, range 0 to 3 <ul> <li>S1RQ_NO_INFORMATION,
      * <li>S1RQ_REQUEST_NOT_ESSENTIAL, <li>S1RQ_REQUEST_ESSENTIAL,
      * <li>S1RP_NO_INFORMATION, <li>S1RP_NOT_PROVIDED and <li>S1RP_PROVIDED. </ul>
      * @param in_service2  The service2, range 0 to 3 <ul> <li>S2RQ_NO_INFORMATION,
      * <li>S2RQ_REQUEST_NOT_ESSENTIAL, <li>S2RQ_REQUEST_ESSENTIAL,
      * <li>S2RP_NO_INFORMATION, <li>S2RP_NOT_PROVIDED and <li>S2RP_PROVIDED. </ul>
      * @param in_service3  The service3, range 0 to 3 <ul> <li>S3RQ_NO_INFORMATION,
      * <li>S3RQ_REQUEST_NOT_ESSENTIAL, <li>S3RQ_REQUEST_ESSENTIAL,
      * <li>S3RP_NO_INFORMATION, <li>S3RP_NOT_PROVIDED and <li>S3RP_PROVIDED. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public UserToUserIndicatorsItu(boolean in_type, boolean in_nwDiscardInd,
            byte in_service1, byte in_service2, byte in_service3)
        throws ParameterRangeInvalidException {
        super(in_type, in_nwDiscardInd);
        this.setService1(in_service1);
        this.setService2(in_service2);
        this.setService3(in_service3);
    }
    /** Gets the Service 1 field of the parameter.
      * @return The Service1 value, range 0 to 3, see UserToUserIndicatorsItu().
      */
    public byte getService1() {
        return m_service1;
    }
    /** Sets the Service 1 field of the parameter.
      * @param aService1  The Service1 value, range 0 to 3, see UserToUserIndicatorsItu().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setService1(byte aService1)
        throws ParameterRangeInvalidException {
        if (0 <= aService1 && aService1 <= 3) {
            m_service1 = aService1;
            return;
        }
        throw new ParameterRangeInvalidException("Service1 value " + aService1 + " out of range.");
    }
    /** Gets the Service 2 field of the parameter.
      * @return Byte the Service2 value, range 0 to 3, see UserToUserIndicatorsItu().
      */
    public byte getService2() {
        return m_service2;
    }
    /** Sets the Service 2 field of the parameter.
      * @param aService2  The Service2 value, range 0 to 3, see UserToUserIndicatorsItu().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setService2(byte aService2)
        throws ParameterRangeInvalidException {
        if (0 <= aService2 && aService2 <= 3) {
            m_service2 = aService2;
            return;
        }
        throw new ParameterRangeInvalidException("Service2 value " + aService2 + " out of range.");
    }
    /** Gets the Service 3 field of the parameter.
      * @return Byte the Service3 value, range 0 to 3, see UserToUserIndicatorsItu().
      */
    public byte getService3() {
        return m_service3;
    }
    /** Sets the Service 3 field of the parameter.
      * @param aService3  The Service3 value, range 0 to 3, see UserToUserIndicatorsItu().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setService3(byte aService3)
        throws ParameterRangeInvalidException {
        if (0 <= aService3 && aService3 <= 3) {
            m_service3 = aService3;
            return;
        }
        throw new ParameterRangeInvalidException("Service3 value " + aService3 + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
      * UserToUserIndicatorsItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.UserToUserIndicatorsItu");
        b.append("\n\tm_service1: " + m_service1);
        b.append("\n\tm_service2: " + m_service2);
        b.append("\n\tm_service3: " + m_service3);
        return b.toString();
    }
    protected byte m_service1;
    protected byte m_service2;
    protected byte m_service3;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
