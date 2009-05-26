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
 * File Name     : TcapUserAddress.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     07/11/2000  Phelim O'Doherty    Added new methods for SubSystem Addressing.
 *                                         Deprecated old setSPC/getSPC/isSPCPresent
 *                                         methods.
 *                                         Added new routingIndicator methods and
 *                                         deprecated old gtRouting methods.
 *                                         Implements JainAddress Interface.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
import jain.protocol.ss7.JainSS7Utility;
import jain.protocol.ss7.tcap.GlobalTitle;
import java.io.Serializable;

/**
* TcapUserAddress represents an address of a TCAP User application.<BR>
* A TCAP User application may handle a number of User Addresses and will
* register as an Event Listener of a
* <a href="JainTcapProvider.html">JainTcapProvider</a> for that User Address.
* <P>Any Events addressed to a User Address will be
* passed to the User application to whom the User Address belongs.
* The User Address comprises of the:
* <UL>
* <LI><B>Signaling Point Code</B>
* <LI><B>Sub-System Number</B>
* <LI><B>Optionally Global Title</B> for use in Global Title translation.
*</UL>
* <BR>
* It is permitted for more than one JainTcapListener to register with
* the same JainTcapProvider with the same User Address and events sent to
* that User Address will be sent to all JainTcapListeners of that User Address.
* i.e. it is up to the implementation of JAIN TCAP to manage how the
* Events are handled between these Listeners.
*
* @version 1.1
* @author Sun Microsystems Inc.
*
* @deprecated As of JAIN TCAP v1.1. This class is no longer needed as a result
* of the addition of the {@link jain.protocol.ss7.SccpUserAddress} class.
*/
public final class TcapUserAddress implements Serializable{

    /**
    * Constructs a TcapUserAddress with the specified Signalling Point Code and
    * Sub-System Number. Using this constructor will automatically set the
    * global title routing indicator to false.
    * @param 	signalingPointCode 	the Signalling Point Code of the User Address.
    * The format of the signalling point code
    * is described at setSignallingPointCode(byte[]).
    * @param 	subSystemNumber		the Sub-System Number of the User Address
    *
    */
    public TcapUserAddress(byte[]  signalingPointCode, short subSystemNumber){
	  this.setSignalingPointCode(signalingPointCode);
	  this.setSubSystemNumber(subSystemNumber);
    }

    /**
    * Constructs a TcapUserAddress with the specified Global Title.
    * Using this constructor will automatically set the
    * global title routing indicator to true.
    * @param <var>globalTitle</var> the Global title
    */
    public TcapUserAddress(GlobalTitle  globalTitle) {
        this.globalTitle = globalTitle;
        this.globalTitleRouting = true;
    }

    /**
    * Gets the Global Title Routing indicator. The indicator indicates if
    * global title will be used by SCCP to route a message.
    * @return  true if Global Title is used for routing, false if SSN is
    * used for routing.
    */
    public boolean isGlobalTitleRouting() {
        return globalTitleRouting;
    }

    /**
    * Sets the Global Title Routing indicator. The indicator indicates if
    * global title will be used by SCCP to route a message. If set to true,
    * SCCP will use the global title to route a message, if set to false,
    * SCCP will use the SSN to route the message.
    * @param <var>gtRoutingIndicator</var> Enable/disable global title routing.
    */
    public void setGlobalTitleRouting(boolean gtRoutingIndicator) {
        this.globalTitleRouting = gtRoutingIndicator;
    }

    /**
    * Indicates if the Signalling Point Code is present in this User Address.
    * @return  true if Signalling Point Code has been set.
    */
    public boolean isSignalingPointCodePresent() {
        return signalingPointCodePresent;
    }

    /**
    * Gets the Signalling Point Code of the User Address. The format of the signalling point code
    * is described at setSignallingPointCode(byte[]).
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public byte[] getSignalingPointCode() throws ParameterNotSetException{
	 if (signalingPointCodePresent) {
	      return signalingPointCode;
	  } else {
              throw new ParameterNotSetException();
	  }
    }

    /**
    * Sets the Signalling Point Code of the User Address.
    *
    * The signalling point code (spc) is a 3 byte array where:<P>
    * <B>spc[0]</B> == Signalling Point ID (member)<BR>
    * <B>spc[1]</B> == Area Id (cluster)<BR>
    * <B>spc[2]</B> == Zone Id (network)<P>
    * The permitted ranges for each value are outlined in the table below. <P>
    * <CENTER>
    * <TABLE BORDER CELLSPACING=2 BORDERCOLOR="#000000" CELLPADDING=7 WIDTH=500>
    * <TR><TD WIDTH="11%" VALIGN="TOP">&nbsp;</TD>
    * <TD WIDTH="30%" VALIGN="TOP">
    * <B><P ALIGN="CENTER">Signalling Point ID</P>
    * <P ALIGN="CENTER">(member)</B></TD>
    * <TD WIDTH="30%" VALIGN="TOP">
    * <B><P ALIGN="CENTER">Area ID</P>
    * <P ALIGN="CENTER">(cluster)</B></TD>
    * <TD WIDTH="30%" VALIGN="TOP">
    * <B><P ALIGN="CENTER">Zone Id</P>
    * <P ALIGN="CENTER">(network)</B></TD>
    * </TR>
    * <TR><TD WIDTH="11%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">ANSI</B></TD>
    * <TD WIDTH="30%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <P ALIGN="CENTER">000-255</TD>
    * <TD WIDTH="30%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <P ALIGN="CENTER">000-255</TD>
    * <TD WIDTH="30%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <P ALIGN="CENTER">000-255</TD>
    * </TR>
    * <TR><TD WIDTH="11%" VALIGN="TOP">
    * <B><P ALIGN="CENTER">ITU</B></TD>
    * <TD WIDTH="30%" VALIGN="TOP">
    * <P ALIGN="CENTER">000-007</TD>
    * <TD WIDTH="30%" VALIGN="TOP">
    * <P ALIGN="CENTER">000-255</TD>
    * <TD WIDTH="30%" VALIGN="TOP">
    * <P ALIGN="CENTER">000-007</TD>
    * </TR>
    * </TABLE>
    * </CENTER>
    * </P>
    * <P>
    * <B>NB:</B>A <i>byte</I> in Java contains a signed integer and thus ranges from -128 to 127.<P>
    * When converting an Integer (intVal) to a byte (byteVal) using the following code: <P>
    * <var>
    * int x;<BR>
    * .....<BR>
    * Integer intVal = new Integer(x);<BR>
    * byte byteVal = intVal.byteValue();</VAR><P>
    * The following values will be obtained:<P>
    *
    * <P ALIGN="CENTER"><CENTER><TABLE BORDER CELLSPACING=1 BORDERCOLOR="#000000" CELLPADDING=7 WIDTH=275>
    * <TR><TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">intObj.intValue()</B></TD>
    * <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">intObj.byteValue()</B></TD>
    * </TR>
    * <TR><TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">0</B></TD>
    * <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">0</B></TD>
    * </TR>
    * <TR><TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">127</B></TD>
    * <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">127</B></TD>
    * </TR>
    * <TR><TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">128</B></TD>
    * <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">-128</B></TD>
    * </TR>
    * <TR><TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">255</B></TD>
    * <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
    * <B><P ALIGN="CENTER">-1</B></TD>
    * </TR>
    * </TABLE>
    * </CENTER></P>
    * <P>
    * This fact should be remembered when supplying bytes to this method.
    * @param  <var>pointCode</var> the signalingPointCode of this TcapUserAddress
    */
    public void setSignalingPointCode(byte[] pointCode) {
        this.signalingPointCode = pointCode;
        signalingPointCodePresent = true;
    }

    /**
    * Gets the Sub-System Number of the User Address.
    * @return  the SubSystemNumber of this TcapUserAddress
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public short getSubSystemNumber()throws ParameterNotSetException{
        if (subSystemNumberPresent) {
            return subSystemNumber;
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Sets the Sub-System Number of the User Address.
    * @param subSystemNumber the SubSystemNumber of this TcapUserAddress
    */
    public void setSubSystemNumber(short subSystemNumber){
	this.subSystemNumber = subSystemNumber;
        subSystemNumberPresent = true;
    }

    /**
    * Gets the Global Title of the User Address. The GlobalTitle is an object, of which the default value is null. If the
    * get accessor method is used and the GlobalTitle object has a null value, then a ParameterNotSetException will be thrown.
    * @return  the GlobalTitle of this TcapUserAddress
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public GlobalTitle getGlobalTitle() throws ParameterNotSetException{
        if (globalTitle == null ) {
            throw new ParameterNotSetException();
            } else {
	        return globalTitle;
	    }
    }

    /**
    * Sets the Global Title of this User Address. The Global Title is an object which can be identified to be one
    * of five different types.
    * <UL>
    *   <LI> GTIndicator0000
    *   <LI> GTIndicator0001
    *   <LI> GTIndicator0010
    *   <LI> GTIndicator0011
    *   <LI> GTIndicator0100
    * </UL
    * @param  globalTitle the GlobalTitle of this TcapUserAddress
    */
    public void setGlobalTitle(GlobalTitle globalTitle) {
        this.globalTitle = globalTitle;
    }

    /**
    * The Signalling Point Code of the User Address.
    * @serial signalingPointCode - a default serializable field
    */
    private byte[] signalingPointCode = null;

    /**
    * The Sub-System Number of the User Address
    * @serial subSystemNumber - a default serializable field
    */
    private short subSystemNumber = 0;

    /**
    * The Global Title of the User Address
    * @serial globalTitle - a default serializable field
    */
    private GlobalTitle globalTitle = null;

    /**
    * Indicates if global title routing is used
    * @serial globalTitleRouting - a default serializable field
    */
    private boolean globalTitleRouting = false;

    /**
    * @serial signalingPointCodePresent - a default serializable field
    */
    private boolean signalingPointCodePresent = false;

    /**
    * @serial subSysrtemNumberPresent - a default serializable field
    */
    private boolean subSystemNumberPresent = false;

    /**
    * Returns a string representation of an instance of this class.
    */
    public String toString() {
        String text = "TcapUserAddress\n";

        try {
            byte[] pointCode = getSignalingPointCode();
            text += "\t\t\tSignalling Point Code: " +
                      new String(JainSS7Utility.bytesToHex(pointCode,
                                                            0,
                                                            pointCode.length)) + "\n";
            } catch(ParameterNotSetException e) {}

        try {
            text += "\t\t\tSub-System Number: " + getSubSystemNumber() + "\n";
        } catch(ParameterNotSetException e) {}

        try {
            text += "\t\t\tGlobal Title: " + getGlobalTitle().toString() + "\n";
        } catch(ParameterNotSetException e) {}
        text += "\t\t\tGlobal Title routing indicator: " + isGlobalTitleRouting() + "\n";

        return text;
    }
}

