// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/CommandType.java <p>
 
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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD |  CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class CommandType

java.lang.Object
  |
  +--javax.megaco.message.CommandType

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class CommandType
   extends java.lang.Object

   Constants used in package javax.megaco.message for defining the type
   of the command, i.e whether a command request or a command response.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_COMMAND_REQ
             Identifies Event object is of type Command Request.
   static int M_COMMAND_RESP
             Identifies Event object is of type Command Response.
   static int M_ACTION_REQ
             Identifies Event object is of type Action Request.
   static int M_ACTION_RESP
             Identifies Event object is of type Action Response.
   static
   CommandType COMMAND_REQ
              Identifies a CommandType object that constructs the class
   with the constant M_COMMAND_REQ.
   static
   CommandType COMMAND_RESP
              Identifies a CommandType object that constructs the class
   with the constant M_COMMAND_RESP.
   static
   CommandType ACTION_REQ
              Identifies a CommandType object that constructs the class
   with the constant M_ACTION_REQ
   static
   CommandType ACTION_RESP
              Identifies a CommandType object that constructs the class
   with the constant M_ACTION_RESP.

   Constructor Summary
   CommandType(int cmd_type)
              Constructs a class initialised with value cmd_type as
   passed to it in the constructor.

   Method Summary
   int getCommandType()
             Returns one of the allowed integer values defined as static
   fields in this class.
   static
   CommandType getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the CommandType object that identifies
   the command type as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_COMMAND_REQ

public static final int M_COMMAND_REQ

          Identifies Event object is of type Command Request. The value
          of this static field shall be set to 1.
     _________________________________________________________________

  M_COMMAND_RESP

public static final int M_COMMAND_RESP

          Identifies Event object is of type Command Response. The value
          of this static field shall be set to 2.
     _________________________________________________________________

  M_ACTION_REQ

public static final int M_ACTION_REQ

          Identifies Event object is of type Action Request. The value of
          this static field shall be set to 3.
     _________________________________________________________________

  M_ACTION_RESP

public static final int M_ACTION_RESP

          Identifies Event object is of type Action Response. The value
          of this static field shall be set to 4.
     _________________________________________________________________

  COMMAND_REQ

public static final CommandType COMMAND_REQ
                    = new CommandType(M_COMMAND_REQ);

          Identifies a CommandType object that constructs the class with
          the constant M_COMMAND_REQ. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  COMMAND_RESP

public static final CommandType COMMAND_RESP
                    = new CommandType(M_COMMAND_RESP);

          Identifies a CommandType object that constructs the class with
          the constant M_COMMAND_RESP. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  ACTION_REQ

public static final CommandType ACTION_REQ
                    = new CommandType(M_ACTION_REQ);

          Identifies a CommandType object that constructs the class with
          the constant M_ACTION_REQ. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  ACTION_RESP

public static final CommandType ACTION_RESP
                    = new CommandType(M_ACTION_RESP);

          Identifies a CommandType object that constructs the class with
          the constant M_ACTION_RESP. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

   Constructor Detail

  CommandType

private CommandType(int cmd_type)

          Constructs a class that initialised with value cmd_type as
          passed to it. The getCommandType method of this class object
          would always return value cmd_type.
     _________________________________________________________________

   Method Detail

  getCommandType

public int getCommandType()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the command
                type, which could to be one of command request, action
                request, command response and action response.
     _________________________________________________________________

  getObject

public static final CommandType getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the CommandType object that identifies the
          command type as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the CommandType object.

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
