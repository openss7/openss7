/*
 @(#) src/java/javax/sip/header/WWWAuthenticateHeader.java <p>
 
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
    This interface represents the WWW-Authenticate response-header. A WWW-Authenticate header field
    value contains an authentication challenge. When a UAS receives a request from a UAC, the UAS
    MAY authenticate the originator before the request is processed. If no credentials (in the
    Authorization header field) are provided in the request, the UAS can challenge the originator to
    provide credentials by rejecting the request with a 401 (Unauthorized) status code. The
    WWW-Authenticate response-header field MUST be included in 401 (Unauthorized) response messages.
    The field value consists of at least one challenge that indicates the authentication scheme(s)
    and parameters applicable to the realm. <p> For Example: <br> <code> WWW-Authenticate: Digest
    realm="atlanta.com", <br> domain="sip:boxesbybob.com", qop="auth", <br>
    nonce="f84f1cec41e6cbe5aea9c8e88d359", opaque="", stale=FALSE, <br> algorithm=MD5 </code>
    @see Parameters
    @version 1.2.2
    @author Monavacon Limited
  */
public interface WWWAuthenticateHeader extends Parameters, Header {
    /**
        Name of WWWAuthenticateHeader.
      */
    static final java.lang.String NAME = "WWW-Authenticate";
    /**
        Sets the scheme of the challenge information for this WWWAuthenticateHeader. For example,
        Digest.
        @param scheme The new string value that identifies the challenge information scheme.
      */
    void setScheme(java.lang.String scheme);
    /**
        Returns the scheme of the challenge information for this WWWAuthenticateHeader.
        @return The string value of the challenge information.
      */
    java.lang.String getScheme();
    /**
        Sets the Realm of the WWWAuthenicateHeader to the realm parameter value. Realm strings MUST
        be globally unique. It is RECOMMENDED that a realm string contain a hostname or domain name.
        Realm strings SHOULD present a human-readable identifier that can be rendered to a user.
        @param realm The new Realm java.lang.String of this WWWAuthenicateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the realm.
      */
    void setRealm(java.lang.String realm)
        throws java.text.ParseException;
    /**
        Returns the Realm value of this WWWAuthenicateHeader. This convenience method returns only
        the realm of the complete Challenge.
        @return The java.lang.String representing the Realm information, null if value is not set.
      */
    java.lang.String getRealm();
    /**
        Sets the Nonce of the WWWAuthenicateHeader to the nonce parameter value.
        @param nonce The new nonce java.lang.String of this WWWAuthenicateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the nonce
        value.
      */
    void setNonce(java.lang.String nonce)
        throws java.text.ParseException;
    /**
        Returns the Nonce value of this WWWAuthenicateHeader.
        @return The java.lang.String representing the nonce information, null if value is not set.
      */
    java.lang.String getNonce();
    /**
        @deprecated Since v1.2. URI is not a supported parameter for this header. <p> Sets the URI
        of the WWWAuthenicateHeader to the URI parameter value.
        @param uri The new URI of this WWWAuthenicateHeader.
      */
    void setURI(URI uri);
    /**
        @deprecated Since v1.2. URI is not a supported parameter for this header. This method should
        return null. <p> Returns the URI value of this WWWAuthenicateHeader, for example DigestURI.
        @return The URI representing the URI information, null if value is not set.
      */
    URI getURI();
    /**
        Sets the Algorithm of the WWWAuthenicateHeader to the new algorithm parameter value.
        @param algorithm The new algorithm java.lang.String of this WWWAuthenicateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        algorithm value.
      */
    void setAlgorithm(java.lang.String algorithm)
        throws java.text.ParseException;
    /**
        Returns the Algorithm value of this WWWAuthenicateHeader.
        @return The java.lang.String representing the Algorithm information, null if the value is not set.
      */
    java.lang.String getAlgorithm();
    /**
        Sets the Qop value of the WWWAuthenicateHeader to the new qop parameter value.
        @param qop The new Qop string of this WWWAuthenicateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the Qop
        value.
      */
    void setQop(java.lang.String qop)
        throws java.text.ParseException;
    /**
        Returns the Qop value of this WWWAuthenicateHeader.
        @return The string representing the Qop information, null if the value is not set.
      */
    java.lang.String getQop();
    /**
        Sets the Opaque value of the WWWAuthenicateHeader to the new opaque parameter value.
        @param opaque The new Opaque string of this WWWAuthenicateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the opaque
        value.
      */
    void setOpaque(java.lang.String opaque)
        throws java.text.ParseException;
    /**
        Returns the Opaque value of this WWWAuthenicateHeader.
        @return The java.lang.String representing the Opaque information, null if the value is not set.
      */
    java.lang.String getOpaque();
    /**
        Sets the Domain of the WWWAuthenicateHeader to the domain parameter value.
        @param domain The new Domain string of this WWWAuthenicateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the domain.
      */
    void setDomain(java.lang.String domain)
        throws java.text.ParseException;
    /**
        Returns the Domain value of this WWWAuthenicateHeader.
        @return The java.lang.String representing the Domain information, null if value is not set.
      */
    java.lang.String getDomain();
    /**
        Sets the value of the stale parameter of the WWWAuthenicateHeader to the stale parameter
        value.
        @param stale The new boolean value of the stale parameter.
      */
    void setStale(boolean stale);
    /**
        Returns the boolean value of the state paramater of this WWWAuthenicateHeader.
        @return The boolean representing if the challenge is stale.
      */
    boolean isStale();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
