// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/sip/header/AuthorizationHeader.java <p>
 
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

package javax.sip.header;

import javax.sip.address.*;
import javax.sip.*;

/**
    The Authorization header is used when a user agent wishes to authenticate itself with a server -
    usually, but not necessarily, after receiving an UNAUTHORIZED Response - by including an
    AuthorizationHeader with the Request. The AuthorizationHeader consists of credentials containing
    the authentication information of the user agent for the realm of the resource being requested.
    <p> This header field, along with Proxy-Authorization, breaks the general rules about multiple
    header field values. Although not a comma- separated list, this header field name may be present
    multiple times, and MUST NOT be combined into a single header line. <p> <dt>For
    Example:<br><code> Authorization: Digest username="Alice", realm="atlanta.com",<br>
    nonce="84a4cc6f3082121f32b42a2187831a9e",<br>
    response="7587245234b3434cc3412213e5f113a5432"</code>
    @see Parameters
    @see WWWAuthenticateHeader
    @see ProxyAuthorizationHeader
    @version 1.2.2
    @author Monavacon Limited
  */
public interface AuthorizationHeader extends Parameters, Header {
    /**
        Name of AuthorizationHeader.
      */
    static final java.lang.String NAME = "Authorization";
    /**
        Sets the scheme of the Response information for this AuthorizationHeader. For example,
        Digest.
        @param scheme The new string value that identifies the response information scheme.
      */
    void setScheme(java.lang.String scheme);
    /**
        Returns the scheme of the Response information for this AuthorizationHeader.
        @return The string value of the response information.
      */
    java.lang.String getScheme();
    /**
        Sets the Realm of the AuthorizationHeader to the realm parameter value. Realm strings MUST
        be globally unique. It is RECOMMENDED that a realm string contain a hostname or domain name.
        Realm strings SHOULD present a human-readable identifier that can be rendered to a user.
        @param realm The new Realm java.lang.String of this AuthorizationHeader.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        realm.
      */
    void setRealm(java.lang.String realm)
        throws java.text.ParseException;
    /**
        Returns the Realm value of this AuthorizationHeader. This convenience method returns only
        the realm of the complete Response.
        @return The java.lang.String representing the Realm information, null if value is not set.
      */
    java.lang.String getRealm();
    /**
        Sets the Username of the AuthorizationHeader to the username parameter value.
        @param username The new Username java.lang.String of this AuthorizationHeader.
        @exception java.text.ParseException Thrown when an error is reached while parsing the
        username.
      */
    void setUsername(java.lang.String username)
        throws java.text.ParseException;
    /**
        Returns the Username value of this AuthorizationHeader. This convenience method returns only
        the username of the complete Response.
        @return The java.lang.String representing the Username information, null if value is not set.
      */
    java.lang.String getUsername();
    /**
        Sets the Nonce of the AuthorizationHeader to the nonce parameter value.
        @param nonce The new nonce java.lang.String of this AuthorizationHeader.
        @exception java.text.ParseException Thrown when an error is reached while parsing the nonce
        value.
      */
    void setNonce(java.lang.String nonce)
        throws java.text.ParseException;
    /**
        Returns the Nonce value of this AuthorizationHeader.
        @return The java.lang.String representing the nonce information, null if value is not set.
      */
    java.lang.String getNonce();
    /**
        Sets the URI of the AuthorizationHeader to the uri parameter value.
        @param uri The new URI of this AuthorizationHeader.
      */
    void setURI(URI uri);
    /**
        Returns the DigestURI value of this AuthorizationHeader.
        @return The URI representing the URI information, null if value is not set.
      */
    URI getURI();
    /**
        Sets the Response of the AuthorizationHeader to the new response parameter value.
        @param response The new response java.lang.String of this AuthorizationHeader.
        @exception java.text.ParseException Thrown when an error is reached while parsing the
        Response.
      */
    void setResponse(java.lang.String response)
        throws java.text.ParseException;
    /**
        Returns the Response value of this AuthorizationHeader.
        @return The java.lang.String representing the Response information.
      */
    java.lang.String getResponse();
    /**
        Sets the Algorithm of the AuthorizationHeader to the new algorithm parameter value.
        @param algorithm The new algorithm java.lang.String of this AuthorizationHeader.
        @exception java.text.ParseException Thrown when an error was reached while parsing the
        algorithm value.
      */
    void setAlgorithm(java.lang.String algorithm)
        throws java.text.ParseException;
    /**
        Returns the Algorithm value of this AuthorizationHeader.
        @return The java.lang.String representing the Algorithm information, null if the value is not set.
      */
    java.lang.String getAlgorithm();
    /**
        Sets the CNonce of the AuthorizationHeader to the cNonce parameter value.
        @param cNonce The new cNonce java.lang.String of this AuthorizationHeader.
        @exception java.text.ParseException Thrown when an error was reached while parsing the
        cNonce value.
      */
    void setCNonce(java.lang.String cNonce)
        throws java.text.ParseException;
    /**
        Returns the CNonce value of this AuthorizationHeader.
        @return The java.lang.String representing the cNonce information, null if value is not set.
      */
    java.lang.String getCNonce();
    /**
        Sets the Opaque value of the AuthorizationHeader to the new opaque parameter value.
        @param opaque The new Opaque string of this AuthorizationHeader.
        @exception java.text.ParseException Thrown whan an error was found while parsing the opaque
        value.
      */
    void setOpaque(java.lang.String opaque)
        throws java.text.ParseException;
    /**
        Returns the Opaque value of this AuthorizationHeader.
        @return The java.lang.String representing the Opaque information, null if the value is not set.
      */
    java.lang.String getOpaque();
    /**
        Sets the MessageQop value of the AuthorizationHeader to the new qop parameter value.
        @param qop The new Qop string of this AuthorizationHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the Qop
        value.
      */
    void setQop(java.lang.String qop)
        throws java.text.ParseException;
    /**
        Returns the Qop value of this AuthorizationHeader.
        @return The string representing the Qop information, null if the value is not set.
      */
    java.lang.String getQop();
    /**
        Sets the Nonce Count of the AuthorizationHeader to the nonceCount parameter value.
        @param nonceCount The new nonceCount integer of this AuthorizationHeader.
        @exception java.text.ParseException - Thrown when an error was found while parsing the
        nonceCount value.
      */
    void setNonceCount(int nonceCount)
        throws java.text.ParseException;
    /**
        Returns the Nonce Count value of this AuthorizationHeader.
        @return The integer representing the nonceCount information, -1 if value is not set.
      */
    int getNonceCount();
}
