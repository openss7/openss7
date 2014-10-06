/*
 @(#) $RCSfile: BusinessGroupAnsi.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:06 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:06 $ by $Author: brian $
 */

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI ISUP Business Group parameter. This methods provide
  * access to the various sub-fields of this parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class BusinessGroupAnsi implements java.io.Serializable {

    /** Party Selection, no indication. */
    public static final byte PS_NO_INDICATION = 0; 
    /** Party Selection, calling party number. */
    public static final byte PS_CALLING_PARTY_NUMBER = 0x01; 
    /** Party Selection, called party number. */
    public static final byte PS_CALLED_PARTY_NUMBER = 0x02;
    /** Party Selection, connected party number. */
    public static final byte PS_CONNECTED_PARTY_NUMBER = 0x03;
    /** Party Selection, redirecting number. */
    public static final byte PS_REDIRECTING_NUMBER = 0x04;
    /** Party Selection, original called number. */
    public static final byte PS_ORIGINAL_CALLED_NUMBER = 0x05;

    /** Line privileges, fixed. */
    public static final boolean LPII_FIXED_LINE_PRIVILEGES = false;
    /** Line privileges, customer defines. */
    public static final boolean LPII_CUSTOMER_DEFINED_LINE_PRIVILEGES = true;

    /** Business group Id type, multi-location business group id. */
    public static final boolean BGIT_MULT_LOCATION_BUSINESS_GROUP_IDENTIFIER = false; 
    /** Business group Id type, interworking with private network id. */
    public static final boolean BGIT_INTERWORKING_WITH_PRIVATE_NETWORKS_IDENTIFIER = true; 

    /** Attendant status, no indication. */
    public static final boolean AS_NO_INDICATION = false; 
    /** Attendant status, attendant line. */
    public static final boolean AS_ATTENDANT_LINE = true; 

    /** Line privilege, unrestricted. */
    public static final byte LP_UNRESTRICTED = 0x00; 
    /** Line privilege, semi-restricted. */
    public static final byte LP_SEMI_RESTRICTED = 0x01; 
    /** Line privilege, fully restricted. */
    public static final byte LP_FULLY_RESTRICTED = 0x02; 
    /** Line privilege, fully restricted intra-switch. */
    public static final byte LP_FULLY_RESTRICTED_INTRA_SWITCH = 0x03; 
    /** Line privilege, denied. */
    public static final byte LP_DENIED = 0x04; 	

    /** Constructs a new ANSI BusinessGroup class, parameters with default values.  */
    public BusinessGroupAnsi() {
        super();
    }
    /** Constructs an ANSI BusinessGroup class from the input parameters specified.
      * @param in_partySel  The party selector, range 0 to 15; <ul>
      * <li>PS_NO_INDICATION, <li>PS_CALLING_PARTY_NUMBER, <li>PS_CALLED_PARTY_NUMBER,
      * <li>PS_CONNECTED_PARTY_NUMBER, <li>PS_REDIRECTING_NUMBER and
      * <li>PS_ORIGINAL_CALLED_NUMBER.  </ul>
      * @param in_linePrivInd  The line privileges information indicator; <ul>
      * <li>LPII_FIXED_LINE_PRIVILEGES and <li>LPII_CUSTOMER_DEFINED_LINE_PRIVILEGES.
      * </ul>
      * @param in_bgIdType  The business group identifiertType; <ul>
      * <li>BGIT_MULT_LOCATION_BUSINESS_GROUP_IDENTIFIER and
      * <li>BGIT_INTERWORKING_WITH_PRIVATE_NETWORKS_IDENTIFIER.  </ul>
      * @param in_attendantStatus  The attendant status; <ul> <li>AS_NO_INDICATION and
      * <li>AS_ATTENDANT_LINE.  </ul>
      * @param in_bgId  The business group identifier, range 0 to 0xffffff.
      * @param in_subGroupId  The sub-group identifier, range 0 to 0xffff.
      * @param in_linePriv  The line privileges, range 0 to 255; <ul>
      * <li>LP_UNRESTRICTED, <li>LP_SEMI_RESTRICTED, <li>LP_FULLY_RESTRICTED,
      * <li>LP_FULLY_RESTRICTED_INTRA_SWITCH and <li>LP_DENIED.  </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public BusinessGroupAnsi(byte in_partySel, boolean in_linePrivInd, boolean
            in_bgIdType, boolean in_attendantStatus, int in_bgId, int
            in_subGroupId, short in_linePriv)
        throws ParameterRangeInvalidException {
        if (PS_NO_INDICATION > in_partySel || in_partySel > PS_ORIGINAL_CALLED_NUMBER)
            throw new ParameterRangeInvalidException("Party Selection " + in_partySel + " is out of bounds.");
        if (LP_UNRESTRICTED > in_linePriv || in_linePriv > LP_DENIED)
            throw new ParameterRangeInvalidException("Line Privilege " + in_linePriv + " is out of bounds.");
        m_partySel = in_partySel;
        m_linePrivInd = in_linePrivInd;
        m_bgIdType = in_bgIdType;
        m_attendantStatus = in_attendantStatus;
        m_bgId = in_bgId;
        m_subGroupId = in_subGroupId;
        m_linePriv = in_linePriv;
    }
    /** Gets the Party Selector field of the parameter.  
      * @return The PartySelector value, range 0 to 15, see BusinessGroupAnsi().  */
    public byte getPartySelector() {
        return m_partySel;
    }
    /** Sets the Party Selector field of the parameter.
      * @param aPartySelector The PartySelector value, range 0 to 15, see
      * BusinessGroupAnsi().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setPartySelector(byte aPartySelector) throws ParameterRangeInvalidException {
        if (PS_NO_INDICATION > aPartySelector || aPartySelector > PS_ORIGINAL_CALLED_NUMBER)
            throw new ParameterRangeInvalidException("Party Selection " + aPartySelector + " is out of bounds.");
        m_partySel = aPartySelector;
    }
    /** Gets the Line Privileges Info Indicator field of the parameter.
      * @return Boolean the LinePrivilegesInfoIndicator value, see BusinessGroupAnsi().  */
    public boolean getLinePrivInfoInd() {
        return m_linePrivInd;
    }
    /** Sets the Line Privileges Info Indicator field of the parameter.
      * @param aLinePrivilegesInfoIndicator  The Line Privileges Info Indicator value,
      * see BusinessGroupAnsi().  */
    public void setLinePrivInfoInd(boolean aLinePrivilegesInfoIndicator) {
        m_linePrivInd = aLinePrivilegesInfoIndicator;
    }
    /** Gets the Business Group Identifier Type field of the parameter.
      * @return Boolean the BusinessGroupIdentifierType value, see BusinessGroupAnsi().  */
    public boolean getBusinessGroupIdType() {
        return m_bgIdType;
    }
    /** Sets the Business Group Identifier Type field of the parameter.
      * @param aBusinessGroupIdentifierType  The BusinessGroupIdentifierType value, see
      * BusinessGroupAnsi().  */
    public void setBusinessGroupIdType(boolean aBusinessGroupIdentifierType) {
        m_bgIdType = aBusinessGroupIdentifierType;
    }
    /** Gets the Attendant Status field of the parameter.
      * @return The AttendantStatus value, see BusinessGroupAnsi().  */
    public boolean getAttendantStatus() {
        return m_attendantStatus;
    }
    /** Sets the Attendant Status field of the parameter.
      * @param aAttendantStatus  The Attendant Status value, see BusinessGroupAnsi().  */
    public void setAttendantStatus(boolean aAttendantStatus) {
        m_attendantStatus = aAttendantStatus;
    }
    /** Gets the Business Group Identifier field of the parameter.
      * @return The BusinessGroupIdentifier value, range 0 to 0xffffff.  */
    public int getBusinessGroupId() {
        return m_bgId;
    }
    /** Sets the Business Group Identifier field of the parameter.
      * @param aBusinessGroupIdentifier  The BusinessGroupIdentifier value, range 0 to
      * 0xffffff.
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setBusinessGroupId(int aBusinessGroupIdentifier) throws ParameterRangeInvalidException {
        m_bgId = aBusinessGroupIdentifier;
    }
    /** Gets the Sub Group Identifier field of the parameter.
      * @return The SubGroupIdentifier value, range 0 to 0xffff.  */
    public int getSubGroupId() {
        return m_subGroupId;
    }
    /** Sets the Sub Group Identifier field of the parameter.
      * @param aSubGroupIdentifier  The SubGroupIdentifier value, range 0 to 0xffff.
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setSubGroupId(int aSubGroupIdentifier) throws ParameterRangeInvalidException {
        m_subGroupId = aSubGroupIdentifier;
    }
    /** Gets the Line Privileges field of the parameter.
      * @return The LinePrivileges value, range 0 to 255, see BusinessGroupAnsi().  */
    public short getLinePriv() {
        return m_linePriv;
    }
    /** Sets the Line Privileges field of the parameter.
      * @param aLinePrivileges  The LinePrivileges value, range 0 to 255, see
      * BusinessGroupAnsi().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setLinePriv(short aLinePrivileges) throws ParameterRangeInvalidException {
        if (LP_UNRESTRICTED > aLinePrivileges || aLinePrivileges > LP_DENIED)
            throw new ParameterRangeInvalidException("Line Privilege " + aLinePrivileges + " is out of bounds.");
        m_linePriv = aLinePrivileges;
    }
    /** The toString method retrieves a string containing the values of the members of the BusinessGroupAnsi class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\nBusinessGroupAnsi:");
        buffer.append("\n\tParty Selection: " + m_partySel);
        buffer.append("\n\tLine Privileges: " + m_linePrivInd);
        buffer.append("\n\tBusiness Group ID Type: " + m_bgIdType);
        buffer.append("\n\tAttendant Status: " + m_attendantStatus);
        buffer.append("\n\tBusiness Group Id: " + m_bgId);
        buffer.append("\n\tSubgroup Id: " + m_subGroupId);
        buffer.append("\n\tLine Privilege: " + m_linePriv);
        return buffer.toString();
    }
    private byte m_partySel;
    private boolean m_linePrivInd;
    private boolean m_bgIdType;
    private boolean m_attendantStatus;
    private int m_bgId;
    private int m_subGroupId;
    private short m_linePriv;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
