/*
 @(#) $RCSfile: ResponseEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:51 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:51 $ by $Author: brian $
 */

package javax.sip;

import javax.sip.message.*;

/**
    This class represents a Response event that is passed from a SipProvider to its SipListener.
    This specification handles the passing of Response messages to the application with the event
    model. An application (SipListener) registers with the SIP protocol stack (SipProvider) and
    listens for Response events from the SipProvider. <p> This specification defines a single
    Response event object to handle all Response messages. The Response event encapsulates the
    Response message that can be retrieved from getResponse(). Therefore the event type of a
    Response event can be determined as follows: <p> <code> eventType ==
    ResponseEvent.getResponse().getStatusCode(); </code> <p> A Response event also encapsulates the
    client transaction upon which the Response is correlated, i.e. the client transaction of the
    Request message upon which this is a Response. <p> ResponseEvent contains the following
    elements: <ul> <li> source - the source of the event i.e. the SipProvider sending the
    ResponseEvent.  <li> clientTransaction - the client transaction this ResponseEvent is associated
    with.  <li> Response - the Response message received on the SipProvider that needs passed to the
    application encapsulated in a ResponseEvent. </ul>
    @version 1.2.2
    @author Monavacon Limited
  */
public class ResponseEvent extends java.util.EventObject {
    /**
        Constructs a ResponseEvent encapsulating the Response that has been received by the
        underlying SipProvider. This ResponseEvent once created is passed to
        SipListener.processResponse(ResponseEvent) method of the SipListener for application
        processing.
        @param source The source of ResponseEvent i.e. the SipProvider.
        @param clientTransaction Client transaction upon which this Response was sent.
        @param response The Response message received by the SipProvider.
      */
    public ResponseEvent(java.lang.Object source, ClientTransaction clientTransaction, Dialog dialog, Response response) {
        super(source);
        m_clientTransaction = clientTransaction;
        m_dialog = dialog;
        m_response = response;
    }
    /**
        Gets the client transaction associated with this ResponseEvent.
        @return Client transaction associated with this ResponseEvent.
      */
    public ClientTransaction getClientTransaction() {
        return m_clientTransaction;
    }
    /**
        Gets the Response message encapsulated in this ResponseEvent.
        @return The response associated with this ResponseEvent.
      */
    public Response getResponse() {
        return m_response;
    }
    /**
        Gets the Dialog associated with the event or null if no dialog exists. This method separates
        transaction support from dialog support. This enables application developers to access the
        dialog associated to this event without having to query the transaction associated to the
        event. For example the transaction associated with the event may return 'null' because the
        final response for the transaction has already been received and the stack has no more
        record of the transaction.  This situation can occur when a UAC sends requests out through a
        forking proxy. Responses that all refer to the same transaction may be sent by the targets
        of the fork but each response may be stamped with a different To tag, thus referring to
        different Dialogs on the UAC. The first final response terminates the transaction but the
        UAC may want to create a Dialog on a subsequent response.
        @return The dialog associated with the response event or null if there is no dialog.
        @since v1.2
      */
    public Dialog getDialog() {
        return m_dialog;
    }
    private ClientTransaction m_clientTransaction;
    private Dialog m_dialog;
    private Response m_response;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
