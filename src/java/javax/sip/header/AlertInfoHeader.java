/*
 @(#) src/java/javax/sip/header/AlertInfoHeader.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.sip.header;

import javax.sip.address.*;
import javax.sip.*;

/**
    When present in an INVITE request, the Alert-Info header field specifies an alternative ring
    tone to the UAS. When present in a 180 (Ringing) response, the Alert-Info header field specifies
    an alternative ringback tone to the UAC. A typical usage is for a proxy to insert this header
    field to provide a distinctive ring feature. <p> The Alert-Info header field can introduce
    security risks, which are identical to the Call-Info header field risk, see section 20.9 of
    RFC3261. In addition, a user SHOULD be able to disable this feature selectively. This helps
    prevent disruptions that could result from the use of this header field by untrusted elements.
    <p> <dt>For Example:<br><code>Alert-Info: jcp.org/yeeha.wav</code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface AlertInfoHeader extends Parameters, Header {
    /**
        Name of the AlertInfoHeader.
      */
    public static final java.lang.String NAME = "Alert-Info";
    /**
        Sets the AlertInfo of the AlertInfoHeader to the alertInfo parameter value.
        @param alertInfo The new Alert Info URI of this AlertInfoHeader.
      */
    public void setAlertInfo(URI alertInfo);
    /**
        Returns the AlertInfo value of this AlertInfoHeader.
        @return The URI representing the AlertInfo.
      */
    public URI getAlertInfo();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
