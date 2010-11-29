// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: Message.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:28:37 $ <p>
 
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
 
 Last Modified $Date: 2010-11-28 14:28:37 $ by $Author: brian $
 */

package javax.sip.message;

import javax.sip.header.*;
import javax.sip.*;

/**
    A SIP message is either a request from a client to a server, or a response from a server to a
    client. Both Request and Response messages use the basic format of RFC 2822, even though the
    syntax differs in character set and syntax specifics. (SIP allows header fields that would not
    be valid RFC 2822 header fields, for example.) Both types of messages consist of a method name,
    address and protocol version, one or more header fields which describe the routing of the
    message, and an optional message-body. The message-body contains a session description in a
    format such as Session Description Protocol see JSR 141. <p> This interface contains common
    elements of both Request and Response such as: <ul> <li> Generic accessor functions to headers.
    <li> Convenience accessor to the expiration value of the message.  <li> Convenience header
    accessor methods for the body content type, language, disposition and length.  <li> Accessor
    methods to the body content itself. </ul> Although the SIP Protocol allows headers of a given
    kind to be interspaced with headers of different kinds, an implementation of this specification
    is required to organize headers so that headers that can appear multiple times in a SIP Message
    (such as the Via header) are grouped together and can be retrieved collectively and iterated
    over.  Although an implementation may use short forms internally, the specification refers to
    all headers by their long form names.
    @see Request
    @see Response
    @see Header
    @version 1.2.2
    @author Monavacon Limited
  */
public interface Message extends java.lang.Cloneable, java.io.Serializable {
    /**
        Adds the new Header to the existing list of Headers contained in this Message. The Header is
        added to the end of the List and will appear in that order in the SIP Message. <p> Required
        Headers that are singletons should not be added to the message as they already exist in the
        message and therefore should be changed using the setHeader(Header) method. <p> This method
        should be used to support the special case of adding required ViaHeaders to a message. When
        adding a ViaHeader using this method the implementation will add the ViaHeader to the top of
        the ViaHeader list, and not the end like all other Headers.
        @param header The new Header to be added to the existing Headers List.
      */
    void addHeader(Header header);
    /**
        Adds the new Header to the end of existing list of Headers contained in this Message. The
        Header is added to the end of the List and will appear in that order in the SIP Message. <p>
        Required Headers that are singletons should not be added to the message as they already
        exist in the message and therefore should be changed using the setHeader(Header) method.
        This does the same thing as addHeader in all cases including the Via header. Add a header
        explicitly to the end of a list of headers.
        @param header The new Header to be added to the end of the existing list of headers
        @exception java.lang.NullPointerException If the argument is null.
        @exception SipException If the header is a singleton and an instance of the header already
        exists.
        @since v1.2
      */
    void addLast(Header header)
        throws SipException, java.lang.NullPointerException;
    /**
        Adds the new Header to the head of the existing list of Headers contained in this Message.
        The Header is added to the head of the List and will appear in that order in the SIP
        Message. <p> Required Headers that are singletons should not be added to the message as they
        already exist in the message and therefore should be changed using the setHeader(Header)
        method.
        @param header The new Header to be added to the existing Headers List.
        @exception SipException If the header to be added is a singleton and an instance of the
        header already exists.
        @exception java.lang.NullPointerException If the argument is null.
        @since v1.2
      */
    void addFirst(Header header)
        throws SipException, java.lang.NullPointerException;
    /**
        Removes the first header from a list of headers. If there's only one header of this kind,
        then it is removed from the message.
        @param headerName The name of the header to be removed.
        @exception java.lang.NullPointerException If the arg is null
        @since v1.2
      */
    void removeFirst(java.lang.String headerName)
        throws java.lang.NullPointerException;
    /**
        Removes the last header from a list of headers. If there's only one header of this kind,
        then it is removed from the message.
        @param headerName The name of the header to be removed.
        @exception java.lang.NullPointerException
        @since v1.2
      */
    void removeLast(java.lang.String headerName)
        throws java.lang.NullPointerException;
    /**
        Removes the Header of the supplied name from the list of headers in this Message. If
        multiple headers exist then they are all removed from the header list. If no headers exist
        then this method returns silently. This method should not be used to remove required
        Headers, required Headers should be replaced using the setHeader(Header).
        @param headerName The new string value name of the Header to be removed.
      */
    void removeHeader(java.lang.String headerName);
    /**
        Gets a ListIterator over the set of all all the header names in this Message. Note that the
        order of the Header Names in the ListIterator is same as the order in which they appear in
        the SIP Message.
        @return The ListIterator over the set of all the Header Names in the Message.
      */
    java.util.ListIterator<java.lang.String> getHeaderNames();
    /**
        Gets a ListIterator over all the Headers of the newly specified name in this Message. Note
        that order of the Headers in ListIterator is the same as the order in which they appear in
        the SIP Message.
        @param headerName The new string name of Header types requested.
        @return The ListIterator over all the Headers of the specified name in the Message, this
        method returns an empty ListIterator if no Headers exist of this header type.
      */
    java.util.ListIterator<Header> getHeaders(java.lang.String headerName);
    /**
        Gets the Header of the specified name in this Message. If multiple Headers of this header
        name exist in the message, the first header in the message is returned.
        @param headerName The new string name of Header type requested.
        @return The Header of the specified name in the Message, this method returns null if the
        Header does not exist.
      */
    Header getHeader(java.lang.String headerName);
    /**
        Returns a ListIterator over all the UnrecognizedHeaders in this Message. Note the order of
        the UnrecognizedHeaders in the ListIterator is the same as order in which they appeared in
        the SIP Message. UnrecognizedHeaders are headers that the underlying implementation does not
        recognize. If the message is missing a required header (From, To, Call-ID, CSeq, Via) the
        entire message willl be dropped by the underlying implementation and the header will not be
        included in the list.  Headers that are part of the supported set of headers but are not
        properly formatted will be included in this list. Note that Headers that are not part of the
        supported set of headers are retrieved as Extension Headers. These must have a name:value
        format else they will be rejected by the underling implementation and included in this list.
        A Proxy should not delete UnrecognizedHeaders and should add these Headers to the end of the
        header list of the Message that is being forwarded.  A User Agent may display these
        unrecognized headers to the user.
        @return The ListIterator over all the UnrecognizedHeaders in the Message represented as
        Strings, this method returns an empty ListIterator if no UnrecognizedHeaders exist.
      */
    java.util.ListIterator<Header> getUnrecognizedHeaders();
    /**
        Sets the new Header to replace existings Header of that type in the message. If the SIP
        message contains more than one Header of the new Header type it should replace the first
        occurance of this Header and removes all other Headers of this type. If no Header of this
        type exists this header is added to the end of the SIP Message. This method should be used
        to change required Headers and overwrite optional Headers.
        @param header The new Header to replace any existing Headers of that type.
      */
    void setHeader(Header header);
    /**
        Set the ContentLengthHeader of this Message. The actual content length for the outgoing
        message will be computed from the content assigned. If the content is speficied as an object
        it will be converted to a java.lang.String before the message is sent out and the content length
        computed from the length of the string.  If the message content is specified in bytes, the
        length of the byte array specified will be used to determine the content length header, that
        is in both cases, the length of the content overrides any value specified in the
        content-length header.
        @param contentLength The new ContentLengthHeader object containing the content length value
        of this Message.
      */
    void setContentLength(ContentLengthHeader contentLength);
    /**
        Gets the ContentLengthHeader of the body content of this
        Message. This is the same as this.getHeader(Content-Length);
        @return The ContentLengthHeader of the message body.
      */
    ContentLengthHeader getContentLength();
    /**
        Sets the ContentLanguageHeader of this Message. This overrides the ContentLanguageHeader set
        using the setHeaders method. If no ContentLanguageHeader exists in this message this
        ContentLanguageHeader is added to the end of the Header List.
        @param contentLanguage The new ContentLanguageHeader object containing the content language
        value of this Message.
      */
    void setContentLanguage(ContentLanguageHeader contentLanguage);
    /**
        Gets the ContentLanguageHeader of this Message. This is the same as
        this.getHeader(Content-Langauge);
        @return The ContentLanguageHeader of the message body.
      */
    ContentLanguageHeader getContentLanguage();
    /**
        Sets the ContentEncodingHeader of this Message. This overrides the ContentEncodingHeader set
        using the setHeaders method. If no ContentEncodingHeader exists in this message this
        ContentEncodingHeader is added to the end of the Header List.
        @param contentEncoding The new ContentEncodingHeader object containing the content encoding
        values of this Message.
      */
    void setContentEncoding(ContentEncodingHeader contentEncoding);
    /**
        Gets the ContentEncodingHeader of this Message. This is the same as
        this.getHeader(Content-Encoding);
        @return The ContentEncodingHeader of the message body.
      */
    ContentEncodingHeader getContentEncoding();
    /**
        Sets the ContentDispositionHeader of this Message. This overrides the
        ContentDispositionHeader set using the setHeaders method. If no ContentDispositionHeader
        exists in this message this ContentDispositionHeader is added to the end of the Header List.
        @param contentDisposition The new ContentDispositionHeader object containing the content
        disposition value of this Message.
      */
    void setContentDisposition(ContentDispositionHeader contentDisposition);
    /**
        Gets the ContentDispositionHeader of this Message. This is the same as
        this.getHeader(Content-Disposition);
        @return The ContentDispositionHeader of the message body.
      */
    ContentDispositionHeader getContentDisposition();
    /**
        Sets the body of this Message, with the ContentType defined by the new ContentTypeHeader
        object and the string value of the content.
        @param content The new Object value of the content of the Message.
        @param contentTypeHeader The new ContentTypeHeader object that defines the content type
        value.
        @exception java.text.ParseException Thrown when error was found while parsing the body.
      */
    void setContent(java.lang.Object content, ContentTypeHeader contentTypeHeader)
        throws java.text.ParseException;
    /**
        Gets the body content of the Message as a byte array.
        @return The body content of the Message as a byte array, this method returns null if a body
        does not exist.
      */
    byte[] getRawContent();
    /**
        Gets the body content of the Message as an Object.
        @return The body content of the Message as an Object, this method returns null if a body
        does not exist.
      */
    java.lang.Object getContent();
    /**
        Removes the body content from this Message and all associated entity headers, if a body
        exists, this method returns sliently if no body exists.
      */
    void removeContent();
    /**
        Sets the ExpiresHeader of this Message. This overrides the ExpiresHeader set using the
        setHeaders method. If no ExpiresHeader exists in this message this ExpiresHeader is added to
        the end of the Header List.
        @param expires The new ExpiresHeader object containing the expires values of this Message.
      */
    void setExpires(ExpiresHeader expires);
    /**

        Gets the ExpiresHeader of this Message. This is the same as this.getHeader(Expires).
        @return The ExpiresHeader of the message body.
      */
    ExpiresHeader getExpires();
    /**
        Sets the protocol version of SIP being used by this Message.
        @param version The new java.lang.String object containing the version of the SIP Protocol of this
        Message.
        @exception java.text.ParseException Thrown when error was found while parsing the version
        argument.
      */
    void setSIPVersion(java.lang.String version)
        throws java.text.ParseException;
    /**
        Gets the protocol version of SIP being used by this Message.
        @return The protocol version of the SIP protocol of this message.
      */
    java.lang.String getSIPVersion();
    /**
        Creates and returns a deep copy of the Message. This methods must ensure a deep copy of the
        message, so that it can be modified without effecting the original message. This provides
        useful functionality for proxying Requests and Responses, for example: <ul> <li> Recieve a
        message.  <li> Create a deep clone of the message.  <li> Modify necessary headers.  <li>
        Proxy the message using the send methods on the SipProvider. </ul> The message contents are
        cloned as follows: <ul> <li> If the content is of type byte[] a new byte[] array is
        allocated and the original contents are copied over to the cloned Message.  <li> If the
        content is of type java.lang.String then a new java.lang.String equal to the old java.lang.String is allocated and
        assigned to the cloned Message.  <li> If the content is of type Object and it has a public
        clone method then it is invoked and the resultant Object is used in the new cloned Message.
        </ul>
        @return A deep copy of Message.
      */
    java.lang.Object clone();
    /**
        Compare this SIP Message for equality with another.  Implementations need only compare
        Request/Response line, From, To, CallID, MaxForwards, CSeq and Via headers for message
        equality.
        @param object The object to compare this Message with.
        @return True if obj is an instance of this class representing the same SIP Message as this
        (on the basis of comparing the headers above), false otherwise.
      */
    boolean equals(java.lang.Object object);
    /**
        Gets a integer hashcode representation of the Header. This method overrides the hashcode
        method in java.lang.Object. Only the Request/Response line and the required headers should
        be used to generate the unique hashcode of a message.
        @return Integer representation of the Message hashcode.
        @since v1.2
      */
    int hashCode();
    /**

          Gets string representation of Message

        Overrides:
                toString in class java.lang.Object

        @return
                java.lang.String representation of Message.
      */
    java.lang.String toString();
}


