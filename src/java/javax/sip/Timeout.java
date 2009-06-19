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
    This class contains the enumerations that define whether a timeout has occured in the underlying
    implementation. The application gets informed on whether a retransmission or transaction timer
    has expired.  There are two types of Timeout, namely: <ul> <li> RETRANSMIT - This type is used
    to alert an application that the underlying retransmit timer has expired so an application can
    resend the message specific to a transaction. This timer is defaulted to 500ms and is doubled
    each time it is fired until the transaction expires TRANSACTION. The default retransmission
    value can be changed per transaction using Transaction.setRetransmitTimer(int). The RETRANSMIT
    type is exposed to the following applications as follows: <blockquote><ul> <li> User Agent -
    Retransmissions on Invite transactions are the responsibility of the application. This is due to
    the three way handshake for an INVITE Request. All other retransmissions are handled by the
    underlying implementation.  Therefore the application will only receive this Timeout type
    specific to Invite transactions.  <li> User Agent with No Retransmission - an application can
    configure an implementation to handle all retransmissions using property characteristics of the
    SipStack. Therefore a Timeout of this type will not be passed to the application.  The
    application will only get notified when the TRANSACTION occurs.  <li> Stateful Proxy - a
    stateful proxy remembers transaction state about each incoming request and any requests it sends
    as a result of processing the incoming request. The underlying implementation will handle the
    retransmissions of all messages it sends and the application will not receive RETRANSMIT events,
    however the application will get notified of TRANSACTION events. As an Invite transaction is end
    to end a stateful proxy will not handle the retransmissions of messages on an Invite
    transaction, unless it decides to respond to the Invite transaction, in essence becoming an User
    Agent Server and as such should behave as described by the User Agent semantics above bearing in
    mind the retranmission property of the underlying implementation.  <li> Stateless Proxy - as a
    stateless proxy acts as a simple forwarding agent, i.e. it simply forwards every message it
    receives upstream, it keeps no transaction state for messages. The implementation does not
    retransmit messages, therefore an application will not receive RETRANSMIT events on a message
    handled statelessly. If retransmission semantics are required by an application using a
    stateless method, it is the responsibility of the application to provide this feature, not the
    underlying implementation.  </ul></blockquote> <li> TRANSACTION - This type is used to alert an
    application that the underlying transaction has expired. A transaction timeout typically occurs
    at a time 64*T1 were T1 is the initial value of the RETRANSMIT, usually defaulted to 500ms. The
    TRANSACTION type is exposed to the following applications as follows: <blockquote><ul> <li> User
    Agent - All retransmissions except retransmissions on Invite transactions are the responsibility
    of the underlying implementation, i.e. Invite transactions are the responsibility of the
    application. Therefore the application will only recieve TRANSACTION Timeout events on
    transactions that are not Invite transactions.  <li> User Agent with No Retransmission - an
    application can configure an implementation to handle all retransmissions using property
    characteristics of the SipStack. Therefore a TRANSACTION Timeout will be fired to the
    application on any transaction that expires including an Invite transaction.  <li> Stateful
    Proxy - a stateful proxy remembers transaction state about each incoming request and any
    requests it sends as a result of processing the incoming request. The underlying implementation
    handles the retransmissions of all messages it sends and will notify the application of
    TRANSACTION events on any of its transactions. As an Invite transaction is end to end a stateful
    proxy will not handle transaction timeouts on an Invite transaction, unless it decides to
    respond to the Invite transaction, in essence becoming an User Agent Server and as such should
    behave as described by the User Agent semantics above bearing in mind the retransmission
    property of the underlying implementation.  <li> Stateless Proxy - as a stateless proxy acts as
    a simple forwarding agent, i.e. it simply forwards every message it receives upstream, it keeps
    no transaction state of the messages. The implementation does not maintain transaction state,
    therefore an application will not receive TRANSACTION events on a message handled statelessly.
    If transaction timeout semantics are required by an application using a stateless method, it the
    responsibility of the application to provide this feature, not the underlying implementation.
    </ul></blockquote> </ul>
    @version 1.2.2
    @author Monavacon Limited
  */
public final class Timeout implements java.io.Serializable {
    /**
        This constant value indicates the internal value of the Retransmit timeout.  This constant
        has an integer value of 0.
      */
    public static final int _RETRANSMIT = 0;
    /**
        This constant value indicates the internal value of the Transaction timeout.  This constant
        has an integer value of 1.
      */
    public static final int _TRANSACTION = 1;
    /**
        Private constructor for building constant class instances.
      */
    private Timeout(int value) {
        m_value = value;
    }
    /**
        This method returns the object value of the Timeout.
        @param timeout The integer value of the Timeout.
        @return The Timeout Object.
      */
    public Timeout getObject(int timeout) {
        switch (timeout) {
            case _RETRANSMIT:
                return RETRANSMIT;
            case _TRANSACTION:
                return TRANSACTION;
        }
        return null;
    }
    /**
        This method returns the integer value of the Timeout.
        @return The integer value of the Timeout.
      */
    public int getValue() {
        return m_value;
    }
    /**
        This method returns a string version of this class.
        @return The string version of the Timeout.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.sip.Timeout");
        b.append("\n\tm_value = ");
        switch (this.m_value) {
            case _RETRANSMIT:
                b.append("RETRANSMIT");
                break;
            case _TRANSACTION:
                b.append("TRANSACTION");
                break;
        }
        return b.toString();
    }
    /**
        Private internal integer value of the constant class.
      */
    private int m_value;
    /**
        This constant value indicates the "Retransmit" timeout.
      */
    public static final Timeout RETRANSMIT = new Timeout(_RETRANSMIT);
    /**
        This constant value indicates the "Transaction" timeout.
      */
    public static final Timeout TRANSACTION = new Timeout(_TRANSACTION);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
