/*
 @(#) $RCSfile: Router.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:52 $ <p>
 
 Copyright &copy; 2008-2010  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 
 Last Modified $Date: 2009-06-21 11:35:52 $ by $Author: brian $
 */

package javax.sip.address;

import javax.sip.message.*;
import javax.sip.*;

/**
    The Router interface may be implemented by the application to provide custom routing logic. It
    is used to determine the next hop for a given request. <p> For backwards compatibility reasons,
    the default behavior of the stack is to consult the application provided Router implementation
    for all requests outside of a dialog. This is controlled through the stack property
    javax.sip.USE_ROUTER_FOR_ALL_URIS which defaults to true when not set. <p> This specification
    recommends to set the stack property javax.sip.USE_ROUTER_FOR_ALL_URIS to false. This will cause
    the stack to only consult the application provided Router implementation for requests with a
    non-SIP URI as request URI (such as tel: or pres:) and without Route headers. This enables an
    application to implement DNS lookups and other resolution algorithms <p> When
    javax.sip.USE_ROUTER_FOR_ALL_URIS is set to false, the next hop is determined according to the
    following algorithm: <ul> <li>If the request contains one or more Route headers, use the URI of
    the topmost Route header as next hop, possibly modifying the request in the process if the
    topmost Route header contains no lr parameter(See Note below)) <li>Else, if the property
    javax.sip.OUTBOUND_PROXY is set, use its value as the next hop <li>Otherwise, use the request
    URI as next hop. If the request URI is not a SIP URI, call getNextHop(Request) provided by the
    application. </ul> Note: In case the topmost Route header contains no 'lr' parameter (which
    means the next hop is a strict router), the implementation will perform 'Route Information
    Postprocessing' as described in RFC3261 section 16.6 step 6 (also known as "Route header
    popping").  That is, the following modifications will be made to the request: <ol> <li>The
    implementation places the Request-URI into the Route header field as the last value.  <li>The
    implementation then places the first Route header field value into the Request-URI and removes
    that value from the Route header field. </ol> Subsequently, the request URI will be used as next
    hop target. <p> The location (classname) of the user-defined Router object is supplied in the
    Properties object passed to the SipFactory.createSipStack(Properties) method upon creation of
    the SIP Stack object. The Router object must accept a SipStack as an argument to the constructor
    in order for the Router to access attributes of the SipStack The constructor of an object
    implementing the Router interface must be RouterImpl(SipStack sipStack, java.lang.String outboundProxy) {}
    <p> The routing policy can not be changed dynamically, i.e. the SipStack needs to be deleted and
    re-created. Outbound proxy should be passed to the SipFactory.createSipStack(Properties) method
    upon creation of the SIP Stack object. <p> <h5>Application Notes</h5> A UAC application which
    desires to use a particular outbound proxy should prepend a Route header with the URI of that
    proxy (and 'lr' flag if appropriate). Alternatively, it may achieve the same result by setting
    the OUTBOUND_PROXY property (although the Route header approach is more flexible and therefore
    RECOMMENDED) <p> A proxy application may either rewrite the request URI (if the proxy is
    responsible for the domain), or prepend a Route header.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface Router {
    /**
        Gets the Outbound Proxy parameter of this Router, this method may return null if no outbound
        proxy is defined.
        @return The Outbound Proxy of this Router.
        @see Hop
      */
    public Hop getOutboundProxy();
    /**
        @deprecated Since v1.2. This method is replaced with getNextHop(Request) method which
        returns the next Hop for this request. <p> Gets the ListIterator of the hops of the default
        Route. This method may return null if a default route is not defined.
      */
    public java.util.ListIterator<Hop> getNextHops(Request request);
    /**
        Gets the next Hop from this Router for the specified request, this method may return null if
        a default route is not defined.
        @return The next Hop from this Router for the Request.
        @exception SipException When there is something wrong with the request.
        @since v1.2
        @see Hop
      */
    public Hop getNextHop(Request request)
        throws SipException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
