// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: QuarantineHandling.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:22 $ <p>
 
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
Class QuarantineHandling

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.QuarantineHandling

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class QuarantineHandling
   extends java.lang.Object
   implements java.io.Serializable

   Specifies the handling of "quarantine" events by the Media Gateway.
   These are events that have been detected by the Gateway before the
   arrival of the NotificationRequest in which this quarantine handling
   specification appears, but that have not yet been sent in a Notify to
   the Call Agent.

   See Also: 
          NotificationRequest, Notify, Serialized Form
     _________________________________________________________________

   Field Summary
   static int DISCARD
             Signifies that quarantined events are to be discarded.
   static QuarantineHandling DiscardLoop
             Encapsulates the DISCARD and LOOP constants.
   static QuarantineHandling DiscardStep
             Encapsulates the DISCARD and STEP constants.
   static int LOOP
             Signifies that multiple notifications are allowed.
   static int PROCESS
             Signifies that quarantined events are to be processed.
   static QuarantineHandling ProcessLoop
             Encapsulates the PROCESS and LOOP constants.
   static QuarantineHandling ProcessStep
             Encapsulates the PROCESS and STEP constants.
   static int STEP
             Signifies that at most one notification is expected.



   Method Summary
    int getNotificationConstraint()
             Gets the constraint on the number of notifications allowed
   (STEP or LOOP).
    int getTreatment()
             Gets the treatment of quarantined events (PROCESS or
   DISCARD).
    java.lang.String toString()
             Returns a string of the form
   "<loopControl>,<processControl>".



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  DISCARD

public static final int DISCARD

          Signifies that quarantined events are to be discarded.
     _________________________________________________________________

  DiscardLoop

public static final QuarantineHandling DiscardLoop

          Encapsulates the DISCARD and LOOP constants.
     _________________________________________________________________

  DiscardStep

public static final QuarantineHandling DiscardStep

          Encapsulates the DISCARD and STEP constants.
     _________________________________________________________________

  LOOP

public static final int LOOP

          Signifies that multiple notifications are allowed.
     _________________________________________________________________

  PROCESS

public static final int PROCESS

          Signifies that quarantined events are to be processed.
     _________________________________________________________________

  ProcessLoop

public static final QuarantineHandling ProcessLoop

          Encapsulates the PROCESS and LOOP constants.
     _________________________________________________________________

  ProcessStep

public static final QuarantineHandling ProcessStep

          Encapsulates the PROCESS and STEP constants.
     _________________________________________________________________

  STEP

public static final int STEP

          Signifies that at most one notification is expected.

   Method Detail

  getNotificationConstraint

public int getNotificationConstraint()

          Gets the constraint on the number of notifications allowed
          (STEP or LOOP).

        Returns:
                STEP or LOOP.
     _________________________________________________________________

  getTreatment

public int getTreatment()

          Gets the treatment of quarantined events (PROCESS or DISCARD).

        Returns:
                PROCESS or DISCARD.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns a string of the form "<loopControl>,<processControl>".

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
