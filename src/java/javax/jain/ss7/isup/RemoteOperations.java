/*
 @(#) $RCSfile: RemoteOperations.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:05 $ <p>
 
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

/** A class representing the ITU and ANSI ISUP Remote Operations parameter.This class provides access
  * methods for all the sub-fields in this parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class RemoteOperations implements java.io.Serializable {
    public static final byte PPF_REMOTE_OPERATIONS_PROTOCOL = 0;
    public static final boolean EI_NEXT_OCTET_EXIST = false;
    public static final boolean EI_LAST_OCTET = true;
    /** Constructs a new RemoteOperations class with default values.  */
    public RemoteOperations() {
    }
    /** Constructs a RemoteOperations class from the input parameters specified.
      * @param in_protoProfile  The protocol profile, range 0 to 31; <ul>
      * <li>PPF_REMOTE_OPERATIONS_PROTOCOL. </ul>
      * @param in_isComp  The extensition indicator; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_comp  The components.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public RemoteOperations(byte in_protoProfile, boolean in_isComp, byte[] in_comp)
        throws ParameterRangeInvalidException {
        this.setProtocolProfile(in_protoProfile);
        this.setComponentInd(in_isComp);
        this.setComponents(in_comp);
    }
    /** Gets the Protocol Profile field of the parameter.
      * @return byte  Contains the ProtocolProfile value, range 0 to 31, see RemoteOperations().  */
    public byte getProtocolProfile() {
        return m_protocolProfile;
    }
    /** Sets the Protocol Profile field of the parameter.
      * @param aProtocolProfile  The Protocol Profile value, range 0 to 31, see RemoteOperations().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setProtocolProfile(byte aProtocolProfile)
        throws ParameterRangeInvalidException {
        if (0 > aProtocolProfile || aProtocolProfile > 31)
            throw new ParameterRangeInvalidException("ProtocolProfile value " + aProtocolProfile + " out of range.");
        m_protocolProfile = aProtocolProfile;
    }
    /** Gets the components present Indicator field, for getting Extension Indicator indicating
      * whether components follow or not, of the parameter.
      * @return boolean  The Extension Indicator value specifying whether components are present or
      * not, see RemoteOperations().  */
    public boolean getComponentInd() {
        return m_componentFlag;
    }
    /** Sets the components indicator for setting Extension Indicator indicating whether components
      * follow or not.
      * @param componentFlag  The ExtensionIndicator value containing whether the components are
      * present or not, see RemoteOperations().  */
    public void setComponentInd(boolean componentFlag) {
        m_componentFlag = componentFlag;
    }
    /** Gets the Components field of the parameter.
      * This field is not interpreted by the Provider implementation.
      * @return Byte array of the Components.  */
    public byte[] getComponents() {
        return m_components;
    }
    /** Sets the Components field of the parameter.
      * This field is not interpreted by the Provider Implementation.
      * @param components  The components embedded in the Remote Operations parameter.  */
    public void setComponents(byte[] components) {
        m_components = components;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * RemoteOperations class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.RemoteOperations");
        b.append("\n\tm_protocolProfile: " + m_protocolProfile);
        b.append("\n\tm_componentFlag: " + m_componentFlag);
        b.append("\n\tm_components: ");
        if (m_components != null)
            b.append(JainSS7Utility.bytesToHex(m_components, 0, m_components.length));
        return b.toString();
    }
    protected byte m_protocolProfile;
    protected boolean m_componentFlag;
    protected byte[] m_components;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
