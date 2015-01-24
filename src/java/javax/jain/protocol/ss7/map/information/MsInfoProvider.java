/*
 @(#) src/java/javax/jain/protocol/ss7/map/information/MsInfoProvider.java <p>
 
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

package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This interface defines the methods required to provide the MS Information Capability
  * on a MAP protocol stack, the implementation of which will be vendor specific. The
  * object that implements the MsInfoProvider interface must be named MsInfoProviderImpl.
  * <p>
  *
  * The methods defined in this interface will be used by the MAP API User application to
  * send the following primitives (events) to the Provider object:- <ul>
  * <li>MsInfoReqEvent, sent with the method processMsInfoOperation(MsInfoReqEvent event,
  * MsInfoListener listener) </ul> <p>
  *
  * Requesting information (status and location) of a Mobile Station by sending
  * MsInfoReqEvent is an "outgoing transaction" because the MAP API User initiates the
  * transaction. For the MS Information Capability there are no "incoming transactions",
  * and therefore no methods are needed to add and remove listeners for such transaction.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface MsInfoProvider {
    /** This method is used by the JAIN MAP API User to initiate an "outgoing transaction"
      * for requesting information (status and location) of a Mobile Station. The
      * transaction is identified by a transactionId which is allocated by the provider.
      * <p>
      *
      * The listener used to handle the transaction result on an "outgoing transaction" is
      * provided as a parameter in this method.
      * @param event  A reference to the MsInfoReqEvent object.
      * @param listener  A reference to the listner object that the JAIN MAP API User will
      * use to receive the result of this transaction.
      * @return A transactionId identifying the transaction is returned.
      * @exception SS7MandatoryParameterNotSetException  Thrown if a mandatory parameter
      * of the MsInfoReqEvent is not set.
      */
    public long processMsInfoOperation(MsInfoReqEvent event, MsInfoListener listener)
        throws SS7MandatoryParameterNotSetException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
