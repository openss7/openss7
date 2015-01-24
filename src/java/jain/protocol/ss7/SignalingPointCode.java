/*
 @(#) src/java/jain/protocol/ss7/SignalingPointCode.java <p>
 
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

package jain.protocol.ss7;

import jain.*;

/**
  * This class represents an SS7 signaling point code.  The Signaling
  * Point Code can be implemented in one of the following 2 ways:- <ul>
  * <li> ITU_14bit - Split 3-8-3 across member/cluster/zone.
  * <li> ANSI_24bit - Split 8-8-8 across member/cluster/zone. </ul>
  *
  * <code>SSSAAAZZZ</code> <p>
  *
  * where: <p>
  *
  * <code> SSS == Member </code><br>
  * <code> AAA == Cluster </code><br>
  * <code> ZZZ == Zone </code><br> <p>
  *
  * The permitted ranges for each value are outlined in the table below.
  *
  * <table align="center" border=1 cellpadding=1>
  * <caption>Ranges of components.</caption>
  * <tr>
  * <th>Variant</th>
  * <th>Member</th>
  * <th>Cluster</th>
  * <th>Zone</th>
  * </tr>
  * <tr>
  * <th>ANSI</th>
  * <td align="center">000-255</td>
  * <td align="center">000-255</td>
  * <td align="center">000-255</td>
  * </tr>
  * <tr>
  * <th>ITU</th>
  * <td align="center">000-007</td>
  * <td align="center">000-255</td>
  * <td align="center">000-007</td>
  * </tr>
  * </table> <p>
  *
  * <h4>Implementation Notes:</h4> <p>
  * Some experts...  There is no way of enforcing the ranges on these
  * values asside from the outside range enforced over all variants
  * because there is nothing in the singalling point code object that
  * indicates whether it is ANSI or ITU.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SignalingPointCode implements java.io.Serializable {
    /**
      * Constructs a SignalingPointCode with the specified Member,
      * Cluster, and Zone.
      * @param member
      * The new member field supplied to the constructor.
      * @param cluster
      * The new cluster field supplied to the constructor.
      * @param zone
      * The new zone field supplied to the constructor.
      * @since JAIN TCAP v1.1
      */
    public SignalingPointCode(int member, int cluster, int zone) {
        super();
        setSignalingPointCode(member, cluster, zone);
    }
    /**
      * Sets the Member field of the Signaling Point Code
      * @param member
      * The member field of the Signaling Point Code.
      */
    public void setMember(int member) {
        m_member = member;
        m_memberIsSet = true;
    }
    /**
      * Gets the Member field of the Signaling Point Code.
      * @exception MandatoryParameterNotSetException
      * Thrown if this parameter has not been set.
      */
    public int getMember()
        throws MandatoryParameterNotSetException {
        if (m_memberIsSet)
            return m_member;
        throw new MandatoryParameterNotSetException("Member: not set.");
    }
    /**
      * Sets the Cluster field of the Signaling Point Code.
      * @param cluster
      * The new cluster of the Signaling Point Code.
      */
    public void setCluster(int cluster) {
        m_cluster = cluster;
        m_clusterIsSet = true;
    }
    /**
      * Gets the Cluster field of the Signaling Point Code.
      * @exception MandatoryParameterNotSetException
      * Thrown if this parameter has not been set.
      */
    public int getCluster()
        throws MandatoryParameterNotSetException {
        if (m_clusterIsSet)
            return m_cluster;
        throw new MandatoryParameterNotSetException("Cluster: not set.");
    }
    /**
      * Sets the Zone field of the Signaling Point Code.
      * @param zone
      * The new zone of the Signaling Point Code.
      */
    public void setZone(int zone) {
        m_network = zone;
        m_networkIsSet = true;
    }
    /**
      * Gets the Zone field of the Signaling Point Code.
      * @exception MandatoryParameterNotSetException
      * Thrown if this parameter has not been set.
      */
    public int getZone()
        throws MandatoryParameterNotSetException {
        if (m_networkIsSet)
            return m_network;
        throw new MandatoryParameterNotSetException("Network: not set.");
    }
    /**
      * Sets the member field, containing the member field of the
      * signaling point code.
      * @param member
      * The new member field.
      * @param cluster
      * The new cluster field.
      * @param zone
      * The new zone field.
      */
    public void setSignalingPointCode(int member, int cluster, int zone) {
        setMember(member);
        setCluster(cluster);
        setZone(zone);
    }
    /**
      * Gets the SignalingPointCode, containing the member, cluster and
      * zone fields of the SignalingPointCode.
      * @return
      * The SignalingPointCode of this Sccp User Address.
      * @exception MandatoryParameterNotSetException
      * Thrown if this parameter has not been set.
      * @since JAIN TCAP v1.1
      * @see SignalingPointCode
      */
    public int[] getSignalingPointCode()
        throws MandatoryParameterNotSetException {
        int[] components = new int[3];
        components[0] = getMember();
        components[1] = getCluster();
        components[2] = getZone();
        return components;
    }
    /**
      * Separates a Signaling Point Code out into the three fields
      * member, cluster, and zone.
      * @param bitCode
      * The Signaling Point Code stored as a bitfield.
      * @param variant
      * The stack specification of the signaling point code to be
      * decoded (ANSI/ITU).
      * @return
      * The member,cluster,zone stored in an array.
      */
    public static int[] convertSPCToMemberClusterZone(int bitCode, int variant) {
        int[] components = new int[3];
        switch (variant) {
            case AddressConstants.ANSI_SPC_FORMAT:
                components[0] = ((bitCode & 0x000000FF) >>  0);
                components[1] = ((bitCode & 0x0000FF00) >>  8);
                components[2] = ((bitCode & 0x00FF0000) >> 16);
                break;
            case AddressConstants.ITU_SPC_FORMAT:
            default:
                components[0] = ((bitCode & 0x00000007) >>  0);
                components[1] = ((bitCode & 0x000007F8) >>  3);
                components[2] = ((bitCode & 0x00003800) >> 11);
                break;
        }
        return components;
    }
    /**
      * java.lang.String representation of class SignalingPointCode.
      * @return
      * java.lang.String provides description of class SignalingPointCode.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njain.protocol.ss7.SignalingPointCode");
        b.append("\n\tm_member = " + m_member);
        b.append("\n\tm_memberIsSet = " + m_memberIsSet);
        b.append("\n\tm_cluster = " + m_cluster);
        b.append("\n\tm_clusterIsSet = " + m_clusterIsSet);
        b.append("\n\tm_network = " + m_network);
        b.append("\n\tm_networkIsSet = " + m_networkIsSet);
        return b.toString();
    }
    protected int m_member = 0;
    protected boolean m_memberIsSet = false;
    protected int m_cluster = 0;
    protected boolean m_clusterIsSet = false;
    protected int m_network = 0;
    protected boolean m_networkIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
