/*
 @(#) $RCSfile: IsupUserAddress.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:47 $ <p>
 
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
 
 Last Modified $Date: 2011-09-02 08:46:47 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** The IsupUserAddress class consists of destination point code, network identifier
  * and CIC range.  Destination point code is the address of the SS7 node in the
  * network, on which the voice circuits terminate, and network indicator indicates
  * whether the point code belongs to a national or an international network.  CIC is
  * used to identify a voice circuit between two SS7 nodes.  It is used to identify
  * the circuits controlled by a JAIN ISUP application (JainIsupListener
  * implementation).  An ISUP user application may handle a number of User Addresses
  * and will register as an JainIsupListener of a JainIsupProvider object.  Any Events
  * addressed to a User Address will be passed to the User application that has
  * registered with that JainIsupProvider for the User Address.  The User Address
  * consists of the following; <p><ul><li>Destination Point Code, <li>Circuit
  * Identification Code, <li>Network Indicator.</ul> <p>
  *
  * Note that, because the IsupAddress does not contain an Originating Point Code, it
  * does not uniquely identify a set of circuits in the SS7 network.  Only when used
  * in conjunction with a given JainIsupProvider object (with its associated Signaling
  * Point Code) does the IsupAddress then uniquely identify a set of circuits within
  * the SS7 network.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public final class IsupUserAddress implements SS7Address, java.io.Serializable {
    /** Constructs a IsupUserAddress with the specified Signalling Point Code, CIC
      * range and network indicator.
      * @param dpc Destination point code in User Address.  The term
      * <i>Destination</i> Point Code here is a misnomer; this signaling point code is
      * only the <em>Destination</em> point code in IsupEvent objects passed from the
      * JainIsupListener object to the JainIsupProvider object.
      * @param startCIC Starting (inclusive) CIC in the CIC range of the User Address.
      * @param endCIC Ending (inclusive) CIC in the CIC range of the User Address.
      * @param networkInd Newtork indicator in the User Address.
      * @exception Thrown when value is out of range.  */
    public IsupUserAddress(SignalingPointCode dpc, int startCIC, int endCIC, NetworkIndicator networkInd)
        throws ParameterRangeInvalidException {
        try {
            int[] pc = new int[3];
            pc[0] = dpc.getMember();
            pc[1] = dpc.getCluster();
            pc[2] = dpc.getZone();
        } catch (Exception e) {
            throw new ParameterRangeInvalidException("Signaling Point Code out of range.");
        }
        if (startCIC > endCIC)
            throw new ParameterRangeInvalidException("Start CIC " + startCIC + " and End CIC " + endCIC + " are disjoint.");
        if (0 > startCIC || startCIC > 16383)
            throw new ParameterRangeInvalidException("Start CIC " + startCIC + " is out of range.");
        if (0 > endCIC || endCIC > 16383)
            throw new ParameterRangeInvalidException("End CIC " + endCIC + " is out of range.");
        m_spc = dpc;
        m_scic = startCIC;
        m_ecic = endCIC;
        m_ni = networkInd;
    }
    /** Gets the Network Indicator of the User Address.  The indicator indicates if
      * the point code belongs to the international or national network.
      * @return The network indicator associated with the User Addresss, see
      * JainIsupStack.  */
    public NetworkIndicator getNetworkIndicator() {
        return m_ni;
    }
    /** Gets the Signaling Point Code of the User Address.  The singaling point code
      * is a 9 digit number that has been defined in the JAIN SS7 layers as;
      * <code>SSSAAAZZZ</code> where <code>SSS</code>, is the Signalling point id
      * (member), <code>AAA</code> is the area id (cluster), and <code>SSS</code>, is
      * the zone id (network).  For example, <code>001002003</code> is a signaling
      * point code with a member value of <code>1</code>, a cluster value of
      * <code>2</code> and a network value of <code>3</code>.  Note that this is the
      * reverse of the order normally written in the industry as numbers and dashes
      * notation as <code>[N[N[N]]].[C[C[C]]].[M[M[M]]</code> where zeros are not
      * normally written; that is, outside of JAIN, the signaling point code example
      * above would be written as <code>3-2-1</code>.
      * @return The signaling point code associated with the User Address.  */
    public SignalingPointCode getSignalingPointCode() {
        return m_spc;
    }
    /** Gets the start CIC of the User Address.  The Start CIC must never be greater
      * than the End CIC.  The CIC value will be between 0 and 4095 (inclusive) for
      * ITU and between 0 and 4095 for 12-bit CIC ANSI and between 0 and 16383
      * (inclusive) for 14-bit CIC ANSI.
      * @return The Start CIC of the User Address.  */
    public int getStartCIC() {
        return m_scic;
    }
    /** Gets the End CIC of the User Address.  See getStartCIC for details.
      * @return The End CIC of the User Address.  */
    public int getEndCIC() {
        return m_ecic;
    }
    /** Checks if two user addresses are the same or not.
      * @param obj ISUP User addres to be compared with.
      * @return True if the two User Addresses are the same, false otherwise.  */
    public boolean equals(java.lang.Object obj) {
        if (obj instanceof IsupUserAddress) {
            IsupUserAddress isupUA = (IsupUserAddress) obj;
            if (!m_spc.equals(isupUA.m_spc))
                return false;
            if (m_scic != isupUA.m_scic)
                return false;
            if (m_ecic != isupUA.m_ecic)
                return false;
            if (!m_ni.equals(isupUA.m_ni))
                return false;
            return true;
        }
        return false;
    }
    /** Returns a hash code for this IsupUserAddress.
      * @return A hashcode for this User Address.  */
    public int hashCode() {
        int result = 17;
        result = 37 * result + m_ecic;
        result = 37 * result + m_scic;
        return result;
    }
    /** Checks if the CIC range in this user address is a suberset of the CIC range in
      * the specified user address.  That is, the CIC range in the specified user
      * address is contained in the CIC range in this user address.  Start CIC in the
      * specified ISUP user address should be less than the end CIC.
      * @param isupUA The User Address to be compared.
      * @return True if the CIC range in isupUA is a (strict) subset of the CIC range
      * in this user address, false otherwise.  Note that if equals() returns true,
      * isSupersetOf() returns false for the same pair of objects.  */
    public boolean isSupersetOf(IsupUserAddress isupUA) {
        if (m_scic > m_ecic)
            return false;
        if (isupUA.m_scic > isupUA.m_ecic)
            return true;
        if (m_scic > isupUA.m_scic || m_ecic < isupUA.m_ecic)
            return false;
        if (m_scic < isupUA.m_scic || m_ecic > isupUA.m_ecic)
            return true;
        return false;
    }

    /** The toString method retrieves a string containing the values of the members of
      * the IsupUserAddress class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\nIsupUserAddress:");
        buffer.append("\n\tm_spc: " + m_spc);
        buffer.append("\n\tm_scic: " + m_scic);
        buffer.append("\n\tm_ecic: " + m_ecic);
        buffer.append("\n\tm_npi: " + m_ni);
        return buffer.toString();
    }

    protected SignalingPointCode m_spc;
    protected int m_scic;
    protected int m_ecic;
    protected NetworkIndicator m_ni;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
