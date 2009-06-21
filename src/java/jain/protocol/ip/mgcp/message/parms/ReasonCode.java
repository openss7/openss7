// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: ReasonCode.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:22 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:22 $ by $Author: brian $
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
Class ReasonCode

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.ReasonCode

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class ReasonCode
   extends java.lang.Object
   implements java.io.Serializable

   Defines the Reason Codes that are used in DeleteConnection commands
   sent by the Media Gateway, in RestartInProgress commands, and in
   responses to Audit requests.

   Reason code constants are defined for all reason codes that have been
   defined in RFC 2705. Each such constant is also encapsulated in a
   final static object of class ReasonCode, so that a reason code can be
   inserted in a command or response object simply by referring to one of
   these predefined ReasonCode objects.

   Invoking getValue() on one of these objects returns the integer value
   that is encapsulated by the object. Invoking getComment() returns the
   comment associated with the reason code, as a java.lang.String. Invoking
   toString() (which overrides java.lang.Object.toString()) returns a
   java.lang.String that displays the integer value of the reason code, followed by
   the associated comment.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static ReasonCode Endpoint_Malfunctioning
             ReasonCode object that signifies that endpoint is
   malfunctioning.
   static int ENDPOINT_MALFUNCTIONING
             Signifies that endpoint is malfunctioning.
   static ReasonCode Endpoint_Out_Of_Service
             ReasonCode object that signifies that endpoint has been
   taken out of service.
   static int ENDPOINT_OUT_OF_SERVICE
             Signifies that endpoint has been taken out of service.
   static ReasonCode Endpoint_State_Is_Nominal
             ReasonCode object that signifies that endpoint state is
   nominal.
   static int ENDPOINT_STATE_IS_NOMINAL
             Signifies that endpoint state is nominal.
   static ReasonCode Loss_Of_Lower_Layer_Connectivity
             ReasonCode object that signifies loss of lower layer
   connectivity (e.g., downstream sync).
   static int LOSS_OF_LOWER_LAYER_CONNECTIVITY
             Signifies loss of lower layer connectivity (e.g., downstream
   sync).



   Method Summary
    java.lang.String getComment()
             Gets the return comment string set in this Reason Code
   object.
    int getValue()
             Gets the integer-valued reason code.
    java.lang.String toString()
             Returns the reason code, followed by the associated comment,
   as a java.lang.String.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  Endpoint_Malfunctioning

public static final ReasonCode Endpoint_Malfunctioning

          ReasonCode object that signifies that endpoint is
          malfunctioning.
     _________________________________________________________________

  ENDPOINT_MALFUNCTIONING

public static final int ENDPOINT_MALFUNCTIONING

          Signifies that endpoint is malfunctioning.
     _________________________________________________________________

  Endpoint_Out_Of_Service

public static final ReasonCode Endpoint_Out_Of_Service

          ReasonCode object that signifies that endpoint has been taken
          out of service.
     _________________________________________________________________

  ENDPOINT_OUT_OF_SERVICE

public static final int ENDPOINT_OUT_OF_SERVICE

          Signifies that endpoint has been taken out of service.
     _________________________________________________________________

  Endpoint_State_Is_Nominal

public static final ReasonCode Endpoint_State_Is_Nominal

          ReasonCode object that signifies that endpoint state is
          nominal.
     _________________________________________________________________

  ENDPOINT_STATE_IS_NOMINAL

public static final int ENDPOINT_STATE_IS_NOMINAL

          Signifies that endpoint state is nominal. (This code is used
          only in response to audit requests.)
     _________________________________________________________________

  Loss_Of_Lower_Layer_Connectivity

public static final ReasonCode Loss_Of_Lower_Layer_Connectivity

          ReasonCode object that signifies loss of lower layer
          connectivity (e.g., downstream sync).
     _________________________________________________________________

  LOSS_OF_LOWER_LAYER_CONNECTIVITY

public static final int LOSS_OF_LOWER_LAYER_CONNECTIVITY

          Signifies loss of lower layer connectivity (e.g., downstream
          sync).

   Method Detail

  getComment

public java.lang.String getComment()

          Gets the return comment string set in this Reason Code object.

        Returns:
                The string to be used as the comment associated with this
                Reason Code.
     _________________________________________________________________

  getValue

public int getValue()

          Gets the integer-valued reason code.

        Returns:
                The integer value of the reason code.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns the reason code, followed by the associated comment, as
          a java.lang.String.

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
