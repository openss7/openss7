/*
 @(#) $RCSfile: AuditEndpointResponse.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:20 $ <p>
 
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
    The acknowledgment to an MGCP AuditEndpoint command.
  */
public final class AuditEndpointResponse extends JainMgcpResponseEvent {
    /**
        Constructs a new AuditEndpointResponse Event object that either returns positive
        acknowledgment, if none of the optional parameters are set, or returns whatever optional
        parameters are set, along with the Return Code.
        @param returnCode The return code.
        @param transactionHandle The transaction handle that corresponds to the Command Event that
        elicited this Response Event.
        @exception java.lang.IllegalArgumentException Thrown if the return code parameter object
        reference is null-valued.
      */
    public AuditEndpointResponse(java.lang.Object source, ReturnCode returnCode)
        throws java.lang.IllegalArgumentException {
        super(source, returnCode, Constants.RESP_AUDIT_ENDPOINT);
    }
    /**
        Constructs a new AuditEndpointResponse Event object that returns a list of endpoint
        identifiers in response to an AuditEndpoint command in which the Endpoint Identifier
        parameter was specified using the wildcard convention.
        @param returnCode The return code.
        @param transactionHandle The transaction handle that corresponds to the Command Event that
        elicited this Response Event.
        @param endpointIdentifierList An array of endpoint identifiers. The endpoint identifiers are
        those that match the wildcard specification given in the eliciting AuditEndpoint command.
        @exception java.lang.IllegalArgumentException Thrown if the object reference to the return
        code parameter or to the list of endpoint identifiers is null-valued, or if any of the
        endpoint identifiers in the list of endpoint identifiers is null-valued.
      */
    public AuditEndpointResponse(java.lang.Object source, ReturnCode returnCode,
            EndpointIdentifier[] endpointIdentifierList)
        throws java.lang.IllegalArgumentException {
        this(source, returnCode);
        setEndpointIdentifierList(endpointIdentifierList);
    }
    /**
        Gets the Bearer Information.
        @return The Bearer Information
      */
    public BearerInformation getBearerInformation() {
        return m_bearerInformation;
    }
    /**
        Gets the list of endpoint capabilities.
        @return The list of endpoint capability values.
      */
    public CapabilityValue[] getCapabilities() {
        return m_capabilities;
    }
    /**
        Gets the Connection Identifiers.
        @return The Connection Identifiers as an array of ConnectionId objects.
      */
    public ConnectionIdentifier[] getConnectionIdentifiers() {
        return m_connectionIdentifiers;
    }
    /**
        Gets the list of MGCP events that the endpoint has been instructed to detect.
        @return The list of MGCP events that the endpoint has been instructed to detect.
      */
    public EventName[] getDetectEvents() {
        return m_detectEvents;
    }
    /**
        Gets the Digit Map.
        @return The Digit Map.
      */
    public DigitMap getDigitMap() {
        return m_digitMap;
    }
    /**
        Gets the Endpoint Identifier list.
        @return An array of endpoint identifiers.
      */
    public EndpointIdentifier[] getEndpointIdentifierList() {
        return m_endpointIdentifierList;
    }
    /**
        Gets the Event States.
        @return The Event States
      */
    public EventName[] getEventStates() {
        return m_eventStates;
    }
    /**
        Gets the NotifiedEntity.
        @return The Notified Entity.
      */
    public NotifiedEntity getNotifiedEntity() {
        return m_notifiedEntity;
    }
    /**
        Gets the Observed Events.
        @return The Observed Events.
      */
    public EventName[] getObservedEvents() {
        return m_observedEvents;
    }
    /**
        Gets the Reason Code.
        @return The Reason Code
      */
    public ReasonCode getReasonCode() {
        return m_getReasonCode;
    }
    /**
        Gets the Requested Events.
        @return The Requested Events.
      */
    public RequestedEvent[] getRequestedEvents() {
        return m_requestedEvents;
    }
    /**
        Gets the Request Identifier.
        @return The Request Identifier.
      */
    public RequestIdentifier getRequestIdentifier() {
        return m_requestIdentifier;
    }
    /**
        Gets the Restart Delay.
        @return The Restart Delay
      */
    public int getRestartDelay() {
        return m_restartDelay;
    }
    /**
        Gets the Restart Method.
        @return The Restart Method
      */
    public RestartMethod getRestartMethod() {
        return m_restartMethod;
    }
    /**
        Gets the Signal Requests.
        @return The Signal Requests
      */
    public EventName[] getSignalRequests() {
        return m_signalRequests;
    }
    /**
        Sets the Bearer Information.
        @param bearerInformation The Bearer Information.
      */
    public void setBearerInformation(BearerInformation bearerInformation) {
        m_bearerInformation = bearerInformation;
    }
    /**
        Sets the endpoint capabilities parameter.
        @param capabilities A list of endpoint capability values.
      */
    public void setCapabilities(CapabilityValue[] capabilities) {
        m_capabilities = capabilities;
    }
    /**
        Sets the Connection Identifiers.
        @param connectionIdentifiers The Connection Identifiers
      */
    public void setConnectionIdentifiers(ConnectionIdentifier[] connectionIdentifiers) {
        m_connectionIdentifiers = connectionIdentifiers;
    }
    /**
        Sets the list of MGCP events that the endpoint has been instructed to detect.
        @param detectEvents The list of MGCP events that the endpoint has been instructed to
        detect.
      */
    public void setDetectEvents(EventName[] detectEvents) {
        m_detectEvents = detectEvents;
    }
    /**
        Sets the Digit Map.
        @param digitMap The Digit Map.
      */
    public void setDigitMap(DigitMap digitMap) {
        m_digitMap = digitMap;
    }
    /**
        Sets the Endpoint Identifier list.
        @param endpointIdentifierList An array of endpoint identifiers.
        @exception java.lang.IllegalArgumentException Thrown if the object reference to the list
        of endpoint identifiers is null-valued, or if any of the endpoint identifiers in the list of
        endpoint identifiers is null-valued.
      */
    public void setEndpointIdentifierList(EndpointIdentifier[] endpointIdentifierList)
        throws java.lang.IllegalArgumentException {
        m_endpointIdentifierList = endpointIdentifierList;
    }
    /**
        Sets the Event States.
        @param eventStates The Event States
      */
    public void setEventStates(EventName[] eventStates) {
        m_eventStates = eventStates;
    }
    /**
        Sets the NotifiedEntity.
        @param notifiedEntity The Notified Entity.
      */
    public void setNotifiedEntity(NotifiedEntity notifiedEntity) {
        m_notifiedEntity = notifiedEntity;
    }
    /**
        Sets the Observed Events.
        @param observedEvents The Observed Events
      */
    public void setObservedEvents(EventName[] observedEvents) {
        m_observedEvents = observedEvents;
    }
    /**
        Sets the Reason Code.
        @param reasonCode The Reason Code
      */
    public void setReasonCode(ReasonCode reasonCode) {
        m_reasonCode = reasonCode;
    }
    /**
        Sets the Requested Events.
        @param requestedEvents The Requested Events.
      */
    public void setRequestedEvents(RequestedEvent[] requestedEvents) {
        m_requestedEvents = requestedEvents;
    }
    /**
        Sets the Request Identifier.
        @param requestIdentifier The Request Identifier
      */
    public void setRequestIdentifier(RequestIdentifier requestIdentifier) {
        m_requestIdentifier = requestIdentifier;
    }
    /**
        Sets the Restart Delay.
        @param restartDelay The Restart Delay
        @exception java.lang.IllegalArgumentException Thrown if restart delay is not in the range
        0-999999.
      */
    public void setRestartDelay(int restartDelay)
        throws java.lang.IllegalArgumentException {
        m_restartDelay = restartDelay;
    }
    /**
        Sets the Restart Method.
        @param restartMethod The Restart Method
      */
    public void setRestartMethod(RestartMethod restartMethod) {
        m_restartMethod = restartMethod;
    }
    /** Sets the Signal Requests.
        @param signalRequests The Signal Requests.
      */
    public void setSignalRequests(EventName[] signalRequests) {
        m_signalRequests = signalRequests;
    }
    /**
        Overrides java.lang.Object.toString().
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
