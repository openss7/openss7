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
    A CSeq header field in a request contains a single decimal sequence number and the request
    method. The CSeq header field serves to identify and order transactions within a dialog, to
    provide a means to uniquely identify transactions, and to differentiate between new requests and
    request retransmissions. Two CSeq header fields are considered equal if the sequence number and
    the request method are identical. <ul> <li>Method - The method part of CSeq is case-sensitive
    and MUST match that of the request.  <li>Sequence number - The sequence number is chosen by the
    requesting client and is unique within a single value of Call-ID. The sequence number MUST be
    expressible as a 32-bit unsigned integer and MUST be less than 2**31. For non-REGISTER requests
    outside of a dialog, the sequence number value is arbitrary. Consecutive Requests that differ in
    method, headers or body, but have the same CallIdHeader must contain strictly monotonically
    increasing and contiguous sequence numbers; sequence numbers do not wrap around.
    Retransmissions of the same Request carry the same sequence number, but an INVITE Request with a
    different message body or different headers (a "re-invitation") acquires a new, higher sequence
    number. A server must echo the CSeqHeader from the Request in its Response. If the method value
    is missing in the received CSeqHeader, the server fills it in appropriately. ACK and CANCEL
    Requests must contain the same CSeqHeader sequence number (but not method) as the INVITE Request
    they refer to, while a BYE Request cancelling an invitation must have a higher sequence number.
    An user agent server must remember the highest sequence number for any INVITE Request with the
    same CallIdHeader. The server must respond to, and then discard, any INVITE Request with a lower
    sequence number.</ul> As long as a client follows the above guidelines, it may use any mechanism
    it would like to select CSeq header field values. <p> <h5>Forked Requests:</h5> Forked Requests
    must have the same CSeqHeader as there would be ambiguity otherwise between these forked
    Requests and later BYE Requests issued by the client user agent.<p> For Example:<br><code> CSeq:
    4711 INVITE</code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface CSeqHeader extends Header {
    /**
        Name of the CSeqHeader.
      */
    static final java.lang.String NAME = "CSeq";
    /**
        Sets the method of CSeqHeader.
        @param method The method of the Request of this CSeqHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        value.
      */
    void setMethod(java.lang.String method)
        throws java.text.ParseException;
    /**
        Gets the method of CSeqHeader
        @return Method of CSeqHeader.
      */
    java.lang.String getMethod();
    /**
        @deprecated This method is replaced with setSeqNumber(long) with type long. <p> Sets the
        sequence number value of the CSeqHeader. The sequence number MUST be expressible as a 32-bit
        unsigned integer and MUST be less than 2**32 -1
        @param sequenceNumber The new sequence number of this CSeqHeader.
        @exception InvalidArgumentException Thrown when supplied value is less than zero.
        @since v1.1
      */
    void setSequenceNumber(int sequenceNumber) throws InvalidArgumentException;
    /**
        @deprecated This method is replaced with getSeqNumber() with type long. <p> Gets the
        sequence number of this CSeqHeader.
        @return Sequence number of the CSeqHeader.
        @since v1.1
      */
    int getSequenceNumber();
    /**
        Sets the sequence number value of the CSeqHeader.
        @param sequenceNumber The new sequence number of this CSeqHeader.
        @exception InvalidArgumentException Thrown when supplied value is less than zero.
        @since v1.2
      */
    void setSeqNumber(long sequenceNumber) throws InvalidArgumentException;
    /**
        Gets the sequence number of this CSeqHeader.
        @return Sequence number of the CSeqHeader.
        @since v1.2
      */
    long getSeqNumber();
    /**
        Compare this CSeqHeader for equality with another. This method overrides the equals method
        in javax.sip.Header. This method specifies object equality as outlined by RFC3261. Two CSeq
        header fields are considered equal if the sequence number and the request method are
        identical. When comparing header fields, field names are always case-insensitive. Unless
        otherwise stated in the definition of a particular header field, field values, parameter
        names, and parameter values are case-insensitive. Tokens are always case-insensitive. Unless
        specified otherwise, values expressed as quoted strings are case-sensitive.
        @param obj The object to compare this CSeqHeader with.
        @return True if obj is an instance of this class representing the same CseqHeader as this,
        false otherwise.
        @since v1.2
      */
    boolean equals(java.lang.Object obj);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
