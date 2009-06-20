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

import javax.sip.header.*;
import javax.sip.*;

/**
    This class represents SIP URIs, that may have either a sip: or sips: scheme. All SIP
    implementations MUST support the sip: URI scheme.  <p> SIP and SIPS URIs are used for
    addressing. They are similar to email addresses in that they are of the form user@host where
    user is either a user name or telephone number, and host is a host or domain name, or a numeric
    IP address. Additionally, SIP and SIPS URIs may contain parameters and headers (although headers
    are not legal in all contexts). A SipURI can be embedded in web pages, business cards or other
    hyperlinks to indicate that a particular user or service can be called via SIP. <p> Within a SIP
    Message, SipURIs are used to indicate the source and intended destination of a Request,
    redirection addresses and the current destination of a Request. Normally all these Headers will
    contain SipURIs. <p> Syntactically, SIP and SIPS URIs are identical except for the name of the
    URI scheme. The semantics differs in that the SIPS scheme implies that the identified resource
    is to be contacted using TLS.  Because SIP and SIPS URIs are syntactically identical and because
    they're used the same way, they're both represented by the SipURI interface. <p> The SipURI
    interface extends the generic URI interface and provides additional convenience methods for the
    following components of a SipURI address, above the generic URI interface: <p> <li>User - The
    set of valid telephone-subscriber strings is a subset of valid user strings. The user URI
    parameter exists to distinguish telephone numbers from user names that happen to look like
    telephone numbers. If the user string contains a telephone number formatted as a
    telephone-subscriber, the user parameter value "phone" SHOULD be present. Even without this
    parameter, recipients of SIP and SIPS URIs MAY interpret the pre-@ part as a telephone number if
    local restrictions on the name space for user name allow it.  <li>UserPassword - A password
    associated with the user. While the SIP and SIPS URI syntax allows this field to be present, its
    use is NOT RECOMMENDED, because the passing of authentication information in clear text (such as
    URIs) has proven to be a security risk in almost every case where it has been used. For
    instance, transporting a PIN number in this field exposes the PIN.  <li>URI parameters -
    Parameters affecting a request constructed from this URI. URI parameters are added after the
    hostport component and are separated by semi-colons. URI parameters take the form:
    parameter-name "=" parameter-value Even though an arbitrary number of URI parameters may be
    included in a URI, any given parameter-name MUST NOT appear more than once.  The SipURI
    interface also provides convenience methods for the manipulation of popular parameters in a
    SipURI address, namely: <ul> <li>Lr Parameter - the element responsible for this resource
    implements the routing mechanisms specified in RFC 3261.  <li>Method - The method of the SIP
    request constructed from the URI.  <li>MAddr Parameter - the server address to be contacted for
    this user.  <li>TTL Parameter - the time-to-live value when packets are sent using UDP
    multicast.  <li>User Parameter - the set of valid telephone-subscriber strings.  <li>Transport
    Parameter - specifies which transport protocol to use for sending requests and responses to this
    entity </ul> <li>URI Headers - Header fields to be included in a request constructed from the
    URI. Headers fields in the SIP request can be specified with the "?" mechanism within a URI. The
    header names and values are encoded in ampersand separated 'hname = hvalue' pairs.  The special
    hname "body" indicates that the associated hvalue is the message-body of the SIP request.
    <li>Secure - This determines if the scheme of this URI is either sip: or sips:, where sips: is
    secure. <p> See section 19.1.2 of RFC3261 for the use of SIP and SIPS URI components based on
    the context in which the URI appears.
    @version 1.2.2
    @author Monavacon Limited
    @see FromHeader
    @see ToHeader
    @see ContactHeader
    @see URI
  */
public interface SipURI extends URI, Parameters {
    /**
        Sets the user of SipURI. The identifier of a particular resource at the host being
        addressed. The user and the user password including the '@' sign make up the user-info.
        @param user The new java.lang.String value of the user.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setUser(java.lang.String user)
        throws java.text.ParseException;
    /**
        Returns the user part of this SipURI.
        @return The user part of this SipURI, this value may be null.
      */
    public java.lang.String getUser();
    /**
        Sets the user password associated with the user of SipURI.  While the SIP and SIPS URI
        syntax allows this field to be present, its use is NOT RECOMMENDED, because the passing of
        authentication information in clear text (such as URIs) has proven to be a security risk in
        almost every case where it has been used. The user password and the user including the @
        sign make up the user-info.
        @param userPassword The new java.lang.String value of the user password.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setUserPassword(java.lang.String userPassword)
        throws java.text.ParseException;
    /**
        Gets user password of SipURI, or null if it is not set.
        @return The user password of this SipURI.
      */
    public java.lang.String getUserPassword();
    /**
        Returns true if this SipURI is secure i.e. if this SipURI represents a sips URI. A sip URI
        returns false.
        @return True if this SipURI represents a sips URI, and false if it represents a sip URI.
      */
    public boolean isSecure();
    /**
        Sets the scheme of this URI to sip or sips depending on whether the argument is true or
        false. The default value is false.
        @param secure The boolean value indicating if the SipURI is secure.
      */
    public void setSecure(boolean secure);
    /**
        Set the host part of this SipURI to the newly supplied host parameter.
        @param host The new integer value of the host of this SipURI.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setHost(java.lang.String host)
        throws java.text.ParseException;
    /**
        Returns the host part of this SipURI.
        @return The host part of this SipURI.
      */
    public java.lang.String getHost();
    /**
        Set the port part of this SipURI to the newly supplied port parameter.
        @param port The new integer value of the port of this SipURI.
      */
    public void setPort(int port);
    /**
        Returns the port part of this SipURI.
        @return The port part of this SipURI.
      */
    public int getPort();
    /**
        Removes the port part of this SipURI. If no port is specified the stack will assume the
        default port.
      */
    public void removePort();
    /**
        Returns the value of the named header, or null if it is not set.  SIP/SIPS URIs may specify
        headers. As an example, the URI sip:jimmy@jcp.org?priority=urgent has a header "priority"
        whose value is "urgent".
        @param name Name of header to retrieve.
        @return The value of specified header.
      */
    public java.lang.String getHeader(java.lang.String name);
    /**
        Sets the value of the specified header fields to be included in a request constructed from
        the URI. If the header already had a value it will be overwritten.
        @param name A java.lang.String specifying the header name.
        @param value A java.lang.String specifying the header value.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setHeader(java.lang.String name, java.lang.String value)
        throws java.text.ParseException;
    /**
        Returns an Iterator over the java.lang.String names of all headers present in this SipURI.
        @return An Iterator over all the header names.
      */
    public java.util.Iterator getHeaderNames();
    /**
        Returns the value of the "transport" parameter, or null if this is not set. This is
        equivalent to getParameter("transport").
        @return The transport paramter of the SipURI.
      */
    public java.lang.String getTransportParam();
    /**
        Sets the value of the "transport" parameter. This parameter specifies which transport
        protocol to use for sending requests and responses to this entity. The following values are
        defined: "udp", "tcp", "sctp", "tls", but other values may be used also.  This method is
        equivalent to setParameter("transport", transport). Transport parameter constants are
        defined in the ListeningPoint.
        @param transport New value for the "transport" parameter.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setTransportParam(java.lang.String transport)
        throws java.text.ParseException;
    /**
        Returns the value of the "ttl" parameter, or -1 if this is not set. This method is
        equivalent to getParameter("ttl").
        @return The value of the ttl parameter.
      */
    public int getTTLParam();
    /**
        Sets the value of the ttl parameter. The ttl parameter specifies the time-to-live value when
        packets are sent using UDP multicast. This is equivalent to setParameter("ttl", ttl).
        @param ttl New value of the ttl parameter.
        @exception InvalidArgumentException Thrown when the supplied value is less than zero,
        excluding -1 the default not set value.
      */
    public void setTTLParam(int ttl)
        throws InvalidArgumentException;
    /**
        Returns the value of the method parameter, or null if this is not set. This is equivalent to
        getParameter("method").
        @return The value of the method parameter.
      */
    public java.lang.String getMethodParam();
    /**
        Sets the value of the method parameter. This specifies which SIP method to use in requests
        directed at this URI. This is equivalent to setParameter("method", method).
        @param method New value java.lang.String value of the method parameter
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setMethodParam(java.lang.String method)
        throws java.text.ParseException;
    /**
        Sets the value of the user parameter. The user URI parameter exists to distinguish telephone
        numbers from user names that happen to look like telephone numbers. This is equivalent to
        setParameter("user", user).
        @param userParam New value java.lang.String value of the method parameter
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setUserParam(java.lang.String userParam)
        throws java.text.ParseException;
    /**
        Returns the value of the userParam, or null if this is not set.  This is equivalent to
        getParameter("user").
        @return The value of the userParam of the SipURI.
      */
    public java.lang.String getUserParam();
    /**
        Returns the value of the maddr parameter, or null if this is not set. This is equivalent to
        getParameter("maddr").
        @return The value of the maddr parameter.
      */
    public java.lang.String getMAddrParam();
    /**
        Sets the value of the maddr parameter of this SipURI. The maddr parameter indicates the
        server address to be contacted for this user, overriding any address derived from the host
        field. This is equivalent to setParameter("maddr", maddr).
        @param mAddr New value of the maddr parameter.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        user value.
      */
    public void setMAddrParam(java.lang.String mAddr)
        throws java.text.ParseException;
    /**
        Returns whether the the lr parameter is set. This is equivalent to hasParameter("lr"). This
        interface has no getLrParam as RFC3261 does not specify any values for the "lr" paramater.
        @return True if the "lr" parameter is set, false otherwise.
      */
    public boolean hasLrParam();
    /**
        Sets the value of the lr parameter of this SipURI. The lr parameter, when present, indicates
        that the element responsible for this resource implements the routing mechanisms specified
        in RFC 3261. This parameter will be used in the URIs proxies place in the Record-Route
        header field values, and may appear in the URIs in a pre-existing route set.
      */
    public void setLrParam();
    /**
        This method returns the URI as a string.
        @return The stringified version of the URI.
      */
    public java.lang.String toString();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
