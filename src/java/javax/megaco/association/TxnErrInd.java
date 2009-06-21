// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: TxnErrInd.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:35 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:35 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class TxnErrInd

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                 |
                 +--javax.megaco.association.TxnErrInd

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class TxnErrInd
   extends AssociationEvent

   The class extends JAIN MEGACO Association Events. This class is
   created by the stack when it needs to send error response for the
   Command event received from the application. The event received could
   be Command request event or the Command response event. On
   encountering error while processing the command event, stack would
   delete the transaction and send error response to the application. If
   there occurs error while processing command request event, then the
   stack would delete the transaction and expect the application to
   re-create the transaction. Similarly if the error occurs while
   processing command response event, then the stack would delete the
   transaction and will re-create the transaction if the transaction is
   retransmitted from peer entity.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   TxnErrInd (java.lang.Object source, int assocHandle, int txnHandle)
       throws javax.megaco.InvalidArgumentException

           Constructs a Transaction Error Indication Event object. This
   object is created by the stack on encountering error while procesing
   command event.

   Method Summary
   final ErrorCode getErrorCode()
           throws javax.megaco.ParameterNotSetException

             Specifies the error code due to which the command event was
   a failure. This method is to be called by the application only on
   identifying that the command event could not be sucessfully processed.
   final void setErrorCode(ErrorCode ecode)
           throws javax.megaco.InvalidArgumentException

             Sets an integer value that identifies the error code which
   further qualifies the error due to which the command event could not
   be processed successfuly.

   Methods inherited from class javax.megaco. CommandEvent
   getAssocHandle, getAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  TxnErrInd

public TxnErrInd(java.lang.Object source, int assocHandle, int txnHandle)
                     throws javax.megaco.InvalidArgumentException

          Constructs a Command Error Indication Event object.


        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of transaction handle passed to this method is
                less than 0.

   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as Transaction Error
          Indication event object. This method overrides the
          corresponding method of the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of Command Error Indication event.
                This is defined as M_TXN_ERR_IND in AssocEventType.
     _________________________________________________________________

  getErrorCode

public final ErrorCode getErrorCode()
                     javax.megaco.ParameterNotSetException

          This method returns the error code qualifying why the Command
          event could not be processed successfuly.


        Returns:
                Returns an integer value that identifies the error code
                specifying why the execution of the Command event could
                not be successful.

        Throws:
                ParameterNotSetException - This exception would be raised
                if the error code is not set.
     _________________________________________________________________

  setErrorCode

public final void setErrorCode(ErrorCode ecode)
                     throws javax.megaco.InvalidArgumentException

          This method sets the error code specifying why the Command
          event request could not be executed successfuly.


        Parameters:
                ecode - The error code correspondingto why the Command
                event request could not be executed successfuly.

        Throws:
                InvalidArgumentException If the error code specified is
                invalid, then this exception is raised.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright © 2001 Hughes Software Systems
       HUGHES SOFTWARE SYSTEMS and JAIN JSPA SIGNATORIES PROPRIETARY
        This document contains proprietary information that shall be
     distributed, routed or made available only within Hughes Software
       Systems and JAIN JSPA Signatory Companies, except with written
                   permission of Hughes Software Systems
             _________________________________________________

   22 December 2003


    If you have any comments or queries, please mail them to
    Jmegaco_hss@hss.hns.com
*/
