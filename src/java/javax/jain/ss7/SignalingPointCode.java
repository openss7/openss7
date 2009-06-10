/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */


package javax.jain.ss7;

import javax.jain.*;

/**
  * Represents the SS7 signaling point code.
  * The Signaling Point Code can be implemented in one of the following
  * 2 ways <ul>
  * <li> ITU_14bit - Split 3-8-3 across member/cluster/zone.
  * <li> ANSI_24bit - Split 8-8-8 across member/cluster/zone. <ul>
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
  * <p>
  *
  * <table align="center" border=1 cellpadding=1>
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
      */
    public SignalingPointCode(int member, int cluster, int zone) {
        super();
        setSignalingPointCode(member,cluster,zone);
    }
    /**
      * Indicates whether some other object is "equal to" this one, i.e,
      * same member, cluster and zone value.
      * @param o
      * The reference object which to compare.
      * @return
      * True if this Signaling Point Code is the same as the argument
      * object; false otherwise.
      */
    public boolean equals(java.lang.Object o) {
        if (o == null)
            return false;
        if (o instanceof SignalingPointCode) {
            SignalingPointCode spc = (SignalingPointCode) o;
            if (spc.m_member != m_member ||
                    spc.m_memberIsSet != m_memberIsSet)
                return false;
            if (spc.m_cluster!= m_cluster||
                    spc.m_clusterIsSet != m_clusterIsSet)
                return false;
            if (spc.m_network != m_network ||
                    spc.m_networkIsSet != m_networkIsSet)
                return false;
            return true;
        }
        return false;
    }
    /**
      * Returns a hash code value for the object.
      * This method is supported for the benefit of hashtables such as
      * those provided by java.util.Hashtable.
      * @return
      * A hashcode value for this object.
      */
    public int hashCode() {
        StringBuffer b = new StringBuffer();
        b.append(m_member);
        b.append(m_cluster);
        b.append(m_network);
        java.lang.String s = b.toString();
        return s.hashCode();
    }
    /**
      * Sets the Member field of the Signaling Point Code.
      * @param member
      * The member field of the Signaling Point Code.
      */
    public void setMember(int member) {
        m_member = member;
        m_memberIsSet = true;
    }
    /**
      * Gets the Member field of the Signaling Point Code.
      * @return
      * The member field of the signaling point code.
      */
    public int getMember() {
        return m_member;
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
      * @return
      * The cluster field of the signaling point code.
      */
    public int getCluster() {
        return m_cluster;
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
      * @return
      * The zone of the signaling point code.
      */
    public int getZone() {
        return m_network;
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
      * The SignalingPointCode of this Address.
      */
    public int[] getSignalingPointCode() {
        int[] result = new int[3];
        result[0] = getMember();
        result[1] = getCluster();
        result[2] = getZone();
        return result;
    }
    /**
      * Separates a Signaling Point Code out into the three fields
      * member, cluster, and zone.
      * @param bitcode
      * The Signaling Point Code stored as a bitfield.
      * @param variant
      * The stack specification of the signaling point code to be
      * decoded (ANSI/ITU).
      * @return
      * The member,cluster,zone stored in an array.
      * @exception InvalidArgumentException
      * Throw if an invalid SPCformat is supplied to this method.
      */
    public static int[] convertSPCToMemberClusterZone(int bitCode, SPCFormat variant)
        throws InvalidArgumentException {
        int[] result = new int[3];
        switch (variant.intValue()) {
            case SPCFormat.M_ANSI_SPC_FORMAT:
                result[0] = ((bitCode & 0x000000FF) >>  0);
                result[1] = ((bitCode & 0x0000FF00) >>  8);
                result[2] = ((bitCode & 0x00FF0000) >> 16);
                break;
            case SPCFormat.M_ITU_SPC_FORMAT:
                result[0] = ((bitCode & 0x00000007) >>  0);
                result[1] = ((bitCode & 0x000007F8) >>  3);
                result[2] = ((bitCode & 0x00003800) >> 11);
                break;
        }
        return result;
    }
    /**
      * String representation of class SignalingPointCode.
      * @return
      * String provides description of class SignalingPointCode.
      */
    public java.lang.String toString() {
        return new String(m_network + "." + m_cluster + "." + m_member);
    }
    protected int m_member = 0;
    protected boolean m_memberIsSet = false;
    protected int m_cluster = 0;
    protected boolean m_clusterIsSet = false;
    protected int m_network = 0;
    protected boolean m_networkIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
