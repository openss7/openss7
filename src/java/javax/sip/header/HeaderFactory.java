/*
 @(#) $RCSfile: HeaderFactory.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:53 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:53 $ by $Author: brian $
 */

package javax.sip.header;

import javax.sip.address.*;
import javax.sip.*;

/**
    This interface provides factory methods that allow an application to create Header object from a
    particular implementation of JAIN SIP.  This class is a singleton and can be retrieved from the
    SipFactory.createHeaderFactory().
    @version 1.2.2
    @author Monavacon Limited
  */
public interface HeaderFactory {
    /**
        Creates a new AcceptEncodingHeader based on the newly supplied encoding value.
        @param encoding The new string containing the encoding value.
        @return The newly created AcceptEncodingHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        encoding value.
      */
    AcceptEncodingHeader createAcceptEncodingHeader(java.lang.String encoding)
        throws java.text.ParseException;
    /**
        Creates a new AcceptHeader based on the newly supplied contentType and contentSubType
        values.
        @param contentType The new string content type value.
        @param contentSubType The new string content sub-type value.
        @return The newly created AcceptHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the content
        type or content subtype value.
      */
    AcceptHeader createAcceptHeader(java.lang.String contentType, java.lang.String contentSubType)
        throws java.text.ParseException;
    /**
        Creates a new AcceptLanguageHeader based on the newly supplied language value.
        @param language The new Locale value of the language.
        @return The newly created AcceptLanguageHeader object.
      */
    AcceptLanguageHeader createAcceptLanguageHeader(java.util.Locale language);
    /**
        Creates a new AlertInfoHeader based on the newly supplied alertInfo value.
        @param alertInfo The new URI value of the alertInfo.
        @return The newly created AlertInfoHeader object.
      */
    AlertInfoHeader createAlertInfoHeader(URI alertInfo);
    /**
        Creates a new AllowEventsHeader based on the newly supplied event type value.
        @param eventType The new string containing the eventType value.
        @return The newly created AllowEventsHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        eventType value.
      */
    AllowEventsHeader createAllowEventsHeader(java.lang.String eventType)
        throws java.text.ParseException;
    /**
        Creates a new AllowHeader based on the newly supplied method value.
        @param method The new string containing the method value.
        @return The newly created AllowHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        value.
      */
    AllowHeader createAllowHeader(java.lang.String method)
        throws java.text.ParseException;
    /**
        Creates a new AuthenticationInfoHeader based on the newly supplied response value.
        @param response The new string value of the response.
        @return The newly created AuthenticationInfoHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        response value.
      */
    AuthenticationInfoHeader createAuthenticationInfoHeader(java.lang.String response)
        throws java.text.ParseException;
    /**
        Creates a new AuthorizationHeader based on the newly supplied scheme value.
        @param scheme The new string value of the scheme.
        @return The newly created AuthorizationHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the scheme value.
      */
    AuthorizationHeader createAuthorizationHeader(java.lang.String scheme)
        throws java.text.ParseException;
    /**
        @deprecated Since 1.2. Use createCSeqHeader(long, java.lang.String) method with type long. <p> Creates
        a new CSeqHeader based on the newly supplied sequence number and method values.
        @param sequenceNumber The new integer value of the sequence number.
        @param method The new string value of the method.
        @return The newly created CSeqHeader object.
        @exception InvalidArgumentException Thrown when supplied sequence number is less than zero.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        value.
      */
    CSeqHeader createCSeqHeader(int sequenceNumber, java.lang.String method)
        throws java.text.ParseException, InvalidArgumentException;
    /**
        Creates a new CSeqHeader based on the newly supplied sequence number and method values.
        @param sequenceNumber The new long value of the sequence number.
        @param method The new string value of the method.
        @return The newly created CSeqHeader object.
        @exception InvalidArgumentException Thrown when supplied sequence number is less than zero.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        value.
        @since v1.2
      */
    CSeqHeader createCSeqHeader(long sequenceNumber, java.lang.String method)
        throws java.text.ParseException, InvalidArgumentException;
    /**
        Creates a new CallIdHeader based on the newly supplied callId value.
        @param callId The new string value of the call-id.
        @return The newly created CallIdHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the callId
        value.
      */
    CallIdHeader createCallIdHeader(java.lang.String callId)
        throws java.text.ParseException;
    /**
        Creates a new CallInfoHeader based on the newly supplied callInfo value.
        @param callInfo The new URI value of the callInfo.
        @return The newly created CallInfoHeader object.
      */
    CallInfoHeader createCallInfoHeader(URI callInfo);
    /**
        Creates a new ContactHeader based on the newly supplied address value.
        @param address The new Address value of the address.
        @return The newly created ContactHeader object.
      */
    ContactHeader createContactHeader(Address address);
    /**
        Creates a new wildcard ContactHeader. This is used in Register requests to indicate to the
        server that it should remove all locations the at which the user is currently available.
        This implies that the following conditions are met: <ul> <li>
        ContactHeader.getAddress.getUserInfo() == *; <li> ContactHeader.getAddress.isWildCard() ==
        true; <li> ContactHeader.getExpires() == 0; </ul>
        @return The newly created wildcard ContactHeader.
      */
    ContactHeader createContactHeader();
    /**
        Creates a new ContentDispositionHeader based on the newly
        supplied contentDisposition value.
        @param contentDispositionType The new string value of the contentDisposition.
        @return The newly created ContentDispositionHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        contentDisposition value.
      */
    ContentDispositionHeader createContentDispositionHeader(java.lang.String contentDispositionType)
        throws java.text.ParseException;
    /**
        Creates a new ContentEncodingHeader based on the newly supplied encoding value.
        @param encoding The new string containing the encoding value.
        @return The newly created ContentEncodingHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        encoding value.
      */
    ContentEncodingHeader createContentEncodingHeader(java.lang.String encoding)
        throws java.text.ParseException;
    /**
        Creates a new ContentLanguageHeader based on the newly supplied contentLanguage value.
        @param contentLanguage The new Locale value of the contentLanguage.
        @return The newly created ContentLanguageHeader object.
      */
    ContentLanguageHeader createContentLanguageHeader(java.util.Locale contentLanguage);
    /**
        Creates a new ContentLengthHeader based on the newly supplied contentLength value.
        @param contentLength The new integer value of the contentLength.
        @return The newly created ContentLengthHeader object.
        @exception InvalidArgumentException Thrown when supplied contentLength is less than zero.
      */
    ContentLengthHeader createContentLengthHeader(int contentLength)
        throws InvalidArgumentException;
    /**
        Creates a new ContentTypeHeader based on the newly supplied contentType and contentSubType
        values.
        @param contentType The new string content type value.
        @param contentSubType The new string content sub-type value.
        @return The newly created ContentTypeHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the content
        type or content subtype value.
      */
    ContentTypeHeader createContentTypeHeader(java.lang.String contentType, java.lang.String contentSubType)
        throws java.text.ParseException;
    /**
        Creates a new DateHeader based on the newly supplied date value.
        @param date The new Calender value of the date.
        @return The newly created DateHeader object.
      */
    DateHeader createDateHeader(java.util.Calendar date);
    /**
        Creates a new ErrorInfoHeader based on the newly supplied errorInfo value.
        @param errorInfo The new URI value of the errorInfo.
        @return The newly created ErrorInfoHeader object.
      */
    ErrorInfoHeader createErrorInfoHeader(URI errorInfo);
    /**
        Creates a new EventHeader based on the newly supplied eventType value.
        @param eventType The new string value of the eventType.
        @return The newly created EventHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        eventType value.
      */
    EventHeader createEventHeader(java.lang.String eventType)
        throws java.text.ParseException;
    /**
        Creates a new ExpiresHeader based on the newly supplied expires value.
        @param expires The new integer value of the expires.
        @return The newly created ExpiresHeader object.
        @exception InvalidArgumentException Thrown when supplied expires is less than zero.
      */
    ExpiresHeader createExpiresHeader(int expires)
        throws InvalidArgumentException;
    /**
        Creates a new Header based on the newly supplied name and value values. This method can be
        used to create ExtensionHeaders.
        @param name The new string name of the Header value.
        @param value The new string value of the Header.
        @return The newly created Header object.
        @exception java.text.ParseException Thrown when an error was found while parsing the name or
        value values.
        @see ExtensionHeader
      */
    Header createHeader(java.lang.String name, java.lang.String value)
        throws java.text.ParseException;
    /**
        Creates a new List of Headers based on a supplied comma seperated list of Header values for
        a single header name. This method can be used only for SIP headers whose grammar is of the
        form header = header-name HCOLON header-value *(COMMA header-value) that allows for
        combining header fields of the same name into a comma-separated list. Note that the Contact
        header field allows a comma-separated list unless the header field value is "*"
        @param headers The new string comma seperated list of Header values.
        @return The newly created List of Header objects.
        @exception java.text.ParseException Thrown when an error was found while parsing the headers
        value or a List of that Header type is not allowed.
      */
    java.util.List<Header> createHeaders(java.lang.String headers)
        throws java.text.ParseException;
    /**
        Creates a new FromHeader based on the newly supplied address and tag values.
        @param address The new Address object of the address.
        @param tag The new string value of the tag.
        @return The newly created FromHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the tag
        value.
      */
    FromHeader createFromHeader(Address address, java.lang.String tag)
        throws java.text.ParseException;
    /**
        Creates a new InReplyToHeader based on the newly supplied callId value.
        @param callId The new string containing the callId value.
        @return The newly created InReplyToHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the callId
        value.
      */
    InReplyToHeader createInReplyToHeader(java.lang.String callId)
        throws java.text.ParseException;
    /**
        Creates a new MaxForwardsHeader based on the newly supplied maxForwards value.
        @param maxForwards The new integer value of the maxForwards.
        @return The newly created MaxForwardsHeader object.
        @exception InvalidArgumentException Thrown when supplied maxForwards is less than zero or
        greater than 255.
      */
    MaxForwardsHeader createMaxForwardsHeader(int maxForwards)
        throws InvalidArgumentException;
    /**
        Creates a new MimeVersionHeader based on the newly supplied mimeVersion values.
        @param majorVersion The new integer value of the majorVersion.
        @param minorVersion The new integer value of the minorVersion.
        @return The newly created MimeVersionHeader object.
        @exception InvalidArgumentException Thrown when supplied majorVersion or minorVersion is
        less than zero.
      */
    MimeVersionHeader createMimeVersionHeader(int majorVersion, int minorVersion)
        throws InvalidArgumentException;
    /**
        Creates a new MinExpiresHeader based on the newly supplied minExpires value.
        @param minExpires The new integer value of the minExpires.
        @return The newly created MinExpiresHeader object.
        @exception InvalidArgumentException Thrown when supplied minExpires is less than zero.
      */
    MinExpiresHeader createMinExpiresHeader(int minExpires)
        throws InvalidArgumentException;
    /**
        Creates a new OrganizationHeader based on the newly supplied organization value.
        @param organization The new string value of the organization.
        @return The newly created OrganizationHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        organization value.
      */
    OrganizationHeader createOrganizationHeader(java.lang.String organization)
        throws java.text.ParseException;
    /**
        Creates a new PriorityHeader based on the newly supplied priority value.
        @param priority The new string value of the priority.
        @return The newly created PriorityHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        priority value.
      */
    PriorityHeader createPriorityHeader(java.lang.String priority)
        throws java.text.ParseException;
    /**
        Creates a new ProxyAuthenticateHeader based on the newly supplied scheme value.
        @param scheme The new string value of the scheme.
        @return The newly created ProxyAuthenticateHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the scheme
        value.
      */
    ProxyAuthenticateHeader createProxyAuthenticateHeader(java.lang.String scheme)
        throws java.text.ParseException;
    /**
        Creates a new ProxyAuthorizationHeader based on the newly supplied scheme value.
        @param scheme The new string value of the scheme.
        @return The newly created ProxyAuthorizationHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the scheme
        value.
      */
    ProxyAuthorizationHeader createProxyAuthorizationHeader(java.lang.String scheme)
        throws java.text.ParseException;
    /**
        Creates a new ProxyRequireHeader based on the newly supplied optionTag value.
        @param optionTag The new string OptionTag value.
        @return The newly created ProxyRequireHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        optionTag value.
      */
    ProxyRequireHeader createProxyRequireHeader(java.lang.String optionTag)
        throws java.text.ParseException;
    /**
        Creates a new RAckHeader based on the newly supplied rSeqNumber, cSeqNumber and method
        values.
        @param rSeqNumber The new integer value of the rSeqNumber.
        @param cSeqNumber The new integer value of the cSeqNumber.
        @param method The new string value of the method.
        @return The newly created RAckHeader object.
        @exception InvalidArgumentException Thrown when supplied rSeqNumber or cSeqNumber is less
        than zero or greater than than 2**31-1.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        value.
      */
    RAckHeader createRAckHeader(int rSeqNumber, int cSeqNumber, java.lang.String method)
        throws InvalidArgumentException, java.text.ParseException;
    /**
        Creates a new RSeqHeader based on the newly supplied sequenceNumber value.
        @param sequenceNumber The new integer value of the sequenceNumber.
        @return The newly created RSeqHeader object.
        @exception InvalidArgumentException Thrown when supplied sequenceNumber is less than zero or
        greater than than 2**31-1.
      */
    RSeqHeader createRSeqHeader(int sequenceNumber)
        throws InvalidArgumentException;
    /**
        Creates a new ReasonHeader based on the newly supplied reason value.
        @param protocol The new string value of the protocol.
        @param cause The new integer value of the cause.
        @param text The new string value of the text.
        @return The newly created ReasonHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        protocol or text value.
        @exception InvalidArgumentException Thrown when supplied cause is less than zero.
      */
    ReasonHeader createReasonHeader(java.lang.String protocol, int cause, java.lang.String text)
        throws InvalidArgumentException, java.text.ParseException;
    /**
        Creates a new RecordRouteHeader based on the newly supplied address value.
        @param address The new Address object of the address.
        @return The newly created RecordRouteHeader object.
      */
    RecordRouteHeader createRecordRouteHeader(Address address);
    /**
        Creates a new ReplyToHeader based on the newly supplied address value.
        @param address The new Address object of the address.
        @return The newly created ReplyToHeader object.
      */
    ReplyToHeader createReplyToHeader(Address address);
    /**
        Creates a new ReferToHeader based on the newly supplied address value.
        @param address The new Address object of the address.
        @return The newly created ReferToHeader object.
      */
    ReferToHeader createReferToHeader(Address address);
    /**
        Creates a new RequireHeader based on the newly supplied optionTag value.
        @param optionTag The new string value containing the optionTag value.
        @return The newly created RequireHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the List of
        optionTag value.
      */
    RequireHeader createRequireHeader(java.lang.String optionTag)
        throws java.text.ParseException;
    /**
        Creates a new RetryAfterHeader based on the newly supplied retryAfter value.
        @param retryAfter The new integer value of the retryAfter.
        @return The newly created RetryAfterHeader object.
        @exception InvalidArgumentException Thrown when supplied retryAfter is less than zero.
      */
    RetryAfterHeader createRetryAfterHeader(int retryAfter)
        throws InvalidArgumentException;
    /**
        Creates a new RouteHeader based on the newly supplied address value.
        @param address The new Address object of the address.
        @return The newly created RouteHeader object.
      */
    RouteHeader createRouteHeader(Address address);
    /**
        Creates a new ServerHeader based on the newly supplied List of product values.
        @param product The new List of values of the product.
        @return The newly created ServerHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the List of
        product values.
      */
    ServerHeader createServerHeader(java.util.List<java.lang.String> product)
        throws java.text.ParseException;
    /**
        Creates a new SubjectHeader based on the newly supplied subject value.
        @param subject The new string value of the subject.
        @return The newly created SubjectHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the subject
        value.
      */
    SubjectHeader createSubjectHeader(java.lang.String subject)
        throws java.text.ParseException;
    /**
        Creates a new SubscriptionStateHeader based on the newly supplied subscriptionState value.
        @param subscriptionState The new string value of the subscriptionState.
        @return The newly created SubscriptionStateHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        subscriptionState value.
      */
    SubscriptionStateHeader createSubscriptionStateHeader(java.lang.String subscriptionState)
        throws java.text.ParseException;
    /**
        Creates a new SupportedHeader based on the newly supplied optionTag value.
        @param optionTag The new string containing the optionTag value.
        @return The newly created SupportedHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        optionTag value.
      */
    SupportedHeader createSupportedHeader(java.lang.String optionTag)
        throws java.text.ParseException;
    /**
        Creates a new TimeStampHeader based on the newly supplied timeStamp value.
        @param timeStamp The new float value of the timeStamp.
        @return The newly created TimeStampHeader object.
        @exception InvalidArgumentException Thrown when supplied timeStamp is less than zero.
      */
    TimeStampHeader createTimeStampHeader(float timeStamp)
        throws InvalidArgumentException;
    /**
        Creates a new ToHeader based on the newly supplied address and tag values.
        @param address The new Address object of the address.
        @param tag The new string value of the tag, this value may be null.
        @return The newly created ToHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the tag
        value.
      */
    ToHeader createToHeader(Address address, java.lang.String tag)
        throws java.text.ParseException;
    /**
        Creates a new UnsupportedHeader based on the newly supplied optionTag value.
        @param optionTag The new string containing the optionTag value.
        @return The newly created UnsupportedHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the List of
        optionTag value.
      */
    UnsupportedHeader createUnsupportedHeader(java.lang.String optionTag)
        throws java.text.ParseException;
    /**
        Creates a new UserAgentHeader based on the newly supplied List of product values.
        @param product The new List of values of the product.
        @return The newly created UserAgentHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the List of
        product values.
      */
    UserAgentHeader createUserAgentHeader(java.util.List<java.lang.String> product)
        throws java.text.ParseException;
    /**
        Creates a new ViaHeader based on the newly supplied uri and branch values.
        @param host The new string value of the host.
        @param port The new integer value of the port.
        @param transport The new string value of the transport.
        @param branch The new string value of the branch.
        @return The newly created ViaHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the host,
        transport or branch value.
        @exception InvalidArgumentException Thrown when the supplied port is invalid.
      */
    ViaHeader createViaHeader(java.lang.String host, int port, java.lang.String transport, java.lang.String branch)
        throws java.text.ParseException, InvalidArgumentException;
    /**
        Creates a new WWWAuthenticateHeader based on the newly supplied scheme value.
        @param scheme The new string value of the scheme.
        @return The newly created WWWAuthenticateHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the scheme
        values.
      */
    WWWAuthenticateHeader createWWWAuthenticateHeader(java.lang.String scheme)
        throws java.text.ParseException;
    /**
        Creates a new WarningHeader based on the newly supplied agent, code and comment values.
        @param agent The new string value of the agent.
        @param code The new boolean integer of the code.
        @param comment The new string value of the comment.
        @return The newly created WarningHeader object.
        @exception java.text.ParseException Thrown when an error was found while parsing the agent
        or comment values.
        @exception InvalidArgumentException Thrown when an invalid integer code is given for the
        WarningHeader.
      */
    WarningHeader createWarningHeader(java.lang.String agent, int code, java.lang.String comment)
        throws InvalidArgumentException, java.text.ParseException;
    /**
        Creates a new SIP-ETag header with the supplied tag value
        @param etag The new tag token
        @return The newly created SIP-ETag header
        @exception java.text.ParseException When an error occurs during parsing of the etag
        parameter
        @since 1.2
      */
    SIPETagHeader createSIPETagHeader(java.lang.String etag)
        throws java.text.ParseException;
    /**
        Creates a new SIP-If-Match header with the supplied tag value
        @param etag The new tag token
        @return The newly created SIP-If-Match header
        @exception java.text.ParseException When an error occurs during parsing of the etag
        parameter
        @since 1.2
      */
    SIPIfMatchHeader createSIPIfMatchHeader(java.lang.String etag)
        throws java.text.ParseException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
