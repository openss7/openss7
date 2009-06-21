/*
 @(#) $RCSfile: ContentEncodingHeader.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:53 $ <p>
 
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
    A ContentEncodingHeader is used as a modifier to the "media-type".  When present, its value
    indicates what additional content codings have been applied to the entity-body, and thus what
    decoding mechanisms must be applied in order to obtain the media-type referenced by the
    ContentTypeHeader. The ContentEncodingHeader is primarily used to allow a body to be compressed
    without losing the identity of its underlying media type. <p> If multiple encodings have been
    applied to an entity, the ContentEncodings must be listed in the order in which they were
    applied. All content-coding values are case-insensitive. Clients MAY apply content encodings to
    the body in requests. A server MAY apply content encodings to the bodies in responses. The
    server MUST only use encodings listed in the Accept-Encoding header field in the request.  If
    the server is not capable of decoding the body, or does not recognize any of the content-coding
    values, it must send a UNSUPPORTED_MEDIA_TYPE Response, listing acceptable encodings in an
    AcceptEncodingHeader.
    @see ContentDispositionHeader
    @see ContentLengthHeader
    @see ContentTypeHeader
    @see ContentLanguageHeader
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ContentEncodingHeader extends Encoding, Header {
    /**
        Name of ContentEncodingHeader.
      */
    static final java.lang.String NAME = "Content-Encoding";
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
