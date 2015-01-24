/*
 @(#) src/java/javax/csapi/cc/jcc/JccAddress.java <p>
 
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

package javax.csapi.cc.jcc;


/**
<P>An <CODE>JccAddress</CODE> object represents what we commonly think of 
as a "telephone number". 

<H4>Introduction</H4> 
An address uniquely identifies a communication endpoint--physical or logical. 
Its string representation is obtained by the method {@link #getName()}. 

<p> Address objects may be classified into two categories: 
<EM>local</EM> and <EM>remote</EM>.
A "Local"
Address is one physically or administratively serviced by the existing
Provider. Conversely, a "Remote" address is not served by this Provider. 
A remote Address may not have the full visibility or control capabilities
of a local address. (By impaired visibility, we mean connection states
involving this address may not be as finely discriminated; by impaired
control, we mean that not all the connection's methods involving this 
address may be functional.)
 Note that applications never explicitly create 
new Address objects. 

<H4>Address and Call Objects</H4> 
Address objects are related to Call objects via the Connection
object. The Connection object has a <EM>state</EM> which describes 
the current relationship between the Call and the Address. Each Address 
object may be part of more than one telephone call, and in each case,
is represented by a separate Connection object. 
<p> 

An Address is associated with a Call until the Connection moves into
the {@link JccConnection#DISCONNECTED} state. 
@since 1.0
*/


/** This interface represents the JccAddress. The constants defined in
  * this class might be moved to other (more general) classes in later
  * releases.
  * @version 1.2.2
  * @author Monavacon Limited
  * @since 1.0 */
public interface JccAddress {
    /** Returns the type of this Address object. The type of Address can
      * denote whether it is an IP address or a telephone address with a
      * particular numbering scheme such as E.164. This method is
      * expected to return one of the constants defined in this class.
      * <em>This methods is expected to change, it will return values
      * that will be defined by the general JAIN protocol
      * address.<p></em>
      * @return
      * The type of this Address object.  */
    public int getType();
    /** This constant value indicates that no address is present.  This
      * constant has an integer value of -1.  */
    final public static int NOT_PRESENT = -1;
    /** This constant value indicates that the address plan is
      * undefined.  This constant has an integer value of 0.  */
    final public static int UNDEFINED = 0;
    /** This constant value indicates that the address is an IP address.
      * For Ipv4 the dotted quad notation is used. Also for IPv6 the
      * dotted notation is used. The address can optionally be followed
      * by a port number separated by a colon.  This constant has an
      * integer value of 1.  */
    final public static int IP = 1;
    /** This constant value indicates that the address is an Ipv4 class
      * D address or Ipv6 equivalent in dotted notation.  This constant
      * has an integer value of 2.  */
    final public static int MULTICAST = 2;
    /** This constant value indicates that the address is a non
      * multicast or broadcast IP address in dotted notation.  This
      * constant has an integer value of 3.  */
    final public static int UNICAST = 3;
    /** This constant value indicates that the address is an
      * international number without the international access code,
      * including the country code and excluding the leading zero of the
      * area code.  This constant has an integer value of 4.  */
    final public static int E164 = 4;
    /** This constant value indicates that the address is an E.164
      * mobile number.  This constant has an integer value of 5.  */
    final public static int E164_MOBILE = 5;
    /** This constant value indicates that the address is an ATM End
      * System Address in binary format (40 bytes).  This constant has
      * an integer value of 6.  */
    final public static int AESA = 6;
    /** This constant value indicates that the address is an uniform
      * resource locator as defined in IETF RFC 1738.  This constant has
      * an integer value of 7.  */
    final public static int URL = 7;
    /** This constant value indicates that the address is an binary
      * representation of the Network Service Access Point.  This
      * constant has an integer value of 8.  */
    final public static int NSAP = 8;
    /** This constant value indicates that the address is an e-mail
      * address as specified in IETF RFC822.  This constant has an
      * integer value of 9.  */
    final public static int SMTP = 9;
    /** This constant value indicates that the address is an X400
      * address structured as a set of attibute value pairs separated by
      * semicolons.  */
    final public static int X400 = 10;
    /** This constant value indicates that the address is a H.323
      * address. A H.323 Address may be identified by a list of
      * addresses of different types, including URLs.  For example:
      * "31161249111,http://www.aepona.com,somebody@aepona.com" This
      * would be a H.323 address which is a combination of a E.164
      * address, an URL address and an e-mail address.  This constant
      * has an integer value of 11.  */
    final public static int H323 = 11;
    /** This constant value indicates that the address is a Session
      * Initiation Protocol address as specified in RFC2543. For
      * example: "sip:somebody@sun.com" This constant has an integer
      * value of 12.  */
    final public static int SIP = 12;
    /** This constant value indicates that the address is a Global
      * Title.  The format of a Global Title is GTI.TT.NP.NA.AI where:
      * <ul> <li>GTI = Global Title Indicator <li>TT = Translation Type
      * <li>NP = Numbering Plan <li>NA = Nature of Address Indicator
      * <li>AI = Address Information </ul> TT, NP, NA are all optional
      * depending on GT Indicator and standard being used.  For example:
      * 1.0.0.0.1234 This constant has an integer value of 13.  */
    final public static int GT = 13;
    /** This constant value indicates that the address is a Sub System
      * Number.  For example: "6.255.255.255", where the 6 equals the
      * Subsystem number, that is 6 is equal to the Home Location
      * Register seperated by a fullstop from the Signaling Point Code
      * which must be in the format of X.X.X This constant has an
      * integer value of 14.  */
    final public static int SSN = 14;
    /** Returns the string representation of the JccAddress. Note that
      * each JccAddress possesses a unique string representation within
      * a given JccProvider.  @return The "unique" string representation
      * of this JccAddress.  @since 1.0b */
    public java.lang.String getName();
    /** Retrieves the Jccprovider handling this address object. This
      * JccProvider object is valid throughout the lifetime of the
      * JccAddress and does not change once the JccAddress is created.
      * @return JccProvider object managing this call.  @since 1.0b */
    public JccProvider getProvider();
}
 
// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
