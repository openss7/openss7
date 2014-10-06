// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: AssocState.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:14 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:14 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Use Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class AssocState

java.lang.Object
  |
  +--javax.megaco.association.AssocState

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          None
     _________________________________________________________________

   public class AssocState
   extends java.lang.Object

   Constants used in package javax.megaco.association. This forms the
   class for the association state of the Association package. The
   association here refers to the association w.r.t the peer i.e., MG-MGC
   association. The state here refers to the state of the control
   association between MG-MGC. These states would assist the application
   in making decisions regarding when and what commands to initiate
   towards the peer. For eg. untill the association is in state
   M_ASSOC_STATE_REG_COMPL,  the application should not send any commands
   to the stack. Similarly, if the association is in
   M_ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS state and if the application
   is MGC, then it should refrain from establishing new calls, therefore
   it should not send new ADD commands.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_ASSOC_STATE_IDLE
               Identifies association is in IDLE state.
     Its value shall be set to 1.
   static int M_ASSOC_STATE_REG_IN_PRGS
               Identifies association is in Registration state.
     Its value shall be set to 2.
   static int M_ASSOC_STATE_REG_COMPL
               Identifies association is in Registration completed state.
               Its value shall be set to 3.
   static int M_ASSOC_STATE_DISCONNECTED
               Identifies association is in Disconnected state.
               Its value shall be set to 4.
   static int M_ASSOC_STATE_HANDOFF_IN_PRGS
               Identifies association is in Handoff state.
               Its value shall be set to 5.
   static int M_ASSOC_STATE_FAILOVER_IN_PRGS
               Identifies association is in Handoff state.
               Its value shall be set to 6.
   static int M_ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS
               Identifies association is in Forced shutdown state.
               Its value shall be set to 7.
   static int M_ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS
               Identifies association is in Registration state.
               Its value shall be set to 8.

                                                                   static
                                                               AssocState

   ASSOC_STATE_IDLE
               Identifies a AssocState object that constructs the class
   with the constant M_ASSOC_STATE_IDLE.

                                                                   static
                                                               AssocState

   ASSOC_STATE_REG_IN_PRGS
               Identifies a AssocState object that constructs the class
   with the field constant M_ASSOC_STATE_REG_IN_PRGS.

                                                                   static
                                                               AssocState

   ASSOC_STATE_REG_COMPL
               Identifies a AssocState object that constructs the class
   with the field constant M_ASSOC_STATE_REG_COMPL.

                                                                   static
                                                               AssocState

   ASSOC_STATE_DISCONNECTED
               Identifies a AssocState object that constructs the class
   with the field constant M_ASSOC_STATE_DISCONNECTED.

                                                                   static
                                                               AssocState

   ASSOC_STATE_HANDOFF_IN_PRGS
               Identifies a AssocState object that constructs the class
   with the field constant M_ASSOC_STATE_HANDOFF_IN_PRGS.

                                                                   static
                                                               AssocState

   ASSOC_STATE_FAILOVER_IN_PRGS
               Identifies a AssocState object that constructs the class
   with the field constant M_ASSOC_STATE_FAILOVER_IN_PRGS.

                                                                   static
                                                               AssocState

   ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS
               Identifies a AssocState object that constructs the class
   with the field constant M_ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS.

                                                                   static
                                                               AssocState

   ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS
               Identifies a AssocState object that constructs the class
   with the field constant M_ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS.

   Constructor Summary
   AssocState(int assoc_state)
              Constructs a class that initialised with value assoc_state
   as passed to it in the constructor.

   Method Summary
   int getAssocState()
             Returns one of the allowed integer values defined as static
   fields in this class.

                                                                   static
                                                               AssocState

   getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the AssocState object that identifies
   the association state as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_ASSOC_STATE_IDLE

public static final int M_ASSOC_STATE_IDLE

          Identifies association is in the IDLE state. The association is
          in this state when a Listener registers with the provider for a
          unique UserId. In this state the association database has been
          allocated, and has been initialised with the parameters
          speciified, but no protocol procedures have been initiated as
          yet.
          Its value shall be set to 1.
     _________________________________________________________________

  M_ASSOC_STATE_REG_IN_PRGS

public static final int M_ASSOC_STATE_REG_IN_PRGS

          Identifies association is in the Registration state. This
          association state state identifies that the registration
          procedures as per the MEGACO protocol have been initiated. The
          association is in this state when CreateAssocReq has been sent
          to stack. If the application is MGC, then in this state it will
          wait for MG to send ServiceChange command on ROOT termination.
          And if the application is MG, then it would have sent
          ServiceChange command to MGC with the parameters specified in
          the CreateAssocReq and is now waiting for its response to come.
          Its value shall be set to 2.
     _________________________________________________________________

  M_ASSOC_STATE_REG_COMPL

public static final int M_ASSOC_STATE_REG_COMPL

          Identifies association is in the Registration completed state.
          This association state identifies that the registration
          procedures have been completed and the association is ready for
          exchanging messages with the peer entity.
          Its value shall be set to 3.
     _________________________________________________________________

  M_ASSOC_STATE_DISCONNECTED

public static final int M_ASSOC_STATE_DISCONNECTED

          Identifies association is in the Disconnected state. This
          association state identifies that the the communication with
          the peer entity has been lost. If the application is MG, then
          the stack will try to contact the secondary MGCs and follow the
          procedures as defined by the protocol, and if the application
          is MGC, then it will wait for the MG would come up and send
          registration request again.
          Its value shall be set to 4.
     _________________________________________________________________

  M_ASSOC_STATE_HANDOFF_IN_PRGS

public static final int M_ASSOC_STATE_HANDOFF_IN_PRGS

          Identifies association is in the Handoff state. This state is
          valid only for MG. The association is in this state because it
          has received HANDOFF from MGC. In this state MG is trying to
          re-register with the Handed Off MGC.
          Its value shall be set to 5.
     _________________________________________________________________

  M_ASSOC_STATE_FAILOVER_IN_PRGS

public static final int M_ASSOC_STATE_FAILOVER_IN_PRGS

          Identifies association is in the Failover state. This state is
          valid only only if the application is MGC. The association is
          in this state because it has received Failover from MG
          indicating switcover of active MG to the redundant MG. In this
          state the stack at MGC is waiting for the ServiceChange command
          (on ROOT termination) from the redundant MG.
          Its value shall be set to 6.
     _________________________________________________________________

  M_ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS

public static final int M_ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS

          Identifies association is in the Forced shutdown state. This
          association state identifies that the association is going down
          with FORCED method. The stack in this state is waiting for the
          response of the ServiceChange request sent to peer. On receipt
          of response, the stack would change the association state to
          M_ASSOC_STATE_IDLE.
          Its value shall be set to 7.
     _________________________________________________________________

  M_ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS

public static final int M_ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS

          Identifies association is in the Graceful shutdown state. This
          association state identifies that the association is going down
          with GRACEFUL method. If the Graceful delay is specified, then
          the timer would run in this state.
          Its value shall be set to 8.
     _________________________________________________________________

  ASSOC_STATE_IDLE

public static final AssocState ASSOC_STATE_IDLE
            = new AssocState(M_ASSOC_STATE_IDLE);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_IDLE.
     _________________________________________________________________

  ASSOC_STATE_REG_IN_PRGS

public static final AssocState ASSOC_STATE_REG_IN_PRGS
            = new AssocState(M_ASSOC_STATE_REG_IN_PRGS);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_REG_IN_PRGS. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  ASSOC_STATE_REG_COMPL

public static final AssocState ASSOC_STATE_REG_COMPL
            = new AssocState(M_ASSOC_STATE_REG_COMPL);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_REG_COMPL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  ASSOC_STATE_DISCONNECTED

public static final AssocState ASSOC_STATE_DISCONNECTED
            = new AssocState(M_ASSOC_STATE_DISCONNECTED);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_DISCONNECTED. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  ASSOC_STATE_HANDOFF_IN_PRGS

public static final AssocState ASSOC_STATE_HANDOFF_IN_PRGS
            = new AssocState(M_ASSOC_STATE_HANDOFF_IN_PRGS);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_HANDOFF_IN_PRGS. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  ASSOC_STATE_FAILOVER_IN_PRGS

public static final AssocState ASSOC_STATE_FAILOVER_IN_PRGS
            = new AssocState(M_ASSOC_STATE_FAILOVER_IN_PRGS);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_FAILOVER_IN_PRGS. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS

public static final AssocState ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS
            = new AssocState(M_ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_FORCED_SHUTDOWN_IN_PRGS. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS

public static final AssocState ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS
            = new AssocState(M_ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS);

          Identifies a AssocState object that constructs the class with
          the constant M_ASSOC_STATE_GRACEFUL_SHUTDOWN_IN_PRGS. Since it
          is reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

   Constructor Detail

  AssocState

private AssocState(int assoc_state)

          Constructs a class that initialised with value assoc_state as
          passed to it. The getAssocState method of this class object
          would always return value assoc_state.
     _________________________________________________________________

   Method Detail

  getAssocState

public int getAssocState()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the state of the
                association.
     _________________________________________________________________

  getObject

public static final AssocState getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the AssocState object that identifies the
          association state as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the AssocState object.

        throws:
                IllegalArgumentException() - If the value passed to this
                method is invalid, then this exception is raised.
     _________________________________________________________________

  readResolve

private java.lang.Object readResolve()

        This method must be implemented to perform instance substitution
   during serialization. This method is required for reference
   comparison. This method if not implimented will simply fail each time
   we compare an Enumeration value against a de-serialized Enumeration
   instance.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Use Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD| CONSTR |
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
