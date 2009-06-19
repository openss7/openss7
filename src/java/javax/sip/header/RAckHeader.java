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
    This interface represents the RAck header, as defined by RFC3262, this header is not part of
    RFC3261. <p> The PRACK messages contain an RAck header field, which indicates the sequence
    number of the provisional response that is being acknowledged (each provisional response is
    given a sequence number, carried in the RSeq header field in the Provisional response). The
    acknowledgements are not cumulative, and the specifications recommend a single outstanding
    provisional response at a time, for purposes of congestion control. <p> The RAck header contains
    two numbers and a method tag. The first number is the sequence number from the RSeqHeader in the
    provisional response that is being acknowledged. The next number is the sequence number that is
    copied from the CSeqHeader along with the method tag, from the response that is being
    acknowledged. <p> For Example: <br> <code> RAck: 776656 1 INVITE </code> <p> A server must
    ignore Headers that it does not understand. A proxy must not remove or modify Headers that it
    does not understand.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface RAckHeader extends Header {
    /**
        Name of RAckHeader.
      */
    static final java.lang.String NAME = "RAck";
    /**
        Sets the method of RAckHeader, which correlates to the method of the CSeqHeader of the
        provisional response being acknowledged.
        @param method The new string value of the method of the RAckHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the method
        value.
      */
    void setMethod(java.lang.String method)
        throws java.text.ParseException;
    /**
        Gets the method of RAckHeader.
        @return Method of RAckHeader.
      */
    java.lang.String getMethod();
    /**
        Sets the sequence number value of the CSeqHeader of the provisional response being
        acknowledged. The sequence number MUST be expressible as a 32-bit unsigned integer and MUST
        be less than 2**31.
        @param cSeqNumber The new cSeq number of this RAckHeader.
        @exception InvalidArgumentException Thrown when the supplied value is less than zero.
      */
    void setCSeqNumber(int cSeqNumber)
        throws InvalidArgumentException;
    /**
        Gets the CSeq sequence number of this RAckHeader.
        @return The integer value of the cSeq number of the RAckHeader.
      */
    int getCSeqNumber();
    /**
        Sets the sequence number value of the RSeqHeader of the provisional response being
        acknowledged. The sequence number MUST be expressible as a 32-bit unsigned integer and MUST
        be less than 2**31.
        @param rSeqNumber The new rSeq number of this RAckHeader.
        @exception InvalidArgumentException Thrown when the supplied value is less than zero.
      */
    void setRSeqNumber(int rSeqNumber)
        throws InvalidArgumentException;
    /**
        Gets the RSeq sequence number of this RAckHeader.
        @return The integer value of the RSeq number of the RAckHeader.
      */
    int getRSeqNumber();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
