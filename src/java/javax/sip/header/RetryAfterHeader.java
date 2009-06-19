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
    The Retry-After header field identifies the time to retry the request after recipt of the
    response. It can be used with a 500 (Server Internal Error) or 503 (Service Unavailable)
    response to indicate how long the service is expected to be unavailable to the requesting client
    and with a 404 (Not Found), 413 (Request Entity Too Large), 480 (Temporarily Unavailable), 486
    (Busy Here), 600 (Busy), or 603 (Decline) response to indicate when the called party anticipates
    being available again. The value of this field is a positive integer number of seconds (in
    decimal) after the time of the response. <p> An optional comment can be used to indicate
    additional information about the time of callback. An optional "duration" parameter indicates
    how long the called party will be reachable starting at the initial time of availability. If no
    duration parameter is given, the service is assumed to be available indefinitely. <p> For
    Examples: <br> <code> Retry-After: 18000;duration=3600 <br> Retry-After: 120 (I'm in a meeting)
    </code>
    @see Parameters
    @see Header
    @version 1.2.2
    @author Monavacon Limited
  */
public interface RetryAfterHeader extends Header, Parameters {
    /**
        Name of RetryAfterHeader.
      */
    static final java.lang.String NAME = "Retry-After";
    /**
        Sets the retry after value of the RetryAfterHeader. The retry after value MUST be greater
        than zero and MUST be less than 2**31.
        @param retryAfter The new retry after value of this RetryAfterHeader.
        @exception InvalidArgumentException Thron when the supplied value is less than zero.
      */
    void setRetryAfter(int retryAfter) throws InvalidArgumentException;
    /**
        Gets the retry after value of the RetryAfterHeader. This retry after value is relative time.
        @return The retry after value of the RetryAfterHeader.
      */
    int getRetryAfter();
    /**
        Gets the comment of RetryAfterHeader.
        @return The comment of this RetryAfterHeader, return null if no comment is available.
      */
    java.lang.String getComment();
    /**
        Sets the comment value of the RetryAfterHeader.
        @param comment The new comment string value of the RetryAfterHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        comment.
      */
    void setComment(java.lang.String comment) throws java.text.ParseException;
    /**
        Sets the duration value of the RetryAfterHeader. The retry after value MUST be greater than
        zero and MUST be less than 2**31.
        @param duration The new duration value of this RetryAfterHeader.
        @exception InvalidArgumentException Thrown when supplied value is less than zero.
      */
    void setDuration(int duration) throws InvalidArgumentException;
    /**
        Gets the duration value of the RetryAfterHeader. This duration value is relative time.
        @return The duration value of the RetryAfterHeader, return zero if not set.
      */
    int getDuration();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
