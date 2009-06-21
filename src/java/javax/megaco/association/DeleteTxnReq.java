// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: DeleteTxnReq.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:35 $ <p>
 
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
    PREV CLASSNEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class DeleteTxnReq

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                    |
                    +--javax.megaco.association.DeleteTxnReq

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class DeleteTxnReq
   extends AssociationEvent

   The class extends JAIN MEGACO Association Events. This delete
   transaction request is used for deleting the transaction which have
   been allocated by the stack on indication from the application. This
   class can be used to delete a individual transaction or all the
   transactions. To delete a individual transaction, transaction Id is
   set to a valid value and to delete all the transactions, transaction
   Id is set to 0.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   DeleteTxnReq (java.lang.Object source, int assocHandle)
              Constructs a Delete Association Event object without a
   transaction identifier.
   DeleteTxnReq (java.lang.Object source, int assocHandle, int txnHandle)
       throwsjavax.megaco.InvalidArgumentException

              Constructs a Delete Association Event object with specific
   transaction identifier.

   Method Summary

                                                                final int

   getTxnHandle()
              Gets an object identifier that specifies the transaction
   identifier. If the transaction identifier specified is set to 0, then
   this would be the case when the transaction identifier is to represent
   all transactions. If the transaction identifier is not set, then this
   method returns value 0.
   final void setTxnHandle(int txnHandle)
       throwsjavax.megaco.InvalidArgumentException

              Sets an object identifier that specifies the transaction
   identifier. If the user wants to delete all transactions, then it
   would set the transaction identifier to 0.

   Methods inherited from class javax.megaco.AssociationEvent
   getAssocHandle, getAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  DeleteTxnReq

   public DeleteTxnReq(java.lang.Object source, int assocHandle)

   Constructs a Delete Transaction Request Event object.

        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.
     _________________________________________________________________

  DeleteTxnReq

public DeleteTxnReq(java.lang.Object source, int assocHandle, int txnHandle)
    throws javax.megaco.InvalidArgumentException
     Constructs a Delete Transaction Request Event object with transaction iden
tifier.

        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.
                txnHandle - The transaction handle to uniquely identify
                transaction for which the delete transaction event is to
                be applied.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of transaction handle passed to this method is
                less than 0.



   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as delete transaction
          request event This method overrides the corresponding method of
          the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of delete transaction request event.
                This is defined as M_DELETE_TXN_REQ in AssocEventType.
     _________________________________________________________________

  getTxnHandle

   public final int getTxnHandle()

          Gets an object identifier that specifies the transaction
          identifier. If the transaction identifier is set to 0, then
          this would be the case when the transaction identifier is to
          represent all transactions.
          If the transaction identifier is not set, then this method
          returns 0, indicating all transactions.


        Returns:
                Returns an integer value that specifies the transaction
                identifier.
     _________________________________________________________________

  setTxnHandle

public final void setTxnHandle(int transactionHandle)
          throwsjavax.megaco.InvalidArgumentException

          This method sets the transaction identifier. To delete all
          transactions, the transaction identifier is set to 0.


        Parameters:
                A reference to transaction identifier.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of transaction handle passed to this method is
                less than 0.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASSNEXT CLASS FRAMESNO FRAMES
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
