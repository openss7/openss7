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

javax.megaco
Interface MegacoProvider
     _________________________________________________________________

   public interface MegacoProvider

   This interface must be implemented by any class that represents the
   JAIN MEGACO abstraction of an MEGACO stack and that interacts directly
   with a proprietary (vendor-specific) implementation of that MEGACO
   stack.
     _________________________________________________________________

   Method Summary
    int addMegacoListener (MegacoListener listener,
              UserId UserId)
             throws java.util.TooManyListenersException,
              javax.megaco.InvalidUserIdException,
              javax.megaco.AssocHandleExhaustedException,
              javax.megaco.InvalidArgumentException

             Adds a JAIN MEGACO Listener, with a unique user Id, to the
   list of Event Listeners registered on an implementation of the
   MegacoProvider interface. The stack shall allocate the assoc handle
   which would be used by the listener for subsequent communication with
   the stack.
     MegacoStack getMegacoStack()
             Returns an object reference to the underlying JAIN MEGACO
   Stack to which this JAIN MEGACO Provider is bound.
     int  getAssocHandle (UserId UserId)
             throwsjavax.megaco.NonExistentAssocException

             Returns an object reference to Association handle with
   reference to the userId.
    void  removeMegacoListener (MegacoListener MegacoListener,
                             int assocHandle)
             throws javax.megaco.InvalidArgumentException

             Removes the user Id with which the Listener has been
   registered with the Provider. If the specified Listener is registered
   with the Provider, with the only user Id corresponding to which the
   user Id is provided, then removes the Listener from the list of Event
   Listeners.
    void sendMegacoAssocEvent (AssociationEvent jainMegacoAssocEvent)

             Sends a Association event MEGACO commands to the MEGACO
   stack.

                                                                      int

   sendMegacoCmdEvent (CommandEvent jainMegacoCmdEvent)

             Sends a Command event MEGACO commands and responses into the
   MEGACO stack.

   Method Detail

  addMegacoListener

   public int addMegacoListener(MegacoListener listener,
                             UserId UserId)
                      throws java.util.TooManyListenersException,
                                    javax.megaco.InvalidUserIdException,

   javax.megaco.AssocHandleExhaustedException,
                                    javax.megaco.InvalidArgumentException

          Adds a JAIN MEGACO Listener to the list of Event Listeners
          registered on an implementation of the MegacoProvider
          interface. On registration the stack allocates a unique
          identifier called Association identifier for this User Id. The
          association handle is further used by the listener application
          for further communication with the stack. It is also possible
          to have same listener registered with the provider multiple
          times with the different User Id. On every registration the
          provider would generate a new association identifier unique for
          that User Id. In case the User Id specified is already
          registered, then the stack would return the existing
          association identifier. This make it possible to have multiple
          listeners registered with the same association Identifier. In
          this case the stack shall broadcast event to all the registered
          Listeners for that association identifier.


        Parameters:
                listener - A reference to an object that implements the
                MegacoListener interface and that is to be added to the
                list of Event Listeners.
                UserId - A reference to an object of UserId. This
                contains the local, remote entity identities and also
                optionally the listener interface identity. The User Id
                here uniquiely identifies the pair of local and set of
                remote entities. The Local entity Id of UserId will go in
                the "mId" field on the message header for the commands
                emanating from the stack and local entity and vector of
                Remote Ids shall be used by the stack to validate and
                determine the association for messages arriving from the
                peer.

        Throws:
                java.util.TooManyListenersException - Thrown if adding
                this Listener would exceed the number of JAIN MEGACO
                Listeners permitted by the implementation.
                javax.megaco.InvalidUserIdException - Thrown if
                registering with the UserId of the Listener cannot be
                done because already another listener had registered with
                the same local and remote entity and listener instance in
                the userId.
                javax.megaco.AssocHandleExhaustedException - Thrown if
                registering with the UserId of the Listener cannot be
                done because this involves creating new assoc handle and
                the maximum number of assoc handles have already been
                allocated.
                javax.megaco.InvalidArgumentException - Thrown if the
                reference of ListenerId or UserId passed to this method
                is NULL.

        Returns:
                An integer value corresponding to the association. The
                listener is supposed to use this idenfier to seek the
                services of the stack. The same identifier is used to
                send Association and Command events to stack for this
                association.
     _________________________________________________________________

  getMegacoStack

public MegacoStack getMegacoStack()

          Returns an object reference to the underlying JAIN MEGACO Stack
          to which this JAIN MEGACO Provider is bound. The Jain MEGACO
          Provider is bound to the MEGACO stack when the provider object
          is obtained using the MEGACO stack interface.


        Returns:
                An object reference to the underlying JAIN MEGACO Stack
                to which this JAIN MEGACO Provider is bound.
     _________________________________________________________________

  getAssocHandle

public int getAssocHandle(UserId UserId)
      throws javax.megaco.NonExistentAssocException

        Returns an integer value to the assoc handle allocated by the
   stack during registration of the listener for the corresponding user
   Id.

        Parameters:
                UserId - A reference to an object of UserId. This
                contains the local, remote entity identities and also
                optionally the listener interface identity.

        Returns:

          An integer value corresponding to the assoc handle identifier.

        Throws:

          javax.megaco.NonExistentAssocException - Thrown if the no
          listener is registered with the specified user Id.
     _________________________________________________________________

  removeMegacoListener

public void removeMegacoListener(MegacoListener listener, int assocHandle)
      throws javax.megaco.InvalidArgumentException

          Removes the user Id registered for the specified listener from
          the MegacoProvider interface. If the user Id is the only Id
          with which the listener is attached to the provider, then
          removes the JAIN MEGACO Listener from the list of Event
          Listeners registered on an implementation of the MegacoProvider
          interface.


        Parameters:

          listener - A reference to an object that implements the
          MegacoListener interface and that is to be removed from the
          list of Event Listeners.
          assocHandle - An integer value of the association handle
          identity. This is allocated by the stack on registration
          initiated by the listener.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if the
                object reference to the MegacoListener does not
                correspond to a registered Listener, or if the listener
                is not registered for the given assochandle.
     _________________________________________________________________

  sendMegacoAssocEvent

public void sendMegacoAssocEvent(AssociationEvent jainMegacoAssocEvent)

            Sends a MEGACO association level request into the MEGACO
   stack. The association level request includes parameters for control
   association between MG and MGC. This includes parameters for creating,
   modifying and deleting the association with the peer. This also
   includes class for deleting the transactions sent to peer.

        Parameters:
                jainMegacoAssocEvent - A reference to object of class
                AssociationEvent.
     _________________________________________________________________

  sendMegacoCmdEvent

public int sendMegacoCmdEvent(CommandEvent jainMegacoCmdEvent)

     Sends a MEGACO command request or response into the MEGACO stack.
     This includes protocol command request and responses that can be
     exchanged between MG and MGC.

     The integer value would be returned by this method only when
     initiating a new Transaction towards the peer. There are two
     methods of initiating transaction request towards peer. In the
     first case, the value is retuned synchronously as the return value
     of this method and in the other case new allocation is requestd
     asynchronously by application using CreateTxnReq and CreateTxnResp
     sequence.

   Parameters:
       jainMegacoCmdEvent - A refernce to the object of class
   CommandEvent.

   Returns:
           An integer value corresponding to the transaction handle
          identifier.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
     PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
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
