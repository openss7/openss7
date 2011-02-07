// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: AssocEventType.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:14 $ <p>
 
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

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD |  CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class AssocEventType

java.lang.Object
  |
  +--javax.megaco.association.AssocEventType

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class AssocEventType
   extends java.lang.Object

   Constants used in package javax.megaco.association. This defines the
   type of the association event. It qualifies each association event.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_CREATE_ASSOC_REQ
              Identifies Event object is of type Create Association
   Request.
   static int M_DELETE_ASSOC_REQ
              Identifies Event object is of type Delete Association
   Request.
   static int M_MODIFY_ASSOC_REQ
              Identifies Event object is of type Modify Association
   Request.
   static int M_ASSOC_STATE_IND
              Identifies Event object is of type Association State change
   indication due to trigger from remote or internal timeout or transport
   level asynchronous detection.
   static int M_CREATE_ASSOC_RESP
              Identifies Event object is of type Create Association
   Response.
   static int M_DELETE_ASSOC_RESP
              Identifies Event object is of type Delete Association
   Response.
   static int M_MODIFY_ASSOC_RESP
              Identifies Event object is of type Modify Association
   Response.
   static int M_ASSOC_CONFIG_REQ
              Identifies Event object is of type configuring the
   association parameters.
   static int M_ASSOC_CONFIG_RESP
              Identifies Event object is of type configuring the
   association parameters response.
   static int M_DELETE_TXN_REQ
              Identifies Event object is of type Delete Transaction
   Request.
   static int M_DELETE_TXN_RESP
              Identifies Event object is of type Delete Transaction
   Response.
   static int M_CREATE_TXN_REQ
              Identifies Event object is of type Create Transaction
   Request.
   static int M_CREATE_TXN_RESP
              Identifies Event object is of type Create Transaction
   Response.
   static int M_TXN_ERR_IND
              Identifies Event object is of type Transaction Error
   Indication.

                                                                   static

   AssocEventType CREATE_ASSOC_REQ
              Identifies a AssocEventType object that constructs the
   class with the constant M_CREATE_ASSOC_REQ.

                                                                   static

   AssocEventType DELETE_ASSOC_REQ
              Identifies a AssocEventType object that constructs the
   class with the constant M_DELETE_ASSOC_REQ.

                                                                   static

   AssocEventType MODIFY_ASSOC_REQ
              Identifies a AssocEventType object that constructs the
   class with the constant M_MODIFY_ASSOC_REQ.

                                                                   static

   AssocEventType ASSOC_STATE_IND
              Identifies a AssocEventType object that constructs the
   class with the constant M_ASSOC_STATE_IND.

                                                                   static

   AssocEventType CREATE_ASSOC_RESP
              Identifies a AssocEventType object that constructs the
   class with the constant M_CREATE_ASSOC_RESP.

                                                                   static

   AssocEventType DELETE_ASSOC_RESP
              Identifies a AssocEventType object that constructs the
   class with the constant M_DELETE_ASSOC_RESP.

                                                                   static

   AssocEventType MODIFY_ASSOC_RESP
              Identifies a AssocEventType object that constructs the
   class with the constant M_MODIFY_ASSOC_RESP.

                                                                   static

   AssocEventType ASSOC_CONFIG_REQ
              Identifies a AssocEventType object that constructs the
   class with the constant M_ASSOC_CONFIG_REQ.

                                                                   static

   AssocEventType ASSOC_CONFIG_RESP
              Identifies a AssocEventType object that constructs the
   class with the constant M_ASSOC_CONFIG_RESP.

                                                                   static

   AssocEventType DELETE_TXN_REQ
              Identifies a AssocEventType object that constructs the
   class with the constant M_DELETE_TXN_REQ.

                                                                   static

   AssocEventType DELETE_TXN_RESP
              Identifies a AssocEventType object that constructs the
   class with the constant M_DELETE_TXN_RESP.

                                                                   static

   AssocEventType CREATE_TXN_REQ
              Identifies a AssocEventType object that constructs the
   class with the constant M_CREATE_TXN_REQ.

                                                                   static

   AssocEventType CREATE_TXN_RESP
              Identifies a AssocEventType object that constructs the
   class with the constant M_CREATE_TXN_RESP.

                                                                   static

   AssocEventType TXN_ERR_IND
              Identifies a AssocEventType object that constructs the
   class with the constant M_TXN_ERR_IND.

   Constructor Summary
   AssocEventType(int event_type)
              Constructs a class that initialised with value event_type
   as passed to it in the constructor.

   Method Summary
   int getAssocEventType()
             Returns one of the allowed integer values defined as static
   fields in this class.

                                                                   static

   AssocEventType getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the AssocEventType object that
   identifies the event type as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_CREATE_ASSOC_REQ

public static final int M_CREATE_ASSOC_REQ

          Identifies Event object is of type Create Association Request.
          Its value shall be set equal to 1.
     _________________________________________________________________

  M_DELETE_ASSOC_REQ

public static final int M_DELETE_ASSOC_REQ

          Identifies Event object is of type Delete Association Request.
          Its value shall be set equal to 2.
     _________________________________________________________________

  M_MODIFY_ASSOC_REQ

public static final int M_MODIFY_ASSOC_REQ

          Identifies Event object is of type Modify Association Request.
          Its value shall be set equal to 3.
     _________________________________________________________________

  M_ASSOC_STATE_IND

public static final int M_ASSOC_STATE_IND

          Identifies Event object is of type Association State Change
          Indication. Its value shall be set equal to 4.
     _________________________________________________________________

  M_CREATE_ASSOC_RESP

public static final int M_CREATE_ASSOC_RESP

          Identifies Event object is of type Create Association Response.
          Its value shall be set equal to 5.
     _________________________________________________________________

  M_DELETE_ASSOC_RESP

public static final int M_DELETE_ASSOC_RESP

          Identifies Event object is of type Delete Association Response.
          Its value shall be set equal to 6.
     _________________________________________________________________

  M_MODIFY_ASSOC_RESP

public static final int M_MODIFY_ASSOC_RESP

          Identifies Event object is of type Modify Association Response.
          Its value shall be set equal to 7.
   ______________________________________________________________________

  M_ASSOC_CONFIG_REQ

public static final int M_ASSOC_CONFIG_REQ

   Identifies Event object is of type Association Configuration Request.
          Its value shall be set equal to 8.
          _______________________________________________________________

  M_ASSOC_CONFIG_RESP

public static final int M_ASSOC_CONFIG_RESP

Identifies Event object is of type Association Configuration Response. Its valu
e shall be set equal to 9.
     _________________________________________________________________

  M_DELETE_TXN_REQ

public static final int M_DELETE_TXN_REQ

          Identifies Event object is of type Delete Transaction Request.
          Its value shall be set equal to 10.
     _________________________________________________________________

  M_DELETE_TXN_RESP

public static final int M_DELETE_TXN_RESP

          Identifies Event object is of type Delete Transaction Response.
          Its value shall be set equal to 11.
     _________________________________________________________________

  M_CREATE_TXN_REQ

public static final int M_CREATE_TXN_REQ

          Identifies Event object is of type Create Transaction Request.
          Its value shall be set equal to 12.
     _________________________________________________________________

  M_CREATE_TXN_RESP

public static final int M_CREATE_TXN_RESP

          Identifies Event object is of type Create Transaction Response.
          Its value shall be set equal to 13.
     _________________________________________________________________

  M_TXN_ERR_IND

public static final int M_TXN_ERR_IND

          Identifies Event object is of type Transaction Error
          Indication. Its value shall be set equal to 14.
     _________________________________________________________________

  CREATE_ASSOC_REQ

public static final AssocEventType CREATE_ASSOC_REQ
                    = new AssocEventType(M_CREATE_ASSOC_REQ);

          Identifies a AssocEventType object that constructs the class
          with the constant M_CREATE_ASSOC_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  DELETE_ASSOC_REQ

public static final AssocEventType DELETE_ASSOC_REQ
                    = new AssocEventType(M_DELETE_ASSOC_REQ);

          Identifies a AssocEventType object that constructs the class
          with the constant M_DELETE_ASSOC_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MODIFY_ASSOC_REQ

public static final AssocEventType MODIFY_ASSOC_REQ
                    = new AssocEventType(M_MODIFY_ASSOC_REQ);

          Identifies a AssocEventType object that constructs the class
          with the constant M_MODIFY_ASSOC_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  ASSOC_STATE_IND

public static final AssocEventType ASSOC_STATE_IND
                    = new AssocEventType(M_ASSOC_STATE_IND);

          Identifies a AssocEventType object that constructs the class
          with the constant M_ASSOC_STATE_IND. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  CREATE_ASSOC_RESP

public static final AssocEventType CREATE_ASSOC_RESP
                    = new AssocEventType(M_CREATE_ASSOC_RESP);

          Identifies a AssocEventType object that constructs the class
          with the constant M_CREATE_ASSOC_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  DELETE_ASSOC_RESP

public static final AssocEventType DELETE_ASSOC_RESP
                    = new AssocEventType(M_DELETE_ASSOC_RESP);

          Identifies a AssocEventType object that constructs the class
          with the constant M_DELETE_ASSOC_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MODIFY_ASSOC_RESP

public static final AssocEventType MODIFY_ASSOC_RESP
                    = new AssocEventType(M_MODIFY_ASSOC_RESP);

          Identifies a AssocEventType object that constructs the class
          with the constant M_MODIFY_ASSOC_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
   ______________________________________________________________________

  ASSOC_CONFIG_REQ

public static final AssocEventType ASSOC_CONFIG_REQ
                    = new AssocEventType(M_ASSOC_CONFIG_REQ);

          Identifies a AssocEventType object that constructs the class
          with the constant M_ASSOC_CONFIG_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
   ______________________________________________________________________

  ASSOC_CONFIG_RESP

public static final AssocEventType ASSOC_CONFIG_RESP
                    = new AssocEventType(M_ASSOC_CONFIG_RESP);

          Identifies a AssocEventType object that constructs the class
          with the constant M_ASSOC_CONFIG_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  DELETE_TXN_REQ

public static final AssocEventType DELETE_TXN_REQ
                    = new AssocEventType(M_DELETE_TXN_REQ);

          Identifies a AssocEventType object that constructs the class
          with the constant M_DELETE_TXN_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  DELETE_TXN_RESP

public static final AssocEventType DELETE_TXN_RESP
                    = new AssocEventType(M_DELETE_TXN_RESP);

          Identifies a AssocEventType object that constructs the class
          with the constant M_DELETE_TXN_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  CREATE_TXN_REQ

public static final AssocEventType CREATE_TXN_REQ
                    = new AssocEventType(M_CREATE_TXN_REQ);

          Identifies a AssocEventType object that constructs the class
          with the constant M_CREATE_TXN_REQ. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  CREATE_TXN_RESP

public static final AssocEventType CREATE_TXN_RESP
                    = new AssocEventType(M_CREATE_TXN_RESP);

          Identifies a AssocEventType object that constructs the class
          with the constant M_CREATE_TXN_RESP. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  TXN_ERR_IND

public static final AssocEventType TXN_ERR_IND
                    = new AssocEventType(M_TXN_ERR_IND);

          Identifies a AssocEventType object that constructs the class
          with the constant M_TXN_ERR_IND. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

   Constructor Detail

  AssocEventType

private AssocEventType(int event_type)

          Constructs a class that initialised with value event_type as
          passed to it. The getReturnStatus method of this class object
          would always return value event_type.
     _________________________________________________________________

   Method Detail

  getAssocEventType

public int getAssocEventType()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the association
                event as sent to stack or received from stack.
     _________________________________________________________________

  getObject

public static final AssocEventType getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the AssocEventType object that identifies
          the return status as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the AssocEventType object.

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
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
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
