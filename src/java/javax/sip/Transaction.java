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
    Transactions are a fundamental component of SIP. A transaction is a request sent by a client
    transaction to a server transaction, along with all responses to that request sent from the
    server transaction back to the client transactions. User agents contain a transaction layer, as
    do stateful proxies. Stateless proxies do not contain a transaction layer. This specification
    provides the capabilities to allow either the SipProvider or SipListener to handle transactional
    functionality. <p> This interface represents a generic transaction interface defining the
    methods common between client and server transactions.
    @see TransactionState
    @version 1.2.2
    @author Monavacon Limited
  */
public interface Transaction extends java.io.Serializable {
    /**
        Gets the dialog object of this transaction object. A dialog only exists for a transaction
        when a session is setup between a User Agent Client and a User Agent Server, either by a 1xx
        Provisional Response for an early dialog or a 200OK Response for a committed dialog. <ul>
        <li> If the stack is configured with the AUTOMATIC_DIALOG_SUPPORT property set to ON (
        default behavior ) then the following behavior is defined: <blockquote><ul> <li> If the
        transaction is associated with an existing Dialog or could result in a Dialog being created
        in the future (ie. the stack is configured to recognize the method as a Dialog creating
        method or is one of the natively supported dialog creating methods such as INVITE, SUBSCRIBE
        or REFER), then the implementation must either associate the transaction with the existing
        Dialog or create a Dialog with null state.  <li> If the Transaction is neither dialog
        creating nor can be associated with an existing dialog, then the implementation must return
        null when the application issues getDialog on the transaction.  </ul></blockquote> <li> If
        the stack is configured with AUTOMATIC_DIALOG property set to OFF then the stack does not
        automatically create a Dialog for a transaction nor does it maintain an association between
        dialog and transaction on behalf of the application. Hence this method will return null. It
        is the responsibility of the application to create a Dialog and associate it with the
        transaction when the response is sent. </ul>
        @return The dialog object of this transaction object or null if no dialog exists.
        @see Dialog
      */
    Dialog getDialog();
    /**
        Returns the current state of the transaction. Returns the current TransactionState of this
        Transaction or null if a ClientTransaction has yet been used to send a message.
        @return A TransactionState object determining the current state of the transaction.
      */
    TransactionState getState();
    /**
        Returns the current value of the retransmit timer in milliseconds used to retransmit
        messages over unreliable transports for this transaction.
        @return The integer value of the retransmit timer in milliseconds.
        @exception java.lang.UnsupportedOperationException Thrown when this method is not supported
        by the underlying implementation.
      */
    int getRetransmitTimer() throws java.lang.UnsupportedOperationException;
    /**
        Sets the value of the retransmit timer to the newly supplied timer value. The retransmit
        timer is expressed in milliseconds and its default value is 500ms. This method allows the
        application to change the transaction retransmit behavior for different networks. For
        example the gateway proxy, the internal intranet is likely to be relatively uncongested and
        the endpoints will be relatively close. The external network is the general Internet. This
        functionality allows different retransmit times for either side.
        @param retransmitTimer The new integer value of the retransmit timer in milliseconds.
        @exception java.lang.UnsupportedOperationException Thrown when this method is not supported
        by the underlying implementation.
      */
    void setRetransmitTimer(int retransmitTimer) throws java.lang.UnsupportedOperationException;
    /**
        Returns a unique branch identifer that identifies this transaction. The branch identifier is
        used in the ViaHeader.  The uniqueness property of the branch ID parameter to facilitate its
        use as a transaction ID, was not part of RFC 2543. The branch ID inserted by an element
        compliant with the RFC3261 specification MUST always begin with the characters "z9hG4bK".
        These 7 characters are used as a magic cookie, so that servers receiving the request can
        determine that the branch ID was constructed to be globally unique. The precise format of
        the branch token is implementation-defined. This method should always return the same branch
        identifier for the same transaction.
        @return The new branch that uniquely identifies this transaction.
      */
    java.lang.String getBranchId();
    /**
        Returns the request that created this transaction. The transaction state machine needs to
        keep the Request that resulted in the creation of this transaction while the transaction is
        still alive. Applications also need to access this information, e.g. a forking proxy server
        may wish to retrieve the original Invite request to cancel branches of a fork when a final
        Response has been received by one branch.
        @return The Request message that created this transaction.
      */
    Request getRequest();
    /**
        This method allows applications to associate application context with the transaction. This
        specification does not define the format of this data, this the responsibility of the
        application and is dependent on the application. This capability may be useful for proxy
        servers to associate the transaction to some application state. The context of this
        application data is un-interpreted by the stack.
        @param applicationData Un-interpreted application data.
        @since v1.2
      */
    void setApplicationData(java.lang.Object applicationData);
    /**
        Returns the application data associated with the transaction.This specification does not
        define the format of this application specific data. This is the responsibility of the
        application.
        @return Application data associated with the transaction by the application.
        @since v1.2
      */
    java.lang.Object getApplicationData();
    /**
        Terminate this transaction and immediately release all stack resources associated with it.
        When a transaction is terminated using this method, a transaction terminated event is sent
        to the listener. If the transaction is already associated with a dialog, it cannot be
        terminated using this method. Instead, the dialog should be deleted to remove the
        transaction.
        @exception ObjectInUseException Thrown when the transaction cannot be terminated as it is
        associated to a dialog.
        @since v1.2
      */
    void terminate() throws ObjectInUseException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
