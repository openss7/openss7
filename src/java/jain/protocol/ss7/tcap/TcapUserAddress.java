/*
 @(#) src/java/jain/protocol/ss7/tcap/TcapUserAddress.java <p>
 
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

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * TcapUserAddress represents an address of a TCAP User application.  A
  * TCAP User application may handle a number of User Addresses and will
  * register as an Event Listener of a JainTcapProvider for that User
  * Address. <p>
  * Any Events addressed to a User Address will be passed to the User
  * application to whom the User Address belongs. The User Address
  * comprises of the: <ul>
  * <li>Signaling Point Code
  * <li>Sub-System Number
  * <li>Optionally Global Title for use in Global Title translation.
  * </ul>
  * It is permitted for more than one JainTcapListener to register with
  * the same JainTcapProvider with the same User Address and events sent
  * to that User Address will be sent to all JainTcapListeners of that
  * User Address. i.e. it is up to the implementation of JAIN TCAP to
  * manage how the Events are handled between these Listeners.
  * @version 1.2.2
  * @author Monavacon Limited
  * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as
  * a result of the addition of the SccpUserAddress class.
  */
public final class TcapUserAddress implements java.io.Serializable {
    /** @deprecated
      * Constructs a TcapUserAddress with the specified Signalling Point
      * Code and Sub-System Number. Using this constructor will
      * automatically set the global title routing indicator to false.
      * @param signalingPointCode
      * The Signalling Point Code of the User Address. The format of the
      * signalling point code is described at
      * setSignallingPointCode(byte[]).
      * @param subSystemNumber
      * The Sub-System Number of the User Address.  */
    public TcapUserAddress(byte[] signalingPointCode, short subSystemNumber) {
        this.setSignalingPointCode(signalingPointCode);
        this.setSubSystemNumber(subSystemNumber);
    }
    /** @deprecated
      * Constructs a TcapUserAddress with the specified Global Title.
      * Using this constructor will automatically set the global title
      * routing indicator to true.
      * @param globalTitle
      * The Global title.  */
    public TcapUserAddress(GlobalTitle globalTitle) {
        this.globalTitle = globalTitle;
        this.globalTitleRouting = true;
    }
    /** @deprecated
      * Gets the Global Title Routing indicator. The indicator indicates
      * if global title will be used by SCCP to route a message.
      * @return
      * True if Global Title is used for routing, false if SSN is used
      * for routing.  */
    public boolean isGlobalTitleRouting() {
        return globalTitleRouting;
    }
    /** @deprecated
      * Sets the Global Title Routing indicator. The indicator indicates
      * if global title will be used by SCCP to route a message. If set
      * to true, SCCP will use the global title to route a message, if
      * set to false, SCCP will use the SSN to route the message.
      * @param gtRoutingIndicator
      * Enable/disable global title routing.  */
    public void setGlobalTitleRouting(boolean gtRoutingIndicator) {
        this.globalTitleRouting = gtRoutingIndicator;
    }
    /** @deprecated
      * Indicates if the Signalling Point Code is present in this User
      * Address.
      * @return
      * True if Signalling Point Code has been set.  */
    public boolean isSignalingPointCodePresent() {
        return signalingPointCodePresent;
    }
    /** @deprecated
      * Gets the Signalling Point Code of the User Address. The format
      * of the signalling point code is described at
      * setSignallingPointCode(byte[]).
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.
      */
    public byte[] getSignalingPointCode()
        throws ParameterNotSetException {
	 if (signalingPointCodePresent)
             return signalingPointCode;
         throw new ParameterNotSetException("Signaling Point Code: not set.");
    }
    /** @deprecated
      * Sets the Signalling Point Code of the User Address. The
      * signalling point code (spc) is a 3 byte array where: <ul>
      * <li> spc[0] == Signalling Point ID (member)
      * <li> spc[1] == Area Id (cluster)
      * <li> spc[2] == Zone Id (network) </ul>
      * The permitted ranges for each value are outlined in the table
      * below. <p>
      * <table align="center" border=1 cellpadding=1>
      * <tr>
      * <th rowspan=2>Variant</th>
      * <th>SP ID</th>
      * <th>Area ID</th>
      * <th>Zone ID</th>
      * </tr>
      * <tr>
      * <th>(member)</th>
      * <th>(cluster)</th>
      * <th>(network)</th>
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
      * NB:A byte in Java contains a signed integer and thus ranges from -128 to 127. <p>
      * When converting an Integer (intVal) to a byte (byteVal) using the following code:
      * @param pointCode
      * The signalingPointCode of this TcapUserAddress.  */
    public void setSignalingPointCode(byte[] pointCode) {
        this.signalingPointCode = pointCode;
        this.signalingPointCodePresent = true;
    }
    /** @deprecated
      * Gets the Sub-System Number of the User Address.
      * @return
      * The SubSystemNumber of this TcapUserAddress.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public short getSubSystemNumber()
        throws ParameterNotSetException {
        if (subSystemNumberPresent)
            return subSystemNumber;
        throw new ParameterNotSetException("SubSystem Number: not set.");
    }
    /** @deprecated
      * Sets the Sub-System Number of the User Address.
      * @param subSystemNumber
      * The SubSystemNumber of this TcapUserAddress.  */
    public void setSubSystemNumber(short subSystemNumber) {
	this.subSystemNumber = subSystemNumber;
        this.subSystemNumberPresent = true;
    }
    /** @deprecated
      * Gets the Global Title of the User Address. The GlobalTitle is an
      * object, of which the default value is null. If the get accessor
      * method is used and the GlobalTitle object has a null value, then
      * a ParameterNotSetException will be thrown.
      * @return
      * The GlobalTitle of this TcapUserAddress.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public GlobalTitle getGlobalTitle()
        throws ParameterNotSetException {
        if (globalTitle != null )
            return globalTitle;
        throw new ParameterNotSetException("Global Title: not set.");
    }
    /** @deprecated
      * Sets the Global Title of this User Address. The Global Title is
      * an object which can be identified to be one of five different
      * types. <ul>
      * <li>GTIndicator0000
      * <li>GTIndicator0001
      * <li>GTIndicator0010
      * <li>GTIndicator0011
      * <li>GTIndicator0100 </ul>
      * @param globalTitle
      * The GlobalTitle of this TcapUserAddress.  */
    public void setGlobalTitle(GlobalTitle globalTitle) {
        this.globalTitle = globalTitle;
    }
    /** @deprecated
      * Returns a string representation of an instance of this class.  */
    public java.lang.String toString() {
        java.lang.String text = "TcapUserAddress\n";
        try {
            byte[] pointCode = getSignalingPointCode();
            text += "\t\t\tSignalling Point Code: " + new java.lang.String(JainSS7Utility.bytesToHex(pointCode, 0, pointCode.length)) + "\n";
        } catch (ParameterNotSetException e) {
        }
        try {
            text += "\t\t\tSub-System Number: " + getSubSystemNumber() + "\n";
        } catch (ParameterNotSetException e) {
        }
        try {
            text += "\t\t\tGlobal Title: " + getGlobalTitle().toString() + "\n";
        } catch (ParameterNotSetException e) {
        }
        text += "\t\t\tGlobal Title routing indicator: " + isGlobalTitleRouting() + "\n";
        return text;
    }
    /** The Signalling Point Code of the User Address.
      * @serial signalingPointCode
      * - a default serializable field.  */
    private byte[] signalingPointCode = null;
    /** The Sub-System Number of the User Address.
      * @serial subSystemNumber
      * - a default serializable field.  */
    private short subSystemNumber = 0;
    /** The Global Title of the User Address.
      * @serial globalTitle
      * - a default serializable field.  */
    private GlobalTitle globalTitle = null;
    /** Indicates if global title routing is used.
      * @serial globalTitleRouting
      * - a default serializable field.  */
    private boolean globalTitleRouting = false;
    /** WHether the Signaling Point Code is present.
      * @serial signalingPointCodePresent
      * - a default serializable field.  */
    private boolean signalingPointCodePresent = false;
    /** Whether the SubSystem Number is present.
      * @serial subSystemNumberPresent
      * - a default serializable field.  */
    private boolean subSystemNumberPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
