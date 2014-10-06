/*
 @(#) $RCSfile: MessageFactory.java,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:49 $ <p>
 
 Copyright &copy; 2008-2011  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 
 Last Modified $Date: 2011-09-02 08:46:49 $ by $Author: brian $
 */

package javax.sip.message;

import javax.sip.address.*;
import javax.sip.header.*;
import javax.sip.*;

/**
    This interface provides factory methods that allow an application to create Request and Response
    messages from a particular implementation of JAIN SIP. This class is a singleton and can be
    retrieved from the SipFactory.createMessageFactory().
    @version 1.2.2
    @author Monavacon Limited
  */
public interface MessageFactory {
    /**
        Creates a new Request message of type specified by the method paramater, containing the URI
        of the Request, the mandatory headers of the message with a body in the form of a Java
        object and the body content type.
        @param requestURI The new URI object of the requestURI value of this Message.
        @param method The new string of the method value of this Message.
        @param callId The new CallIdHeader object of the callId value of this Message.
        @param cSeq The new CSeqHeader object of the cSeq value of this Message.
        @param from The new FromHeader object of the from value of this Message.
        @param to The new ToHeader object of the to value of this Message.
        @param via The new List object of the ViaHeaders of this Message.
        @param contentType The new ContentTypeHeader object of the
        @param content Type value of this Message.
        @return The newly created Request object.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        or the body.
      */
    Request createRequest(URI requestURI, java.lang.String method, CallIdHeader callId, CSeqHeader cSeq, FromHeader from, ToHeader to, java.util.List via, MaxForwardsHeader maxForwards, ContentTypeHeader contentType, java.lang.Object content) throws java.text.ParseException;
    /**
        Creates a new Request message of type specified by the method paramater, containing the URI
        of the Request, the mandatory headers of the message with a body in the form of a byte array
        and body content type.
        @param requestURI The new URI object of the requestURI value of this Message.
        @param method The new string of the method value of this Message.
        @param callId The new CallIdHeader object of the callId value of this Message.
        @param cSeq The new CSeqHeader object of the cSeq value of this Message.
        @param from The new FromHeader object of the from value of this Message.
        @param to The new ToHeader object of the to value of this Message.
        @param via The new List object of the ViaHeaders of this Message.
        @param contentType The new ContentTypeHeader object of the content type value of this
        Message.
        @param content The new byte array of the body content value of this Message.
        @return The newly created Request object.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        or the body.
      */
    Request createRequest(URI requestURI, java.lang.String method, CallIdHeader callId, CSeqHeader cSeq, FromHeader from, ToHeader to, java.util.List via, MaxForwardsHeader maxForwards, ContentTypeHeader contentType, byte[] content) throws java.text.ParseException;
    /**
        Creates a new Request message of type specified by the method paramater, containing the URI
        of the Request, the mandatory headers of the message. This new Request does not contain a
        body.
        @param requestURI The new URI object of the requestURI value of this Message.
        @param method The new string of the method value of this Message.
        @param callId The new CallIdHeader object of the callId value of this Message.
        @param cSeq The new CSeqHeader object of the cSeq value of this Message.
        @param from The new FromHeader object of the from value of this Message.
        @param to The new ToHeader object of the to value of this Message.
        @param via The new List object of the ViaHeaders of this Message.
        @return The newly created Request object.
        @exception java.text.ParseException Thrown when an error was found while parsing the method.
      */
    Request createRequest(URI requestURI, java.lang.String method, CallIdHeader callId, CSeqHeader cSeq, FromHeader from, ToHeader to, java.util.List via, MaxForwardsHeader maxForwards) throws java.text.ParseException;
    /**
        Create a new SIP Request object based on a specific string value. This method parses the
        supplied string into a SIP Request. The request string should only consist of the SIP
        portion of the Request and not the content. Supplying a null argument creates an empty SIP
        Request which may be used to end out "keep alive" messages for a connection.
        @param request The new string value of the Request.
        @return The newly created Request object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        request.
      */
    Request createRequest(java.lang.String request) throws java.text.ParseException;
    /**
        Creates a new Response message of type specified by the statusCode paramater, containing the
        mandatory headers of the message with a body in the form of a Java object and the body
        content type.
        @param statusCode The new integer of the statusCode value of this Message.
        @param callId The new CallIdHeader object of the callId value of this Message.
        @param cSeq The new CSeqHeader object of the cSeq value of this Message.
        @param from The new FromHeader object of the from value of this Message.
        @param to The new ToHeader object of the to value of this Message.
        @param via The new List object of the ViaHeaders of this Message.
        @param contentType The new ContentTypeHeader object of the content type value of this
        Message.
        @param content The new Object of the body content value of this Message.
        @return The newly created Response object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        statusCode or the body.
      */
    Response createResponse(int statusCode, CallIdHeader callId, CSeqHeader cSeq, FromHeader from, ToHeader to, java.util.List via, MaxForwardsHeader maxForwards, ContentTypeHeader contentType, java.lang.Object content) throws java.text.ParseException;
    /**
        Creates a new Response message of type specified by the statusCode paramater, containing the
        mandatory headers of the message with a body in the form of a byte array and the body
        content type.
        @param statusCode The new integer of the statusCode value of this Message.
        @param callId The new CallIdHeader object of the callId value of this Message.
        @param cSeq The new CSeqHeader object of the cSeq value of this Message.
        @param from The new FromHeader object of the from value of this Message.
        @param to The new ToHeader object of the to value of this Message.
        @param via The new List object of the ViaHeaders of this Message.
        @param contentType The new ContentTypeHeader object of the content type value of this
        Message.
        @param content The new byte array of the body content value of this Message.
        @return The newly created Response object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        statusCode or the body.
      */
    Response createResponse(int statusCode, CallIdHeader callId, CSeqHeader cSeq, FromHeader from, ToHeader to, java.util.List via, MaxForwardsHeader maxForwards, ContentTypeHeader contentType, byte[] content) throws java.text.ParseException;
    /**
        Creates a new Response message of type specified by the statusCode paramater, containing the
        mandatory headers of the message. This new Response does not contain a body.
        @param statusCode The new integer of the statusCode value of this Message.
        @param callId The new CallIdHeader object of the callId value of this Message.
        @param cSeq The new CSeqHeader object of the cSeq value of this Message.
        @param from The new FromHeader object of the from value of this Message.
        @param to The new ToHeader object of the to value of this Message.
        @param via The new List object of the ViaHeaders of this Message.
        @return The newly created Response object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        statusCode.
      */
    Response createResponse(int statusCode, CallIdHeader callId, CSeqHeader cSeq, FromHeader from, ToHeader to, java.util.List via, MaxForwardsHeader maxForwards) throws java.text.ParseException;
    /**
        Creates a new Response message of type specified by the statusCode paramater, based on a
        specific Request with a new body in the form of a Java object and the body content type.
        Only the required headers are copied from the Request.
        @param statusCode The new integer of the statusCode value of this Message.
        @param request The received Reqest object upon which to base the Response.
        @param contentType The new ContentTypeHeader object of the content type value of this
        Message.
        @param content The new Object of the body content value of this Message.
        @return The newly created Response object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        statusCode or the body.
      */
    Response createResponse(int statusCode, Request request, ContentTypeHeader contentType, java.lang.Object content) throws java.text.ParseException;
    /**
        Creates a new Response message of type specified by the statusCode paramater, based on a
        specific Request with a new body in the form of a byte array and the body content type.
        Only the required headers are copied from the Request.
        @param statusCode The new integer of the statusCode value of this Message.
        @param request The received Reqest object upon which to base the Response.
        @param contentType The new ContentTypeHeader object of the content type value of this
        Message.
        @param content The new byte array of the body content value of this Message.
        @return The newly created Response object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        statusCode or the body.
      */
    Response createResponse(int statusCode, Request request, ContentTypeHeader contentType, byte[] content) throws java.text.ParseException;
    /**
        Creates a new Response message of type specified by the statusCode paramater, based on a
        specific Request message. This new Response does not contain a body. Only the required
        headers are copied from the Request.
        @param statusCode The new integer of the statusCode value of this Message.
        @param request The received Reqest object upon which to base the Response.
        @return The newly created Response object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        statusCode.
      */
    Response createResponse(int statusCode, Request request) throws java.text.ParseException;
    /**
        Creates a Response from a java.lang.String. This method parses the supplied string into a SIP
        Response. The response string should only consist of the SIP portion of the Response and not
        the content.
        @param response Is a string representing the response. The argument should only contain the
        Sip Headers and not the body of the response.
        @exception java.text.ParseException Thrown when error has been reached unexpectedly while
        parsing the response.
        Since: v1.2
      */
    Response createResponse(java.lang.String response) throws java.text.ParseException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
