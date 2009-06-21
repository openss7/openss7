// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: CreateTxnResp.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:35 $ <p>
 
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
Class CreateTxnResp

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                 |
                 +--javax.megaco.association.CreateTxnResp

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class CreateTxnResp
   extends AssociationEvent

   The class extends JAIN MEGACO Association Events. This class is
   created by the stack when it needs to send back response for the
   create transaction request received from the application.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   CreateTxnResp (java.lang.Object source, int assocHandle,int
   exchangeId)
           Constructs a Create Transaction Response Event object. This
   object is created by the stack.

   Method Summary
   final int getEventStatus()
             Returns an integer value that identifies that the
   association event could be handled successfuly or not. This method is
   called by the application to check the result of the create
   transaction request sent to the stack. The possible values that this
   can take is defined in ReturnStatus.
   final void setEventStatus( ReturnStatus eventStatus)
       throwsjavax.megaco.InvalidArgumentException

             Sets a static object that is derived class of the
   ReturnStatus, that identifies whether the association event could be
   handled successfuly or not.
   final int getErrorCode()
           throws javax.megaco.MethodInvocationException

             Specifies the error code due to which the association event
   was a failure. This method is to be called by the application only on
   identifying that the association event could not be sucessfully
   processed.
   final void setErrorCode(ErrorCode ecode)
           throws javax.megaco.InvalidArgumentException

             Sets an integer value that identifies the error code which
   further qualifies the error due to which the create association event
   could not be processed successfuly.
   final void setTxnHandle(int txnHandle)
       throwsjavax.megaco.InvalidArgumentException

           Sets an integer value that identifies the transaction. This
   identifier is used by the application when sending the command request
   events to stack.
   final void getTxnHandle()
           throws javax.megaco.ParameterNotSetException

             This method returns the Transaction Id as allocated by the
   stack on receipt of the create transaction request from the stack.

                                                                      int

   getExchangeId()

              Returns the exchange Id value. This Id is used for
   co-relating the create transaction request and response. This exchange
   Id value must be same as the value for the exchange Id of the create
   transaction request.

   Methods inherited from class javax.megaco. AssociationEvent
   getAssocHandle, getAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  CreateTxnResp

public CreateTxnResp(java.lang.Object source, int assocHandle,int exchangeId)
                     throws javax.megaco.InvalidArgumentException

          Constructs a Create Transaction Request Event object.


        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.
                exchangeId- The exchange Identifier value.

   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as create association
          request event. This method overrides the corresponding method
          of the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of create transaction request event.
                This is defined as M_CREATE_TXN_RESP in AssocEventType.
     _________________________________________________________________

  getEventStatus

public final int getEventStatus()

          This method returns whether the create transaction event was
          successful or not.


        Returns:
                Returns an integer value that identifies whether the
                create transaction event issued earlier could be
                performed successfuly or not. The values are defined in
                ReturnStatus.
     _________________________________________________________________

  setEventStatus

public final void setEventStatus(ReturnStatus returnStatus)
        throws javax.megaco.InvalidArgumentException

          This method sets the status of whether the processing of the
          create transaction event was successful or not.


        Parameters:
                The return status of the processing of the create
                transaction event. The static object corresponding to the
                return status which are one of the derived classes of the
                ReturnStatus shall be set.

         
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Return Status passed to this method is
                NULL.
     _________________________________________________________________

  getErrorCode

public final int getErrorCode()
                     javax.megaco.ParameterNotSetException

          This method returns the error code qualifying why the create
          transaction event could not be processed successfuly.


        Returns:
                Returns an integer value that identifies the error code
                specifying why the execution of the create transaction
                event could not be successful. The possible values are
                field constants defined for the class ErrorCode.

        Throws:
                MethodInvocationException - This exception would be
                raised if the return status is set to M_SUCCESS, the
                error code is not set and hence should not invoke this
                method.
     _________________________________________________________________

  setErrorCode

public final void setErrorCode(ErrorCode ecode)
                     throws javax.megaco.InvalidArgumentException

          This method sets the error code specifying why the create
          transaction event could not be executed successfuly.


        Parameters:
                ecode - The error code correspondingto why the create
                transaction event could not be executed successfuly.

        Throws:
                InvalidArgumentException If the return status is not set
                to M_FAILURE, the error code should not be set.
     _________________________________________________________________

  setTxnHandle

public final void setTxnHandle(int txnHandle)
                     throws javax.megaco.InvalidArgumentException

          This method sets the transaction identifier. To send multiple
          commands in the same transaction, application may use the same
          transaction identifier for the command request events.


        Parameters:
                txnHandle - The transaction identifier value that has
                been successfully created by the stack.

        Throws:
                InvalidArgumentException This exception is raised if the
                value of transaction handle passed to this method is less
                than or equal to 0.
     _________________________________________________________________

  getTxnHandle

public final int getTxnHandle()
                     throws javax.megaco.ParameterNotSetException

          This method returns the transation Id that has been allocated
          by the stack. The transaction Id was allocatd on receipt of the
          create transaction request from the application.


        Returns:
                The transaction identifier value that has been
                successfully created by the stack.



        Throws:
                ParameterNotSetException If the event status is set to
                M_FAILURE, then this method should not be invoked.
     _________________________________________________________________

  getExchangeId

public int getExchangeId()

          Gets the echange identifier value. This identifier is used for
          corellating the create transaction request and response.


        Returns:
                Returns the exchange identifier value.

           
     _________________________________________________________________
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
