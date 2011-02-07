// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: RegularConnectionParm.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:09 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:09 $ by $Author: brian $
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
Class RegularConnectionParm

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.ConnectionParm
        |
        +--jain.protocol.ip.mgcp.message.parms.RegularConnectionParm

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class RegularConnectionParm
   extends ConnectionParm
   implements java.io.Serializable

   Class that defines a connection parameter (e.g., number of packets
   sent) that is NOT an extended parameter. A connection parameter
   describes the status of a connection. Returned by Gateway when a
   connection is deleted.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int JITTER
             Signifies that this ConnectionParm object represents the
   average inter-packet arrival jitter, in milliseconds.
   static int LATENCY
             Signifies that this ConnectionParm object represents the
   average latency, in milliseconds.
   static int OCTETS_RECEIVED
             Signifies that this ConnectionParm object represents the
   number of octets received.
   static int OCTETS_SENT
             Signifies that this ConnectionParm object represents the
   number of octets sent.
   static int PACKETS_LOST
             Signifies that this ConnectionParm object represents the
   number of packets lost.
   static int PACKETS_RECEIVED
             Signifies that this ConnectionParm object represents the
   number of packets received.
   static int PACKETS_SENT
             Signifies that this ConnectionParm object represents the
   number of packets sent.



   Constructor Summary
   RegularConnectionParm(int connectionParmType, int connectionParmValue)
             Constructs a new ConnectionParm object that is not an
   extension parameter.



   Method Summary
    java.lang.String toString()
             Returns a java.lang.String of the form "<parmName>=<parmValue>," where
   <parmName> is the name of the connection parameter (e.g., "PS" for
   "packets sent") and <parmValue> is the numeric value of the parameter.



   Methods inherited from class
   jain.protocol.ip.mgcp.message.parms.ConnectionParm
   getConnectionParmType, getConnectionParmValue



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  JITTER

public static int JITTER

          Signifies that this ConnectionParm object represents the
          average inter-packet arrival jitter, in milliseconds.
     _________________________________________________________________

  LATENCY

public static int LATENCY

          Signifies that this ConnectionParm object represents the
          average latency, in milliseconds.
     _________________________________________________________________

  OCTETS_RECEIVED

public static int OCTETS_RECEIVED

          Signifies that this ConnectionParm object represents the number
          of octets received.
     _________________________________________________________________

  OCTETS_SENT

public static int OCTETS_SENT

          Signifies that this ConnectionParm object represents the number
          of octets sent.
     _________________________________________________________________

  PACKETS_LOST

public static int PACKETS_LOST

          Signifies that this ConnectionParm object represents the number
          of packets lost.
     _________________________________________________________________

  PACKETS_RECEIVED

public static int PACKETS_RECEIVED

          Signifies that this ConnectionParm object represents the number
          of packets received.
     _________________________________________________________________

  PACKETS_SENT

public static int PACKETS_SENT

          Signifies that this ConnectionParm object represents the number
          of packets sent.

   Constructor Detail

  RegularConnectionParm

public RegularConnectionParm(int connectionParmType,
                             int connectionParmValue)
                      throws java.lang.IllegalArgumentException

          Constructs a new ConnectionParm object that is not an extension
          parameter.

        Parameters:
                connectionParmType - The type of the connection
                parameter. Must be one of the values: PACKETS_SENT;
                OCTETS_SENT; PACKETS_RECEIVED; OCTETS_RECEIVED;
                PACKETS_LOST; JITTER; or LATENCY.
                connectionParmValue - The integer value of the connection
                parameter. Must be in the range 0-999999999.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the type
                of the connection parameter is not one of the acceptable
                values or if the connection parameter value is outside
                the range 0-999999999.

   Method Detail

  toString

public java.lang.String toString()

          Returns a java.lang.String of the form "<parmName>=<parmValue>," where
          <parmName> is the name of the connection parameter (e.g., "PS"
          for "packets sent") and <parmValue> is the numeric value of the
          parameter.

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
