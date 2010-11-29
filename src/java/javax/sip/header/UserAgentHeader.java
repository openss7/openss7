/*
 @(#) $RCSfile: UserAgentHeader.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:28:37 $ <p>
 
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

package javax.sip.header;

import javax.sip.*;

/**
    The User-Agent header field contains information about the UAC originating the request. This is
    for statistical purposes, the tracing of protocol violations, and automated recognition of user
    agents for the sake of tailoring Responses to avoid particular user agent limitations. However
    revealing the specific software version of the user agent might allow the user agent to become
    more vulnerable to attacks against software that is known to contain security holes.
    Implementers SHOULD make the User-Agent header field a configurable option. <p> For Example:
    <br> <code> User-Agent: Softphone Beta1.5 </code>
    @see ServerHeader
    @see ViaHeader
    @version 1.2.2
    @author Monavacon Limited
  */
public interface UserAgentHeader extends Header {
    /**
        Name of UserAgentHeader.
      */
    static final java.lang.String NAME = "User-Agent";
    /**
        Returns the List of product values.
        @return The List of strings identifying the software of this ServerHeader.
      */
    java.util.ListIterator<java.lang.String> getProduct();
    /**
        Sets the List of product values of the ServerHeader.
        @param product A List of Strings specifying the product values.
        @exception java.text.ParseException Thrown when an error was found while parsing the List of
        product value strings.
      */
    void setProduct(java.util.List<java.lang.String> product)
        throws java.text.ParseException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
