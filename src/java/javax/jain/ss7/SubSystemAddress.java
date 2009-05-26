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
 * File Name     : SubSystemAddress.java
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

/**
 * SubsystemAddress represents an address of an SS7 User application. <BR>
 * The User Address comprises of the following mandatory parameters:
 *  <UL>
 *    <LI> <B>Signaling Point Code</B>
 *    <LI> <B>Sub-System Number</B>
 *  </UL>
 *  <BR>
 *
 *
 * @author     Sun Microsystems Inc.
 * @version    1.0
 */
public final class SubSystemAddress implements Serializable {

    /**
    * Constructs a SubsystemAddress with the specified Signalling Point Code and
    * Sub-System Number.
    *
    * @param  signalingPointCode  the Signaling Point Code of the Subsystem Address.
    * @param  subSystemNumber     the Sub-System Number of the Subsystem Address
    */
    public SubSystemAddress(SignalingPointCode signalingPointCode, short subSystemNumber) {
        setSignalingPointCode(signalingPointCode);
        setSubSystemNumber(subSystemNumber);
    }

    /**
    * Sets the SubSystem number of this SubSystem Address. The SubSystem number
    * should be greater than 0 and less than 255.
    *
    * @param  subSystemNumber the Subsystem Number of this SubSystem Address
    * @exception  IllegalArgumentException thrown if an out of range SubSystem
    *            Number is passed to this method
    */
    public void setSubSystemNumber(int subSystemNumber) throws IllegalArgumentException {
        if (subSystemNumber > 0 && subSystemNumber <= 255) {
            m_SubSystemNumber = subSystemNumber;
        } else {
            throw new IllegalArgumentException("SSN: Out of Range");
        }
    }

    /**
    * Returns the Signaling Point Code of this SubSystem Number.<P>
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
    *        Cluster</P> </B>
    *      </TD>
    *
    *      <TD WIDTH="30%" VALIGN="TOP">
    *        <B><P ALIGN="CENTER">
    *
    *        Zone</P> </B>
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
    *
    * @return    the signalingPointCode of this Subsystem Address
    */
    public SignalingPointCode getSignalingPointCode() {
        return m_SignalingPointCode;
    }

        /**
    * Sets the Signaling Point Code of this SubSystem Number.<P>
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
    *        Cluster</P> </B>
    *      </TD>
    *
    *      <TD WIDTH="30%" VALIGN="TOP">
    *        <B><P ALIGN="CENTER">
    *
    *        Zone</P> </B>
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
    * @param    the signalingPointCode of this Subsystem Address
    */
    public void setSignalingPointCode(SignalingPointCode spc) {
            m_SignalingPointCode = spc;
    }

    /**
    * Gets the SubSystem number of this Subsystem Address.
    *
    * @return    the SubSystem number of this SubSystem Address
    */
    public int getSubSystemNumber() {
        return m_SubSystemNumber;
    }

    /**
    * String representation of class SubSystemAddress
    *
    * @return    String provides description of class SubSystemAddress
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nsubSystemNumber = ");
        buffer.append(this.m_SubSystemNumber);
        buffer.append("\n\nsignalingPointCode:");
        if (this.m_SignalingPointCode != null) {
            buffer.append(this.m_SignalingPointCode.toString());
        } else {
            buffer.append("value is null");
        }
        return buffer.toString();
    }

    /**
    * The Sub-System Number of the SubSystem Address
    *
    * @serial    m_SubSystemNumber - a default serializable field
    */
    private int m_SubSystemNumber = -1;

    /**
    * The Signalling Point Code of the SubSystem Address.
    *
    * @serial    m_SignalingPointCode - a default serializable field
    */
    private SignalingPointCode m_SignalingPointCode = null;
}

