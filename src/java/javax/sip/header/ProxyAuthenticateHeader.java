/*
 @(#) $RCSfile: ProxyAuthenticateHeader.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:53 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:53 $ by $Author: brian $
 */

package javax.sip.header;

import javax.sip.*;

/**
    A Proxy-Authenticate header field value contains an authentication challenge. When a UAC sends a
    request to a proxy server, the proxy server MAY authenticate the originator before the request
    is processed. If no credentials (in the Proxy-Authorization header field) are provided in the
    request, the proxy can challenge the originator to provide credentials by rejecting the request
    with a 407 (Proxy Authentication Required) status code. The proxy MUST populate the 407 (Proxy
    Authentication Required) message with a Proxy-Authenticate header field value applicable to the
    proxy for the requested resource.  The field value consists of a challenge that indicates the
    authentication and parameters applicable to the proxy for this RequestURI. <p> Note - Unlike its
    usage within HTTP, the ProxyAuthenticateHeader must be passed upstream in the Response to the
    UAC. In SIP, only UAC's can authenticate themselves to proxies. <p> Proxies MUST NOT add values
    to the Proxy-Authorization header field.  All 407 (Proxy Authentication Required) responses MUST
    be forwarded upstream toward the UAC following the procedures for any other response. It is the
    UAC's responsibility to add the Proxy-Authorization header field value containing credentials
    for the realm of the proxy that has asked for authentication. <p> When the originating UAC
    receives the 407 (Proxy Authentication Required) it SHOULD, if it is able, re-originate the
    request with the proper credentials. It should follow the same procedures for the display of the
    "realm" parameter that are given above for responding to 401. If no credentials for a realm can
    be located, UACs MAY attempt to retry the request with a username of "anonymous" and no password
    (a password of ""). The UAC SHOULD also cache the credentials used in the re-originated request.
    <p> For Example: <br> <code> Proxy-Authenticate: Digest realm="jcp.org",
    domain="sip:ss1.duke.com", <br> qop="auth", nonce="f84f1cec41e6cbe5aea9c8e88d359", opaque="",
    <br> stale=FALSE, algorithm=MD5 </code>
    @see Parameters
    @see ProxyAuthorizationHeader
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ProxyAuthenticateHeader extends WWWAuthenticateHeader {
    /**
        Name of ProxyAuthenticateHeader.
      */
    static final java.lang.String NAME = "Proxy-Authenticate";
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
