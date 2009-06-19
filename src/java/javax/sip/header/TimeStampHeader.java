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
    The Timestamp header field describes when the UAC sent the request to the UAS. When a 100
    (Trying) response is generated, any Timestamp header field present in the request MUST be copied
    into this 100 (Trying) response. If there is a delay in generating the response, the UAS SHOULD
    add a delay value into the Timestamp value in the response.  This value MUST contain the
    difference between the time of sending of the response and receipt of the request, measured in
    seconds. Although there is no normative behavior defined here that makes use of the header, it
    allows for extensions or SIP applications to obtain RTT estimates, that may be used to adjust
    the timeout value for retransmissions. <p> For Example: <br> <code> Timestamp: 54 </code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface TimeStampHeader extends Header {
    /**
        Name of TimeStampHeader.
      */
    static final java.lang.String NAME = "Timestamp";
    /**
        @deprecated This method is replaced with setTimeStamp(float).  Sets the timestamp value of
        this TimeStampHeader to the new timestamp value passed to this method.
        @param timeStamp The new float timestamp value.
        @exception InvalidArgumentException Thrown when the timestamp value argument is a negative
        value.
      */
    void setTimeStamp(float timeStamp) throws InvalidArgumentException;
    /**
        @deprecated This method is replaced with getTime().  Gets the timestamp value of this
        TimeStampHeader.
        @return The timestamp value of this TimeStampHeader.
      */
    float getTimeStamp();
    /**
        Gets the timestamp value of this TimeStampHeader.
        @return The timestamp value of this TimeStampHeader.
        @since v1.2
      */
    long getTime();
    /**
        Sets the timestamp value of this TimeStampHeader to the new timestamp value passed to this
        method. This method allows applications to conveniantly use System.currentTimeMillis to set
        the timeStamp value.
        @param timeStamp The new long timestamp value.
        @exception InvalidArgumentException Thrown when the timestamp value argument is a negative
        value.
        @since v1.2
      */
    void setTime(long timeStamp) throws InvalidArgumentException;
    /**
        @deprecated This method is replaced with getTimeDelay().  Gets delay of TimeStampHeader.
        This method returns -1 if the delay parameter is not set.
        @return The delay value of this TimeStampHeader
      */
    float getDelay();
    /**
        @deprecated This method is replaced with setTimeDelay(int).  Sets the new delay value of the
        TimestampHeader to the delay parameter passed to this method
        @param delay The new float delay value.
        @exception InvalidArgumentException Thrown when the delay value argumenmt is a negative
        value other than the default value -1.
      */
    void setDelay(float delay) throws InvalidArgumentException;
    /**
        Gets delay of TimeStampHeader. This method returns -1 if the delay parameter is not set.
        @return The delay value of this TimeStampHeader as an integer.
        @since v1.2
      */
    int getTimeDelay();
    /**
        Sets the new delay value of the TimestampHeader to the delay parameter passed to this method
        @param delay The new int delay value.
        @exception InvalidArgumentException Thrown when the delay value argumenmt is a negative
        value other than the default value -1.
        @since v1.2
      */
    void setTimeDelay(int delay) throws InvalidArgumentException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
