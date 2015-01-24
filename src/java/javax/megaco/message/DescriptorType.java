// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/DescriptorType.java <p>
 
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
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD |  CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class DescriptorType

java.lang.Object
  |
  +--javax.megaco.message.DescriptorType

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class DescriptorType
   extends java.lang.Object

   Constants used in package javax.megaco for defining different
   descriptors that can occur in the commands/actions.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_EVENT_DESC
              Identifies descriptor to be event descriptor.
   static int M_DIGIT_MAP_DESC
              Identifies descriptor to be digit map descriptor.
   static int M_SIGNAL_DESC
              Identifies descriptor to be signal descriptor.
   static int M_MEDIA_DESC
              Identifies descriptor to be media descriptor.
   static int M_STREAM_DESC
              Identifies descriptor to be stream descriptor.
   static int M_LOCAL_CONTROL_DESC
              Identifies descriptor to be local control descriptor.
   static int M_TERMINATION_STATE_DESC
              Identifies descriptor to be termination state descriptor.
   static int M_SERVICE_CHANGE_DESC
              Identifies descriptor to be service change descriptor.
   static int M_SERVICE_CHANGE_RESP_DESC
              Identifies descriptor to be service change response
   descriptor.
   static int M_AUDIT_CAP_DESC
              Identifies descriptor to be audit capability descriptor.
   static int M_AUDIT_VAL_DESC
              Identifies descriptor to be audit value descriptor.
   static int M_AUDIT_CAP_REPLY_DESC
              Identifies descriptor to be audit capability reply
   descriptor.
   static int M_AUDIT_VAL_REPLY_DESC
              Identifies descriptor to be audit value reply descriptor.
   static int M_EVENT_BUF_DESC
              Identifies descriptor to be event buffer descriptor.
   static int M_STATISTICS_DESC
              Identifies descriptor to be statistics descriptor.
   static int M_PACKAGE_DESC
              Identifies descriptor to be package descriptor.
   static int M_ERROR_DESC
              Identifies descriptor to be error descriptor.
   static int M_MODEM_DESC
              Identifies descriptor to be modem descriptor.
   static int M_MUX_DESC
              Identifies descriptor to be mux descriptor.
   static int M_TOPOLOGY_DESC
              Identifies descriptor to be topology descriptor.
   static int M_CTX_TERM_AUDIT_DESC
              Identifies descriptor to be context termination audit
   descriptor.
   static int M_OBSERVED_EVENT_DESC
              Identifies descriptor to be observed event descriptor.
   static
   DescriptorType EVENT_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_EVENT_DESC.
   static
   DescriptorType DIGIT_MAP_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_DIGIT_MAP_DESC.
   static
   DescriptorType SIGNAL_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_SIGNAL_DESC.
   static
   DescriptorType MEDIA_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_MEDIA_DESC.
   static
   DescriptorType STREAM_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_STREAM_DESC.
   static
   DescriptorType LOCAL_CONTROL_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_LOCAL_CONTROL_DESC.
   static
   DescriptorType TERMINATION_STATE_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_TERMINATION_STATE_DESC.
   static
   DescriptorType SERVICE_CHANGE_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_SERVICE_CHANGE_DESC.
   static
   DescriptorType SERVICE_CHANGE_RESP_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_SERVICE_CHANGE_RESP_DESC.
   static
   DescriptorType AUDIT_CAP_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_AUDIT_CAP_DESC.
   static
   DescriptorType AUDIT_VAL_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_AUDIT_VAL_DESC.
   static
   DescriptorType AUDIT_CAP_REPLY_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_AUDIT_CAP_REPLY_DESC.
   static
   DescriptorType AUDIT_VAL_REPLY_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_AUDIT_VAL_REPLY_DESC.
   static
   DescriptorType STATISTICS_BUF_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_STATISTICS_BUF_DESC.
   static
   DescriptorType PACKAGE_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_PACKAGE_DESC.
   static
   DescriptorType ERROR_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_ERROR_DESC.
   static
   DescriptorType MODEM_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_MODEM_DESC.
   static
   DescriptorType MUX_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_MUX_DESC.
   static
   DescriptorType TOPOLOGY_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_TOPOLOGY_DESC.
   static
   DescriptorType CTX_TERM_AUDIT_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_CTX_TERM_AUDIT_DESC.
   static
   DescriptorType OBSERVED_EVENT_DESC
              Identifies a DescriptorType object that constructs the
   class with the constant M_OBSERVED_EVENT_DESC.

   Constructor Summary
   DescriptorType(int desc_type)
              Constructs a class initialised with value desc_type as
   passed to it in the constructor.

   Method Summary
   int getDescType()
             Returns one of the allowed integer values defined as static
   fields in this class.
   static
   DescriptorType getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the DescriptorType object that
   identifies the descriptor type as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_EVENT_DESC

public static final int M_EVENT_DESC

          Identifies descriptor to be of type event descriptor. Its value
          shall be set equal to 1.
     _________________________________________________________________

  M_DIGIT_MAP_DESC

public static final int M_DIGIT_MAP_DESC

          Identifies descriptor to be of type digit map descriptor. Its
          value shall be set equal to 2.
     _________________________________________________________________

  M_SIGNAL_DESC

public static final int M_SIGNAL_DESC

          Identifies descriptor to be of type signal descriptor. Its
          value shall be set equal to 3.
     _________________________________________________________________

  M_MEDIA_DESC

public static final int M_MEDIA_DESC

          Identifies descriptor to be of type media descriptor. Its value
          shall be set equal to 4.
     _________________________________________________________________

  M_STREAM_DESC

public static final int M_STREAM_DESC

          Identifies descriptor to be of type stream descriptor. Its
          value shall be set equal to 5.
     _________________________________________________________________

  M_LOCAL_CONTROL_DESC

public static final int M_LOCAL_CONTROL_DESC

          Identifies descriptor to be of type local control descriptor.
          Its value shall be set equal to 6.
     _________________________________________________________________

  M_TERMINATION_STATE_DESC

public static final int M_TERMINATION_STATE_DESC

          Identifies descriptor to be of type termination state
          descriptor. Its value shall be set equal to 7.
     _________________________________________________________________

  M_SERVICE_CHANGE_DESC

public static final int M_SERVICE_CHANGE_DESC

          Identifies descriptor to be of type service change descriptor.
          Its value shall be set equal to 8.
     _________________________________________________________________

  M_SERVICE_CHANGE_RESP_DESC

public static final int M_SERVICE_CHANGE_RESP_DESC

          Identifies descriptor to be of type service change response
          descriptor. Its value shall be set equal to 9.
     _________________________________________________________________

  M_AUDIT_CAP_DESC

public static final int M_AUDIT_CAP_DESC

          Identifies descriptor to be of type audit capability
          descriptor. Its value shall be set equal to 10.
     _________________________________________________________________

  M_AUDIT_VAL_DESC

public static final int M_AUDIT_VAL_DESC

          Identifies descriptor to be of type audit value descriptor. Its
          value shall be set equal to 11.
     _________________________________________________________________

  M_AUDIT_CAP_REPLY_DESC

public static final int M_AUDIT_CAP_REPLY_DESC

          Identifies descriptor to be of type audit capability reply
          descriptor. Its value shall be set equal to 12.
     _________________________________________________________________

  M_AUDIT_VAL_REPLY_DESC

public static final int M_AUDIT_VAL_REPLY_DESC

          Identifies descriptor to be of type audit value reply
          descriptor. Its value shall be set equal to 13.
     _________________________________________________________________

  M_EVENT_BUF_DESC

public static final int M_EVENT_BUF_DESC

          Identifies descriptor to be of type event buffer descriptor.
          Its value shall be set equal to 14.
     _________________________________________________________________

  M_STATISTICS_DESC

public static final int M_STATISTICS_DESC

          Identifies descriptor to be of type statistics descriptor. Its
          value shall be set equal to 15.
     _________________________________________________________________

  M_PACKAGE_DESC

public static final int M_PACKAGE_DESC

          Identifies descriptor to be of type package descriptor. Its
          value shall be set equal to 16.
     _________________________________________________________________

  M_ERROR_DESC

public static final int M_ERROR_DESC

          Identifies descriptor to be of type error descriptor. Its value
          shall be set equal to 17.
     _________________________________________________________________

  M_MODEM_DESC

public static final int M_MODEM_DESC

          Identifies descriptor to be of type modem descriptor. Its value
          shall be set equal to 18.
     _________________________________________________________________

  M_MUX_DESC

public static final int M_MUX_DESC

          Identifies descriptor to be of type mux descriptor. Its value
          shall be set equal to 19.
     _________________________________________________________________

  M_TOPOLOGY_DESC

public static final int M_TOPOLOGY_DESC

          Identifies descriptor to be of type topology descriptor. Its
          value shall be set equal to 20.
     _________________________________________________________________

  M_OBSERVED_EVENT_DESC

public static final int M_OBSERVED_EVENT_DESC

          Identifies descriptor to be of type observed event descriptor.
          Its value shall be set equal to 21.
     _________________________________________________________________

  EVENT_DESC

public static final DescriptorType EVENT_DESC
        = new DescriptorType(M_EVENT_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_EVENT_DESC. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  DIGIT_MAP_DESC

public static final DescriptorType DIGIT_MAP_DESC
        = new DescriptorType(M_DIGIT_MAP_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_DIGIT_MAP_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  SIGNAL_DESC

public static final DescriptorType SIGNAL_DESC
        = new DescriptorType(M_SIGNAL_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_SIGNAL_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MEDIA_DESC

public static final DescriptorType MEDIA_DESC
        = new DescriptorType(M_MEDIA_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_MEDIA_DESC. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  STREAM_DESC

public static final DescriptorType STREAM_DESC
        = new DescriptorType(M_STREAM_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_STREAM_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  LOCAL_CONTROL_DESC

public static final DescriptorType LOCAL_CONTROL_DESC
        = new DescriptorType(M_LOCAL_CONTROL_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_LOCAL_CONTROL_DESC. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  TERMINATION_STATE_DESC

public static final DescriptorType TERMINATION_STATE_DESC
        = new DescriptorType(M_TERMINATION_STATE_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_TERMINATION_STATE_DESC. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SERVICE_CHANGE_DESC

public static final DescriptorType SERVICE_CHANGE_DESC
        = new DescriptorType(M_SERVICE_CHANGE_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_SERVICE_CHANGE_DESC. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SERVICE_CHANGE_RESP_DESC

public static final DescriptorType SERVICE_CHANGE_RESP_DESC
        = new DescriptorType(M_SERVICE_CHANGE_RESP_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_SERVICE_CHANGE_RESP_DESC. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  AUDIT_CAP_DESC

public static final DescriptorType AUDIT_CAP_DESC
        = new DescriptorType(M_AUDIT_CAP_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_AUDIT_CAP_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  AUDIT_VAL_DESC

public static final DescriptorType AUDIT_VAL_DESC
        = new DescriptorType(M_AUDIT_VAL_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_AUDIT_VAL_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  AUDIT_CAP_REPLY_DESC

public static final DescriptorType AUDIT_CAP_REPLY_DESC
        = new DescriptorType(M_AUDIT_CAP_REPLY_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_AUDIT_CAP_REPLY_DESC. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  AUDIT_VAL_REPLY_DESC

public static final DescriptorType AUDIT_VAL_REPLY_DESC
        = new DescriptorType(M_AUDIT_VAL_REPLY_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_AUDIT_VAL_REPLY_DESC. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  EVENT_BUF_DESC

public static final DescriptorType EVENT_BUF_DESC
        = new DescriptorType(M_EVENT_BUF_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_EVENT_BUF_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  STATISTICS_DESC

public static final DescriptorType STATISTICS_DESC
        = new DescriptorType(M_STATISTICS_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_STATISTICS_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  PACKAGE_DESC

public static final DescriptorType PACKAGE_DESC
        = new DescriptorType(M_PACKAGE_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_PACKAGE_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  ERROR_DESC

public static final DescriptorType ERROR_DESC
        = new DescriptorType(M_ERROR_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_ERROR_DESC. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  MODEM_DESC

public static final DescriptorType MODEM_DESC
        = new DescriptorType(M_MODEM_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_MODEM_DESC. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  MUX_DESC

public static final DescriptorType MUX_DESC
        = new DescriptorType(M_MUX_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_MUX_DESC. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  TOPOLOGY_DESC

public static final DescriptorType TOPOLOGY_DESC
        = new DescriptorType(M_TOPOLOGY_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_TOPOLOGY_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  CTX_TERM_AUDIT_DESC

public static final DescriptorType CTX_TERM_AUDIT_DESC
        = new DescriptorType(M_CTX_TERM_AUDIT_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_CTX_TERM_AUDIT_DESC. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  OBSERVED_EVENT_DESC

public static final DescriptorType OBSERVED_EVENT_DESC
        = new DescriptorType(M_OBSERVED_EVENT_DESC);

          Identifies a DescriptorType object that constructs the class
          with the constant M_OBSERVED_EVENT_DESC. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

   Constructor Detail

  DescriptorType

private DescriptorType(int desc_type)

          Constructs a class that initialised with value desc_type as
          passed to it. The getDescType method of this class object would
          always return value desc_type.
     _________________________________________________________________

   Method Detail

  getDescType

public int getDescType()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the desc type of
                the descriptor, which could to be one of possible values
                of constants defined by this class.
     _________________________________________________________________

  getObject

public static final DescriptorType getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the DescriptorType object that identifies
          the descriptor type as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the DescriptorType object.

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
