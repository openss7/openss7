// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/CmdResponseType.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD |  CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class CmdResponseType

java.lang.Object
  |
  +--javax.megaco.message.CmdResponseType

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class CmdResponseType
   extends java.lang.Object

   Constants used in package javax.megaco.message

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_ADD_RESP
              Identifies Event object is of type Add Command Response.
   static int M_MODIFY_RESP
              Identifies Event object is of type Modify Command Response.
   static int M_MOVE_RESP
              Identifies Event object is of type Move Command Response.
   static int M_SERVICE_CHANGE_RESP
              Identifies Event object is of type Service Change Command
   Response.
   static int M_NOTIFY_RESP
              Identifies Event object is of type Notify Command Response.
   static int M_AUDIT_VAL_RESP
              Identifies Event object is of type Audit Value Command
   Response.
   static int M_AUDIT_CAP_RESP
              Identifies Event object is of type Audit Capability Command
   Response.
   static int M_SUBTRACT_RESP
              Identifies Event object is of type Subtract Command
   Response.
   static
   CmdResponseType ADD_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_ADD_RESP.
   static
   CmdResponseType MODIFY_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_MODIFY_RESP.
   static
   CmdResponseType MOVE_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_MOVE_RESP
   static
   CmdResponseType SUBTRACT_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_SUBTRACT_RESP.
   static
   CmdResponseType SERVICE_CHANGE_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_SERVICE_CHANGE_RESP.
   static
   CmdResponseType NOTIFY_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_NOTIFY_RESP.
   static
   CmdResponseType AUDIT_VAL_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_AUDIT_VAL_RESP.
   static
   CmdResponseType AUDIT_CAP_RESP
              Identifies a CmdResponseType object that constructs the
   class with the constant M_AUDIT_CAP_RESP.

   Constructor Summary
   CmdResponseType(int cmd_type)
              Constructs a class initialised with value cmd_type as
   passed to it in the constructor.

   Method Summary
   int getResponseType()
             Returns one of the allowed integer values defined as static
   fields in this class.
   static
   CmdResponseType getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the CmdResponseType object that
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

  M_ADD_RESP

public static final int M_ADD_RESP

          Identifies Event object is of type Add Command Response. Its
          value shall be set equal to 1.
     _________________________________________________________________

  M_MODIFY_RESP

public static final int M_MODIFY_RESP

          Identifies Event object is of type Modify Command Response. Its
          value shall be set equal to 2.
     _________________________________________________________________

  M_MOVE_RESP

public static final int M_MOVE_RESP

          Identifies Event object is of type Move Command Response. Its
          value shall be set equal to 3.
     _________________________________________________________________

  M_SERVICE_CHANGE_RESP

public static final int M_SERVICE_CHANGE_RESP

          Identifies Event object is of type Service Change Command
          Response. Its value shall be set equal to 4.
     _________________________________________________________________

  M_NOTIFY_RESP

public static final int M_NOTIFY_RESP

          Identifies Event object is of type Notify Command Response. Its
          value shall be set equal to 5.
     _________________________________________________________________

  M_AUDIT_VAL_RESP

public static final int M_AUDIT_VAL_RESP

          Identifies Event object is of type Audit Value Command
          Response. Its value shall be set equal to 6.
     _________________________________________________________________

  M_AUDIT_CAP_RESP

public static final int M_AUDIT_CAP_RESP

          Identifies Event object is of type Audit Capability Command
          Response. Its value shall be set equal to 7.
     _________________________________________________________________

  M_SUBTRACT_RESP

public static final int M_SUBTRACT_RESP

          Identifies Event object is of type Subtract Command Response.
          Its value shall be set equal to 8.
     _________________________________________________________________

  ADD_RESP

public static final CmdResponseType ADD_RESP
                    = new CmdResponseType(M_ADD_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_ADD_RESP. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  MODIFY_RESP

public static final CmdResponseType MODIFY_RESP
                    = new CmdResponseType(M_MODIFY_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_MODIFY_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MOVE_RESP

public static final CmdResponseType MOVE_RESP
                    = new CmdResponseType(M_MOVE_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_MOVE_RESP. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  SERVICE_CHANGE_RESP

public static final CmdResponseType SERVICE_CHANGE_RESP
                    = new CmdResponseType(M_SERVICE_CHANGE_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_SERVICE_CHANGE_RESP. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  NOTIFY_RESP

public static final CmdResponseType NOTIFY_RESP
                    = new CmdResponseType(M_NOTIFY_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_NOTIFY_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  AUDIT_VAL_RESP

public static final CmdResponseType AUDIT_VAL_RESP
                    = new CmdResponseType(M_AUDIT_VAL_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_AUDIT_VAL_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  AUDIT_CAP_RESP

public static final CmdResponseType AUDIT_CAP_RESP
                    = new CmdResponseType(M_AUDIT_CAP_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_AUDIT_CAP_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  SUBTRACT_RESP

public static final CmdResponseType SUBTRACT_RESP
                    = new CmdResponseType(M_SUBTRACT_RESP);

          Identifies a CmdResponseType object that constructs the class
          with the constant M_SUBTRACT_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

   Constructor Detail

  CmdResponseType

private CmdResponseType(int cmd_type)

          Constructs a class that initialised with value cmd_type as
          passed to it. The getResponseType method of this class object
          would always return value cmd_type.
     _________________________________________________________________

   Method Detail

  getResponseType

public int getResponseType()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the command
                Response type, which could to be one of static constants
                defined by this class.
     _________________________________________________________________

  getObject

public static final CmdResponseType getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the CmdResponseType object that identifies
          the command type as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the CmdResponseType object.

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
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
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
