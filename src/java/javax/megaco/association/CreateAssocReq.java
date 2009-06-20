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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class CreateAssocReq

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                 |
                 +--javax.megaco.association.CreateAssocReq

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class CreateAssocReq
   extends AssociationEvent

   The class extends JAIN MEGACO Association Events. This would cause the
   stack to configure the association with the parameters specified and
   initiate the protocol procedures to create an association with the
   peer. If the application is MGC, then the stack would expect
   ServiceChange messages from MG. And  if the application if MG, then
   the stack would send ServiceChange command with the specified
   ServiceChangeMethod and ServiceChangeReason to MGC.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   CreateAssocReq (java.lang.Object source, int assocHandle)

              Constructs a Create Association request event object.

   Method Summary
   LocalAddr getLocalAddr()
              Returns the local entity transport address. If the local
   address is not set, then this shall return a NULL value.
   void setLocalAddr(LocalAddr localAddr)
       throws javax.megaco.InvalidArgumentException

              Sets the local entity transport address for this User Id.

                                                                     void

   setRemoteAddr(RemoteAddr[] remoteAddr)

       throws javax.megaco.InvalidArgumentException

             Sets the list of remote entity transport addresses for this
   User Id.

                                                             RemoteAddr[]

   getRemoteAddr()
              Gets the list of remote entity address for this User Id. If
   the remote address is not set, then this shall return a NULL value.
    int getSrvChngReason()
              Returns the object reference corresponding to the value of
   the service change reason. If Service Change Reason is not set then
   this shall return value 0.
    void  setSrvChngReason(SrvChngReason reason)
       throws javax.megaco.InvalidArgumentException

              Sets one of the valid values of service change reason as
   defined in the class SrvChngReason.

                                                                      int

   getSrvChngMethod()
              Returns the object reference corresponding to the value of
   the service change method. If Service Change Method is not set then
   this shall return value 0.

                                                                     void

   setSrvChngMethod(SrvChngMethod method)
       throws javax.megaco.InvalidArgumentException

              Sets one of the valid values of service change method as
   defined in the class SrvChngMethod.
    LocalAddr getSrvChngAddress()
              Returns the value of the service change address. If this
   returns a null value then it implies that the service change address
   is not present.
    void  setSrvChngAddress(LocalAddr serviceChngAddress)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change address.
    LocalAddr getHandOffMGCId()
              Returns the value of the identity of MGC to which the
   association is to be handed off. If this returns a null value then it
   implies that the Handed Off MGC Id is not present.
    void  setHandOffMGCId(LocalAddr handedOffMGCId)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the identity of MGC to which the
   association is to be hand offed. This method automatically sets the
   Hand off mgcId present to true.

                                                                      int

   getEncodingFormat()
              Returns the encoding format configured for use encoding
   towards peer. If not encoding format is set, it shall return value 0.

                                                                     void

   setEncodingFormat(MegacoEncodingFormant format)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the encoding format.

   Methods inherited from class javax.megaco. AssociationEvent
   getAssocHandle, getAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  CreateAssocReq

public CreateAssocReq(java.lang.Object source, int assocHandle)

          Constructs a Create Association Request Event object.


        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.

   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as create association
          request event. This method overrides the corresponding method
          of the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of create association request event.
                This is defined as M_CREATE_ASSOC_REQ in AssocEventType.
     _________________________________________________________________

  getLocalAddr

public LocalAddr getLocalAddr()

          Gets the local entity transport address. The local address
          specifies the transport address which the stack would use to
          send MEGACO messages to peer.


        Returns:
                Returns the local entity transport address. If the local
                address field is not present, then this method would
                return NULL.
     _________________________________________________________________

  setLocalAddr

   public void setLocalAddr(LocalAddr localAddr)
       throws javax.megaco.InvalidArgumentException

          This method sets the local entity identity. The localAddr
          specifies the transport address which the stack would use to
          send transactions to peer.


        Parameter:
                localAddr - The local entity transport address. Since the
                format of the message header Id is same as that of the
                transport address, the value of this may or may not be
                different from the local entity configured in the user Id
                as specified in the addMegacoListener method of
                MegacoProvider interface.

        Throws:
               InvalidArgumentException : This exception is raised if the
          local transport address specified is invalid.
     _________________________________________________________________

  getRemoteAddr

public RemoteAddr[] getRemoteAddr()

          Gets the List of remote entity transport address for the User
          Id. The remote address specified in the address to which the
          stack would send MEGACO messages.
          There is one to one correspondence between the list of remote
          address specified here and the list of remote entity Ids in the
          UserId class. The messages comming from a particular remote
          entity must have same message header Id as the corresponding
          remote entity Id in UserId class.

    
        Returns:
              Returns the list of remote entity transport address. If the
          remote address parameter is not present, then this method would
          return NULL.
     _________________________________________________________________

  setRemoteAddr

public void setRemoteAddr(RemoteAddr[] remoteAddr)
        throws javax.megaco.InvalidArgumentException

          Sets the list of remote entity identities of the user Id. The
          remote address specified in the address to which the stack
          would send MEGACO messages.
          There is one to one correspondence between the list of remote
          address specified here and the list of remote entity Ids in the
          UserId class. The messages comming from a particular remote
          entity must have same message header Id as the corresponding
          remote entity Id in UserId class.


        Parameters:
                remoteAddr - List of remote entity transport addresses of
                the MGC/MG.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Remote Address passed to this method is
                NULL.
     _________________________________________________________________

  getSrvChngReason

public int getSrvChngReason()

          Gets the integer value which identifies the service change
          reason. This parameter is required on if the application is MG.
          The MG stack would use the same ServiceChange Reason in the
          ServiceChange command request sent to peer MGC.


        Returns:
                Returns the integer value corresponding to the service
                change reason. If the ServiceChangeReason is not set,
                then this method would return value 0. The possible
                values are field constants defined for the class
                SrvChngReason.
     _________________________________________________________________

  setSrvChngReason

public void setSrvChngReason(SrvChngReason reason)
        throws javax.megaco.InvalidArgumentException

          This method sets the service change reason. This parameter is
          required on if the application is MG. The MG stack would use
          the same ServiceChange Reason in the ServiceChange command
          request sent to peer MGC.


        Parameter:
                reason - The object reference to ServiceChange Reason.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change Reason passed to this
                method is NULL.
     _________________________________________________________________

  getSrvChngMethod

public int getSrvChngMethod()

          Gets the integer value which identifies the service change
          method. This parameter is required on if the application is MG.
          The MG stack would use the same ServiceChange Method in
          ServiceChange command request sent to peer MGC.


        Returns:
                Returns the integer value corresponding to the service
                change method. If the ServiceChangeMethod is not set,
                then this method would return value 0. The possible
                values are field constants defined for the class
                SrvChngMethod.
     _________________________________________________________________

  setSrvChngMethod

public void setSrvChngMethod(SrvChngMethod method)
        throws javax.megaco.InvalidArgumentException

          This method sets the service change method. This parameter is
          required on if the application is MG. The MG stack would use
          the same ServiceChange Method in ServiceChange command request
          sent to peer MGC.


        Parameter:
                method - The object reference to ServiceChange Method.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change Method passed to this
                method is NULL.
     _________________________________________________________________

  getSrvChngAddress

public LocalAddr getSrvChngAddress()

          Gets the service change address.


        Returns:
                Returns the service change address. if the service change
                address is not set, then this method would return NULL.


     _________________________________________________________________

  setSrvChngAddress

   public void setSrvChngAddress(LocalAddr serviceChngAddress)
       throws javax.megaco.InvalidArgumentException

          This method sets the service change address.
          The local entity specfied earlier is the transport address
          would be used for initial registration and then subsequently,
          the address specified in the serviceChngAddress field would be
          used for the local transport address.
          If the application is MGC, then on receipt of ServiceChange
          command (on ROOT termination), it will reply with
          ServiceChangeAddress field in serviceChangeReplyDescriptor
          descriptor, with value as set using this method. And If the
          application is MG, then the stack will send the ServiceChange
          command (on ROOT termination) with ServiceChangeAddress field
          in serviceChangeDescriptor descriptor, with value as set using
          this method.


        Parameter:
                The service change address.

        Throws:
               InvalidArgumentException : This exception is raised if the
          service change address specified is invalid.
     _________________________________________________________________

  getHandOffMGCId

public LocalAddr getHandOffMGCId()

          Gets the identity of the MGC to which the association is to be
          handoffed. This parameter may be set if the application is MGC.


        Returns:
                Returns the identity of the MGC to which the association
                is to be handoffed. If the HandedOff MGC Id is missing,
                then method returns NULL.

           
     _________________________________________________________________

  setHandOffMGCId

   public void setHandOffMGCId(LocalAddr handedOffMGCId)
       throws javax.megaco.InvalidArgumentException

          This method sets the identity of the MGC to which the
          association is to be handoffed. This parameter may be set if
          the application is MGC.
          If this parameter is set, then on receipt of service change
          request from MG (with ROOT termination), the stack will reply
          with MgcIdToTry field in serviceChangeDescriptor descriptor,
          with value as set using this method.


        Parameter:
                The identity of the MGC to which the association is to be
                handoffed.

        Throws:
               InvalidArgumentException : This exception is raised if the
          HandedOffMGCId specified is invalid.
     _________________________________________________________________

  getEncodingFormat

public int getEncodingFormat()

          Gets the integer value which identifies the encoding format.


        Returns:
                Returns the integer value which identifies the value of
                encoding format. If the encoding format has not been
                specified, then this method would return value 0. The
                possible values returned are the field constants defined
                for the class EncodingFormat.
     _________________________________________________________________

  setEncodingFormat

public void setEncodingFormat(EncodingFormat format)

        throws javax.megaco.InvalidArgumentException


          This method sets the encoding format which is to be configured.


        Parameter:
                The object reference to derived class of EncodingFormat
                class which gives value of encoding format.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Encoding Format passed to this method is
                NULL.
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
