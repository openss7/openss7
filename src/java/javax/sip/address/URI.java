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

package javax.sip.address;

import javax.sip.*;

/**
    This class represents a generic URI. This is the base interface for any type of URI. These are
    used in SIP requests to identify the callee and also in Contact, From, and To headers. <p> The
    generic syntax of URIs is defined in RFC 2396.
    @version 1.2.2
    @author Monavacon Limited
    @see TelURL
    @see SipURI
  */
public interface URI extends java.lang.Cloneable, java.io.Serializable {
    /**
        Returns the value of the "scheme" of this URI, for example "sip", "sips" or "tel".
        @return The scheme paramter of the URI.
      */
    public java.lang.String getScheme();
    /**
        Creates and returns a deep copy of the URI. This methods must ensure a deep copy of the URI,
        so that when a URI is cloned the URI can be modified without effecting the original URI.
        This provides useful functionality for proxying Requests and Responses. This method
        overrides the clone method in java.lang.Object.
        @return A deep copy of URI.
      */
    public java.lang.Object clone();
    /**
        This method determines if this is a URI with a scheme of "sip" or "sips".
        @return True if the scheme is "sip" or "sips", false otherwise.
      */
    public boolean isSipURI();
    /**
        This method returns the URI as a string.
        @return java.lang.String The stringified version of the URI.
      */
    public java.lang.String toString();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
