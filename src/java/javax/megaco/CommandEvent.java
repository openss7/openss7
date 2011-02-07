// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: CommandEvent.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:13 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:13 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco
Class CommandEvent

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.CommandEvent

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          CommandReq, CommandResp , ContextInfoReq, ContextInfoResp
     _________________________________________________________________

   public abstract class CommandEvent
   extends java.util.EventObject

   The base class for all JAIN MEGACO Command Events. This is an abstract
   class and hence cannot be created as a separate object. This class
   acts as the base class for the Command and Context request and
   response classes.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   CommandEvent (java.lang.Object source,int  assocHandle,
                         int txnHandle,
                         int actionHandle,
                         boolean txn_or_cmd_status,
                         boolean isFirstCommandInAction)

              Constructs a Command Event object. This is an abstract
   class and can be called only by the derived classes.

   Method Summary
    Descriptor[] getDescriptor()
             Gets the Descriptor information for all the descriptor in
   this command.
    int setDescriptor (Descriptor[] descriptorInfo)
       throws javax.megaco.InvalidArgumentException

             Sets the vector of Descriptor Information for this command.
    Termination getTermination()
             Gets the Termination information for this command.
    int setTermination (Termination termination)
       throws javax.megaco.InvalidArgumentException

             Sets the Termination Information for this command.
    int  getAssocHandle()
             Gets the Association Handle for which this command is to be
   sent.
    int getTxnHandle()
             Gets the Transaction Handle for which this command is to be
   sent.
    int getActionHandle()
             Gets the Action Handle for which this command is to be sent.
    abstract int getCommandIdentifier()
             Returns an integer value that identifies this event object
   as a command request event or response event. This is abstract method
   and shall be defined in the derived classes.
    boolean isLastCommandInTxn()
              Returns true if the command is the last command in the
   transaction.
    boolean isFirstCommandInAction()
              Returns true if the command is the first command in the
   Action.
    boolean setFirstCommandInAction()
             Sets the flag to indicate that the command is the first
   command in the Action.
    boolean setLastCommandInTxn()
              Sets the flag to indicate that this command is the last
   command in the transaction.
    void setCntxtInfo (ContextInfo contextInfo)
       throwsjavax.megaco.InvalidArgumentException

              Sets the context id and its related parameters.
    ContextInfo getCntxtInfo()
              Gets the context id and returns an unsigned 32bit value.

                                                                     void

   setExchangeId(int exchangeId)
       throws javax.megaco.InvalidArgumentException

              Sets the exchange Id value. This Id is used for co-relating
   the create transaction request and response. This exchange Id value
   must be same as the value for the exchange Id of the create
   transaction request.

                                                                      int

   getExchangeId()
        Sets the exchange Id value. This Id is used for co-relating the
   create transaction request and response. This exchange Id value must
   be same as the value for the exchange Id of the create transaction
   request.

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  CommandEvent

public CommandEvent(java.lang.Object source,int  assocHandle,int txnHandle,
                     int actionHandle,
                     boolean txn_or_cmd_status,
                     boolean isFirstCommandInAction)

          Constructs a Command Event object. This is an abstract class
          and can be called only by the derived classes.


        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.
                txnHandle - The transaction handle that shall uniquely
                identify the transaction id for the transaction in which
                the command shall be sent.

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


       There are two methods of initiating transacction request towards
       peer. In the first case, the value is retuned synchronously, if
       the transaction handle specified here is 0 and in the other case
       new allocation is requestd asynchronously by application using
       CreateTxnReq/ CreateTxnResp sequence.

   actionHandle - The action handle uniquely identifies the action within
   a transaction. The action handle field is used for request-response
   synchronisation.
    1. If the request is sent from application to the remote entity, then
       the action handle is allocated by the application. On receipt of
       the response from the peer for the same request, the stack will
       use the same action handle when giving the confirmation to the
       application.
    2. If the indication received from stack is to be sent to the
       application, then the action handle is allocated by the stack. The
       response sent by the application to the stack mus have the same
       action handle as received in the indication.

   Note: The context id could have been used as the action handle but
   contention occurs when there are multiple action in the same
   transaction all having context id as 'null' or 'choose' or '*'.
   txn_or_cmd_status - In case of command request, this parameter
   specifies whether the command is last command in the transaction. And
   in case of command response, this parameter specifies whether the
   command response is the last response for the wildcarded request
   received.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action.

   Method Detail

  getDescriptor

public Descriptor[] getDescriptor()

          Gets the Descriptor information for all the descriptor in this
          command.


        Returns:
                The vector of the reference to the object identifier of
                type descriptor information.
     _________________________________________________________________

  setDescriptor

public int setDescriptor(Descriptor[] descriptor)

       throws javax.megaco.InvalidArgumentException

          Sets the vector of Descriptor Information for this command. In
          cases of error it returns M_FAILURE and also sets the error
          code in this object. Else shall return M_SUCCESS.


        Parameters:
                The vector of reference to the object identifier of type
                descriptor information.

        Throws:
               InvalidArgumentException : This exception is raised if the
          reference of vector of Descriptor Ids passed to this method is
          set NULL.
     _________________________________________________________________

  getTermination

public Termination getTermination()

          Gets the Termination for this command.


        Returns:
                The reference to the object identifier of type
                Termination. If no termination is specified for this
                command, then this method returns NULL.
     _________________________________________________________________

  setTermination

public int setTermination(Termination termination)

       throws javax.megaco.InvalidArgumentException

          Sets the Termination for this command. In cases of error
          (termination type mismatch) it returns M_FAILURE and also sets
          the error code in this object. Else shall return M_SUCCESS.


        Parameters:
                The reference to the object identifier of type
                Termination.

        Throws:
          InvalidArgumentException : This exception is raised if the
          reference of Termination Id passed to this method is NULL.
     _________________________________________________________________

  getAssocHandle

public int getAssocHandle()

          Gets the Association Handle for which this command is to be
          sent. This is required by the stack for each command from the
          application to the stack, for stack to identify to which remote
          address the command has to be sent and also from which of the
          local transport it needs to be sent. This is allocated by the
          stack when Listener registers with the provider.


        Returns:
                Reference to the association handle which uniquely
                identifies the MG-MGC pair.
     _________________________________________________________________

  getTxnHandle

public int getTxnHandle()

          Gets the Transaction Handle for which this command is to be
          sent.

       The transaction handle that shall uniquely identify the
   transaction id for the transaction in which the command shall be sent.

          The transaction handle is allocated by the stack either on
                request from User application or on receipt of the
                transaction indication from peer.

          If the response is to be sent for the transaction received,
                then the application sends the same transaction handle
                that has been received by it in the indication.

          If the confirmation is to be sent by the stack to the
                application due to receipt of a response from the peer
                stack for a request sent by the stack, then the
                transaction handle shall be same as received in the
                command request by the stack.

   Returns:
          Reference to the transaction handle which uniquely identifies
          the transaction in which the command has to be sent.
     _________________________________________________________________

  getActionHandle

public int getActionHandle()

          Gets the Action Handle for which this command is to be sent.
          The action handle uniquely identifies the action within a
          transaction. The action handle field is used for
          request-response synchronisation.

          + If the request is sent from application to the remote entity,
            then the action handle is allocated by the application. On
            receipt of the response from the peer for the same request,
            the stack will use the same action handle when giving the
            confirmation to the application.
          + If the indication received from stack is to be sent to the
            application, then the action handle is allocated by the
            stack. The response sent by the application to the stack mus
            have the same action handle as received in the indication.

          Note: The action handle can be different from the context id
          when there are multiple action in the same transaction all
          having context id as 'null' or 'choose' or '*'.


   Returns:
       Reference to the action handle which uniquely identifies the
       action in which the command has to be sent.
     _________________________________________________________________

  getCommandIdentifier

public int getCommandIdentifier()

          This is a virtual method and shall be defined in the derived
          classes.
          See javax.megaco.message.CommandType for the definition of the
          constants for the Command events. This is not set in this
          object but is retrieved from the derived classes. Hence all
          derived classes need to implement this method.


        Returns:
                Returns an integer value that identifies this event
                object as a command request event or a command response
                event.
     _________________________________________________________________

  isLastCommandInTxn

public boolean isLastCommandInTxn()

          Stack or application needs to know whether the current command
          is the last command in the transaction. Stack needs to know
          this so that it can it can now form the MEGACO message and
          hence send the message to the remote stack entity.


        Returns:
                Returns true if the command is the last command in the
                transaction.
     _________________________________________________________________

  isFirstCommandInAction

public boolean isFirstCommandInAction()

          This flag indicates whether new action has started.


        Returns:
                Returns true if the command is the first command in the
                Action.
     _________________________________________________________________

  setFirstCommandInAction

public void isFirstCommandInAction()

          Sets the flag to indicate that the command is the first command
          in the Action.


     _________________________________________________________________

  setLastCommandInTxn

public void setLastCommandInTxn()

          Sets the flag to indicate that this command is the last command
          in the transaction.


     _________________________________________________________________

  setCntxtInfo

public setCntxtInfo(ContextInfo contextInfo)

          throwsjavax.megaco.InvalidArgumentException

              Sets the context id and the related parameters.


        Throws:
   InvalidArgumentException : This exception is raised if the reference
   of context info passed to this method is NULL.
     _________________________________________________________________

  getCntxtInfo

public ContextInfo getCntxtInfo()

          Gets the context id and and the related parameters.


        Returns:
                Returns the object reference for the context info. If no
                context info is specified for this command, then this
                method return NULL.
     _________________________________________________________________

  getExchangeId

public int getExchangeId()

          Gets the exchange identifier value. This identifier is used for
          corellating the Command events and the error response events.
          If no exchange identifier is specified, then this method will
          return 0.


        Returns:
                Returns the exchange identifier value.

           
     _________________________________________________________________

  setExchangeId

   public void setExchangeId(int exchangeId)
       throws javax.megaco.InvalidArgumentException

          This method sets the exchange Id value. This value is used for
          corellating the create transaction request and response. The
          exchange Id value must be same as the value set for the create
          transaction request from the application.


        Parameter:
                exchangeId- The exchange Identifier value.

        Throws:
               InvalidArgumentException : This exception is raised if the
          exchange Id specified is invalid.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
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
