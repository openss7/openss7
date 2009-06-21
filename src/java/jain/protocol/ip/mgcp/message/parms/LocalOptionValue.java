// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: LocalOptionValue.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:21 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:21 $ by $Author: brian $
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
Class LocalOptionValue

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.LocalOptionValue

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          Bandwidth, CompressionAlgorithm, EchoCancellation,
          EncryptionMethod, GainControl, LocalOptionExtension,
          PacketizationPeriod, ResourceReservation, SilenceSuppression,
          TypeOfNetwork, TypeOfService
     _________________________________________________________________

   public class LocalOptionValue
   extends java.lang.Object
   implements java.io.Serializable

   Specifies a Local Connection Options Value. Note that this is an
   abstract class. "Get" accessor methods are defined for each subclass
   that derives from this class, which, by default, throw an exception.
   Each subclass defines its own overriding accessor method, so that
   invoking an accessor method on the wrong subclass would result in an
   exception being thrown.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int BANDWIDTH
             Signifies that this LocalOptionValue object represents a
   bandwidth value or range.
   static int COMPRESSION_ALGORITHM
             Signifies that this LocalOptionValue object represents a
   list of compression algorithm names.
   static int ECHO_CANCELLATION
             Signifies that this LocalOptionValue object represents the
   state of echo cancellation.
   static int ENCRYPTION_METHOD
             Signifies that this LocalOptionValue object represents an
   encryption method/key.
   static int GAIN_CONTROL
             Signifies that this LocalOptionValue object represents gain
   control.
   static int LOCAL_OPTION_EXTENSION
             Signifies that this LocalOptionValue object represents a
   Local Option Extension parameter.
   static int PACKETIZATION_PERIOD
             Signifies that this LocalOptionValue object represents a
   packetization period.
   static int RESOURCE_RESERVATION
             Signifies that this LocalOptionValue object represents
   resource reservation.
   static int SILENCE_SUPPRESSION
             Signifies that this LocalOptionValue object represents the
   state of silence suppression.
   static int TYPE_OF_NETWORK
             Signifies that this LocalOptionValue object represents type
   of network.
   static int TYPE_OF_SERVICE
             Signifies that this LocalOptionValue object represents type
   of service.



   Constructor Summary
   protected LocalOptionValue(int localOptionValueType)
             Constructs a new LocalOptionValue object.



   Method Summary
    int getBandwidthLowerBound()
             Gets the lower bound of the bandwidth range.
    int getBandwidthUpperBound()
             Gets the upper bound of the bandwidth range.
    java.lang.String[] getCompressionAlgorithmNames()
             Gets the list of compression algorithm names.
    boolean getEchoCancellation()
             Gets the on/off state of echo cancellation.
    int getEncryptionMethod()
             Gets the encryption method.
    int getGainControl()
             Gets the gain control value.
    java.lang.String getLocalOptionExtensionName()
             Gets the name of the local option extension parameter.
    int getLocalOptionValueType()
             Gets the type of the Local Option Value.
    int getPacketizationPeriodLowerBound()
             Gets the lower bound of the packetization period.
    int getPacketizationPeriodUpperBound()
             Gets the upper bound of the packetization period.
    int getResourceReservation()
             Gets the resource reservation (guaranteed, controlled load,
   best effort).
    boolean getSilenceSuppression()
             Gets the on/off state of silence suppression.
    int getTypeOfNetwork()
             Gets the type of network.
    byte getTypeOfService()
             Gets the type of service.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait, wait, wait



   Field Detail

  BANDWIDTH

public static final int BANDWIDTH

          Signifies that this LocalOptionValue object represents a
          bandwidth value or range.
     _________________________________________________________________

  COMPRESSION_ALGORITHM

public static final int COMPRESSION_ALGORITHM

          Signifies that this LocalOptionValue object represents a list
          of compression algorithm names.
     _________________________________________________________________

  ECHO_CANCELLATION

public static final int ECHO_CANCELLATION

          Signifies that this LocalOptionValue object represents the
          state of echo cancellation.
     _________________________________________________________________

  ENCRYPTION_METHOD

public static final int ENCRYPTION_METHOD

          Signifies that this LocalOptionValue object represents an
          encryption method/key.
     _________________________________________________________________

  GAIN_CONTROL

public static final int GAIN_CONTROL

          Signifies that this LocalOptionValue object represents gain
          control.
     _________________________________________________________________

  LOCAL_OPTION_EXTENSION

public static final int LOCAL_OPTION_EXTENSION

          Signifies that this LocalOptionValue object represents a Local
          Option Extension parameter.
     _________________________________________________________________

  PACKETIZATION_PERIOD

public static final int PACKETIZATION_PERIOD

          Signifies that this LocalOptionValue object represents a
          packetization period.
     _________________________________________________________________

  RESOURCE_RESERVATION

public static final int RESOURCE_RESERVATION

          Signifies that this LocalOptionValue object represents resource
          reservation.
     _________________________________________________________________

  SILENCE_SUPPRESSION

public static final int SILENCE_SUPPRESSION

          Signifies that this LocalOptionValue object represents the
          state of silence suppression.
     _________________________________________________________________

  TYPE_OF_NETWORK

public static final int TYPE_OF_NETWORK

          Signifies that this LocalOptionValue object represents type of
          network.
     _________________________________________________________________

  TYPE_OF_SERVICE

public static final int TYPE_OF_SERVICE

          Signifies that this LocalOptionValue object represents type of
          service.

   Constructor Detail

  LocalOptionValue

protected LocalOptionValue(int localOptionValueType)

          Constructs a new LocalOptionValue object.

   Method Detail

  getBandwidthLowerBound

public int getBandwidthLowerBound()
                           throws JainIPMgcpException

          Gets the lower bound of the bandwidth range.

        Returns:
                The lower bound of the bandwidth range.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent a bandwidth value or range.
     _________________________________________________________________

  getBandwidthUpperBound

public int getBandwidthUpperBound()
                           throws JainIPMgcpException

          Gets the upper bound of the bandwidth range.

        Returns:
                The upper bound of the bandwidth range.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent a bandwidth value or range.
     _________________________________________________________________

  getCompressionAlgorithmNames

public java.lang.String[] getCompressionAlgorithmNames()
                                                throws JainIPMgcpException

          Gets the list of compression algorithm names.

        Returns:
                The list of compression algorithm names.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent a list of compression algorithm names.
     _________________________________________________________________

  getEchoCancellation

public boolean getEchoCancellation()
                            throws JainIPMgcpException

          Gets the on/off state of echo cancellation.

        Returns:
                The on/off state of echo cancellation.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent the state of echo cancellation.
     _________________________________________________________________

  getEncryptionMethod

public int getEncryptionMethod()
                        throws JainIPMgcpException

          Gets the encryption method.

        Returns:
                The encryption method (as an integer value: CLEAR,
                BASE64, URI).

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent encryption method.
     _________________________________________________________________

  getGainControl

public int getGainControl()
                   throws JainIPMgcpException

          Gets the gain control value.

        Returns:
                The gain control value.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent the gain control value.
     _________________________________________________________________

  getLocalOptionExtensionName

public java.lang.String getLocalOptionExtensionName()
                                             throws JainIPMgcpException

          Gets the name of the local option extension parameter.

        Returns:
                The name of the local option extension parameter, as a
                java.lang.String.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent a local option extension parameter.
     _________________________________________________________________

  getLocalOptionValueType

public int getLocalOptionValueType()

          Gets the type of the Local Option Value.

        Returns:
                An integer value that indicates which type of Local
                Option Value is represented by this object.
     _________________________________________________________________

  getPacketizationPeriodLowerBound

public int getPacketizationPeriodLowerBound()
                                     throws JainIPMgcpException

          Gets the lower bound of the packetization period.

        Returns:
                The lower bound of the packetization period.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent a packetization period.
     _________________________________________________________________

  getPacketizationPeriodUpperBound

public int getPacketizationPeriodUpperBound()
                                     throws JainIPMgcpException

          Gets the upper bound of the packetization period.

        Returns:
                The upper bound of the packetization period.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent a packetization period.
     _________________________________________________________________

  getResourceReservation

public int getResourceReservation()
                           throws JainIPMgcpException

          Gets the resource reservation (guaranteed, controlled load,
          best effort).

        Returns:
                The resource reservation type (as an integer value:
                GUARANTEED, CONTROLLED_LOAD, BEST_EFFORT).

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent resource reservation.
     _________________________________________________________________

  getSilenceSuppression

public boolean getSilenceSuppression()
                              throws JainIPMgcpException

          Gets the on/off state of silence suppression.

        Returns:
                The on/off state of silence suppression.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent the state of silence suppression.
     _________________________________________________________________

  getTypeOfNetwork

public int getTypeOfNetwork()
                     throws JainIPMgcpException

          Gets the type of network.

        Returns:
                The type of network (as an integer value: IN, ATM,
                LOCAL).

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent type of network.
     _________________________________________________________________

  getTypeOfService

public byte getTypeOfService()
                      throws JainIPMgcpException

          Gets the type of service.

        Returns:
                The Type of Service as a byte value.

        Throws:
                JainIPMgcpException - Thrown if this object does not
                represent the type of service.
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright © 2000 Sun Microsystems, Inc.
*/
