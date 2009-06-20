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

package jain.protocol.ip.mgcp.message;

/**
    An MGCP CreateConnection command, expressed as an event object, sent by a Call Agent to a Media
    Gateway to request the Gateway to create a connection between two endpoints. This command
    elicits a CreateConnectionResponse event.
    @version 1.2.2
    @author Monavacon Limited
  */
public final class CreateConnection extends JainMgcpCommandEvent {
    /**
        Constructs a new CreateConnection (Command) Event object.
        @param callIdentifier A globally unique parameter that identifies the call (or session) to
        which this connection belongs.
        @param endpointIdentifier Name for the endpoint in the gateway where this command executes.
        @param mode Indicates the mode of operation for this side of the connection; e.g., send,
        receive, etc.
        @exception java.lang.IllegalArgumentException Thrown if the call identifier, endpoint
        identifier, or mode parameter is null-valued.
      */
    public CreateConnection(java.lang.Object source, CallIdentifier callIdentifier,
            EndpointIdentifier endpointIdentifier, ConnectionMode mode)
        throws java.lang.IllegalArgumentException {
        super(source, endpointIdentifiers, Constants.CMD_CREATE_CONNECTION);
        setCallIdentifier(callIdentifier);
        setMode(mode);
    }
    /**
        Gets the Bearer Information parameter.
        @return Returns a reference to the Bearer Information.
      */
    public BearerInformation getBearerInformation() {
        return m_bearerInformation;
    }
    /**
        Gets the Call Identifier.
        @return The Call Identifier.
      */
    public CallIdentifier getCallIdentifier() {
        return m_callIdentifier;
    }
    /**
        Gets the Local Connection Options parameter.
        @return The list of Local Connection Option values.
      */
    public LocalOptionValue[] getLocalConnectionOptions() {
        return m_localConnectionOptions;
    }
    /**
        Gets the Mode parameter.
        @return The Mode parameter.
      */
    public ConnectionMode getMode() {
        return m_mode;
    }
    /**
        Gets the Notification Request parameters.
        @return The Notification Request parameters.
        @see NotificationRequestParms
      */
    public NotificationRequestParms getNotificationRequestParms() {
        return m_notificationRequestParms;
    }
    /**
        Gets the Notified Entity.
        @return The Notified Entity.
      */
    public NotifiedEntity getNotifiedEntity() {
        return m_notifiedEntity;
    }
    /**
        Gets the Remote Connection Descriptor parameter.
        @return The Remote Connection Descriptor parameter.
      */
    public ConnectionDescriptor getRemoteConnectionDescriptor() {
        return m_remoteConnectionDescriptor;
    }
    /**
        Gets the Second Endpoint Identifier.
        @return
                The Second Endpoint Identifier.
      */
    public EndpointIdentifier getSecondEndpointIdentifier() {
        return m_secondEndpointIdentifier;
    }
    /**
        Sets the Bearer Information parameter.
        @param bearerInformation Optional parameter, used to encapsulate bearer information (i.e.,
        encoding method) in this command.
      */
    public void setBearerInformation(BearerInformation bearerInformation) {
        m_bearerInformation = bearerInformation;
    }
    /**
        Sets the Call Identifier.
        @param callIdentifier A globally unique parameter that identifies the call (or session) to
        which this connection belongs.
        @exception java.lang.IllegalArgumentException Thrown if the call identifier parameter is
        null-valued.
      */
    public void setCallIdentifier(CallIdentifier callIdentifier)
        throws java.lang.IllegalArgumentException {
        if (callIdentifier != null) {
            m_callIdentifier = callIdentifier;
            return;
        }
        throw new java.lang.IllegalArgumentException();
    }
    /**
        Sets the Local Connection Options parameter.
        @param localConnectionOptions Optional parameter. A list of local connection option values.
        Used by the Call Agent to direct the handling of the connection by the gateway.
      */
    public void setLocalConnectionOptions(LocalOptionValue[] localConnectionOptions) {
        m_localConnectionOptions = localConnectionOptions;
    }
    /**
        Sets the Mode parameter.
        @param mode Indicates the mode of operation for this side of the connection; e.g., send,
        receive, etc.
        @exception java.lang.IllegalArgumentException Thrown if the mode parameter is null-valued.
      */
    public void setMode(ConnectionMode mode)
        throws java.lang.IllegalArgumentException {
        if (mode != null) {
            m_mode = mode;
            return;
        }
        throw new java.lang.IllegalArgumentException();
    }
    /**
        Sets the Notification Request parameters.
        @param notificationRequestParms Optional parameter. Used to encapsulate notification request
        parameters in this command.
        @see NotificationRequestParms
      */
    public void setNotificationRequestParms(NotificationRequestParms notificationRequestParms) {
        m_notificationRequestParams = notificationRequestParams;
    }
    /**
        Sets the Notified Entity.
        @param notifiedEntity Optional parameter. Specifies where notifications should be sent. If
        not set explicitly, notifications will be sent to originator of this command.
      */
    public void setNotifiedEntity(NotifiedEntity notifiedEntity) {
        m_notifiedEntity = notifiedEntity;
    }
    /**
        Sets the Remote Connection Descriptor parameter.
        @param remoteConnectionDescriptor Optional parameter. The connection descriptor for the
        remote side of a connection, on the other side of the IP network.
        @exception ConflictingParameterException Remote Connection Descriptor and Second Endpoint
        Identifier are mutually exclusive. This exception is thrown if Second Endpoint Identifier is
        already set and an attempt is made to set Remote Connection Descriptor.
      */
    public void setRemoteConnectionDescriptor(ConnectionDescriptor remoteConnectionDescriptor)
        throws ConflictingParameterException {
        m_remoteConnectionDescriptor = remoteConnectionDescriptor;
    }
    /**
        Sets the Second Endpoint Identifier.
        @param secondEndpointIdentifier Optional parameter. Used in conjunction with the
        endpointIdentifier to establish a connection between two endpoints located on the same
        gateway.
        @exception ConflictingParameterException Remote Connection Descriptor and Second Endpoint
        Identifier are mutually exclusive. This exception is thrown if Remote Connection Descriptor
        is already set and an attempt is made to set Second Endpoint Identifier .
      */
    public void setSecondEndpointIdentifier(EndpointIdentifier secondEndpointIdentifier)
        throws ConflictingParameterException {
        m_secondEndpointIdentifier = secondEndpointIdentifier;
    }
    /**
        Overrides java.lang.Object.toString().
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
    private BearerInformation m_bearerInformation;
    private CallIdentifier m_callIdentifier;
    private LocalOptionValue[] m_localConnectionOptions;
    private ConnectionMode m_mode;
    private NotificationRequestParms m_notificationRequestParms;
    private NotifiedEntity m_notifiedEntity;
    private ConnectionDescriptor m_remoteConnectionDescriptor;
    private EndpointIdentifier m_secondEndpointIdentifier;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
