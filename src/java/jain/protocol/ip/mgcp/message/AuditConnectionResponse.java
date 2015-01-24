/*
 @(#) src/java/jain/protocol/ip/mgcp/message/AuditConnectionResponse.java <p>
 
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
    The acknowledgment to an MGCP AuditConnection command.
    @version 1.2.2
    @author Monavacon Limited
  */
public final class AuditConnectionResponse extends JainMgcpResponseEvent {
    /**
        Constructs a new AuditConnectionResponse Event object.
        @param returnCode The return code.
        @param transactionHandle The transaction handle that corresponds to the Command Event that
        elicited this Response Event.
        @exception java.lang.IllegalArgumentException Thrown if return code parameter object
        reference is null-valued.
      */
    public AuditConnectionResponse(java.lang.Object source, ReturnCode returnCode)
        throws java.lang.IllegalArgumentException {
        super(source, returnCode, Constants.RESP_AUDIT_CONNECTION);
    }
    /**
        Gets the Call Identifier.
        @return The current call identifier for this connection.
      */
    public CallIdentifier getCallIdentifier() {
        return m_callIdentifier;
    }
    /**
        Gets the Connection Parameters.
        @return The current set of parameters that describe the status of this connection.
      */
    public ConnectionParm[] getConnectionParms() {
        return m_connectionParms;
    }
    /**
        Gets the Local Connection Descriptor.
        @return The session description that contains information about the addresses and RTP ports
        used for the connection.
      */
    public ConnectionDescriptor getLocalConnectionDescriptor() {
        return m_localConnectionDescriptor;
    }
    /**
        Gets the Local Connection Options.
        @return The list of current Local Connection Options for this connection.
      */
    public LocalOptionValue[] getLocalConnectionOptions() {
        return m_localConnectionOptions;
    }
    /**
        Gets the mode.
        @return The current mode for this connection.
      */
    public ConnectionMode getMode() {
        return m_mode;
    }
    /**
        Gets the Notified Entity.
        @return The current notified entity for this connection.
      */
    public NotifiedEntity getNotifiedEntity() {
        return m_notifiedEntity;
    }
    /**
        Gets the Remote Connection Descriptor.
        @return The current description of the remote connection.
      */
    public ConnectionDescriptor getRemoteConnectionDescriptor() {
        return m_remoteConnectionDescriptor;
    }
    /**
        Sets the Call Identifier.
        @param callIdentifier The current call identifier for this connection.
      */
    public void setCallIdentifier(CallIdentifier callIdentifier) {
        m_callIdentifier = callIdentifier;
    }
    /**
        Sets the Connection Parameters.
        @param connectionParms The current list of connection parameters that describe the status of
        the connection.
      */
    public void setConnectionParms(ConnectionParm[] connectionParms) {
        m_connectionParms = connectionParms;
    }
    /**
        Sets the Local Connection Descriptor string.
        @param localConnectionDescriptor Optional parameter. A session description that contains
        information about the addresses and RTP ports used for the connection. Must be set to null,
        if not used.
      */
    public void setLocalConnectionDescriptor(ConnectionDescriptor localConnectionDescriptor) {
        m_localConnectionDescriptor = localConnectionDescriptor;
    }
    /**
        Sets the Local Connection Options.
        @param localConnectionOptions The list of current Local Connection Options for this
        connection.
      */
    public void setLocalConnectionOptions(LocalOptionValue[] localConnectionOptions) {
        m_localConnectionOptions = localConnectionOptions;
    }
    /**
        Sets the mode.
        @param mode The current mode for this connection.
      */
    public void setMode(ConnectionMode mode) {
        m_mode = mode;
    }
    /**
        Sets the Notified Entity.
        @param notifiedEntity The current notified entity for this connection.
      */
    public void setNotifiedEntity(NotifiedEntity notifiedEntity) {
        m_notifiedEntity = notifiedEntity;
    }
    /**
        Sets the Remote Connection Descriptor string.
        @param remoteConnectionDescriptor The current description of the remote connection.
      */
    public void setRemoteConnectionDescriptor(ConnectionDescriptor remoteConnectionDescriptor) {
        m_remoteConnectionDescriptor = remoteConnectionDescriptor;
    }
    /**
        Overrides java.lang.Object.toString().
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njava.jain.protocol.ip.mgcp.message.AuditConnectionResponse");
        b.append("\n\tm_callIdentifier = ");
        if (m_callIdentifier != null)
            b.append(m_callIdentifier.toString());
        b.append("\n\tm_connectionParms = ");
        if (m_connectionParms != null)
            for (int i = 0; i < m_connectionParms.length; i++)
                b.append("\n\t\t" + m_connectionParms[i].toString());
        b.append("\n\tm_localConnectionDescriptor = ");
        if (m_localConnectionDescriptor != null)
            b.append(m_localConnectionDescriptor.toString());
        b.append("\n\tm_localConnectionOptions = ");
        if (m_localConnectionOptions != null)
            for (int i = 0; i < m_localConnectionOptions.length; i++)
                b.append("\n\t\t" + m_localConnectionOptions.toString());
        b.append("\n\tm_mode = ");
        if (m_mode != null)
            b.append(m_mode.toString());
        b.append("\n\tm_notifierEntity = ");
        if (m_notifiedEntity != null)
            b.append(m_notifiedEntity.toString());
        b.append("\n\tm_remoteConnectionDescriptor = ");
        if (m_remoteConnectionDescriptor != null)
            b.append(m_remoteConnectionDescriptor.toString());
        return b.toString();
    }
    /* Private members. */
    private CallIdentifier m_callIdentifier = null;
    private ConnectionParm[] m_connectionParms = null;
    private ConnectionDescriptor m_localConnectionDescriptor = null;
    private LocalOptionValue[] m_localConnectionOptions = null;
    private ConnectionMode m_mode = null;
    private NotifiedEntity m_notifiedEntity = null;
    private ConnectionDescriptor m_remoteConnectionDescriptor = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
