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

package jain.protocol.ip.mgcp.message.parms;

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

jain.protocol.ip.mgcp.message.parms
Class ResourceReservation

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.LocalOptionValue
        |
        +--jain.protocol.ip.mgcp.message.parms.ResourceReservation

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class ResourceReservation
   extends LocalOptionValue
   implements java.io.Serializable

   Specifies the type of resource reservation: guaranteed; controlled
   load; or best effort.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int BEST_EFFORT
             Signifies best-effort resource reservation.
   static ResourceReservation BestEffort
             Encapsulates the best-effort resource reservation.
   static int CONTROLLED_LOAD
             Signifies controlled load resource reservation.
   static ResourceReservation ControlledLoad
             Encapsulates the controlled load resource reservation.
   static ResourceReservation Guaranteed
             Encapsulates the guaranteed resource reservation.
   static int GUARANTEED
             Signifies guaranteed resource reservation.



   Fields inherited from class
   jain.protocol.ip.mgcp.message.parms.LocalOptionValue
   BANDWIDTH, COMPRESSION_ALGORITHM, ECHO_CANCELLATION,
   ENCRYPTION_METHOD, GAIN_CONTROL, LOCAL_OPTION_EXTENSION,
   PACKETIZATION_PERIOD, RESOURCE_RESERVATION, SILENCE_SUPPRESSION,
   TYPE_OF_NETWORK, TYPE_OF_SERVICE



   Method Summary
    int getResourceReservation()
             Gets the value of this ResourceReservation object as an
   integer constant.
    java.lang.String toString()
             Returns a java.lang.String of the form "r:<resourceReservation>" where
   resourceReservation is one of: "g" for guaranteed; "cl" for controlled
   load; or "be" for best-effort.



   Methods inherited from class
   jain.protocol.ip.mgcp.message.parms.LocalOptionValue
   getBandwidthLowerBound, getBandwidthUpperBound,
   getCompressionAlgorithmNames, getEchoCancellation,
   getEncryptionMethod, getGainControl, getLocalOptionExtensionName,
   getLocalOptionValueType, getPacketizationPeriodLowerBound,
   getPacketizationPeriodUpperBound, getSilenceSuppression,
   getTypeOfNetwork, getTypeOfService



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  BEST_EFFORT

public static final int BEST_EFFORT

          Signifies best-effort resource reservation.
     _________________________________________________________________

  BestEffort

public static final ResourceReservation BestEffort

          Encapsulates the best-effort resource reservation.
     _________________________________________________________________

  CONTROLLED_LOAD

public static final int CONTROLLED_LOAD

          Signifies controlled load resource reservation.
     _________________________________________________________________

  ControlledLoad

public static final ResourceReservation ControlledLoad

          Encapsulates the controlled load resource reservation.
     _________________________________________________________________

  Guaranteed

public static final ResourceReservation Guaranteed

          Encapsulates the guaranteed resource reservation.
     _________________________________________________________________

  GUARANTEED

public static final int GUARANTEED

          Signifies guaranteed resource reservation.

   Method Detail

  getResourceReservation

public int getResourceReservation()

          Gets the value of this ResourceReservation object as an integer
          constant.

        Overrides:
                getResourceReservation in class LocalOptionValue

          Following copied from class:
          jain.protocol.ip.mgcp.message.parms.LocalOptionValue

        Returns:
                The resource reservation type (as an integer value:
                GUARANTEED, CONTROLLED_LOAD, BEST_EFFORT).

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent resource reservation.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns a java.lang.String of the form "r:<resourceReservation>" where
          resourceReservation is one of: "g" for guaranteed; "cl" for
          controlled load; or "be" for best-effort.

        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright © 2000 Sun Microsystems, Inc.
*/
