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
    The Authentication-Info header field provides for mutual authentication with HTTP Digest. A UAS
    MAY include this header field in a 2xx response to a request that was successfully authenticated
    using digest based on the Authorization header field. <p> <dt>For
    Example:<br><code>Authentication-Info: nextnonce="47364c23432d2e131a5fb210812c"</code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface AuthenticationInfoHeader extends Parameters, Header {
    /**
        Name of the AlertInfoHeader.
      */
    static final java.lang.String NAME = "Authentication-Info";
    /**
        Sets the NextNonce of the AuthenticationInfoHeader to the nextNonce parameter value.
        @param nextNonce The new nextNonce String of this AuthenticationInfoHeader.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        nextNonce value.
      */
    void setNextNonce(java.lang.String nextNonce)
        throws java.text.ParseException;
    /**
        Returns the nextNonce value of this AuthenticationInfoHeader.
        @return The String representing the nextNonce information, null if value is not set.
      */
    java.lang.String getNextNonce();
    /**
        Sets the Qop value of the AuthenticationInfoHeader to the new qop parameter value.
        @param qop The new Qop string of this AuthenticationInfoHeader.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        Qop value.
      */
    void setQop(java.lang.String qop)
        throws java.text.ParseException;
    /**
        Returns the messageQop value of this AuthenticationInfoHeader.
        @return The string representing the messageQop information, null if the value is not set.
      */
    java.lang.String getQop();
    /**
        Sets the CNonce of the AuthenticationInfoHeader to the cNonce parameter value.
        @param cNonce The new cNonce String of this AuthenticationInfoHeader.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        cNonce value.
      */
    void setCNonce(java.lang.String cNonce)
        throws java.text.ParseException;
    /**
        Returns the CNonce value of this AuthenticationInfoHeader.
        @return The String representing the cNonce information, null if value is not set.
      */
    java.lang.String getCNonce();
    /**
        Sets the Nonce Count of the AuthenticationInfoHeader to the nonceCount parameter value.
        @param nonceCount The new nonceCount integer of this AuthenticationInfoHeader.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        nonceCount value.
      */
    void setNonceCount(int nonceCount)
        throws java.text.ParseException;
    /**
        Returns the Nonce Count value of this AuthenticationInfoHeader.
        @return The integer representing the nonceCount information, -1 if value is not set.
      */
    int getNonceCount();
    /**
        Sets the Response of the AuthenticationInfoHeader to the new response parameter value.
        @param response The new response String of this AuthenticationInfoHeader.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        Response.
      */
    void setResponse(java.lang.String response)
        throws java.text.ParseException;
    /**
        Returns the Response value of this AuthenticationInfoHeader.
        @return The String representing the Response information.
      */
    java.lang.String getResponse();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
