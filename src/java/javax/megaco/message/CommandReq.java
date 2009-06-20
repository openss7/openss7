// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASSNEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class CommandReq

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.CommandEvent
                    |
                    +--javax.megaco.message.CommandReq

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          None
     _________________________________________________________________

   public class CommandReq
   extends CommandEvent

   The class extends JAIN MEGACO Command Events. This class is used to
   represent megaco command request event. Using this class the
   application can send MEGACO command request to peer.

   See Also:
        Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary

   CommandReq (java.lang.Object source, int assocHandle,
                          int txnHandle,
                          int actionHandle,
                          boolean isLastCommandInTxn,
                          boolean isFirstCommandInAction,
                          int cmdRequestIdentifier)

   throws javax.megaco.InvalidArgumentException

              Constructs a Command Request Event object. The constructor
   of this class is private. To create a object of this class, methods
   for every commad has been provided, which returns the reference of the
   object of this class.

   Method Summary
    int getRequestIdentifier()
              Returns an integer value that identifies this event object
   as the type of command request event. It returns whether it is add
   request command or subtract request command or audit value command
   request or audit capability command request or notify request or
   service change request or modify request or move request.
    boolean isCommandOptional()
             Returns true if the command request is optional.
    boolean isReqWithWildcardResp()
             Returns true if the command request sent requests for a
   wildcarded union response.
    boolean setCommandOptional()
              Sets the flag to indicate that the command request is an
   optional command.
    boolean setReqWithWildcardResp()
              Sets the flag to indicate that the command request wishes a
   wildcard union response from the peer MG for a wild carded request.

                                                                   static
                                                               CommandReq

   MegacoCmdReqAdd(java.lang.Object source, int assocHandle, int
   txnHandle, int actionHandle, boolean isLastCommandInTxn, boolean
   isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns refernce to the object of class CommandReq with the
   request identifier set to M_ADD_REQ.

                                                                   static

   CommandReq MegacoCmdReqMove(java.lang.Object source, int assocHandle,
   int txnHandle, int actionHandle, boolean isLastCommandInTxn, boolean
   isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns reference to the object of class CommandReq with
   the request identifier set to M_MOVE_REQ.

                                                                   static

   CommandReq MegacoCmdReqModify(java.lang.Object source, int
   assocHandle, int txnHandle, int actionHandle, boolean
   isLastCommandInTxn, boolean isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns reference to the object of class CommandReq with
   the request identifier set to M_MODIFY_REQ.

                                                                   static

   CommandReq MegacoCmdReqSrvChng(java.lang.Object source, int
   assocHandle, int txnHandle, int actionHandle, boolean
   isLastCommandInTxn, boolean isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns reference to the object of class CommandReq with
   the request identifier set to M_SERVICE_CHANGE_REQ.

                                                                   static

   CommandReq MegacoCmdReqNotify(java.lang.Object source, int
   assocHandle, int txnHandle, int actionHandle, boolean
   isLastCommandInTxn, boolean isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns reference to the object of class CommandReq with
   the request identifier set to M_NOTIFY_REQ.

                                                                   static

   CommandReq MegacoCmdReqAuditVal(java.lang.Object source, int
   assocHandle, int txnHandle, int actionHandle, boolean
   isLastCommandInTxn, boolean isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns reference to the object of class CommandReq with
   the request identifier set to M_AUDIT_VALUE_REQ.

                                                                   static

   CommandReq MegacoCmdReqAuditCap(java.lang.Object source, int
   assocHandle, int txnHandle, int actionHandle, boolean
   isLastCommandInTxn, boolean isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns reference to the object of class CommandReq with
   the request identifier set to M_AUDIT_CAP_REQ.

                                                                   static

   CommandReq MegacoCmdReqSubtract(java.lang.Object source, int
   assocHandle, int txnHandle, int actionHandle, boolean
   isLastCommandInTxn, boolean isFirstCommandInAction)    throws
   javax.megaco.InvalidArgumentException

              Returns reference to the object of class CommandReq with
   the request identifier set to M_SUBTRACT_REQ.

   Methods inherited from class javax.megaco.CommandEvent
   getDescriptor, setDescriptor, getTermination, setTermination,
   getAssocHandle, getTxnHandle, getActionHandle, getCommandIdentifier,
   isLastCommandInTxn, isFirstCommandInAction, setFirstCommandInAction,
   setLastCommandInTxn, setCntxtInfo, getCntxtInfo

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  CommandReq

private CommandReq(java.lang.Object source, int assocHandle,int txnHandle,
                     int actionHandle,
                     boolean isLastCommandInTxn,
                     boolean isFirstCommandInAction,
                     int cmdRequestIdentifier)

              throws javax.megaco.InvalidArgumentException

          Constructs a Command Request Event object. Since the
          constructor if private, the user of this class will create the
          object of CommandReq class using methods MegacoCmdReqAdd,
          MegacoCmdReqMove etc. This restricts the implementation such
          that the command request event objects created will have
          cmdRequestIdentifier as one of the values defined in the class
          CmdRequestType.


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
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.
   cmdRequestIdentifier - Identifies the value of the command request
   identifier for which the command request event class has been created.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.

   Method Detail

  getCommandIdentifier

public final int getCommandIdentifier()

          This method returns that the command identifier is of type
          command request Event. This method overrides the corresponding
          method of the base class CommandEvent.


        Returns:
                Returns an integer value that identifies this event
                object as a command request event i.e. M_COMMAND_REQ.
     _________________________________________________________________

  getRequestIdentifier

public int getRequestIdentifier()

          This method identifies the Command request type of the class
          instance. See javax.megaco.message.CmdRequestType for the
          definition of the constants for the Command Request events.


        Returns:
                Returns an integer value that identifies this event
                object as the type of command request event. It returns
                whether it is add request command or subtract request
                command or audit value command request or audit
                capability command request or notify request or service
                change request or modify request or move request.
     _________________________________________________________________

  isCommandOptional

public boolean isCommandOptional()

          This method indicates the optional command request of the
          MEGACO command request. The object will have this flag set to
          TRUE when a command is received (e.g. ADD, SUB) from the peer
          with the O- option. This indicates that the peer wants other
          commands in the Transaction to be executed even if this one
          fails.


        Returns:
                Returns true if the command request is optional.
     _________________________________________________________________

  isReqWithWildcardResp

public boolean isReqWithWildcardResp()

          This method indicates the wildcarded command response of the
          MEGACO command request. The object will have this flag set to
          TRUE when a command is received (e.g. ADD, SUB) from the peer
          with the W- option. This indicates that the peer wants single
          wildcarded response for the command request which may have
          multiple responses coresponding to it.


        Returns:
                Returns true if the command request wishes the response
                from the peer MG entity in form of wildcard.
     _________________________________________________________________

  setCommandOptional

public void setCommandOptional()

          This method shall be invoked by the application when it wishes
          the stack to prepend "O-" to the command to indicate to the
          peer (MGC or Gateway as the case may be) that this command is
          an optional command and should not block execution of other
          commands in the transaction.
     _________________________________________________________________

  setReqWithWildcardResp

public void setReqWithWildcardResp()

          Sets the flag to indicate that the command request wishes a
          wildcard union response from the peer for a wild carded
          request. If this parameter is set, then the command sent to
          peer has "W-" preceeding the command name.
     _________________________________________________________________

  MegacoCmdReqAdd

public static CommandReq MegacoCmdReqAdd(java.lang.Object source, int assocHand
le,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_ADD_REQ. This method is invoked to
          obtain the object reference of the class CommandReq. This
          method is valid only if the application is MGC. This would be
          used to send ADD command request to MG.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'choose'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
     _________________________________________________________________

  MegacoCmdReqModify

public static CommandReq MegacoCmdReqModify(java.lang.Object source, int assocH
andle,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_MODIFY_REQ. This method is invoked
          to obtain the object reference of the class CommandReq. This
          method is valid only if the application is MGC. This would be
          used to send MODIFY command request to MG.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
     _________________________________________________________________

  MegacoCmdReqMove

public static CommandReq MegacoCmdReqMove(java.lang.Object source, int assocHan
dle,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_MOVE_REQ. This method is invoked to
          obtain the object reference of the class CommandReq. This
          method is valid only if the application is MGC. This would be
          used to send MOVE command request to MG.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
     _________________________________________________________________

  MegacoCmdReqSrvChng

public static CommandReq MegacoCmdReqSrvChng(java.lang.Object source, int assoc
Handle,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_SERVICE_CHANGE_REQ. This method is
          invoked to obtain the object reference of the class CommandReq.
          This method is valid for both MG and MGC. This would be used to
          send ServiceChange command request to peer.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
     _________________________________________________________________

  MegacoCmdReqNotify

public static CommandReq MegacoCmdReqNotify(java.lang.Object source, int assocH
andle,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_NOTIFY_REQ. This method is invoked
          to obtain the object reference of the class CommandReq. This
          method is valid only if the application is MG. This would be
          used to send NOTIFY command request to MGC.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
     _________________________________________________________________

  MegacoCmdReqAuditCap

public static CommandReq MegacoCmdReqAuditCap(java.lang.Object source, int asso
cHandle,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_AUDIT_CAP_REQ. This method is
          invoked to obtain the object reference of the class CommandReq.
          This method is valid only if the application is MGC. This would
          be used to send AuditCapability command request to MG.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
     _________________________________________________________________

  MegacoCmdReqAuditVal

public static CommandReq MegacoCmdReqAuditVal(java.lang.Object source, int asso
cHandle,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_AUDIT_VAL_REQ. This method is
          invoked to obtain the object reference of the class CommandReq.
          This method is valid only if the application is MGC. This would
          be used to send AuditValue command request to MG.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
     _________________________________________________________________

  MegacoCmdReqSubtract

public static CommandReq MegacoCmdReqSubtract(java.lang.Object source, int asso
cHandle,int txnHandle,
                                 int actionHandle,
                                 boolean isLastCommandInTxn,
                                 boolean isFirstCommandInAction)

              throws javax.megaco.InvalidArgumentException

          This method is used for creating the CommandReq class with the
          request identifier set to M_SUBTRACT_REQ. This method is
          invoked to obtain the object reference of the class CommandReq.
          This method is valid only if the application is MGC. This would
          be used to send SUBTRACT command request to MG.



             Parameters:
          source - A reference to the object, the "source", that is
          logically deemed to be the object upon which the Event in
          question initially occurred.
          assocHandle - The association handle to uniquely identify the
          MG-MGC pair. This is allocated by the stack when the Listener
          registers with the provider with a unique MG-MGC identity.
          txnHandle - The transaction handle that shall uniquely identify
          the transaction id for the transaction in which the command
          shall be sent.

    1. The transaction handle is allocated by the stack either on request
       from User application or on receipt of the transaction indication
       from peer.
    2. If the response is to be sent for the transaction received, then
       the application sends the same transaction handle that has been
       received by it in the indication.
    3. If the confirmation is to be sent by the stack to the application
       due to receipt of a response from the peer stack for a request
       sent by the stack, then the transaction handle shall be same as
       received in the command request by the stack.

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

   Note: The action handle can be different from the context id when
   there are multiple action in the same transaction all having context
   id as 'null' or 'choose' or '*'.
   isLastCommandInTxn - This parameter specifies whether the command is
   last command in the transaction. If this flag is set to TRUE, then the
   stack would sent the transaction request to peer.
   isFirstCommandInAction - This parameter specifies whether the command
   is the first command in the action. This is used to identify the
   Action boundaries.

   Throws:
   InvalidArgumentException : This exception is raised if the value of
   transaction handle or the action handle passed to this method is less
   than 0.
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
