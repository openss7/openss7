// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: ContextInfoReq.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:16 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:16 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class ContextInfoReq

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.CommandEvent
                    |
                    +--javax.megaco.message.ContextInfoReq

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class ContextInfoReq
   extends CommandEvent

   The class extends JAIN MEGACO Command Events. This class is used to
   represent megaco action request event i.e. when an action comes or is
   to be sent without a command, but has context parameters.
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   ContextInfoReq(java.lang.Object source, int assocHandle,
                         int txnHandle,
                         int actionHandle,
                         boolean isLastActionInTransaction)

       throws javax.megaco.InvalidArgumentException

              Constructs an Action Request Event object.

   Method Summary

   Methods inherited from class javax.megaco. CommandEvent
   getAssocHandle, getTransactionHandle, getActionHandle,
   getCommandIdentifier, isLastActionInTransaction,
   setLastActionInTransaction, setContextId, getContextid

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  ContextInfoReq

public ContextInfoReq(java.lang.Object source, int assocHandle,int transactionH
andle,
                     int actionHandle,
                     boolean isLastActionInTransaction)
        throws javax.megaco.InvalidArgumentException
     Constructs a Context Information Request Event object.

        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.
                transactionHandle - The transaction handle that shall
                uniquely identify the transaction id for the transaction
                in which the command shall be sent.

         1. The transaction handle is allocated by the stack either on
            request from User application or on receipt of the
            transaction indication from peer.
         2. If the response is to be sent for the transaction received,
            then the application sends the same transaction handle that
            has been received by it in the indication.
         3. If the confirmation is to be sent by the stack to the
            application due to receipt of a response from the peer stack
            for a request sent by the stack, then the transaction handle
            shall be same as received in the command request by the
            stack.

          actionHandle - The action handle uniquely identifies the action
          within a transaction. The action handle field is used for
          request-response synchronisation.

    1. If the request is sent from application to the remote entity, then
       the action handle is allocated by the application. On receipt of
       the response from the peer for the same request, the stack will
       use the same action handle when giving the confirmation to the
       application.
    2. If the indication received from stack is to be sent to the
       application, then the action handle is allocated by the stack. The
       response sent by the application to the stack mus have the same
       action handle as received in the indication.

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastActionInTransaction - This parameter specifies whether the
   action is last action in the transaction.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.

   Method Detail

  getCommandIdentifier

public final int getCommandIdentifier()

          This method cannot be overridden by the derived class. This
          method returns that the command identifier is of type command
          request Event. This method overrides the corresponding method
          of the base class CommandEvent.


        Returns:
                Returns an integer value that identifies this event
                object as a action request event i.e. M_ACTION_REQ.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright (C) 2001 Hughes Software Systems
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
