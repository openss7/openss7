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

/**
    This class contains the enumerations that define the underlying state of an existing
    transaction. SIP defines four types of transactions, these are Invite Client transactions,
    Invite Server transactions, Non-Invite Client transactions and Non-Invite Server transactions.
    There are six explicit states for the various transactions, namely: <ol> <li> Calling: <ul> <li>
    Invite Client transaction: The initial state, "calling", MUST be entered when the application
    initiates a new client transaction with an INVITE request.</ul> <li> Trying: <ul> <li>
    Non-Invite Client transaction: The initial state "Trying" is entered when the application
    initiates a new client transaction with a request.  <li> Non-Invite Server transaction: The
    initial state "Trying" is entered when the application is passed a request other than INVITE or
    ACK.</ul> <li> Proceeding: <ul> <li> Invite Client transaction: If the client transaction
    receives a provisional response while in the "Calling" state, it transitions to the "Proceeding"
    state.  <li> Non-Invite Client transaction: If a provisional response is received while in the
    "Trying" state, the client transaction SHOULD move to the "Proceeding" state.  <li> Invite
    Server transaction: When a server transaction is constructed for a request, it enters the
    initial state "Proceeding".  <li> Non-Invite Server transaction: While in the "Trying" state, if
    the application passes a provisional response to the server transaction, the server transaction
    MUST enter the "Proceeding" state.</ul> <li> Completed: The "Completed" state exists to buffer
    any additional response retransmissions that may be received, which is why the client
    transaction remains there only for unreliable transports. <ul> <li> Invite Client transaction:
    When in either the "Calling" or "Proceeding" states, reception of a response with status code
    from 300-699 MUST cause the client transaction to transition to "Completed".  <li> Non-Invite
    Client transaction: If a final response (status codes 200-699) is received while in the "Trying"
    or "Proceeding" state, the client transaction MUST transition to the "Completed" state.  <li>
    Invite Server transaction: While in the "Proceeding" state, if the application passes a response
    with status code from 300 to 699 to the server transaction, the state machine MUST enter the
    "Completed" state.  <li> Non-Invite Server transaction: If the application passes a final
    response (status codes 200-699) to the server while in the "Proceeding" state, the transaction
    MUST enter the "Completed" state.</ul> <li> Confirmed: The purpose of the "Confirmed" state is
    to absorb any additional ACK messages that arrive, triggered from retransmissions of the final
    response. Once this time expires the server MUST transition to the "Terminated" state. <ul> <li>
    Invite Server transaction: If an ACK is received while the server transaction is in the
    "Completed" state, the server transaction MUST transition to the "Confirmed" state.</ul> <li>
    Terminated: The transaction MUST be available for garbage collection the instant it enters the
    "Terminated" state. <ul> <li> Invite Client transaction: When in either the "Calling" or
    "Proceeding" states, reception of a 2xx response MUST cause the client transaction to enter the
    "Terminated" state. If amount of time that the server transaction can remain in the "Completed"
    state when unreliable transports are used expires while the client transaction is in the
    "Completed" state, the client transaction MUST move to the "Terminated" state.  <li> Non-Invite
    Client transaction: If the transaction times out while the client transaction is still in the
    "Trying" or "Proceeding" state, the client transaction SHOULD inform the application about the
    timeout, and then it SHOULD enter the "Terminated" state. If the response retransmissions buffer
    expires while in the "Completed" state, the client transaction MUST transition to the
    "Terminated" state.  <li> Invite Server transaction: If in the "Proceeding" state, and the
    application passes a 2xx response to the server transaction, the server transaction MUST
    transition to the "Terminated" state. When the server transaction abandons retransmitting the
    response while in the "Completed" state, it implies that the ACK was never received. In this
    case, the server transaction MUST transition to the "Terminated" state, and MUST indicate to the
    TU that a transaction failure has occurred.  <li> Non-Invite Server transaction: If the request
    retransmissions buffer expires while in the "Completed" state, the server transaction MUST
    transition to the "Terminated" state.</ul> </ol> <p> For each specific transaction state
    machine, refer to RFC3261.
    @version 1.2.2
    @author Monavacon Limited
  */
public final class TransactionState implements java.io.Serializable {
    /**
        This constant value indicates the internal value of the "Calling" constant.  This constant
        has an integer value of 0.
      */
    public static final int _CALLING = 0;
    /**
        This constant value indicates the internal value of the "Trying" constant. This constant has
        an integer value of 1.
      */
    public static final int _TRYING = 1;
    /**
        This constant value indicates the internal value of the "Proceeding" constant.  This
        constant has an integer value of 2.
      */
    public static final int _PROCEEDING = 2;
    /**
        This constant value indicates the internal value of the "Completed" constant.  This constant
        has an integer value of 3.
      */
    public static final int _COMPLETED = 3;
    /**
        This constant value indicates the internal value of the "Confirmed" constant.  This constant
        has an integer value of 4.
      */
    public static final int _CONFIRMED = 4;
    /**
        This constant value indicates the internal value of the "Terminated" constant.  This
        constant has an integer value of 5.
      */
    public static final int _TERMINATED = 5;
    /**
        Private constructor for constant class instances.
      */
    private TransactionState(int value) {
        m_value = value;
    }
    /**
        This method returns the object value of the TransactionState.
        @param transactionState The integer value of the TransactionState.
        @return The TransactionState Object.
      */
    public static TransactionState getObject(int transactionState) {
        switch (transactionState) {
            case _CALLING:
                return CALLING;
            case _TRYING:
                return TRYING;
            case _PROCEEDING:
                return PROCEEDING;
            case _COMPLETED:
                return COMPLETED;
            case _CONFIRMED:
                return CONFIRMED;
            case _TERMINATED:
                return TERMINATED;
        }
        return null;
    }
    /**
        This method returns the integer value of the TransactionState.
        @return The integer value of the TransactionState.
      */
    public int getValue() {
        return m_value;
    }
    /**
        Compare this transaction state for equality with another.
        @param obj The object to which to compare.
        @return True if obj is an instance of this class representing the same transaction state as
        this, false otherwise.
        @since 1.2
      */
    public boolean equals(java.lang.Object obj) {
        if (obj != null && obj instanceof TransactionState) {
            TransactionState other = (TransactionState) obj;
            if (other.m_value == m_value)
                return true;
        }
        return false;
    }
    /**
        Get a hash code value for this transaction state.
        @return A hash code value.
        @since 1.2
      */
    public int hashCode() {
        return m_value;
    }
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.sip.TransactionState");
        b.append("\n\tm_value = ");
        switch (getValue()) {
            case _CALLING:
                b.append("CALLING");
                break;
            case _TRYING:
                b.append("TRYING");
                break;
            case _PROCEEDING:
                b.append("PROCEEDING");
                break;
            case _COMPLETED:
                b.append("COMPLETED");
                break;
            case _CONFIRMED:
                b.append("CONFIRMED");
                break;
            case _TERMINATED:
                b.append("TERMINATED");
                break;
        }
        return b.toString();
    }
    private int m_value;
    /**
        This constant value indicates that the transaction state is "Calling".
      */
    public static final TransactionState CALLING
        = new TransactionState(_CALLING);
    /**
        This constant value indicates that the transaction state is "Trying".
      */
    public static final TransactionState TRYING
        = new TransactionState(_TRYING);
    /**
        This constant value indicates that the transaction state is "Proceeding".
      */
    public static final TransactionState PROCEEDING
        = new TransactionState(_PROCEEDING);
    /**
        This constant value indicates that the transaction state is "Completed".
      */
    public static final TransactionState COMPLETED
        = new TransactionState(_COMPLETED);
    /**
        This constant value indicates that the transaction state is "Confirmed".
      */
    public static final TransactionState CONFIRMED
        = new TransactionState(_CONFIRMED);
    /**
        This constant value indicates that the transaction state is "Terminated".
      */
    public static final TransactionState TERMINATED
        = new TransactionState(_TERMINATED);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
