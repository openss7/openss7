/*
 @(#) $RCSfile: CreateConnectionResponse.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:20 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:20 $ by $Author: brian $
 */

package jain.protocol.ip.mgcp.message;

/**
    The acknowledgment to an MGCP CreateConnection command.
    @version 1.2.2
    @author Monavacon Limited
  */
public final class CreateConnectionResponse extends JainMgcpResponseEvent {
    /**
        Constructs a new CreateConnectionResponse Event object.
        @param returnCode The return code.
        @param transactionHandle The transaction handle that corresponds
        to the Command Event that elicited this Response Event.
        @param connectionIdentifier A unique identifier for the
        connection within one endpoint.
        @exception java.lang.IllegalArgumentException Thrown if either
        the return code or the connection identifier parameter is
        null-valued.
      */
    public CreateConnectionResponse(java.lang.Object source, ReturnCode returnCode, ConnectionIdentifier connectionIdentifier)
        throws java.lang.IllegalArgumentException {
        super(source, returnCode, Constants.RESP_CREATE_CONNECTION);
        setConnectionIdentifier(connectionIdentifier);
    }
    /**
        Gets the Connection Identifier.
        @return The connection identifier.
      */
    public ConnectionIdentifier getConnectionIdentifier() {
        return m_connectionIdentifier;
    }
    /**
        Gets the Local Connection Descriptor.
        @return The session description that contains information about
        the addresses and RTP ports used for the connection.
      */
    public ConnectionDescriptor getLocalConnectionDescriptor() {
        return m_localConnectionDescriptor;
    }
    /**
        Gets the second connection identifier.
        @return The second connection identifier.
      */
    public ConnectionIdentifier getSecondConnectionIdentifier() {
        return m_secondConnectionIdentifier;
    }
    /**
        Gets the Second Endpoint Identifier.
        @return The second endpoint identifier.
      */
    public EndpointIdentifier getSecondEndpointIdentifier() {
        return m_secondEndpointIdentifier;
    }
    /**
        Gets the Specific Endpoint Identifier.
        @return The identifier for the responding endpoint.
      */
    public EndpointIdentifier getSpecificEndpointIdentifier() {
        return m_specificEndpointIdentifier;
    }
    /**
        Sets the Connection Identifier.
        @param connectionIdentifier A unique identifier for the
        connection within one endpoint.
        @exception java.lang.IllegalArgumentException Thrown if the
        connection identifier parameter is null-valued.
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
        Sets the Local Connection Descriptor string.
        @param localConnectionDescriptor Optional parameter. A session
        description that contains information about the addresses and
        RTP ports used for the connection. Must be set to null, if not
        used.
      */
    public void setLocalConnectionDescriptor(ConnectionDescriptor localConnectionDescriptor) {
        m_localConnectionDescriptor = localConnectionDescriptor;
    } 
    /**
        Sets the second connection identifier.
        @param secondConnectionIdentifier Optional parameter.
        Identifies the second connection bound to the second endpoint,
        if that endpoint was specified in the CreateConnection command.
      */
    public void setSecondConnectionIdentifier(ConnectionIdentifier secondConnectionIdentifier) {
        m_secondConnectionIdentifier = secondConnectionIdentifier;
    }
    /**
        Sets the Second Endpoint Identifier. Used when two endpoints in
        the same gateway are to be connected.
        @param secondEndpointIdentifier Optional parameter. Reflects the
        second endpoint identifier that was specified in the
        CreateConnection command. Null, if none was specified.
      */
    public void setSecondEndpointIdentifier(EndpointIdentifier secondEndpointIdentifier) {
        m_secondEndpointIdentifier = secondEndpointIdentifier;
    }
    /**
        EndpointIdentifier specificEndpointIdentifier Sets the Specific
        Endpoint Identifier.
        @param specificEndpointIdentifier Optional parameter.
        Identifies the responding endpoint. Must be set to null, if not
        used.
      */
    public void setSpecificEndpointIdentifier(EndpointIdentifier specificEndpointIdentifier) {
        m_specificEndpointIdentifier = specificEndpointIdentifier;
    }
    /**
        Overrides java.lang.Object.toString().
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
    private ConnectionIdentifier m_connectionIdentifier;
    private ConnectionDescriptor m_localConnectionDescriptor;
    private ConnectionIdentifier m_secondConnectionIdentifier;
    private EndpointIdentifier m_secondEndpointIdentifier;
    private EndpointIdentifier m_specificEndpointIdentifier;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
