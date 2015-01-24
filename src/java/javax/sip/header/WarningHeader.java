/*
 @(#) src/java/javax/sip/header/WarningHeader.java <p>
 
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
    The Warning header field is used to carry additional information about the status of a response.
    Warning header field values are sent with responses and contain a three-digit warning code,
    agent name, and warning text. <ul> <li> Warning Text: The "warn-text" should be in a natural
    language that is most likely to be intelligible to the human user receiving the response. This
    decision can be based on any available knowledge, such as the location of the user, the
    Accept-Language field in a request, or the Content-Language field in a response.  <li> Warning
    Code: The currently-defined "warn-code"s have a recommended warn-text in English and a
    description of their meaning. These warnings describe failures induced by the session
    description. The first digit of warning codes beginning with "3" indicates warnings specific to
    SIP. Warnings 300 through 329 are reserved for indicating problems with keywords in the session
    description, 330 through 339 are warnings related to basic network services requested in the
    session description, 370 through 379 are warnings related to quantitative QoS parameters
    requested in the session description, and 390 through 399 are miscellaneous warnings that do not
    fall into one of the above categories.  Additional "warn-code"s can be defined. </ul> Any server
    may add WarningHeaders to a Response. Proxy servers must place additional WarningHeaders before
    any AuthorizationHeaders.  Within that constraint, WarningHeaders must be added after any
    existing WarningHeaders not covered by a signature. A proxy server must not delete any
    WarningHeader that it received with a Response. <p> When multiple WarningHeaders are attached to
    a Response, the user agent should display as many of them as possible, in the order that they
    appear in the Response. If it is not possible to display all of the warnings, the user agent
    first displays warnings that appear early in the Response. <p> Examples of using Warning Headers
    are as follows: <ul> <li> A UAS rejecting an offer contained in an INVITE SHOULD return a 488
    (Not Acceptable Here) response. Such a response SHOULD include a Warning header field value
    explaining why the offer was rejected.  <li> If the new session description is not acceptable,
    the UAS can reject it by returning a 488 (Not Acceptable Here) response for the re-INVITE. This
    response SHOULD include a Warning header field.  <li> A 606 (Not Acceptable) response means that
    the user wishes to communicate, but cannot adequately support the session described.  The 606
    (Not Acceptable) response MAY contain a list of reasons in a Warning header field describing why
    the session described cannot be supported.  <li> Contact header fields MAY be present in a 200
    (OK) response and have the same semantics as in a 3xx response. That is, they may list a set of
    alternative names and methods of reaching the user.  A Warning header field MAY be present.
    </ul> For Example: <br> <code> Warning: 307 isi.edu "Session parameter 'foo' not understood"
    </code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface WarningHeader extends Header {
    /**
        Name of WarningHeader.
      */
    static final java.lang.String NAME = "Warning";
    /**
        One or more network protocols contained in the session description are not available.
      */
    static final int INCOMPATIBLE_NETWORK_PROTOCOL = 340;
    /**
        One or more network address formats contained in the session description are not available.
      */
    static final int INCOMPATIBLE_NETWORK_ADDRESS_FORMATS = 341;
    /**
        One or more transport protocols described in the session description are not available.
      */
    static final int INCOMPATIBLE_TRANSPORT_PROTOCOL = 342;
    /**
        One or more bandwidth measurement units contained in the session description were not
        understood.
      */
    static final int INCOMPATIBLE_BANDWIDTH_UNITS = 343;
    /**
        One or more media types contained in the session description are not available.
      */
    static final int MEDIA_TYPE_NOT_AVAILABLE = 344;
    /**
        One or more media formats contained in the session description are not available.
      */
    static final int INCOMPATIBLE_MEDIA_FORMAT = 345;
    /**
        One or more of the media attributes in the session description are not supported.
      */
    static final int ATTRIBUTE_NOT_UNDERSTOOD = 346;
    /**
        A parameter other than those listed above was not understood.
      */
    static final int SESSION_DESCRIPTION_PARAMETER_NOT_UNDERSTOOD = 347;
    /**
        The site where the user is located does not support multicast.
      */
    static final int MULTICAST_NOT_AVAILABLE = 348;
    /**
        The site where the user is located does not support unicast communication (usually due to
        the presence of a firewall).
      */
    static final int UNICAST_NOT_AVAILABLE = 349;
    /**
        The bandwidth specified in the session description or defined by the media exceeds that
        known to be available.
      */
    static final int INSUFFICIENT_BANDWIDTH = 350;
    /**
        The warning text can include arbitrary information to be presented to a human user, or
        logged. A system receiving this warning MUST NOT take any automated action.
      */
    static final int MISCELLANEOUS_WARNING = 351;
    /**
        Gets the agent of the server that created this WarningHeader.
        @return The agent of the WarningHeader.
      */
    java.lang.String getAgent();
    /**
        Sets the agent value of the WarningHeader to the new value passed to the method.
        @param agent The new agent value of WarningHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the agent
        value.
      */
    void setAgent(java.lang.String agent) throws java.text.ParseException;
    /**
        Gets text of WarningHeader.
        @return The string text value of the WarningHeader.
      */
    java.lang.String getText();
    /**
        Sets the text of WarningHeader to the newly supplied text value.
        @param text The new text value of the Warning Header.
        @exception java.text.ParseException Thrown when an error was found while parsing the text
        value.
      */
    void setText(java.lang.String text) throws java.text.ParseException;
    /**
        Sets the code of the WarningHeader. The standard RFC3261 codes are defined as constants in
        this class.
        @param code The new code that defines the warning code.
        @exception InvalidArgumentException Thrown when an invalid integer code is given for the
        WarningHeader.
      */
    void setCode(int code) throws InvalidArgumentException;
    /**
        Gets the code of the WarningHeader.
        @return The integer code value of the WarningHeader.
      */
    int getCode();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
