// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/parms/EncryptionMethod.java <p>
 
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
Class EncryptionMethod

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.LocalOptionValue
        |
        +--jain.protocol.ip.mgcp.message.parms.EncryptionMethod

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class EncryptionMethod
   extends LocalOptionValue
   implements java.io.Serializable

   Specifies the encryption method (and key).

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int BASE64
             Signifies "base64" encryption method.
   static int CLEAR
             Signifies "clear" encryption method.
   static int URI
             Signifies "uri" encryption method.



   Fields inherited from class
   jain.protocol.ip.mgcp.message.parms.LocalOptionValue
   BANDWIDTH, COMPRESSION_ALGORITHM, ECHO_CANCELLATION,
   ENCRYPTION_METHOD, GAIN_CONTROL, LOCAL_OPTION_EXTENSION,
   PACKETIZATION_PERIOD, RESOURCE_RESERVATION, SILENCE_SUPPRESSION,
   TYPE_OF_NETWORK, TYPE_OF_SERVICE



   Constructor Summary
   EncryptionMethod(int encryptionMethod, java.lang.String encryptionKey)
             Constructs a new EncryptionMethod object.



   Method Summary
    java.lang.String getEncryptionKey()
             Gets the encryption key.
    int getEncryptionMethod()
             Gets the encryption method.
    java.lang.String toString()
             Returns a java.lang.String of the form
   "k:<encryptionMethod>:<encryptionKey>"



   Methods inherited from class
   jain.protocol.ip.mgcp.message.parms.LocalOptionValue
   getBandwidthLowerBound, getBandwidthUpperBound,
   getCompressionAlgorithmNames, getEchoCancellation, getGainControl,
   getLocalOptionExtensionName, getLocalOptionValueType,
   getPacketizationPeriodLowerBound, getPacketizationPeriodUpperBound,
   getResourceReservation, getSilenceSuppression, getTypeOfNetwork,
   getTypeOfService



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  BASE64

public static final int BASE64

          Signifies "base64" encryption method.
     _________________________________________________________________

  CLEAR

public static final int CLEAR

          Signifies "clear" encryption method.
     _________________________________________________________________

  URI

public static final int URI

          Signifies "uri" encryption method.

   Constructor Detail

  EncryptionMethod

public EncryptionMethod(int encryptionMethod,
                        java.lang.String encryptionKey)
                 throws java.lang.IllegalArgumentException

          Constructs a new EncryptionMethod object.

        Parameters:
                encryptionMethod - One of the permitted encryption
                methods: CLEAR; BASE64; URI. Note: RFC 2705 includes the
                "prompt" method in its specification with the note that
                this method is not usable in MGCP. Thus, "prompt" is
                excluded as a permissible value in JAIN MGCP.
                encryptionKey - A java.lang.String that contains the encryption
                key.

        Throws:
                java.lang.IllegalArgumentException - Thrown if:

              1. Encryption method is other than the three permitted
                 values: CLEAR; BASE64; or URI.
              2. Encryption method = CLEAR and the encryption key does
                 not conform to the specification for such a key in RFC
                 2705.
              3. Encryption method = BASE64 and the encryption key is not
                 an encoded encryption key according to RFC 2705.
              4. Encryption method = URI and the encryption key is not a
                 "URItoObtainKey" as defined in RFC 2705.

   Method Detail

  getEncryptionKey

public java.lang.String getEncryptionKey()

          Gets the encryption key.

        Returns:
                The encryption key, as a java.lang.String.
     _________________________________________________________________

  getEncryptionMethod

public int getEncryptionMethod()

          Gets the encryption method.

        Overrides:
                getEncryptionMethod in class LocalOptionValue

        Returns:
                One of the values: CLEAR; BASE64; or URI.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns a java.lang.String of the form
          "k:<encryptionMethod>:<encryptionKey>"

        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
