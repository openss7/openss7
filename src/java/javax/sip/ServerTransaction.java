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

package javax.sip;

import javax.sip.message.*;

/**
    A server transaction is used by a SipProvider to handle incoming Request messages to fire
    Request events to the SipListener on a specific server transaction and by a User Agent Server
    application to send Response messages to a User Agent Client application. This interfaces
    enables an application to send a Response to a recently received Request in a transaction
    stateful way. <p> A new server transaction is generated in the following ways: <ul> <li> By the
    application by invoking the SipProvider.getNewServerTransaction(Request) for Requests that the
    application wishes to handle.  <li> By the SipProvider by automatically populating the server
    transaction of a RequestEvent for Incoming Requests that match an existing Dialog. Note that a
    dialog-stateful application is automatically transaction stateful too </ul> A server transaction
    of the transaction layer is represented by a finite state machine that is constructed to process
    a particular request under the covers of a stateful SipProvider. The transaction layer handles
    application-layer retransmissions, matching of responses to requests, and application-layer
    timeouts. <p>  The server transaction Id must be unique within the underlying implementation.
    This Id is commonly taken from the branch parameter in the topmost Via header (for RFC3261
    compliant clients), but may also be computed as a cryptographic hash of the To tag, From tag,
    Call-ID header field, the Request-URI of the request received (before translation), the topmost
    Via header, and the sequence number from the CSeq header field, in addition to any Proxy-Require
    and Proxy-Authorization header fields that may be present. The algorithm used to determine the
    id is implementation-dependent. <p> For the detailed server transaction state machines refer to
    Chapter 17 of RFC 3261, the allowable transitions are summarized below: <p> Invite Transaction:
    <br> <code> Proceeding --&gt; Completed --&gt; Confirmed --&gt; Terminated </code> <p>
    Non-Invite Transaction: <br> <code> Trying --&gt; Proceeding --&gt; Completed --&gt; Terminated
    </code>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ServerTransaction extends Transaction {
    /**
        Sends the Response to a Request which is associated with this ServerTransaction. When an
        application wishes to send a Response, it creates a Response using the MessageFactory and
        then passes that Response to this method. The Response message gets sent out on the network
        via the ListeningPoint information that is associated with the SipProvider of this
        ServerTransaction. <p> This method implies that the application is functioning as either a
        UAS or a stateful proxy, hence the underlying implementation acts statefully. When a UAS
        sends a 2xx response to an INVITE, the server transaction is transitions to the
        TerminatedState. The implementation may delay physically removing ServerTransaction record
        from memory to catch retransmissions of the INVITE in accordance with the reccomendation of
        http://bugs.sipit.net/show_bug.cgi?id=769. <p> ACK Processing and final response
        retransmission: If a Dialog is associated with the ServerTransaction then when the UAC sends
        the ACK ( the typical case for User Agents), the Application ( i.e. Listener ) will see a
        ServerTransaction corresponding to the ACK and the corresponding Dialog presented to it. The
        ACK will be presented to the Listener only once in this case. Retransmissions of the OK and
        filtering of ACK retransmission are the responsibility of the Dialog layer of this
        specification. However if no Dialog is associated with the INVITE Transaction, the ACK will
        be presented to the Application with a null Dialog in the RequestEvent and there will be no
        Dialog associated with the ACK Transaction (i.e.  Transaction.getDialog() returns null). In
        this case (when there is no Dialog associated with the original INVITE or ACK) the
        Application is responsible for retransmission of the OK for the INVITE if necessary (i.e. if
        it wants to manage its own dialog layer and function as a User Agent) and for dealing with
        retransmissions of the ACK. This requires that the three way handshake of an INVITE is
        managed by the UAS application and not the implementation of this specification. <p> Note
        that Responses created via Dialog.createReliableProvisionalResponse(int) should be sent
        using Dialog.sendReliableProvisionalResponse(Response)
        @param response The Response to send to the Request.
        @exception SipException Thrown when the SipProvider cannot send the Response for any other
        reason.
        @exception InvalidArgumentException Thrown when the Response is created by
        Dialog.createReliableProvisionalResponse(int) and the application attempts to use this
        method to send the response.
        @see Response
      */
    void sendResponse(Response response) throws SipException, InvalidArgumentException;
    /**
        Enable the timeout retransmit notifications for the ServerTransaction. This method is
        invoked by UAs that do want to be alerted by the stack to retransmit 2XX responses but that
        do NOT want to associate a Dialog. The Default operation is to disable retransmission alerts
        for the Server Transaction when no Dialog is associated with the Server Transaction, as is
        common for a Proxy server. When this method is called, the stack will continue to generate
        Timeout.RETRANSMIT until the application calls Transaction.terminate() or a the listener
        receives a SipListener.processTransactionTerminated(TransactionTerminatedE vent) callback.
        Note that the stack calls SipListener.processTransactionTerminated(TransactionTerminatedE
        vent)asynchronously after it removes the transaction some time after the Transaction state
        is set to TransactionState.TERMINATED ; after which, it maintains no record of the
        Transaction.
        @exception SipException Thrown when a Dialog is already associated with the
        ServerTransaction when the method is called.
        @since 1.2
      */
    void enableRetransmissionAlerts() throws SipException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
