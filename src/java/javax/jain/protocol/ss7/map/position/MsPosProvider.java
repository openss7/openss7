//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This interface defines the methods required to provide the MS Position Capability on a
  * MAP protocol stack, the implementation of which will be vendor specific. The object
  * that implements the MsPosProvider interface must be named MsPosProviderImpl. <p>
  *
  * The methods defined in this interface will be used by the MAP API User application to
  * send the following primitives (events) to the Provider object:- <ul>
  * <li>MsPosReqEvent, sent with the method processMsPosOperation(MsPosReqEvent event,
  * MsPosListener listener)
  * <li>MsPosRespEvent, sent with the method processMsPosOperation(MsPosRespEvent event,
  * long transactionId)
  * <li>MsPosReportRespEvent, sent with the method
  * processMsPosOperation(MsPosReportRespEvent event, long transactionId) </ul> <p>
  *
  * Requesting the position of a Mobile Station by sending MsPosReqEvent is an "outgoing
  * transaction" because the MAP API User initiates the transaction. The MAP API User can
  * also register listeners for "incoming transactions" with the method addMsPosListener,
  * and de-register those listeners with the method removeMsPosListener.  "Incoming
  * transactions" are transactions initiated by a position client (requesting position
  * informatin) or by the network ("pushing" position information). <p>
  *
  * When registering a listener a userAddress (an SS7Address) is provided to which the
  * listener is listening. If the listener shall listen to several addresses then several
  * calls to addMsPosListener have to be made for the same listener. The provider may only
  * forward events to the listener that have an address that is matching one of the
  * registered listener's addresses. <p>
  *
  * If several listeners are registered with the same userAddress, then the provider must
  * send events for that address to all listeners ("multicast"). The provider must however
  * ensure that only one listener responds to the transaction, by rejecting all but the
  * first listener that responds to the transaction. <p>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface MsPosProvider {
    /** This method is used by the JAIN MAP API User to initiate an "outgoing transaction"
      * for requesting the position of a Mobile Station. The transaction is identified by
      * a transactionId which is allocated by the provider. <p>
      *
      * The listener used to handle the transaction result on an "outgoing transaction" is
      * provided as a parameter in this method (and not by using the addMsPosListener
      * method).
      * @param event  A reference to the MsPosReqEvent object.
      * @param listener  A reference to the listner object that the JAIN MAP API User will
      * use to receive the result of this transaction.
      * @return A transactionId identifying the transaction is returned.
      * @exception SS7MandatoryParameterNotSetException  Thrown if a mandatory parameter
      * of the MsPosReqEvent is not set.
      */
    public long processMsPosOperation(MsPosReqEvent event, MsPosListener listener)
        throws SS7MandatoryParameterNotSetException;
    /** This method is used by the JAIN MAP API User to respond to an "incoming
      * transaction" receiving a position query (MsPosIndEvent), by sending an event of
      * the type MsPosRespEvent to the JAIN MAP API Provider. The transaction is
      * identified by a transactionId which was allocated by the provider when the
      * transaction was opened.
      * @param event  A reference to the event object (a MsPosRespEvent object).
      * @param transactionId  The identifier of the transaction.
      * @exception SS7MandatoryParameterNotSetException  Thrown if a mandatory parameter
      * of the event is not set.
      */
    public void processMsPosOperation(MsPosRespEvent event, long transactionId)
        throws SS7MandatoryParameterNotSetException;
    /** This method is used by the JAIN MAP API User to respond to an "incoming
      * transaction" receiving "pushed" position information (MsPosReportIndEvent), by
      * sending an event of the type MsPosReportRespEvent to the JAIN MAP API Provider.
      * The transaction is identified by a transactionId which was allocated by the
      * provider when the transaction was opened.
      * @param event  A reference to the event object (a MsPosReportRespEvent object).
      * @param transactionId  The identifier of the transaction.
      * @exception SS7MandatoryParameterNotSetException  Thrown if a mandatory parameter
      * of the event is not set.
      */
    public void processMsPosOperation(MsPosReportRespEvent event, long transactionId)
        throws SS7MandatoryParameterNotSetException;
    /** This method is used by the JAIN MAP API User to register listeners with the
      * provider to handle "incoming transactions", i.e. transactions that are initiated
      * when the provider sends MsPosIndEvent or MsPosReportIndEvent to the registered
      * listener. <p>
      *
      * Note: Listeners used to handle events on "outgoing transactions" are not
      * registered with this method, but are sent to the provider as a parameter in the
      * processMsPosOperation method when the MsPosReqEvent is sent.
      * @param listener  The listener to be registered for "incoming transactions".
      * @param userAddress  The SS7Address that the listener is listening to.
      * @exception TooManyListenersException  Thrown if the provider cannot handle any
      * more listeners.  This exception normally is used to indicate that only one
      * listener can be registered!
      */
    public void addMsPosListener(MsPosListener listener, SS7Address userAddress)
        throws java.util.TooManyListenersException;
    /** This method is used to de-register a listener previously registered with the
      * provider to handle "incoming transactions".  If the listener was registered for
      * more than one address it is removed as a listener for all the addresses. <p>
      *
      * Note: The same listener object may be used to handle responsens on an "outgoing
      * transaction" as well as to listen for "incoming transactions". If such a listener
      * is removed with this method it stops listening for "incoming transactions", but
      * it's role for "outgoing transactions" are unaffected, i.e. the listener is used
      * for outgoing transactions until those transactions are completed.
      * @param listener  The reference of the listener to be removed.
      * @exception SS7ListenerNotRegisteredException  Thrown when trying to remove a
      * listener that is not registered with this provider.
      */
    public void removeMsPosListener(MsPosListener listener)
        throws SS7ListenerNotRegisteredException;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
