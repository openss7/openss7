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

package javax.sip.header;

import javax.sip.*;

/**
    This interface represents the Reason header, as defined by RFC3326, this header is not part of
    RFC3261. <p> The ReasonHeader provides information on why a SIP request was issued, often useful
    when creating services and also used to encapsulate a final status code in a provisional
    response, which is needed to resolve the "Heterogeneous Error Response Forking Problem". <p> The
    Reason header field appears to be most useful for BYE and CANCEL requests, but it can appear in
    any request within a dialog, in any CANCEL request and in 155 (Update Requested) responses. When
    used in requests, clients and servers are free to ignore this header field. It has no impact on
    protocol processing. <p> Examples of the ReasonHeader usage are: <ul> <li> A SIP CANCEL request
    can be issued if the call has completed on another branch or was abandoned before answer. While
    the protocol and system behavior is the same in both cases, namely, alerting will cease, the
    user interface may well differ. In the second case, the call may be logged as a missed call,
    while this would not be appropriate if the call was picked up elsewhere.  <li> Third party call
    controllers sometimes generate a SIP request upon reception of a SIP response from another
    dialog. Gateways generate SIP requests after receiving messages from a different protocol than
    SIP. In both cases the client may be interested in knowing what triggered the SIP request.  <li>
    An INVITE can sometimes be rejected not because the session initiation was declined, but because
    some aspect of the request was not acceptable. If the INVITE forked and resulted in a rejection,
    the error response may never be forwarded to the client unless all the other branches also
    reject the request. This problem is known as the "Heterogeneous Error Response Forking Problem".
    The header field defined in this draft allows encapsulating the final error response in a 155
    (Update Requested) provisional response. </ul> A server must ignore Headers that it does not
    understand. A proxy must not remove or modify Headers that it does not understand.
    @version 1.2.2
    @author Monvacon Limited
  */
public interface ReasonHeader extends Parameters, Header {
    /**
        Name of ReasonHeader.
      */
    static final java.lang.String NAME = "Reason";
    /**
        Gets the cause value of the ReasonHeader.
        @return The integer value of the cause of the ReasonHeader.
      */
    int getCause();
    /**
        Sets the cause value of the ReasonHeader. Any SIP status code MAY appear in the Reason
        header field of a request, assuming the protocol field of the ReasonHeader is SIP.
        @param cause The new integer value of the cause of the ReasonHeader.
        @exception InvalidArgumentException Thrown when the cause value is less than zero.
      */
    void setCause(int cause)
        throws InvalidArgumentException;
    /**
        Sets the protocol of the ReasonHeader, for example SIP or Q.850.
        @param protocol The new string value of the protocol of the ReasonHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        protocol value.
      */
    void setProtocol(java.lang.String protocol)
        throws java.text.ParseException;
    /**
        Gets the protocol value of the ReasonHeader
        @return The string value of the protocol of the ReasonHeader.
      */
    java.lang.String getProtocol();
    /**
        Sets the text value of the ReasonHeader.
        @param text The new string value of the text of the ReasonHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the text
        value.
      */
    void setText(java.lang.String text)
        throws java.text.ParseException;
    /**
        Gets the text value of the ReasonHeader
        @return The string value of the text of the ReasonHeader.
      */
    java.lang.String getText();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
