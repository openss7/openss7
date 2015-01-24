/*
 @(#) src/java/javax/sip/header/RecordRouteHeader.java <p>
 
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

import javax.sip.*;

/**
    The Record-Route header field is inserted by proxies in a request to force future requests in
    the dialog to be routed through the proxy.  The SipProvider (as opposed to the application)
    should attach Record-Route headers to messages explicitly when forwarding them if necessary. <p>
    The RecordRouteHeader is added to a Request by any proxy that insists on being in the path of
    subsequent Requests for the same call leg. It contains a globally reachable RequestURI that
    identifies the proxy server. Each proxy server adds its Address URI to the beginning of the
    list. <p> The calling user agent client copies the RecordRouteHeaders into RouteHeaders of
    subsequent Requests within the same call leg, reversing the order, so that the first entry is
    closest to the user agent client. If the Response contained a ContactHeader field, the calling
    user agent adds its content as the last RouteHeader. Unless this would cause a loop, a client
    must send subsequent Requests for this call leg to the Address URI in the first RouteHeader and
    remove that entry. <p> Some proxies, such as those controlling firewalls or in an automatic call
    distribution (ACD) system, need to maintain call state and thus need to receive any BYE and ACK
    Requests for the call. <p> For Example: <br> <code> Record-Route: sip:server10.jcp.org;lr,
    sip:bigbox3.duke.jcp.org;lr </code>
    @see RouteHeader
    @see HeaderAddress
    @see Parameters
    @version 1.2.2
    @author Monavacon Limited
  */
public interface RecordRouteHeader extends HeaderAddress, Parameters, Header {
    /**
        Name of RecordRouteHeader.
      */
    static final java.lang.String NAME = "Record-Route";
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
