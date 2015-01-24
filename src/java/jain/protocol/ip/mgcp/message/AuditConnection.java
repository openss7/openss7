/*
 @(#) src/java/jain/protocol/ip/mgcp/message/AuditConnection.java <p>
 
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

package jain.protocol.ip.mgcp.message;

/**
    An MGCP AuditConnection command, expressed as an event object, sent by a Call Agent to a Media
    Gateway to request the Gateway to send parameters of a connection. This command elicits an
    AuditConnectionResponse event.
    @version 1.2.2
    @author Monavacon Limited
  */
public final class AuditConnection extends JainMgcpCommandEvent {
    /**
        Constructs a new AuditConnection (Command) Event object.
        @param source A reference to the object that fired this event.  In the case where the
        JainMgcpProvider fires the event, this parameter should be a reference to the object that
        implements the JainMgcpProvider interface. In the case where the application (the
        JainMgcpListener) fires the event, this parameter should be a reference to the object that
        implements the JainMgcpListener interface.
        @param endpointIdentifier Name for the endpoint in the gateway where this command executes.
        @param connectionIdentifier Identifies the connection within the endpoint whose
        characteristics are being modified.
        @param requestedInfo Specifies which connection parameters are to be returned by the gateway
        in response to this command.
        @param transactionHandle A handle to be used in associating a Response Event object with the
        Command Event object that elicited it.
        @exception java.lang.IllegalArgumentException Thrown if: <ul> <li> Any one or all of the
        endpoint identifier, connection identifier, and the list of requested information parameters
        is (are) null-valued.  <li> Any of the requested information parameters in the list of
        requested information parameters is null-valued.  <li> Any of the requested information
        parameters in the list of requested information parameters is not valid for the
        AuditConnection command.
      */
    public AuditConnection(java.lang.Object source, EndpointIdentifier endpointIdentifier,
            ConnectionIdentifier connectionIdentifier, InfoCode[] requestedInfo)
        throws java.lang.IllegalArgumentException {
        super(source, endpointIdentifier, Constants.CMD_AUDIT_CONNECTION);
        setConnectionIdentifier(connectionIdentifier);
        setRequestedInfo(requestedInfo);
    }
    /**
        Gets the Connection Identifier.
        @return The Connection Identifier.
      */
    public ConnectionIdentifier getConnectionIdentifier() {
        return m_connectionIdentifier;
    }
    /**
        Returns the list of connection parameters the gateway is to return in response to an
        AuditConnection command.
      */
    public InfoCode[] getRequestedInfo() {
        return m_requestedInfo;
    }
    /**
        Sets the Connection Identifier.
        @param connectionIdentifier Identifies the connection within the endpoint whose
        characteristics are being modified.
        @exception java.lang.IllegalArgumentException Thrown if the connection identifier object
        reference is null.
      */
    public void setConnectionIdentifier(ConnectionIdentifier connectionIdentifier)
        throws java.lang.IllegalArgumentException {
        if (connectionIdentifier != null) {
            m_connectionIdentifier = connectionIdentifier;
            return;
        }
        throw new java.lang.IllegalArgumentException();
    }
    /**
        Sets the list of connection parameters the gateway is to return in response to an
        AuditConnection command.
        @param requestedInfo The list of connection parameters.
        @exception java.lang.IllegalArgumentException Thrown if one or more of the requested
        connection parameters are not valid for the AuditConnection command, or if the requested
        information parameter object reference is null.
      */
    public void setRequestedInfo(InfoCode[] requestedInfo)
        throws java.lang.IllegalArgumentException {
        if (requestedInfo != null) {
            m_requestedInfo = requestedInfo;
            return;
        }
        throw new java.lang.IllegalArgumentException();
    }
    /**
        Overrides java.lang.Object.toString().
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njava.jain.protocol.ip.mgcp.message.AuditConnection");
        if (m_connectionIdentifier != null)
            b.append("\n\tm_connectionIdentifier = " + m_connectionIdentifier.toString());
        if (m_requestedInfo != null)
            for (int i = 0; i < m_requestedInfo.length; i++)
                b.append("\n\tm_requestedInfo = " + m_requestedInfo[i].toString());
        return b.toString();
    }
    private InfoCode[] m_requestedInfo = null;
    private ConnectionIdentifier m_connectionIdentifier = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
