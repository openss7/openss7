/*
 @(#) src/java/javax/jain/protocol/ss7/map/information/MsInfoListener.java <p>
 
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

/** This interface defines the methods required to be implemented by a MAP API User in
  * order to use the MS Information Capability provided by a MAP protocol stack. The
  * methods of this interface will be used by the MAP API Provider to deliver events to
  * the MAP API User. <p>
  *
  * The MAP API Provider can emit the event MsInfoConfEvent to the Listener object, using
  * the method processMsInfoEvent. <p>
  *
  * The MAP API Provider can also deliver error messages with the method processMapError.
  * <p>
  *
  * One listener object can be used to handle several/all concurrent transactions, in
  * which case the transactionId is used to identify the transactions, or one listener
  * object may be instantiated for each transaction.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface MsInfoListener extends java.util.EventListener {
    /** This method is used by the JAIN MAP API Provider to deliver events of the type
      * MsInfoConfEvent to the JAIN MAP API User.  The transaction is identified by a
      * transactionId that is allocated by the provider when the transaction is opened.
      * @param event  A reference to the MsInfoConfEvent object.  transactionId - The
      * identifier of the transaction.
      */
    public void processMsInfoEvent(MsInfoConfEvent event, long transactionId);
    /** This method is used when a severe error occurs in the provider which is not
      * related to a specific transaction.
      * @param error  A reference to the error event object.
      */
    public void processMapError(MapErrorEvent error);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
