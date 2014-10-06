/*
 @(#) $RCSfile: MimeVersionHeader.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:53 $ <p>
 
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

import javax.sip.*;

/**
    SIP messages MAY include a single MIME-Version general-header field to indicate what version of
    the MIME protocol was used to construct the message. Use of the MIME-Version header field
    indicates that the message is in full compliance with the MIME protocol as defined in RFC2045.
    Proxies/gateways are responsible for ensuring full compliance (where possible) when exporting
    SIP messages to strict MIME environments. <p> For Example: <br> <code> MIME-Version: 1.0 </code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface MimeVersionHeader extends Header {
    /**
        Name of MimeVersionHeader.
      */
    static final java.lang.String NAME = "MIME-Version";
    /**
        Gets the Minor version value of this MimeVersionHeader.
        @return The Minor version of this MimeVersionHeader.
      */
    int getMinorVersion();
    /**
        Sets the Minor-Version argument of this MimeVersionHeader to the supplied minorVersion
        value.
        @param minorVersion The new minor MIME version.
        @exception InvalidArgumentException Thrown when the supplied value is less than zero.
      */
    void setMinorVersion(int minorVersion)
        throws InvalidArgumentException;
    /**
        Gets the Major version value of this MimeVersionHeader.
        @return The Major version of this MimeVersionHeader.
      */
    int getMajorVersion();
    /**
        Sets the Major-Version argument of this MimeVersionHeader to the supplied majorVersion
        value.
        @param majorVersion The new major MIME version.
        @exception InvalidArgumentException Thrown when the supplied version is less than zero.
      */
    void setMajorVersion(int majorVersion)
        throws InvalidArgumentException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
