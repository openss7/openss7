/*
 @(#) src/java/jain/protocol/ip/mgcp/message/AuditEndpoint.java <p>
 
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
    An MGCP AuditEndpoint command, expressed as an event object, sent by a Call Agent to a Media
    Gateway to request the Gateway to send status information about one or more endpoints. This
    command elicits an AuditEndpointResponse event.
    @version 1.2.2
    @author Monavacon Limited
  */
public class AuditEndpoint extends JainMgcpCommandEvent {
    /**
        Constructs a new AuditEndpoint (Command) Event object that does not specify requested
        information.
        @param endpointIdentifier Name for the endpoint in the gateway where this command executes.
        This parameter may be specified using the "all of" wildcard convention. If this convention
        is used, the gateway should return the list of endpoint identifiers that match the wildcard.
        If a single endpoint is identified, the gateway should return a positive acknowledgment,
        with no additional information.
        @exception java.lang.IllegalArgumentException Thrown if the endpoint identifier parameter
        object reference is null-valued.
      */
    public AuditEndpoint(java.lang.Object source, EndpointIdentifier endpointIdentifier)
        throws java.lang.IllegalArgumentException {
        super(source, endpointIdentifier, Constants.CMD_AUDIT_ENDPOINT);
    }
    /**
        Constructs a new AuditEndpoint (Command) Event object that specifies requested information.
        @param endpointIdentifier Name for the endpoint in the gateway where this command executes.
        The "all of" wildcard convention must not be used with this form of constructor. If the
        endpoint identifier uses the wildcard convention, a WildcardNotPermittedException will be
        thrown.
        @param requestedInfo Specifies what information about the designated endpoint is to be
        returned by the gateway.
        @exception WildcardNotPermittedException Thrown if the endpoint identifier is specified
        using the wildcard convention.
        @exception java.lang.IllegalArgumentException Thrown if: <ul> <li> the object reference for
        either the endpoint identifier or the requested information parameters is null-valued <li>
        or, the list of requested information parameters contains one or more parameters that are
        not valid for the AuditEndpoint command.
      */
    public AuditEndpoint(java.lang.Object source, EndpointIdentifier endpointIdentifier, InfoCode[] requestedInfo)
        throws WildcardNotPermittedException, java.lang.IllegalArgumentException {
        this(source, endpointIdentifier);
        setRequestedInfo(requestedInfo);
    }
    /**
        Clears all the requested information items.
      */
    public void clearAllReqInfoItems() {
        m_requstedInfo = new InfoCode[0];
    }
    /**
        Gets the currently set endpoint parameters to be returned by the gateway.
      */
    public InfoCode[] getRequestedInfo() {
        return m_requestedInfo;
    }
    /**
        Sets the endpoint parameters to be returned by the gateway.
        @param requestedInfo Specifies what information about the designated endpoint is to be
        returned by the gateway.
        @exception java.lang.IllegalArgumentException Thrown if the list of requested information
        parameters contains one or more parameters that are not valid for the AuditEndpoint command
        or if the object reference to the requested information parameter is null-valued.
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
        b.append("\njava.jain.protocol.ip.mgcp.message.AuditEndpoint");
        b.append("\n\tm_requestedInfo = ");
        if (m_requestedInfo != null)
            for (int i = 0; i < m_requestedInfo.length; i++)
                b.append("\n\t\t" + m_requestedInfo[i].toString());
        return b.toString();
    }
    private InfoCode[] m_requestedInfo = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
