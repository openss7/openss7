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
    This interface is the super interface of all SIP headers supported explicitly by this
    specification. Extension Headers can be supported by this specification as required by extending
    the ExtensionHeader assuming other endpoints understand the Header. This specification supports
    the following headers not defined in RFC3261 documented in the following standards track RFCs:
    <ul> <li> RAckHeader - this header is specific to the reliability of provisional Responses. This
    functionality is defined in RFC3262.  <li> RSeqHeader - this header is specific to the
    reliability of provisional Responses. This functionality is defined in RFC3262.  <li>
    AllowEventsHeader - this header is specific to the event notification framework. This
    functionality is defined in RFC3265.  <li> EventHeader - this header is specific to the event
    notification framework. This functionality is defined in RFC3265.  <li> SubscriptionStateHeader
    - this header is specific to the event notification framework. This functionality is defined in
    RFC3265.  <li> ReasonHeader - The Reason Header provides information on why a SIP request was
    issued, often useful when creating services and used to encapsulate a final status code in a
    provisional response. This functionality is defined in RFC3326. </ul> SIP header fields are
    similar to HTTP header fields in both syntax and semantics. Some header fields only make sense
    in requests or responses. These are called request header fields and response header fields,
    respectively. If a header field appears in a message not matching its category (such as a
    request header field in a response), it MUST be ignored. <p> <h5>Header Handling:</h5> Any SIP
    header whose grammar is of the form: <p> <code>header = "header-name" HCOLON header-value
    *(COMMA header-value)</code><p> allows for combining header fields of the same name into a
    comma-separated list. In this specification each Header object has a single value or attribute
    pair. For example a Header whose grammer is of the form: <p> <code>Allow: Invite, Bye;</code>
    <p> would be represented in a SIP message with two AllowHeader objects each containing a single
    attribute, Invite and Bye respectively.  Implementations MUST be able to parse multiple header
    field rows with the same name in any combination of the single-value-per-line or comma-separated
    value forms and translate them into the relevent Header objects defined in this specification.
    <p> The relative order of header objects within messages is not significant. However, it is
    RECOMMENDED that required header and headers which are needed for proxy processing (Via, Route,
    Record-Route, Proxy-Require, Max-Forwards, and Proxy-Authorization, for example) appear towards
    the top of the message to facilitate rapid parsing. <p> The relative order of header objects
    with the same field name is important. Multiple headers with the same name MAY be present in a
    message if and only if the entire field-value for that header field can be defined as a
    comma-separated list as defined by RFC 3261. The exceptions to this rule are the
    WWW-Authenticate, Authorization, Proxy-Authenticate, and Proxy-Authorization header fields.
    Multiple header objects with these names MAY be present in a message, but since their grammar
    does not follow the general form listed above, they MUST NOT be combined into a single header
    field row when sent over the network. <p> Even though an arbitrary number of parameter pairs may
    be attached to a header object, any given parameter-name MUST NOT appear more than once.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface Header extends java.lang.Cloneable, java.io.Serializable {
    /**
        Gets the unique string name of this Header. A name constant is defined in each individual
        Header identifying each Header.
        @return The name of this specific Header.
      */
    java.lang.String getName();
    /**
        Compare this SIP Header for equality with another. This method overrides the equals method
        in java.lang.Object. This method is over-ridden further for each required header (To, From,
        CSeq, Call-ID, Max-Forwards, and Via) which object equality is outlined as specified by
        RFC3261. All optional headers are compared using object equality that is each field in the
        header is used for comparision. When comparing header fields, field names are always
        case-insensitive. Unless otherwise stated in the definition of a particular header field,
        field values, parameter names, and parameter values are case-insensitive.  Tokens are always
        case-insensitive. Unless specified otherwise, values expressed as quoted strings are
        case-sensitive.
        @param obj The object to which to compare this Header.
        @return True if obj is an instance of this class representing the same SIP Header as this,
        false otherwise.
      */
    boolean equals(java.lang.Object obj);
    /**
        Creates and returns a deep copy of the Header. This methods must ensure a deep copy of the
        Header, so that when a message is cloned the Header can be modified without effecting the
        original Header in the message. This provides useful functionality for proxying Requests and
        Responses, for example: <ul> <li> Recieve a message.  <li> Create a deep clone of the
        message and headers.  <li> Modify necessary headers.  <li> Proxy the message using the send
        methods on the SipProvider. </ul> This method overrides the clone method in
        java.lang.Object.
        @return A deep copy of Header.
      */
    java.lang.Object clone();
    /**
        Gets a integer hashcode representation of the Header. This method overrides the hashcode
        method in java.lang.Object.
        @return Integer representation of Header hashcode.
        @since v1.2
      */
    int hashCode();
    /**
        Gets a string representation of the Header. This method overrides the toString method in
        java.lang.Object.
        @return java.lang.String representation of Header.
      */
    java.lang.String toString();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
