package javax.csapi.cc.jcc;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.1
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


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


/**
This interface represents the JccAddress. The constants defined in this class might be 
moved to other (more general) classes in later releases.
@since 1.0
*/
public interface JccAddress {

    /**
    Returns the type of this Address object. The type of Address can 
    denote whether it is an IP address or a telephone address with a 
    particular numbering scheme such as E.164. This method is expected 
    to return one of the constants defined in this class.
    <em>This methods is expected to change, it will return values that will 
    be defined by the general JAIN protocol address.<p></em>
    @return the type of this Address object. 
    */
    
    public int getType();

	/** This constant value indicates that no address is present. 
	This constant has an integer value of -1.
	*/
	final public static int NOT_PRESENT = -1;

	/** This constant value indicates that the address plan is undefined. 
	       This constant has an integer value of 0.
	*/
	final public static int UNDEFINED = 0;

	/**
	 * This constant value indicates that the address is an IP address. For Ipv4
   * the dotted quad notation is used. Also for IPv6 the dotted notation is
   * used. The address can optionally be followed by a port number separated by
   * a colon.
   This constant has an integer value of 1.
	 */
	final public static int IP = 1;

	/**
	 * This constant value indicates that the address is an Ipv4 class D address
   * or Ipv6 equivalent in dotted notation.
   This constant has an integer value of 2.
	 */
	final public static int MULTICAST = 2;

	/**
	 * This constant value indicates that the address is a non multicast or
   * broadcast IP address in dotted notation.
       This constant has an integer value of 3.
	 */
	final public static int UNICAST = 3;

	/**
	 * This constant value indicates that the address is an international number
   * without the international access code, including the country code and
   * excluding the leading zero of the area code.
       This constant has an integer value of 4.
	 */
	final public static int E164 = 4;

	/**      
	This constant value indicates that the address is an E.164 mobile number. 
    This constant has an integer value of 5.
    */

	final public static int E164_MOBILE = 5;

	/**
	 * This constant value indicates that the address is an ATM End System
   * Address in binary format (40 bytes).
   This constant has an integer value of 6.
	 */
	final public static int AESA = 6;

	/**
	 * This constant value indicates that the address is an uniform resource
   * locator as defined in IETF RFC 1738.
   This constant has an integer value of 7.
	 */
	final public static int URL = 7;

	/**
	 * This constant value indicates that the address is an binary representation
   * of the Network Service Access Point.
   This constant has an integer value of 8.
	 */
	final public static int NSAP = 8;

	/**
	 * This constant value indicates that the address is an e-mail address as
   * specified in IETF RFC822.
   This constant has an integer value of 9.
	 */
	final public static int SMTP = 9;

	/**
	 * This constant value indicates that the address is an X400 address
   * structured as a set of attibute value pairs separated by semicolons.
	 */
	final public static int X400 = 10;
	
	/**
    This constant value indicates that the address is a H.323 address. A H.323 Address may be
    identified by a list of addresses of different types, including URLs. 
    For example: "31161249111,http://www.aepona.com,somebody@aepona.com" 
    This would be a H.323 address which is a combination of a E.164 address, an URL address and
    an e-mail address. 
    This constant has an integer value of 11.
	*/
	final public static int H323 = 11;

	/**
    This constant value indicates that the address is a Session Initiation Protocol address as specified
    in RFC2543. For example: "sip:somebody@sun.com" 
    This constant has an integer value of 12.
    */
	final public static int SIP = 12;

	/**
    This constant value indicates that the address is a Global Title. 
    The format of a Global Title is GTI.TT.NP.NA.AI where: 
    <ul>
    <li>GTI = Global Title Indicator 
    <li>TT = Translation Type 
    <li>NP = Numbering Plan 
    <li>NA = Nature of Address Indicator 
    <li>AI = Address Information 
    </ul>
    TT, NP, NA are all optional depending on GT Indicator and standard being used. 
    For example: 1.0.0.0.1234 
    This constant has an integer value of 13.
    */
	final public static int GT = 13;

	/**
    This constant value indicates that the address is a Sub System Number. 
    For example: "6.255.255.255", where the 6 equals the Subsystem number, that is 6 is equal to the
    Home Location Register seperated by a fullstop from the Signaling Point Code which must be in
    the format of X.X.X 
    This constant has an integer value of 14.
    */
	final public static int SSN = 14;
	
	//-------------------------------------------
	//FROM JCP
	
    /**
    Returns the string representation of the JccAddress. Note that  
    each JccAddress possesses a unique string representation within a 
    given JccProvider.  
    
    @return the "unique" string representation of this JccAddress.
    @since 1.0b
    */
    public String getName();
    /**
        Retrieves the Jccprovider handling this address object. This JccProvider
        object is valid throughout the lifetime of the JccAddress and does 
        not change once the JccAddress is created. 

        @return JccProvider object managing this call.
        @since 1.0b
    */
    public JccProvider getProvider();


}
 
