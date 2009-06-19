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
    This interface represents the AllowEvents SIP header, as defined by RFC3265, this header is not
    part of RFC3261. <p> The AllowEventsHeader includes a list of tokens which indicates the event
    packages supported by the client (if sent in a request) or server (if sent in a response). In
    other words, a node sending an AllowEventsHeader is advertising that it can process SUBSCRIBE
    requests and generate NOTIFY requests for all of the event packages listed in that header. <p>
    Any node implementing one or more event packages SHOULD include an appropriate AllowEventsHeader
    indicating all supported events in all methods which initiate dialogs and their responses (such
    as INVITE) and OPTIONS responses. This information is very useful, for example, in allowing user
    agents to render particular interface elements appropriately according to whether the events
    required to implement the features they represent are supported by the appropriate nodes. <p>
    Note that "Allow-Events" headers MUST NOT be inserted by proxies.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface AllowEventsHeader extends Header {
    /** Name of AllowEventsHeader. */
    public static final java.lang.String NAME = "Allow-Events";
    /**
        Sets the eventType defined in this AllowEventsHeader.
        @param eventType The String defining the method supported in this AllowEventsHeader.
        @exception java.text.ParseException Thrown when an error was encountered while parsing the
        Strings defining the eventType supported
      */
    public void setEventType(java.lang.String eventType) throws java.text.ParseException;
    /**
        Gets the eventType of the AllowEventsHeader.
        @return The String object identifing the eventTypes of AllowEventsHeader.
      */
    public java.lang.String getEventType();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
