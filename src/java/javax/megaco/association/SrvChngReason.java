// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/association/SrvChngReason.java <p>
 
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
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class SrvChngReason

java.lang.Object
  |
  +--javax.megaco.association.SrvChngReason

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          None
     _________________________________________________________________

   public class SrvChngReason
   extends java.lang.Object

   Service change reason constants used in package
   javax.megaco.association. This forms the class for the Service change
   reason parameters of the Jain Megaco package.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_SVC_CHNG_REASON_900
              Identifies the service change reason towards the peer shall
   be 900 - Service restored.
   static int M_SVC_CHNG_REASON_901
              Identifies the service change reason towards the peer shall
   be 901 - Cold Boot.
   static int M_SVC_CHNG_REASON_902
              Identifies the service change reason towards the peer shall
   be 902 - Warm Boot.
   static int M_SVC_CHNG_REASON_903
              Identifies the service change reason towards the peer shall
   be 903 - MGC Directed Change.
   static int M_SVC_CHNG_REASON_904
              Identifies the service change reason towards the peer shall
   be 904 - Termination Malfunctioning.
   static int M_SVC_CHNG_REASON_905
              Identifies the service change reason towards the peer shall
   be 905 - Termination Taken Out Of Service.
   static int M_SVC_CHNG_REASON_906
              Identifies the service change reason towards the peer shall
   be 906 - Loss Of Lower Layer Connectivity.
   static int M_SVC_CHNG_REASON_907
              Identifies the service change reason towards the peer shall
   be 907 - Transmission Failure.
   static int M_SVC_CHNG_REASON_908
              Identifies the service change reason towards the peer shall
   be 908 - MG Impending Failure.
   static int M_SVC_CHNG_REASON_909
              Identifies the service change reason towards the peer shall
   be 909 - MGC Impending Failure.
   static int M_SVC_CHNG_REASON_910
              Identifies the service change reason towards the peer shall
   be 910 - Media Capability Failure.
   static int M_SVC_CHNG_REASON_911
              Identifies the service change reason towards the peer shall
   be 911 - Modem Capability Failure.
   static int M_SVC_CHNG_REASON_912
              Identifies the service change reason towards the peer shall
   be 912 - Mux Capability Failure.
   static int M_SVC_CHNG_REASON_913
              Identifies the service change reason towards the peer shall
   be 913 - Signal Capability Failure.
   static int M_SVC_CHNG_REASON_914
              Identifies the service change reason towards the peer shall
   be 914 - Event Capability Failure.
   static int M_SVC_CHNG_REASON_915
              Identifies the service change reason towards the peer shall
   be 915 - State Loss.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_900
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_900.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_901
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_901.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_902
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_902.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_903
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_903.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_904
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_904.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_905
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_905.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_906
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_906.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_907
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_907.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_908
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_908.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_909
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_909.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_910
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_910.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_911
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_911.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_912
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_912.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_913
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_913.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_914
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_914.

                                                                   static
                                                            SrvChngReason

   SVC_CHNG_REASON_915
              Identifies a SrvChngReason object that constructs the class
   with the constant M_SVC_CHNG_REASON_915.

   Constructor Summary
   SrvChngReason(int service_change_reason)
              Constructs a class initialised with value
   service_change_reason as passed to it in the constructor.

   Method Summary
    int getSrvChngReasonId()
             Returns one of the allowed integer values defined as static
   fields in this class which shall identify the service change reason
   Id.

                                                                   static
                                                            SrvChngReason

   getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the SrvChngReason object that
   identifies the service change reason as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_SVC_CHNG_REASON_900

public static final int M_SVC_CHNG_REASON_900

          Identifies the service change reason code as 900.
          Its value shall be set to 900.
     _________________________________________________________________

  M_SVC_CHNG_REASON_901

public static final int M_SVC_CHNG_REASON_901

          Identifies the service change reason code as 901.
          Its value shall be set to 901.
     _________________________________________________________________

  M_SVC_CHNG_REASON_902

public static final int M_SVC_CHNG_REASON_902

          Identifies the service change reason code as 902.
          Its value shall be set to 902.
     _________________________________________________________________

  M_SVC_CHNG_REASON_903

public static final int M_SVC_CHNG_REASON_903

          Identifies the service change reason code as 903.
          Its value shall be set to 903.
     _________________________________________________________________

  M_SVC_CHNG_REASON_904

public static final int M_SVC_CHNG_REASON_904

          Identifies the service change reason code as 904.
          Its value shall be set to 904.
     _________________________________________________________________

  M_SVC_CHNG_REASON_905

public static final int M_SVC_CHNG_REASON_905

          Identifies the service change reason code as 905.
          Its value shall be set to 905.
     _________________________________________________________________

  M_SVC_CHNG_REASON_906

public static final int M_SVC_CHNG_REASON_906

          Identifies the service change reason code as 906.
          Its value shall be set to 906.
     _________________________________________________________________

  M_SVC_CHNG_REASON_907

public static final int M_SVC_CHNG_REASON_907

          Identifies the service change reason code as 907.
          Its value shall be set to 907.
     _________________________________________________________________

  M_SVC_CHNG_REASON_908

public static final int M_SVC_CHNG_REASON_908

          Identifies the service change reason code as 908.
          Its value shall be set to 908.
     _________________________________________________________________

  M_SVC_CHNG_REASON_909

public static final int M_SVC_CHNG_REASON_909

          Identifies the service change reason code as 909.
          Its value shall be set to 909.
     _________________________________________________________________

  M_SVC_CHNG_REASON_910

public static final int M_SVC_CHNG_REASON_910

          Identifies the service change reason code as 910.
          Its value shall be set to 910.
     _________________________________________________________________

  M_SVC_CHNG_REASON_911

public static final int M_SVC_CHNG_REASON_911

          Identifies the service change reason code as 911.
          Its value shall be set to 911.
     _________________________________________________________________

  M_SVC_CHNG_REASON_912

public static final int M_SVC_CHNG_REASON_912

          Identifies the service change reason code as 912.
          Its value shall be set to 912.
     _________________________________________________________________

  M_SVC_CHNG_REASON_913

public static final int M_SVC_CHNG_REASON_913

          Identifies the service change reason code as 913.
          Its value shall be set to 913.
     _________________________________________________________________

  M_SVC_CHNG_REASON_914

public static final int M_SVC_CHNG_REASON_914

          Identifies the service change reason code as 914.
          Its value shall be set to 914.
     _________________________________________________________________

  M_SVC_CHNG_REASON_915

public static final int M_SVC_CHNG_REASON_915

          Identifies the service change reason code as 915.
          Its value shall be set to 915.
     _________________________________________________________________

  SVC_CHNG_REASON_900

public static final SrvChngReason SVC_CHNG_REASON_900
            = new SrvChngReason (M_SVC_CHNG_REASON_900);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_900. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_901

public static final SrvChngReason SVC_CHNG_REASON_901
            = new SrvChngReason (M_SVC_CHNG_REASON_901);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_901. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_902

public static final SrvChngReason SVC_CHNG_REASON_902
            = new SrvChngReason (M_SVC_CHNG_REASON_902);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_902. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_903

public static final SrvChngReason SVC_CHNG_REASON_903
            = new SrvChngReason (M_SVC_CHNG_REASON_903);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_903. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_904

public static final SrvChngReason SVC_CHNG_REASON_904
            = new SrvChngReason (M_SVC_CHNG_REASON_904);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_904. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_905

public static final SrvChngReason SVC_CHNG_REASON_905
            = new SrvChngReason (M_SVC_CHNG_REASON_905);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_906. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_906

public static final SrvChngReason SVC_CHNG_REASON_906
            = new SrvChngReason (M_SVC_CHNG_REASON_906);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_906. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_907

public static final SrvChngReason SVC_CHNG_REASON_907
            = new SrvChngReason (M_SVC_CHNG_REASON_907);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_907. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_908

public static final SrvChngReason SVC_CHNG_REASON_908
            = new SrvChngReason (M_SVC_CHNG_REASON_908);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_908. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_909

public static final SrvChngReason SVC_CHNG_REASON_909
            = new SrvChngReason (M_SVC_CHNG_REASON_909);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_909. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_910

public static final SrvChngReason SVC_CHNG_REASON_910
            = new SrvChngReason (M_SVC_CHNG_REASON_910);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_910. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_911

public static final SrvChngReason SVC_CHNG_REASON_911
            = new SrvChngReason (M_SVC_CHNG_REASON_911);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_911. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_912

public static final SrvChngReason SVC_CHNG_REASON_912
            = new SrvChngReason (M_SVC_CHNG_REASON_912);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_912. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system. This method would return
          reference to the same object every time this method
     _________________________________________________________________

  SVC_CHNG_REASON_913

public static final SrvChngReason SVC_CHNG_REASON_913
            = new SrvChngReason (M_SVC_CHNG_REASON_913);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_913. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_914

public static final SrvChngReason SVC_CHNG_REASON_914
            = new SrvChngReason (M_SVC_CHNG_REASON_914);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_914. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  SVC_CHNG_REASON_915

public static final SrvChngReason SVC_CHNG_REASON_915
            = new SrvChngReason (M_SVC_CHNG_REASON_915);

          Identifies a SrvChngReason object that constructs the class
          with the constant M_SVC_CHNG_REASON_915. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

   Constructor Detail

  SrvChngReason

private SrvChngReason(int service_change_reason)

          Constructs a class that initialised with value
          service_change_reason as passed to it. The getSrvChngReasonId
          method of this class object would always return value
          service_shange_reason.
     _________________________________________________________________

   Method Detail

  getSrvChngReasonId

public int getSrvChngReasonId()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the service
                change reason.
     _________________________________________________________________

  getObject

public static final SrvChngReason getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the SrvChngReason object that identifies
          the service change reason as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the SrvChngReason object.

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
