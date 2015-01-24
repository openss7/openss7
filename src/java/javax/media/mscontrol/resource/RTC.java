/*
 @(#) src/java/javax/media/mscontrol/resource/RTC.java <p>
 
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

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.resource
Class RTC

java.lang.Object
   extended by  javax.media.mscontrol.resource.RTC
     _________________________________________________________________

public class RTC

   extends java.lang.Object

   An RTC (Run Time Control) object associates a trigger condition with
   some action. When the Resource identified by the trigger condition
   emits an event that corresponds to that condition, the action command
   is sent to the Resource whose operation is being controlled by the
   RTC.

   The RTC trigger Symbols are a subset of the event Symbols. The events
   available as RTC triggers are defined by the resource that generates
   them.
     * The name of an RTC trigger has the form: rtcc_< EventName>.
     * The name of an RTC action has the form: rtca_< CommandName>.

   For example:
 static RTC speedUp = new RTC(SignalDetector.rtcc_Pattern[2],
                                      Player.rtca_SpeedUp);
        RTC[] rtcs = {speedUp};
        playEvent = play(..., rtcs, ...);

   In this case, when the signal detector resource matches Pattern 2, the
   Player resource recieves the Player.rtca_speedUp command.

   RTC objects typically appear in media transaction commands as elements
   of the RTC[] rtc argument.
     _________________________________________________________________

   Field Summary
   static RTC[] bargeIn
             For ease of programming, this RTC array can be used when
   only the barge-in feature is required.
   static RTC[] NO_RTC
             A typed constant to use when no RTC is required:
   play("my_file.wav, 0, NO_RTC, myParameters);
   static RTC SigDet_StopPlay
             The common RTC to stop a prompt when a DTMF is detected.
   static RTC SigDet_StopRecord
             The common RTC to stop a recording when a DTMF is detected.



   Constructor Summary
   RTC(Symbol trigger, Symbol action)
             Create an RTC object linking the trigger condition to the
   action command.



   Method Summary
    Symbol getAction()
             Return the Symbol that defines action for this RTC.
    Symbol getTrigger()
             Return the Symbol that defines the trigger condition for
   this RTC.



   Methods inherited from class java.lang.Object
   equals, getClass, hashCode, notify, notifyAll, toString, wait, wait,
   wait



   Field Detail

  SigDet_StopPlay

public static final RTC SigDet_StopPlay

          The common RTC to stop a prompt when a DTMF is detected.
     _________________________________________________________________

  SigDet_StopRecord

public static final RTC SigDet_StopRecord

          The common RTC to stop a recording when a DTMF is detected.
     _________________________________________________________________

  NO_RTC

public static final RTC[] NO_RTC

          A typed constant to use when no RTC is required:
          play("my_file.wav, 0, NO_RTC, myParameters);
     _________________________________________________________________

  bargeIn

public static final RTC[] bargeIn

          For ease of programming, this RTC array can be used when only
          the barge-in feature is required.
          For an example of usage, see RecorderConstants#p_Prompt

   Constructor Detail

  RTC

public RTC(Symbol trigger,
           Symbol action)

          Create an RTC object linking the trigger condition to the
          action command.

          The Symbols used to define an RTC trigger are a subset of the
          EventID Symbols. The applicable RTC triggers are defined by the
          resource that generates the event.

          + The name of an RTC trigger has the form: rtcc_&ltEventName>.
          + The name of an RTC action has the form: rtca_&ltCommandName>.

   Method Detail

  getTrigger

public Symbol getTrigger()

          Return the Symbol that defines the trigger condition for this
          RTC.

        Returns:
                the Symbol that defines the trigger condition for this
                RTC.
     _________________________________________________________________

  getAction

public Symbol getAction()

          Return the Symbol that defines action for this RTC.

        Returns:
                the Symbol that defines action for this RTC.
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright (C) 2007-2008 Oracle and/or its affiliates. (C) Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
package javax.jain.media.mscontrol.resource;
