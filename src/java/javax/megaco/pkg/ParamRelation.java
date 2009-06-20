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

javax.megaco.pkg
Class ParamRelation

java.lang.Object
  |
  +--javax.megaco.pkg.ParamRelation

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          None
     _________________________________________________________________

   public class ParamRelation
   extends java.lang.Object

   Param Relation constants used in package javax.megaco.pkg. This
   defines the param relation for the megaco package.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_PARAM_REL_SET
              Identifies param relation as SET.
   Its value shall be set to 1.
   static int M_PARAM_REL_SUBLIST
              Identifies param relation as SUBLIST.
   Its value shall be set to 2.
   static int M_PARAM_REL_RANGE
              Identifies param relation as RANGE.
   Its value shall be set to 3.
   static int M_PARAM_REL_EQUAL
              Identifies param relation as EQUAL.
   Its value shall be set to 4.
   static int M_PARAM_REL_NOT_EQUAL
              Identifies param relation as NOT EQUAL.
   Its value shall be set to 5.
   static int M_PARAM_REL_GREATER
              Identifies param relation as GREATER.
   Its value shall be set to 6.
   static int M_PARAM_REL_LESS
              Identifies param relation as LESS.
   Its value shall be set to 7.

                                                                   static

   ParamRelation PARAM_REL_SET
              Identifies a ParamRelation object that constructs the class
   with the constant M_PARAM_REL_SET.

                                                                   static

   ParamRelation PARAM_REL_SUBLIST
              Identifies a ParamRelation object that constructs the class
   with the constant M_PARAM_REL_SUBLIST.

                                                                   static

   ParamRelation PARAM_REL_RANGE
              Identifies a ParamRelation object that constructs the class
   with the constant M_PARAM_REL_RANGE.
   static ParamRelation PARAM_REL_EQUAL
              Identifies a ParamRelation object that constructs the class
   with the constant M_PARAM_REL_EQUAL.

                                                                   static

   ParamRelation PARAM_REL_NOT_EQUAL
              Identifies a ParamRelation object that constructs the class
   with the constant M_PARAM_REL_NOT_EQUAL.

                                                                   static

   ParamRelation PARAM_REL_GREATER
              Identifies a ParamRelation object that constructs the class
   with the constant M_PARAM_REL_GREATER.

                                                                   static

   ParamRelation PARAM_REL_LESS
              Identifies a ParamRelation object that constructs the class
   with the constant M_PARAM_REL_LESS.

   Constructor Summary
   ParamRelation(int param_relation)
              Constructs a class initialised with value Parameter
   relation as passed to it in the constructor.

   Method Summary
   int getParamRelation()
             Returns one of the allowed integer values defined as static
   fields in this class which shall identify one of the parameter
   relations.

                                                                   static

   ParamRelation getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the ParamRelation object that
   identifies the parameter relation as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_PARAM_REL_SET

public static final int M_PARAM_REL_SET

          Identifies param relation to be SET.
          Its value shall be set to 1.
     _________________________________________________________________

  M_PARAM_REL_SUBLIST

public static final int M_PARAM_REL_SUBLIST

          Identifies param relation to be SUBLIST.
          Its value shall be set to 2.
     _________________________________________________________________

  M_PARAM_REL_RANGE

public static final int M_PARAM_REL_RANGE

          Identifies param relation to be RANGE.
          Its value shall be set to 3.
     _________________________________________________________________

  M_PARAM_REL_EQUAL

public static final int M_PARAM_REL_EQUAL

          Identifies param relation to be EQUAL.
          Its value shall be set to 4.
     _________________________________________________________________

  M_PARAM_REL_NOT_EQUAL

public static final int M_PARAM_REL_NOT_EQUAL

          Identifies param relation to be NOT EQUAL.
          Its value shall be set to 5.
     _________________________________________________________________

  M_PARAM_REL_GREATER

public static final int M_PARAM_REL_GREATER

          Identifies param relation to be GREATER.
          Its value shall be set to 6.
     _________________________________________________________________

  M_PARAM_REL_LESS

public static final int M_PARAM_REL_LESS

          Identifies param relation to be LESS.
          Its value shall be set to 7.
     _________________________________________________________________

  PARAM_REL_SET

public static final ParamRelation PARAM_REL_SET

        = new ParamRelation(M_PARAM_REL_SET);

          Identifies a Parameter Relation Type object that constructs the
          class with the constant M_PARAM_REL_SET. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  PARAM_REL_SUBLIST

public static final ParamRelation PARAM_REL_SUBLIST

        = new ParamRelation(M_PARAM_REL_SUBLIST);

          Identifies a Parameter Relation Type object that constructs the
          class with the constant M_PARAM_REL_SUBLIST. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  PARAM_REL_RANGE

public static final ParamRelation PARAM_REL_RANGE

        = new ParamRelation(M_PARAM_REL_RANGE);

          Identifies a Parameter Relation Type object that constructs the
          class with the constant M_PARAM_REL_RANGE. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  PARAM_REL_EQUAL

public static final ParamRelation PARAM_REL_EQUAL

        = new ParamRelation(M_PARAM_REL_EQUAL);

          Identifies a Parameter Relation Type object that constructs the
          class with the constant M_PARAM_REL_EQUAL. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  PARAM_REL_NOT_EQUAL

public static final ParamRelation PARAM_REL_NOT_EQUAL

        = new ParamRelation(M_PARAM_REL_NOT_EQUAL);

          Identifies a Parameter Relation Type object that constructs the
          class with the constant M_PARAM_REL_NOT_EQUAL. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  PARAM_REL_GREATER

public static final ParamRelation PARAM_REL_GREATER

        = new ParamRelation(M_PARAM_REL_GREATER);

          Identifies a Parameter Relation Type object that constructs the
          class with the constant M_PARAM_REL_GREATER. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  PARAM_REL_LESS

public static final ParamRelation PARAM_REL_LESS

        = new ParamRelation(M_PARAM_REL_LESS);

          Identifies a Parameter Relation Type object that constructs the
          class with the constant M_PARAM_REL_LESS. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

   Constructor Detail

  ParamRelation

private ParamRelation(int relation_type)

          Constructs a ParamRelation class object initialised with value
          Parameter relation as passed to it.
     _________________________________________________________________

   Method Detail

  getParamRelation

public abstract int getParamRelation()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the termination
                type to be one of SET, or SUBLIST or RANGE or EQUAL or
                NOT EQUAL or GREATER or LESS.
     _________________________________________________________________

  getObject

public static final ParamRelation getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the ParamRelation object that identifies
          the parameter relation type as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the ParamRelation object.

        throws:
                IllegalArgumentException() - If the value passed to this
                method is invalid, then this exception is raised.
     _________________________________________________________________

  readResolve

private Object readResolve()

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
