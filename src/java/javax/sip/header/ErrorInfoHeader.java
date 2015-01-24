/*
 @(#) src/java/javax/sip/header/ErrorInfoHeader.java <p>
 
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
    The Error-Info header field provides a pointer to additional information about the error status
    response. SIP UACs have user interface capabilities ranging from pop-up windows and audio on PC
    softclients to audio-only on "black" phones or endpoints connected via gateways. Rather than
    forcing a server generating an error to choose between sending an error status code with a
    detailed reason phrase and playing an audio recording, the Error-Info header field allows both
    to be sent. The UAC then has the choice of which error indicator to render to the caller. <p> A
    UAC MAY treat a SIP or SIPS URI in an Error-Info header field as if it were a Contact in a
    redirect and generate a new INVITE, resulting in a recorded announcement session being
    established. A non-SIP URI MAY be rendered to the user. <p> Examples:<br><code> SIP/2.0 404 The
    number you have dialed is not in service <br> Error-Info:
    sip:not-in-service-recording@atlanta.com </code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ErrorInfoHeader extends Parameters, Header {
    /**
        Name of the ErroInfoHeader.
      */
    static final java.lang.String NAME = "Error-Info";
    /**
        Sets the ErrorInfo of the ErrorInfoHeader to the errorInfo parameter value.
        @param errorInfo The new ErrorInfo of this ErrorInfoHeader.
      */
    void setErrorInfo(URI errorInfo);
    /**
        Returns the ErrorInfo value of this ErrorInfoHeader. This message may return null if a
        java.lang.String message identifies the ErrorInfo.
        @return The URI representing the ErrorInfo.
      */
    URI getErrorInfo();
    /**
        Sets the Error information message to the new message value supplied to this method.
        @param message The new string value that represents the error message.
        @exception java.text.ParseException Thrown when an error was found while parsing the error
        message.
      */
    void setErrorMessage(java.lang.String message)
        throws java.text.ParseException;
    /**
        Get the Error information message of this ErrorInfoHeader.
        @return The stringified version of the ErrorInfo header.
      */
    java.lang.String getErrorMessage();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
