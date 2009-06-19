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

package javax.sip.address;

import javax.sip.*;

/**
    This interface provides factory methods that allow an application to create Address objects,
    URI's, SipURI's and TelURL's from a particular implementation of this specification. This class
    is a singleton and can be retrieved from the SipFactory.createAddressFactory().
    @version 1.2.2
    @author Monavacon Limited
  */
public interface AddressFactory {
    /**
        Creates a URI based on given URI string. The URI string is parsed in order to create the new
        URI instance. Depending on the scheme the returned may or may not be a SipURI or TelURL cast
        as a URI.
        @param uri The new string value of the URI.
        @exception java.text.ParseException If the URI string is malformed.
      */
    public URI createURI(java.lang.String uri)
        throws java.text.ParseException;
    /**
        Creates a SipURI based on the given user and host components.  The user component may be
        null. <p> This create method first builds a URI in string form using the given components as
        follows: <ul> <li>Initially, the result string is empty.  <li>The scheme followed by a colon
        character ('sip:') is appended to the result.  <li>The user and host are then appended. Any
        character that is not a legal URI character is quoted. </ul> The resulting URI string is
        then parsed in order to create the new SipURI instance as if by invoking the
        createURI(String) constructor; this may cause a URISyntaxException to be thrown.  <p> An
        application that wishes to create a 'sips' URI should call the SipURI.setSecure(boolean)
        with an argument of 'true' on the returned SipURI.
        @param user The new string value of the user, this value may be null.
        @param host The new string value of the host.
        @exception java.text.ParseException Thrown when the URI string is malformed.
      */
    public SipURI createSipURI(java.lang.String user, java.lang.String host)
        throws java.text.ParseException;
    /**
        Creates a TelURL based on given URI string. The scheme should not be included in the
        phoneNumber string argument.
        @param phoneNumber The new string value of the phoneNumber.
        @exception java.text.ParseException If the URI string is malformed.
      */
    public TelURL createTelURL(java.lang.String phoneNumber)
        throws java.text.ParseException;
    /**
        Creates an Address with the new address string value. The address string is parsed in order
        to create the new Address instance. Valid arguments obey the syntax for name-addr tokens in
        RFC3261, for example "Bob ". It is recommended to use the to use the name-addr form
        containing '<' '>' to avoid confusion of URI parameters. As a special case, the string
        argument "*" creates a wildcard Address object with the property that
        ((SipURI)Address.getURI()).getUser() returns a String contain one character "*".
        @param address The new string value of the address.
        @exception java.text.ParseException Thrown to signal that an error has occurred unexpectedly
        while parsing the address value.
      */
    public Address createAddress(java.lang.String address)
        throws java.text.ParseException;
    /**
        Creates an Address with the new URI attribute value.
        @param uri The URI value of the address.
      */
    public Address createAddress(URI uri);
    /**
        Creates an Address with the new display name and URI attribute values.
        @param displayName The new string value of the display name of the address. A null value
        does not set the display name.
        @param uri The new URI value of the address.
        @exception java.text.ParseException Thrown to signal that an error has occurred unexpectedly
        while parsing the displayName value.
      */
    public Address createAddress(java.lang.String displayName, URI uri)
        throws java.text.ParseException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
