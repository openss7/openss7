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
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : SignalingPointCode.java
 * Originator    : Eugene Bell [AePONA]
 * Approver      : Jain Community
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     24/04/2001  Eugene Bell    Initial Version
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

//import java.io.Serializable;
import jain.*;
import jain.protocol.ss7.AddressConstants;

//import java.lang.Math;

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
 * @version    1.1
 */
public class SignalingPointCode implements java.io.Serializable{

    /**
    * Constructs a SignalingPointCode with the specified Member, Cluster, and Zone.
    *
    * @param  member the new member field supplied to the constructor
    * @param  cluster the new cluster field supplied to the constructor
    * @param  zone the new zone field supplied to the constructor
    * @since JAIN TCAP v1.1
    */
    public SignalingPointCode(int member, int cluster, int zone) {
        setSignalingPointCode(member, cluster, zone);
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
    * @exception MandatoryParameterNotSetException thrown if this
    * parameter has not been set
    * @returns  member the new member of the Signaling Point Code
    */
    public int getMember() throws MandatoryParameterNotSetException{
        if (-1 == m_spc[0]) {
            throw new MandatoryParameterNotSetException();
        }
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
    * @exception MandatoryParameterNotSetException thrown if this
    * parameter has not been set
    * @returns  cluster the new cluster of the Signaling Point Code
    */
    public int getCluster() throws MandatoryParameterNotSetException{
        if (-1 == m_spc[1]) {
            throw new MandatoryParameterNotSetException();
        }
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
    * @exception MandatoryParameterNotSetException thrown if this
    * parameter has not been set
    * @returns  zone the new zone of the Signaling Point Code
    */
    public int getZone() throws MandatoryParameterNotSetException{
        if (-1 == m_spc[2]) {
            throw new MandatoryParameterNotSetException();
        }
        return m_spc[2];
    }

    /**
    * Sets the member field, containing the member field of the
    * signaling point code.
    *
    * @param  member              the new member field
    * @param  cluster              the new cluster field
    * @param  zone              the new zone field
    */
    public void setSignalingPointCode(int member, int cluster, int zone) {
        m_spc[0] = member;
        m_spc[1] = cluster;
        m_spc[2] = zone;
    }

    /**
    * Gets the SignalingPointCode, containing the member, cluster and zone fields  of the
    * SignalingPointCode.
    *
    * @return the SignalingPointCode of this Sccp User Address
    * @exception MandatoryParameterNotSetException thrown if this
    * parameter has not been set
    *
    * @since      JAIN TCAP v1.1
    * @see        SignalingPointCode
    */
    public int[] getSignalingPointCode() throws MandatoryParameterNotSetException{
        if (-1 == m_spc[0]) {
            throw new MandatoryParameterNotSetException();
        }
        return m_spc;
    }

    /**
    * Separates a Signaling Point Code out into the three fields member, cluster, and zone.
    *
    * @param bitcode the Signaling Point Code stored as a bitfield
    * @param variant the stack specification of the signaling point code to be decoded (ANSI/ITU)
    *
    * @return the member,cluster,zone stored in an array
    */
    public static int[] convertSPCToMemberClusterZone(int bitCode, int variant){

        int[] bitSplit = new int[3];
        int[] spc = new int[3];

        // set up the number of bits per field we want to extract
        if (variant == AddressConstants.ITU_SPC_FORMAT) {
            bitSplit[0] = 3;
            bitSplit[1] = 8;
            bitSplit[2] = 3;
        } else if (variant == AddressConstants.ANSI_SPC_FORMAT) {
            bitSplit[0] = 8;
            bitSplit[1] = 8;
            bitSplit[2] = 8;
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

    private int[] m_spc = new int[] {-1, -1, -1};
}
