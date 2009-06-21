// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: AssocIndReason.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:34 $ <p>
 
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

javax.megaco.association
Class AssocIndReason

java.lang.Object
  |
  +--javax.megaco.association.AssocIndReason

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          None
     _________________________________________________________________

   public class AssocIndReason
   extends java.lang.Object

   Constants used in package javax.megaco.association. This forms the
   class for the association state change indication reason. The reasons
   indicate why the association has undergone the state change.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_IND_RCVD_FRM_PEER
               Identifies association state change indication reason to
   be indication received from peer.
     Its value shall be set to 1.
   static int M_RETRIES_XPIRED
               Identifies association state change indication reason to
   be retransmissions expiry.
     Its value shall be set to 2.
   static int M_TIMER_EXP_IND
               Identifies association state change indication reason to
   be timer expiry.
               Its value shall be set to 3.
   static int M_OPER_COMPL_IND
               Identifies association state change indication reason to
   be operation completion.
               Its value shall be set to 4.
   static int M_NW_FAILURE_IND
               Identifies association state change indication reason to
   be network failure.
               Its value shall be set to 5.
   static int M_NW_LINKUP_IND
               Identifies association state change indication reason to
   be network linkup.
               Its value shall be set to 6.

                                                                   static

   AssocIndReason IND_RCVD_FRM_PEER
               Identifies a AssocIndReason object that constructs the
   class with the constant M_IND_RCVD_FRM_PEER.

                                                                   static

   AssocIndReason RETRIES_XPIRED
               Identifies a AssocIndReason object that constructs the
   class with the field constant M_RETRIES_XPIRED.

                                                                   static

   AssocIndReason TIMER_EXP_IND
               Identifies a AssocIndReason object that constructs the
   class with the field constant M_TIMER_EXP_IND.

                                                                   static

   AssocIndReason OPER_COMPL_IND
               Identifies a AssocIndReason object that constructs the
   class with the field constant M_OPER_COMPL_IND.

                                                                   static

   AssocIndReason NW_FAILURE_IND
               Identifies a AssocIndReason object that constructs the
   class with the field constant M_NW_FAILURE_IND.

                                                                   static

   AssocIndReason NW_LINKUP_IND
               Identifies a AssocIndReason object that constructs the
   class with the field constant M_NW_LINKUP_IND.

   Constructor Summary
   AssocIndReason(int ind_reason)
              Constructs a class that initialised with value ind_reason
   as passed to it in the constructor.

   Method Summary
   int getAssocIndReason()
             Returns one of the allowed integer values defined as static
   fields in this class.

                                                                   static

   AssocIndReason getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the AssocIndReason object that
   identifies the association state change indication reason as value
   passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_IND_RCVD_FRM_PEER

public static final int M_IND_RCVD_FRM_PEER

          This association state change indication reason identifies that
          the change in the state is due to ServiceChange command (on
          ROOT termination) received from peer. For example in case MGC
          receives ServiceChange command from MG with FORCED method, then
          the stack would send AssociationInd to application, with
          indication reason set to M_IND_RCVD_FRM_PEER and Service Change
          parameters same as what is received in the command.
          Its value shall be set to 1.
     _________________________________________________________________

  M_RETRIES_XPIRED

public static final int M_RETRIES_XPIRED

          This association state change indication reason identifies that
          the change in the state is due to the retries expiry at the
          stack towards the peer. This therefore indicates that the peer
          is unavailable for communication.
          Its value shall be set to 2.
     _________________________________________________________________

  M_TIMER_EXP_IND

public static final int M_TIMER_EXP_IND

          This association state change indication reason identifies that
          the state change is due to the timer expiry. The stack would
          run the Restart delay and Graceful delay at its end. On expiry
          of the timer, the stack would send AssociationInd to the
          application with indication reason set to M_TIMER_EXP_IND.
           Its value shall be set to 3.
     _________________________________________________________________

  M_OPER_COMPL_IND

public static final int M_OPER_COMPL_IND

          This assciation state change indication reason identifies that
          the state change is due to the completion of the local
          operation as initiated by the application. This would generally
          occur in case the user had modify assoc or delete association
          request. Then on completion of the both local as well as the
          protocol processing, the stack would give such an indication.
          Its value shall be set to 4.
     _________________________________________________________________

  M_NW_FAILURE_IND

public static final int M_NW_FAILURE_IND

          This assciation state change indication reason identifies that
          the state change is due to the failure of the network
          connection towards peer.
          Its value shall be set to 5.
     _________________________________________________________________

  M_NW_LINKUP_IND

public static final int M_NW_LINKUP_IND

          This assciation state change indication reason identifies that
          the state change is due to the availability of the network
          connection towards peer.
          Its value shall be set to 6.
     _________________________________________________________________

  IND_RCVD_FRM_PEER

public static final AssocIndReason IND_RCVD_FRM_PEER
                    = new AssocIndReason(M_IND_RCVD_FRM_PEER);

          Identifies a AssocIndReason object that constructs the class
          with the constant M_IND_RCVD_FRM_PEER. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  RETRIES_XPIRED

public static final AssocIndReason RETRIES_XPIRED
                    = new AssocIndReason(M_RETRIES_XPIRED);

          Identifies a AssocIndReason object that constructs the class
          with the constant M_RETRIES_XPIRED. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  TIMER_EXP_IND

public static final AssocIndReason TIMER_EXP_IND
                    = new AssocIndReason(M_TIMER_EXP_IND);

          Identifies a AssocIndReason object that constructs the class
          with the constant M_TIMER_EXP_IND. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  OPER_COMPL_IND

public static final AssocIndReason OPER_COMPL_IND
                    = new AssocIndReason(M_OPER_COMPL_IND);

          Identifies a AssocIndReason object that constructs the class
          with the constant M_OPER_COMPL_IND. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  NW_FAILURE_IND

public static final AssocIndReason NW_FAILURE_IND
                    = new AssocIndReason(M_NW_FAILURE_IND);

          Identifies a AssocIndReason object that constructs the class
          with the constant M_NW_FAILURE_IND. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  NW_LINKUP_IND

public static final AssocIndReason NW_LINKUP_IND
                    = new AssocIndReason(M_NW_LINKUP_IND);

          Identifies a AssocIndReason object that constructs the class
          with the constant M_NW_LINKUP_IND. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

   Constructor Detail

  AssocIndReason

private AssocIndReason(int ind_reason)

          Constructs a class that initialised with value ind_reason as
          passed to it. The getAssocIndReason method of this class object
          would always return value ind_reason.
     _________________________________________________________________

   Method Detail

  getAssocIndReason

public int getAssocIndReason()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the association
                state change indication reason.
     _________________________________________________________________

  getObject

public static final AssocIndReason getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the AssocIndReason object that identifies
          the association state change indication reason as value passed
          to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the AssocIndReason object.

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
