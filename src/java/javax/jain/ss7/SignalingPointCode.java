/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Module Name   : JAIN Common API
 * File Name     : SignalingPointCode.java
 * Originator    : Phelim O'Doherty
 * Approver      : Jain Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     28/09/2001  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7;

import java.io.Serializable;
import javax.jain.*;

import java.lang.Math;

/**
 * The Signaling Point Code can be implemented in one of the following 2 ways
 *
 * <li>ITU_14bit - Split 3-8-3 across member/cluster/zone.
 * <li>ANSI_24bit - Split 8-8-8 across member/cluster/zone.
 *
 *
 *
 *  <B>SSSAAAZZZ</B> <P>
 *
 *  where: <P>
 *
 *  <B>SSS</B> == Member <BR>
 *  <B>AAA</B> == Cluster <BR>
 *  <B>ZZZ</B> == Zone <P>
 *
 *  The permitted ranges for each value are outlined in the table below.
 *  <TABLE BORDER CELLSPACING=2 BORDERCOLOR="#000000" CELLPADDING=7 WIDTH=500>
 *
 *    <TR>
 *
 *      <TD WIDTH="11%" VALIGN="TOP">
 *        &nbsp;
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP">
 *        <B><P ALIGN="CENTER">
 *
 *        Member</P> </B>
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP">
 *        <B><P ALIGN="CENTER">
 *
 *       Cluster</P> </B>
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP">
 *        <B><P ALIGN="CENTER">
 *
 *        Zone </P> </B>
 *      </TD>
 *
 *    </TR>
 *
 *    <TR>
 *
 *      <TD WIDTH="11%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        ANSI</B>
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <P ALIGN="CENTER">
 *
 *        000-255
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <P ALIGN="CENTER">
 *
 *        000-255
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <P ALIGN="CENTER">
 *
 *        000-255
 *      </TD>
 *
 *    </TR>
 *
 *    <TR>
 *
 *      <TD WIDTH="11%" VALIGN="TOP">
 *        <B><P ALIGN="CENTER">
 *
 *        ITU</B>
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP">
 *        <P ALIGN="CENTER">
 *
 *        000-007
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP">
 *        <P ALIGN="CENTER">
 *
 *        000-255
 *      </TD>
 *
 *      <TD WIDTH="30%" VALIGN="TOP">
 *        <P ALIGN="CENTER">
 *
 *        000-007
 *      </TD>
 *
 *    </TR>
 *
 *  </TABLE>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.0
 */
public class SignalingPointCode implements Serializable{
		   
    /**
    * Constructs a SignalingPointCode with the specified Member, Cluster, and Zone.
    *
    * @param  member the new member field supplied to the constructor
    * @param  cluster the new cluster field supplied to the constructor
    * @param  zone the new zone field supplied to the constructor
    */
    public SignalingPointCode(int member, int cluster, int zone) {
        setSignalingPointCode(member, cluster, zone);
    }

    /**
    * Indicates whether some other object is "equal to"
    * this one, i.e. same member, cluster and zone value.
    *
    * @param  o the reference object which to compare
    * @return true if this Signaling Point Code is the same as the argument
    * object; <i>false</i> otherwise.
    */
    public boolean equals(Object o) {

      if(!(o instanceof SignalingPointCode)) {
          return false;
      } else {
        SignalingPointCode spc = (SignalingPointCode)o;
        return (this.getCluster() == spc.getCluster()
          && this.getMember() == spc.getMember()
          && this.getZone() == spc.getZone());
      }
    }

   
    /**
    * Returns a hash code value for the object. This method is supported
    * for the benefit of hashtables such as those provided by
    * java.util.Hashtable.
    *
    * @return a hashcode value for this object.
    */
    public int hashCode( ) {
         StringBuffer strBuff = new StringBuffer( );
         strBuff.append(m_spc[0]);
         strBuff.append(m_spc[1]);
         strBuff.append(m_spc[2]);
         String str = strBuff.toString();

         int hashCode = str.hashCode( );
         return hashCode;
    }

   
    /**
    * Sets the Member field of the Signaling Point Code
    *
    * @param  member the member field of the Signaling Point Code
    */
    public void setMember(int member){
        m_spc[0] = member;
        }

    /**
    * Gets the Member field of the Signaling Point Code
    *
    * @returns  member the new member of the Signaling Point Code
    */
    public int getMember(){
        return m_spc[0];
    }

    /**
    * Sets the Cluster field of the Signaling Point Code
    *
    * @param  cluster the new cluster of the Signaling Point Code
    */
    public void setCluster(int cluster) {
        m_spc[1] = cluster;
    }

    /**
    * Gets the Cluster field of the Signaling Point Code
    *
    * @returns  cluster the new cluster of the Signaling Point Code
    */
    public int getCluster() {
        return m_spc[1];
    }

    /**
    * Sets the Zone field of the Signaling Point Code
    *
    * @param  zone the new zone of the Signaling Point Code
    */
    public void setZone(int zone) {
        m_spc[2] = zone;
    }

    /**
    * Gets the Zone field of the Signaling Point Code
    *
    * @returns  zone the new zone of the Signaling Point Code
    */
    public int getZone() {
        return m_spc[2];
    }

    /**
    * Sets the member field, containing the member field of the
    * signaling point code.
    *
    * @param  member  the new member field
    * @param  cluster the new cluster field
    * @param  zone    the new zone field
    */
    public void setSignalingPointCode(int member, int cluster, int zone) {
        setMember(member);
        setCluster(cluster);
        setZone(zone);
    }

    /**
    * Gets the SignalingPointCode, containing the member, cluster and zone fields  of the
    * SignalingPointCode.
    *
    * @return the SignalingPointCode of this Address
    *
    */
    public int[] getSignalingPointCode() {
        return m_spc;
    }

    /**
    * Separates a Signaling Point Code out into the three fields member, cluster, and zone.
    *
    * @param bitcode the Signaling Point Code stored as a bitfield
    * @param variant the stack specification of the signaling point code to be decoded (ANSI/ITU)
    * @throws InvalidArgumentException throw if an invalid SPCformat is supplied to this method
    * @return the member,cluster,zone stored in an array
    */
    public static int[] convertSPCToMemberClusterZone(int bitCode, SPCFormat variant) throws InvalidArgumentException{

        int[] bitSplit = new int[3];
        int[] spc = new int[3];

        // set up the number of bits per field we want to extract
        if (variant == SPCFormat.ITU_SPC_FORMAT) {
            bitSplit[0] = 3;
            bitSplit[1] = 8;
            bitSplit[2] = 3;
        } else if (variant == SPCFormat.ANSI_SPC_FORMAT) {
            bitSplit[0] = 8;
            bitSplit[1] = 8;
            bitSplit[2] = 8;
        } else {
            throw new IllegalArgumentException("Invalid variant Supplied");
        }
        // shift the bits right until the ones we want are the lsbs, and mask
        // them with the appropriate number ((2^n)-1)
        spc[2] = bitCode & (int)((Math.pow(2, bitSplit[0])) - 1);
        spc[1] = (bitCode >> bitSplit[0]) & (int)((Math.pow(2, bitSplit[1])) - 1);
        spc[0] = (bitCode >> (bitSplit[0] + bitSplit[1])) & (int)((Math.pow(2, bitSplit[2])) - 1);
        return spc;
    }

    /**
    * String representation of class SignalingPointCode
    *
    * @return    String provides description of class SignalingPointCode
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nMember = ");
        buffer.append(this.m_spc[0]);
        buffer.append("\n\nCluster = ");
        buffer.append(this.m_spc[1]);
        buffer.append("\n\nZone = ");
        buffer.append(this.m_spc[2]);

        return buffer.toString();
    }

    //private static int[] spc = new int[3];
    private int[] m_spc = new int[] {-1, -1, -1};

}
