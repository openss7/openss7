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
    The Content-Disposition header field describes how the message body or, for multipart messages,
    a message body part is to be interpreted by the UAC or UAS. This SIP header field extends the
    MIME Content-Type. Several new "disposition-types" of the Content-Disposition header are defined
    by SIP, namely:- <ul> <li>session - indicates that the body part describes a session, for either
    calls or early (pre-call) media.  <li>render - indicates that the body part should be displayed
    or otherwise rendered to the user.  <li>icon - indicates that the body part contains an image
    suitable as an iconic representation of the caller or callee that could be rendered
    informationally by a user agent when a message has been received, or persistently while a dialog
    takes place.  <li>alert - indicates that the body part contains information, such as an audio
    clip, that should be rendered by the user agent in an attempt to alert the user to the receipt
    of a request, generally a request that initiates a dialog.</ul> For backward-compatibility, if
    the Content-Disposition header field is missing, the server SHOULD assume bodies of Content-Type
    application/sdp are the disposition "session", while other content types are "render". <p> If
    this header field is missing, the MIME type determines the default content disposition. If there
    is none, "render" is assumed. <p> <dt>For Example:<br><code>Content-Disposition: session</code>
    @see ContentTypeHeader
    @see ContentLengthHeader
    @see ContentEncodingHeader
    @see ContentLanguageHeader
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ContentDispositionHeader extends Parameters, Header {
    /**
        Name of ContentDispositionHeader.
      */
    static final java.lang.String NAME = "Content-Disposition";
    /**
        Session Disposition Type Constant.
      */
    static final java.lang.String SESSION = "session";
    /**
        Render Disposition Type Constant.
      */
    static final java.lang.String RENDER = "render";
    /**
        Icon Disposition Type Constant.
      */
    static final java.lang.String ICON = "icon";
    /**
        Alert Disposition Type Constant.
      */
    static final java.lang.String ALERT = "alert";
    /**
        Sets the interpretation value of the message body or message body part for this
        ContentDispositionHeader.
        @param dispositionType The new java.lang.String value of the disposition type.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        dispositionType parameter.
      */
    void setDispositionType(java.lang.String dispositionType)
        throws java.text.ParseException;
    /**
        Gets the interpretation of the message body or message body part of this
        ContentDispositionHeader.
        @return Interpretation of the message body or message body part.
      */
    java.lang.String getDispositionType();
    /**
        The handling parameter describes how the UAS should react if it receives a message body
        whose content type or disposition type it does not understand. The parameter has defined
        values of "optional" and "required". If the handling parameter is missing, the value
        "required" SHOULD be assumed.
        @param handling The new java.lang.String value either "optional" or "required".
        @exception java.text.ParseException Thrown when an error was found while parsing the
        handling parameter.
      */
    void setHandling(java.lang.String handling)
        throws java.text.ParseException;
    /**
        Gets the handling information of the unknown content disposition of the
        ContentDispositionHeader.
        @return Handling information for unknown content dispositions.
      */
    java.lang.String getHandling();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
