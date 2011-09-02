/*
 @(#) $RCSfile: ViaHeader.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:49 $ <p>
 
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
 
 Last Modified $Date: 2011-09-02 08:46:49 $ by $Author: brian $
 */

package javax.sip.header;

import javax.sip.*;

/**
    The Via header field indicates the transport used for the transaction and identifies the
    location where the response is to be sent. A Via header field value is added only after the
    transport that will be used to reach the next hop has been selected. When the UAC creates a
    request, it MUST insert a Via into that request. The protocol name and protocol version in the
    header field MUST be SIP and 2.0, respectively. The Via header field value MUST contain a branch
    parameter. This parameter is used to identify the transaction created by that request. This
    parameter is used by both the client and the server. <p> <h5>Branch Parameter:</h5> The branch
    parameter value MUST be unique across space and time for all requests sent by the User Agent.
    The exceptions to this rule are CANCEL and ACK for non-2xx responses. A CANCEL request will have
    the same value of the branch parameter as the request it cancels. An ACK for a non-2xx response
    will also have the same branch ID as the INVITE whose response it acknowledges. <p> The
    uniqueness property of the branch ID parameter, to facilitate its use as a transaction ID, was
    not part of RFC 2543. The branch ID inserted by an element compliant with this specification
    MUST always begin with the characters "z9hG4bK". These 7 characters are used as a magic cookie
    (7 is deemed sufficient to ensure that an older RFC 2543 implementation would not pick such a
    value), so that servers receiving the request can determine that the branch ID was constructed
    in the fashion described by this specification (that is, globally unique).  Beyond this
    requirement, the precise format of the branch token is implementation-defined. JSIP defines a
    convenience function to generate unique branch identifiers at Transaction.getBranchId() <p> A
    common way to create the branch value is to compute a cryptographic hash of the To tag, From
    tag, Call-ID header field, the Request-URI of the request received (before translation), the
    topmost Via header, and the sequence number from the CSeq header field, in addition to any
    Proxy-Require and Proxy-Authorization header fields that may be present. The algorithm used to
    compute the hash is implementation-dependent. <p> Via Processing Rules <ul> <li> Generating
    Requests (UAC): The client originating the Request must insert into the Request a ViaHeader
    containing its host name or network address and, if not the default port number, the port number
    at which it wishes to receive Responses. (Note that this port number can differ from the UDP
    source port number of the Request.) A fully-qualified domain name is recommended.  <li> Request
    Forwarding by Proxies: The proxy MUST insert a Via header field value into the copy before the
    existing Via header field values. This implies that the proxy will compute its own branch
    parameter, which will be globally unique for that branch, and contain the requisite magic
    cookie. Note that this implies that the branch parameter will be different for different
    instances of a spiraled or looped request through a proxy. If a proxy server receives a Request
    which contains its own address in a ViaHeader, it must respond with a 482 (Loop Detected)
    Response. A proxy server must not forward a Request to a multicast group which already appears
    in any of the ViaHeaders. This prevents a malfunctioning proxy server from causing loops. Also,
    it cannot be guaranteed that a proxy server can always detect that the address returned by a
    location service refers to a host listed in the ViaHeader list, as a single host may have
    aliases or several network interfaces.  <li> Response processing by UAC and proxies:
    <blockquote><ul> <li> The first ViaHeader should indicate the proxy or client processing this
    Response. If it does not, discard the Response. Otherwise, remove this ViaHeader.  <li> If there
    is no second ViaHeader, this Response is destined for this client. Otherwise, the processing
    depends on whether the ViaHeader contains a maddr parameter or is a receiver-tagged field.  <li>
    If the second ViaHeader contains a maddr parameter, send the Response to the multicast address
    listed there, using the port indicated in "sent-by", or port 5060 if none is present.  The
    Response should be sent using the TTL indicated in the ttl parameter, or with a TTL of 1 if that
    parameter is not present. For robustness, Responses must be sent to the address indicated in the
    maddr parameter even if it is not a multicast address.  <li> If the second ViaHeader does not
    contain a maddr parameter and is a receiver-tagged ViaHeader, send the Response to the address
    in the received parameter, using the port indicated in the port value, or using port 5060 if
    none is present.  <li> If neither of the previous cases apply, send the Response to the address
    indicated by the host value in the second ViaHeader.  </ul></blockquote> <li> Sending Responses
    (UAS): <blockquote><ul> <li> If the first ViaHeader in the Request contains a maddr parameter,
    send the Response to the multicast address listed there, using the port indicated, or port 5060
    if none is present. The Response should be sent using the TTL indicated in the ttl parameter, or
    with a TTL of 1 if that parameter is not present. For robustness, Responses must be sent to the
    address indicated in the maddr parameter even if it is not a multicast address.  <li> If the
    address in the first ViaHeader differs from the source address of the packet, send the Response
    to the actual packet source address, similar to the treatment for receiver-tagged ViaHeaders.
    <li> If neither of these conditions is true, send the Response to the address contained in the
    host value. If the Request was sent using TCP, use the existing TCP connection if available.
    </ul></blockquote> </ul> Via Parameters: A Via header field value contains the transport
    protocol used to send the message, the client's host name or network address, and possibly the
    port number at which it wishes to receive responses. Transport protocols defined here are "UDP",
    "TCP", "TLS", and "SCTP". "TLS" means TLS over TCP. When a request is sent to a SIPS URI, the
    protocol still indicates "SIP", and the transport protocol is TLS. A Via header field value can
    also contain parameters such as "maddr", "ttl", "received", and "branch". A proxy sending a
    Request to a multicast address must add the maddr parameter to its ViaHeader, and should add the
    ttl parameter. If a server receives a Request which contained an maddr parameter in the topmost
    ViaHeader, it should send the Response to the multicast address listed in the maddr parameter.
    The received parameter is added only for receiver-added ViaHeaders. <p> Two Via header fields
    are equal if their sent-protocol and sent-by fields (including port) are equal, both have the
    same set of parameters, and the values of all parameters are equal.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ViaHeader extends Parameters, Header {
    /**
        Name of ViaHeader.
      */
    static final java.lang.String NAME = "Via";
    /**
        Set the host part of this ViaHeader to the newly supplied host parameter.
        @param host The new value of the host of this ViaHeader
        @exception java.text.ParseException Thrown when an error was found while parsing the host
        value.
      */
    void setHost(java.lang.String host) throws java.text.ParseException;
    /**
        Returns the host part of this ViaHeader.
        @return the string value of the host
      */
    java.lang.String getHost();
    /**
        Set the port part of this ViaHeader to the newly supplied port parameter.
        @param port The new integer value of the port of this ViaHeader
        @exception InvalidArgumentException When the port value is not -1 and <1 or >65535
      */
    void setPort(int port) throws InvalidArgumentException;
    /**
        Returns the port part of this ViaHeader.
        @return The integer value of the port, -1 if not present.
      */
    int getPort();
    /**
        Returns the value of the transport parameter.
        @return The string value of the transport paramter of the ViaHeader.
      */
    java.lang.String getTransport();
    /**
        Sets the value of the transport. This parameter specifies which transport protocol to use
        for sending requests and responses to this entity. The following values are defined: "udp",
        "tcp", "sctp", "tls", but other values may be used also.
        @param transport New value for the transport parameter.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        transport value.
      */
    void setTransport(java.lang.String transport) throws java.text.ParseException;
    /**
        Returns the value of the protocol used.
        @return The string value of the protocol paramter of the ViaHeader.
      */
    java.lang.String getProtocol();
    /**
        Sets the value of the protocol parameter. This parameter specifies which protocol is used,
        for example "SIP/2.0".
        @param protocol New value for the protocol parameter.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        protocol value.
      */
    void setProtocol(java.lang.String protocol) throws java.text.ParseException;
    /**
        Returns the value of the ttl parameter, or -1 if this is not set.
        @return The integer value of the ttl parameter.
      */
    int getTTL();
    /**
        Sets the value of the ttl parameter. The ttl parameter specifies the time-to-live value when
        packets are sent using UDP multicast.
        @param ttl New value of the ttl parameter.
        @exception InvalidArgumentException Thrown when the supplied value is less than zero or
        greater than 255, excluding -1 the default not set value.
      */
    void setTTL(int ttl) throws InvalidArgumentException;
    /**
        Returns the value of the maddr parameter, or null if this is not set.
        @return The string value of the maddr parameter.
      */
    java.lang.String getMAddr();
    /**
        Sets the value of the maddr parameter of this ViaHeader. The maddr parameter indicates the
        server address to be contacted for this user, overriding any address derived from the host
        field.
        @param mAddr New value of the mAddr parameter.
        @exception java.text.ParseException Thrown when an error was found while parsing the mAddr
        value.
      */
    void setMAddr(java.lang.String mAddr) throws java.text.ParseException;
    /**
        Gets the received paramater of the ViaHeader. Returns null if received does not exist.
        @return The string received value of ViaHeader.
      */
    java.lang.String getReceived();
    /**
        Sets the received parameter of ViaHeader.
        @param received The newly supplied received parameter.
        @exception java.text.ParseException Thrown when an error was found while parsing the
        received value.
      */
    void setReceived(java.lang.String received) throws java.text.ParseException;
    /**
        Gets the branch paramater of the ViaHeader. Returns null if branch does not exist.
        @return The string branch value of ViaHeader.
      */
    java.lang.String getBranch();
    /**
        Sets the branch parameter of the ViaHeader to the newly supplied branch value. Note that
        when sending a Request within a transaction, branch id management will be the responsibility
        of the SipProvider; that is the application should not set this value. This method should
        only be used by the application when sending Requests outside of a transaction.
        @param branch The new string branch parmameter of the ViaHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the branch
        value.
      */
    void setBranch(java.lang.String branch) throws java.text.ParseException;
    /**
        Set the rport part of this ViaHeader. This method indicates to the remote party that you
        want it to use rport. It is the applications responsisbility to call this method to inform
        the implementation to set the value of the rport. This allows a client to request that the
        server send the response back to the source IP address and port from which the request
        originated.  See RFC3581
        @exception InvalidArgumentException Thrown when the report value is an illegal integer ( &le; 0
        ).
        @since v1.2
      */
    void setRPort() throws InvalidArgumentException;
    /**
        Returns the rport part of this ViaHeader.
        @return The integer value of the rport or -1 if the rport parameter is not set.
        @since v1.2
      */
    int getRPort();
    /**
        Compare this ViaHeader for equality with another. This method overrides the equals method in
        javax.sip.Header. This method specifies object equality as outlined by RFC3261. Two Via
        header fields are equal if their sent-protocol and sent-by fields are equal, both have the
        same set of parameters, and the values of all parameters are equal. When comparing header
        fields, field names are always case-insensitive. Unless otherwise stated in the definition
        of a particular header field, field values, parameter names, and parameter values are
        case-insensitive. Tokens are always case-insensitive. Unless specified otherwise, values
        expressed as quoted strings are case-sensitive.
        @param obj The object to compare this ViaHeader with.
        @return True if obj is an instance of this class representing the same ViaHeader as this,
        false otherwise.
        @since v1.2
      */
    boolean equals(java.lang.Object obj);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
