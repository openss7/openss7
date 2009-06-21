/*
 @(#) $RCSfile: CallInfoHeader.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:53 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:53 $ by $Author: brian $
 */

package javax.sip.header;

import javax.sip.address.*;
import javax.sip.*;

/**
    The Call-Info header field provides additional information about the caller or callee, depending
    on whether it is found in a request or response. The purpose of the URI is described by the
    "purpose" parameter. The "icon" purpose designates an image suitable as an iconic representation
    of the caller or callee. The "info" purpose describes the caller or callee in general, for
    example, through a web page. The "card" purpose provides a business card, for example, in vCard
    or LDIF formats. <p> Use of the Call-Info header field can pose a security risk. If a callee
    fetches the URIs provided by a malicious caller, the callee may be at risk for displaying
    inappropriate or offensive content, dangerous or illegal content, and so on. Therefore, it is
    RECOMMENDED that a User Agent only render the information in the Call-Info header field if it
    can verify the authenticity of the element that originated the header field and trusts that
    element. This need not be the peer User Agent; a proxy can insert this header field into
    requests. <p> <dt>For Example:<br><code> Call-Info: http://jcp.org/duke/photo.jpg;<br>
    purpose=icon, http://jcp.org/duke/; purpose=info </code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface CallInfoHeader extends Parameters, Header {
    /**
        Name of CallInfoHeader.
      */
    static final java.lang.String NAME = "Call-Info";
    /**
        Sets the Information parameter of this CallInfoHeader. The Information describes the caller
        or callee.
        @param info The new URI value of the location of the information.
      */
    void setInfo(URI info);
    /**
        Gets the URI that represents the location of the info of the caller or callee.
        @return The location of the info of this CallInfoHeader, returns null if no info is present.
      */
    URI getInfo();
    /**
        Sets the purpose parameter of the info of this CallInfoHeader.
        @param purpose The new string value of the purpose of this info.
        @exception java.text.ParseException Thrown when an error was found while parsing the purpose
        value.
      */
    void setPurpose(java.lang.String purpose)
        throws java.text.ParseException;
    /**
        Gets the purpose of the information supplied in this CallInfoHeader.
        @return The sting value of the purpose of the info, returns null if no purpose is present.
      */
    java.lang.String getPurpose();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
