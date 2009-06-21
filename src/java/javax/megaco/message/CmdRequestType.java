// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: CmdRequestType.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:35 $ <p>
 
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
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD |  CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class CmdRequestType

java.lang.Object
  |
  +--javax.megaco.message.CmdRequestType

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class CmdRequestType
   extends java.lang.Object

   Constants used in package javax.megaco.message

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_ADD_REQ
              Identifies Event object is of type Add Command Request.
   static int M_MODIFY_REQ
              Identifies Event object is of type Modify Command Request.
   static int M_MOVE_REQ
              Identifies Event object is of type Move Command Request.
   static int M_SERVICE_CHANGE_REQ
              Identifies Event object is of type Service Change Command
   Request.
   static int M_NOTIFY_REQ
              Identifies Event object is of type Notify Command Request.
   static int M_AUDIT_VAL_REQ
              Identifies Event object is of type Audit Value Command
   Request.
   static int M_AUDIT_CAP_REQ
              Identifies Event object is of type Audit Capability Command
   Request.
   static int M_SUBTRACT_REQ
              Identifies Event object is of type Subtract Command
   Request.
   static
   CmdRequestType ADD_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_ADD_REQ.
   static
   CmdRequestType MODIFY_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_MODIFY_REQ.
   static
   CmdRequestType MOVE_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_MOVE_REQ
   static
   CmdRequestType SUBTRACT_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_SUBTRACT_REQ.
   static
   CmdRequestType SERVICE_CHANGE_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_SERVICE_CHANGE_REQ.
   static
   CmdRequestType NOTIFY_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_NOTIFY_REQ.
   static
   CmdRequestType AUDIT_VAL_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_AUDIT_VAL_REQ.
   static
   CmdRequestType AUDIT_CAP_REQ
              Identifies a CmdRequestType object that constructs the
   class with the constant M_AUDIT_CAP_REQ.

   Constructor Summary
   CmdRequestType(int cmd_type)
              Constructs a class initialised with value cmd_type as
   passed to it in the constructor.

   Method Summary
   int getCmdRequestType()
             Returns one of the allowed integer values defined as static
   fields in this class.
   static
   CmdRequestType getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the CmdRequestType object that
   identifies the command type as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_ADD_REQ

public static final int M_ADD_REQ

          Identifies Event object is of type Add Command Request. Its
          value shall be set equal to 1.
     _________________________________________________________________

  M_MODIFY_REQ

public static final int M_MODIFY_REQ

          Identifies Event object is of type Modify Command Request. Its
          value shall be set equal to 2.
     _________________________________________________________________

  M_MOVE_REQ

public static final int M_MOVE_REQ

          Identifies Event object is of type Move Command Request. Its
          value shall be set equal to 3.
     _________________________________________________________________

  M_SERVICE_CHANGE_REQ

public static final int M_SERVICE_CHANGE_REQ

          Identifies Event object is of type Service Change Command
          Request. Its value shall be set equal to 4.
     _________________________________________________________________

  M_NOTIFY_REQ

public static final int M_NOTIFY_REQ

          Identifies Event object is of type Notify Command Request. Its
          value shall be set equal to 5.
     _________________________________________________________________

  M_AUDIT_VAL_REQ

public static final int M_AUDIT_VAL_REQ

          Identifies Event object is of type Audit Value Command Request.
          Its value shall be set equal to 6.
     _________________________________________________________________

  M_AUDIT_CAP_REQ

public static final int M_AUDIT_CAP_REQ

          Identifies Event object is of type Audit Capability Command
          Request. Its value shall be set equal to 7.
     _________________________________________________________________

  M_SUBTRACT_REQ

public static final int M_SUBTRACT_REQ

          Identifies Event object is of type Subtract Command Request.
          Its value shall be set equal to 8.
     _________________________________________________________________

  ADD_REQ

public static final CmdRequestType ADD_REQ
                    = new CmdRequestType(M_ADD_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_ADD_REQ. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  MODIFY_REQ

public static final CmdRequestType MODIFY_REQ
                    = new CmdRequestType(M_MODIFY_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_MODIFY_REQ. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  MOVE_REQ

public static final CmdRequestType MOVE_REQ
                    = new CmdRequestType(M_MOVE_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_MOVE_REQ. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  SERVICE_CHANGE_REQ

public static final CmdRequestType SERVICE_CHANGE_REQ
                    = new CmdRequestType(M_SERVICE_CHANGE_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_SERVICE_CHANGE_REQ. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  NOTIFY_REQ

public static final CmdRequestType NOTIFY_REQ
                    = new CmdRequestType(M_NOTIFY_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_NOTIFY_REQ. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  AUDIT_VAL_REQ

public static final CmdRequestType AUDIT_VAL_REQ
                    = new CmdRequestType(M_AUDIT_VAL_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_AUDIT_VAL_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  AUDIT_CAP_REQ

public static final CmdRequestType AUDIT_CAP_REQ
                    = new CmdRequestType(M_AUDIT_CAP_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_AUDIT_CAP_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  SUBTRACT_REQ

public static final CmdRequestType SUBTRACT_REQ
                    = new CmdRequestType(M_SUBTRACT_REQ);

          Identifies a CmdRequestType object that constructs the class
          with the constant M_SUBTRACT_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

   Constructor Detail

  CmdRequestType

private CmdRequestType(int cmd_type)

          Constructs a class that initialised with value cmd_type as
          passed to it. The getCmdRequestType method of this class object
          would always return value cmd_type.
     _________________________________________________________________

   Method Detail

  getCmdRequestType

public int getCmdRequestType()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the command
                request type, which could to be one of static constants
                defined by this class.
     _________________________________________________________________

  getObject

public static final CmdRequestType getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the CmdRequestType object that identifies
          the command type as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the CmdRequestType object.

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
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
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
