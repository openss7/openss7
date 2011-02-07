// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: SrvChngMethod.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:15 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:15 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD  | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class SrvChngMethod

java.lang.Object
  |
  +--javax.megaco.association.SrvChngMethod

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
            None
     _________________________________________________________________

   public class SrvChngMethod
   extends java.lang.Object

   Service change method constants used in package
   javax.megaco.association. This forms the class for the Service change
   method parameters of the Jain Megaco package.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_SVC_CHNG_METHOD_GRACEFUL
              Identifies the service change method towards the peer shall
   be graceful.
   static int M_SVC_CHNG_METHOD_FORCED
              Identifies the service change method towards the peer shall
   be forced.
   static int M_SVC_CHNG_METHOD_RESTART
              Identifies the service change method towards the peer shall
   be restart.
   static int M_SVC_CHNG_METHOD_DISCONNECTED
              Identifies the service change method towards the peer shall
   be disconnected.
   static int M_SVC_CHNG_METHOD_HANDOFF
              Identifies the service change method towards the peer shall
   be handoff.
   static int M_SVC_CHNG_METHOD_FAILOVER
              Identifies the service change method towards the peer shall
   be failover.
   static int M_SVC_CHNG_METHOD_EXTENSION
              Identifies the service change method towards the peer shall
   be an extended value.
   static SrvChngMethod SVC_CHNG_METHOD_GRACEFUL
              Identifies a SrvChngMethod object that constructs the class
   with the constant M_SVC_CHNG_METHOD_GRACEFUL.
   static SrvChngMethod SVC_CHNG_METHOD_FORCED
              Identifies a SrvChngMethod object that constructs the class
   with the constant M_SVC_CHNG_METHOD_FORCED.
   static SrvChngMethod SVC_CHNG_METHOD_RESTART
              Identifies a SrvChngMethod object that constructs the class
   with the constant M_SVC_CHNG_METHOD_RESTART.
   static SrvChngMethod SVC_CHNG_METHOD_DISCONNECTED
              Identifies a SrvChngMethod object that constructs the class
   with the constant M_SVC_CHNG_METHOD_DISCONNECTED.
   static SrvChngMethod SVC_CHNG_METHOD_HANDOFF
              Identifies a SrvChngMethod object that constructs the class
   with the constant M_SVC_CHNG_METHOD_HANDOFF.
   static SrvChngMethod SVC_CHNG_METHOD_FAILOVER
              Identifies a SrvChngMethod object that constructs the class
   with the constant M_SVC_CHNG_METHOD_FAILOVER.
   static SrvChngMethod SVC_CHNG_METHOD_EXTENSION
              Identifies a SrvChngMethod object that constructs the class
   with the constant M_SVC_CHNG_METHOD_EXTENSION.

   Constructor Summary
   SrvChngMethod(int service_change_method)
              Constructs a class initialised with value
   service_change_method as passed to it in the constructor.

   Method Summary
   int getSrvChngMethodId()
             Returns one of the allowed integer values defined as static
   fields in this class which shall identify the service change method
   Id.

                                                                   static
                                                            SrvChngMethod

   getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the SrvChngMethod object that
   identifies the service change method as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_SVC_CHNG_METHOD_GRACEFUL

public static final int M_SVC_CHNG_METHOD_GRACEFUL()

          Identifies the service change method code as GRACEFUL.
          Its value shall be set to 1.
     _________________________________________________________________

  M_SVC_CHNG_METHOD_FORCED

public static final int M_SVC_CHNG_METHOD_FORCED()

          Identifies the service change method code as FORCED.
          Its value shall be set to 2.
     _________________________________________________________________

  M_SVC_CHNG_METHOD_RESTART

public static final int M_SVC_CHNG_METHOD_RESTART()

          Identifies the service change method code as RESTART.
          Its value shall be set to 3.
     _________________________________________________________________

  M_SVC_CHNG_METHOD_DISCONNECTED

public static final int M_SVC_CHNG_METHOD_DISCONNECTED()

          Identifies the service change method code as DISCONNECTED.
          Its value shall be set to 4.
     _________________________________________________________________

  M_SVC_CHNG_METHOD_HANDOFF

public static final int M_SVC_CHNG_METHOD_HANDOFF

          Identifies the service change method code as HANDOFF.
          Its value shall be set to 5.
     _________________________________________________________________

  M_SVC_CHNG_METHOD_FAILOVER

public static final int M_SVC_CHNG_METHOD_FAILOVER

          Identifies the service change method code as FAILOVER.
          Its value shall be set to 6.
     _________________________________________________________________

  M_SVC_CHNG_METHOD_EXTENSION

public static final int M_SVC_CHNG_METHOD_EXTENSION

          Identifies the service change method code as EXTENSION.
          Its value shall be set to 7.
     _________________________________________________________________

  SVC_CHNG_METHOD_GRACEFUL

public static final SrvChngMethod SVC_CHNG_METHOD_GRACEFUL
                    = new SrvChngMethod(M_SVC_CHNG_METHOD_GRACEFUL);

          Identifies a SrvChngMethod object that constructs the class
          with the constant M_SVC_CHNG_METHOD_GRACEFUL. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SVC_CHNG_METHOD_FORCED

public static final SrvChngMethod SVC_CHNG_METHOD_FORCED
                    = new SrvChngMethod(M_SVC_CHNG_METHOD_FORCED);

          Identifies a SrvChngMethod object that constructs the class
          with the constant M_SVC_CHNG_METHOD_FORCED. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SVC_CHNG_METHOD_RESTART

public static final SrvChngMethod SVC_CHNG_METHOD_RESTART
                    = new SrvChngMethod(M_SVC_CHNG_METHOD_RESTART);

          Identifies a SrvChngMethod object that constructs the class
          with the constant M_SVC_CHNG_METHOD_RESTART. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SVC_CHNG_METHOD_DISCONNECTED

public static final SrvChngMethod SVC_CHNG_METHOD_DISCONNECTED
                    = new SrvChngMethod(M_SVC_CHNG_METHOD_DISCONNECTED);

          Identifies a SrvChngMethod object that constructs the class
          with the constant M_SVC_CHNG_METHOD_DISCONNECTED. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SVC_CHNG_METHOD_HANDOFF

public static final SrvChngMethod SVC_CHNG_METHOD_HANDOFF
                    = new SrvChngMethod(M_SVC_CHNG_METHOD_HANDOFF);

          Identifies a SrvChngMethod object that constructs the class
          with the constant M_SVC_CHNG_METHOD_HANDOFF. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SVC_CHNG_METHOD_FAILOVER

public static final SrvChngMethod SVC_CHNG_METHOD_FAILOVER
                    = new SrvChngMethod(M_SVC_CHNG_METHOD_FAILOVER);

          Identifies a SrvChngMethod object that constructs the class
          with the constant M_SVC_CHNG_METHOD_FAILOVER. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SVC_CHNG_METHOD_EXTENSION

public static final SrvChngMethod SVC_CHNG_METHOD_EXTENSION
                    = new SrvChngMethod(M_SVC_CHNG_METHOD_EXTENSION);

          Identifies a SrvChngMethod object that constructs the class
          with the constant M_SVC_CHNG_METHOD_EXTENSION. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

   Constructor Detail

  SrvChngMethod

private SrvChngMethod(int srv_chng_method)

          Constructs a class that initialised with value srv_chng_method
          as passed to it. The getSrvChngMethodId method of this class
          object would always return value srv_chng_method.
     _________________________________________________________________

   Method Detail

  getSrvChngMethodId

public int getSrvChngMethodId()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the service
                change method.
     _________________________________________________________________

  getObject

public static final SrvChngMethod getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the SrvChngMethod object that identifies
          the service change method as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the SrvChngMethod object.

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

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
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
