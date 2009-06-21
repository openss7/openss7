// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: UserId.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:34 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:34 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Use Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
     PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco
Class UserId

java.lang.Object
  |
  +--javax.megaco.UserId
     _________________________________________________________________

   public class UserId
   extends java.lang.Object
   implements java.io.Serializable

   The Class User Id represents an identity of the User Application
   interfacing to a specific remote entity. It therefore includes a Local
   Entity Id and a group of  Remote Entity Ids. The Local Entity Id is
   used for encoding the MEGACO messages sent to peer. This Local Entity
   Id is used as an message header Id in the messages sent to peer. The
   list of Remote Entity Ids is used for validating the message header Id
   in the messages comming from peer. The messges coming from peer must
   have message header Id as specified in the set of Remote Entity Ids. A
   single MEGACO User Application may handle a number of UserIds (for
   example a MGC application handling multiple MGs or multiple MGC
   applications each handling a different MG or MGs.

   Any Event addressed to a User Id will be passed to the User
   application to whom the User Id belongs. The User Id comprises of the:
     * Local Entity id
     * Set of remote entity ids
     * Listener Instance for redundancy, where the stack wants to send
       the event specifically to one of the listeners which cater to same
       user Id.

   It is permitted for more than one MegacoListener to register with the
   same MegacoProvider with the same User Id. In this case events
   occuring for that User Id would be sent to all MegacoListeners
   registered for that User Id. Thus it is up to the implementation of
   JAIN MEGACO application to manage how the Events are handled between
   these Listeners.

   See Also:
        Serialized Form
     _________________________________________________________________

   Constructor Summary
   UserId(java.lang.String localEntId, java.lang.String[]
   remoteEntId)     throwsjavax.megaco.InvalidArgumentException

              Constructs a UserId with the specified local entity id and
   list of remote entity ids.
   UserId(java.lang.String localEntId, java.lang.String[] remoteEntId,
   char listenerInstance)

   throwsjavax.megaco.InvalidArgumentException

              Constructs a UserId with the specified local entity id,
   list of remote entity ids and the listener instance identity. The
   listener instance identity needs to be unique for different listeners
   registering with the same local entity id, remote entity list pair.
   The listener instance is an integer value which is allocated by the
   application while initiating a registration from listener to the
   stack.

   Note: When specifying the Remote Entity address in the CreateAssocReq
   event, there needs to be a one-to-one correspondence in the vector
   elements of the Remote Entity Id list passed in this constructor and
   the vector of addresses passed.
     _________________________________________________________________

   Method Summary
   boolean isListenerInstancePresent()
              Indicates if the Listener Instance is present in this User
   Id.
   void setLocalEntId(java.lang.String localEntId)
       throwsjavax.megaco.InvalidArgumentException

             Sets the Local Entity Identity of this User Id.
    void setRemoteEntId(java.lang.String[] remoteEntId)
       throwsjavax.megaco.InvalidArgumentException

             Sets the list of remote entity ids of this User Id.
   void setListenerInstance(char listenerInstance)
              Sets the Listener Instance of this User Id.
   java.lang.String getLocalEntId()
              Gets the local entity id of the User Id.
   java.lang.String[] getRemoteEntId()
              Gets the list of remote entity ids of the User Id.
   char getListenerInstance()
           throws javax.megaco.ParameterNotSetException

              Gets the Listener instance of the User Id.
   java.lang.String toString()
              Returns a string representation of an instance of this
   class.

   Methods inherited from class java.lang.Object
   equals, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  UserId

public UserId(java.lang.String localEntId, java.lang.String[] remoteEntId)

          throwsjavax.megaco.InvalidArgumentException

          Constructs a UserId with the specified local entity id, list of
          remote entity ids and listener Instance. Using this,
          constructor will automatically set the listener Instance to
          false.

        Parameters:
                localEntId - the Local Entity Identity. If the local
                entity is MG, then this gives the MG identity else shall
                give MGC identity. The localEndId specified is used for
                encoding the message header Id used for the megaco
                messages emanating from the stack to the peer. The format
                of the string specified must be in accordance with the
                syntax defined for "mId" by the protocol.
                remoteEntId - the list of remote entity ids of the User
                Id. If there are number of remote entities, then all the
                remote entities are specified in the user Id and any
                event received from any of these remote entities are
                delivered to the same user Id. The list of remote End Ids
                specified here are one to one in correspondence with the
                remote entity address in the CreateAssocReq.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Local Entity Id or Remote Entity Id
                passed to this method is NULL.
     _________________________________________________________________

  UserId

public UserId(java.lang.String localEntId, java.lang.String[] remoteEntId,
                         char listenerInstance)

          throwsjavax.megaco.InvalidArgumentException

          Constructs a UserId with the specified local entity id, list of
          remote entity ids and listener Instance. Using this,
          constructor will automatically set the listener Instance to
          true.

        Parameters:
                localEntId - the Local Entity Identity. If the local
                entity is MG, then this gives the MG identity else shall
                give MGC identity.  The localEndId specified is used for
                encoding the message header Id used for the megaco
                messages emanating from the stack to the peer. The format
                of the string specified must be in accordance with the
                syntax defined for "mId" by the protocol.
                remoteEntId - the list of remote entity ids of the User
                Id. If there are number of remote entities, then all the
                remote entities are specified in the user Id and any
                event received from any of these remote entities are
                delivered to the same user Id. The list of remote End Ids
                specified here are in correspondence with the remote
                entity address in the CreateAssocReq. The format of the
                string specified must be in accordance with the syntax
                defined for "mId" by the protocol.
                listenerInstance - there could be number of listeners
                catering to the same MG-MGC pair. To uniquely deliver
                event to specific listener out of these, the listener
                instance shall be used. If events need to be broadcast to
                all the listeners catering to the same MG-MGC pair then
                the UserId needs to be referred. The listenerInstance is
                a sequential number. This needs to be allocated by the
                application.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Local Entity Id or Remote Entity Id
                passed to this method is NULL.
     _________________________________________________________________

   Method Detail

  isListenerInstancePresent

public boolean isListenerInstancePresent()

          Specifies if the user Id has listener instance present.


        Returns:
                Returns true if listener instance is present, else
                returns false.
     _________________________________________________________________

  setLocalEntId

public void setLocalEntId(java.lang.String localEntId)

          throwsjavax.megaco.InvalidArgumentException
          Sets the Local Entity Identity of the user Id.


        Parameters:
                localEntId - Local Entity Identity of the MG/MGC. The
                format of the string specified must be in accordance with
                the syntax defined for "mId" by the protocol.

        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Local Entity Id passed to this method is
                NULL.
     _________________________________________________________________

  setRemoteEntId

public void setRemoteEntId(java.lang.String[] remoteEntId)

          throwsjavax.megaco.InvalidArgumentException

          Sets the list of remote entity identities of the user Id.


        Parameters:
                remoteEntId - List of remote entity identities of the
                MGC/MG. The format of the string specified must be in
                accordance with the syntax defined for "mId" by the
                protocol.

        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Remote Entity Id passed to this method
                is NULL.
     _________________________________________________________________

  setListenerInstance

public void setListenerInstance(char listenerInstance)

          Sets the listener instance in the of the user Id to indicate
          uniqueness of the listener which caters to the same remote and
          local entity identities.


        Parameters:
                listenerInstance - Identifies the listener instance
                unique identity for the same local and remote entity.
     _________________________________________________________________

  getLocalEntId

public java.lang.String getLocalEntId()

          Gets the Local Entity Identity of the User Id. If the local
          entity Id field is absent, then the method would return NULL.
     _________________________________________________________________

  getRemoteEntId

public java.lang.String[] getRemoteEntId()

       Gets the List of remote entity Identities of the User Id.
     _________________________________________________________________

  getListenerInstance

public char getListenerInstance()
        throws javax.megaco.ParameterNotSetException

          Gets the Instance of the listener with the same local and
          remote entity identity.

        Throws:
                ParameterNotSetException - this exception is thrown if
                this parameter has not yet been set
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns a string representation of an instance of this class.


        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Overview Package  Class Use Tree Deprecated Index Help
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
