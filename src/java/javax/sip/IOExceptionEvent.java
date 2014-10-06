/*
 @(#) $RCSfile: IOExceptionEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:51 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:51 $ by $Author: brian $
 */

package javax.sip;

/**
    This object is used to signal to the application that an IO Exception has occured. The
    transaction state machine requires to report asynchronous IO Exceptions to the application
    immediately (according to RFC 3261). This class represents an IOExceptionEvent that is passed
    from a SipProvider to its SipListener. This event enables an implementation to propagate the
    asynchronous handling of IO Exceptions to the application. An application (SipListener) will
    register with the SIP protocol stack (SipProvider) and listen for IO Exceptions from the
    SipProvider. In many cases, when sending a SIP message, the sending function will return before
    the message was actually sent.  This will happen for example if there is a need to wait for a
    response from a DNS server or to perform other asynchronous actions such as connecting a TCP
    connection. Later on if the message sending fails an IO exception event will be given to the
    application. IO Exception events may also be reported asynchronously when the Transaction State
    machine attempts to resend a pending request. Note that synchronous IO Exceptions are presented
    to the caller as SipException.
    @since v1.2
    @version 1.2.2
    @author Monavacon Limited
  */
public class IOExceptionEvent extends java.util.EventObject {
    /**
        Constructor.
        @param source The object that is logically deemed to have caused the IO Exception (dialog/transaction/provider).
        @param remoteHost Host where the request/response was heading.
        @param port Port where the request/response was heading.
        @param transport Transport ( i.e. UDP/TCP/TLS)..
      */
    public IOExceptionEvent(java.lang.Object source, java.lang.String remoteHost, int port, java.lang.String transport) {
        super(source);
        m_remoteHost = remoteHost;
        m_port = port;
        m_transport = transport;
    }
    /**
        Return the host where Socket was pointing.
        @return Host associated with the IOException.
      */
    public java.lang.String getHost() {
        return m_remoteHost;
    }
    /**
        Returns the port where the socket was trying to send amessage.
        @return Port associated with the IOException.
      */
    public int getPort() {
        return m_port;
    }
    /**
        Return transport used for the failed write attempt.
        @return The transaction associated with the IOException.
      */
    public java.lang.String getTransport() {
        return m_transport;
    }
    private java.lang.String m_remoteHost;
    private int m_port;
    private java.lang.String m_transport;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
