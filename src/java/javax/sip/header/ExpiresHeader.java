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
    The Expires header field gives the relative time after which the message (or content) expires.
    The precise meaning of this is method dependent. The expiration time in an INVITE does not
    affect the duration of the actual session that may result from the invitation.  Session
    description protocols may offer the ability to express time limits on the session duration,
    however. The value of this field is an integral number of seconds (in decimal) between 0 and
    (2**32)-1, measured from the receipt of the request. Malformed values SHOULD be treated as
    equivalent to 3600. <p> This interface represents the Expires entity-header. The ExpiresHeader
    is optional in both REGISTER and INVITE Requests. <ul> <li>REGISTER - When a client sends a
    REGISTER request, it MAY suggest an expiration interval that indicates how long the client would
    like the registration to be valid. There are two ways in which a client can suggest an
    expiration interval for a binding: through an Expires header field or an "expires" Contact
    header parameter.  The latter allows expiration intervals to be suggested on a per-binding basis
    when more than one binding is given in a single REGISTER request, whereas the former suggests an
    expiration interval for all Contact header field values that do not contain the "expires"
    parameter.  <li>INVITE - The UAC MAY add an Expires header field to limit the validity of the
    invitation. If the time indicated in the Expires header field is reached and no final answer for
    the INVITE has been received, the UAC core SHOULD generate a CANCEL request for the INVITE.</ul>
    Example:<br><code>Expires: 5</code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ExpiresHeader extends Header {
    /**
        Name of ExpiresHeader.
      */
    static final java.lang.String NAME = "Expires";
    /**
        Sets the relative expires value of the ExpiresHeader in units of seconds. The expires value
        MUST be between zero and (2**31)-1.
        @param expires The new expires value of this ExpiresHeader.
        @exception InvalidArgumentException Thrown when supplied value is less than zero.
      */
    void setExpires(int expires)
        throws InvalidArgumentException;
    /**
        Gets the expires value of the ExpiresHeader. This expires value is relative time.
        @return The expires value of the ExpiresHeader.
      */
    int getExpires();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
