// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/pkg/MgcpEvent.java <p>
 
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

package jain.protocol.ip.mgcp.pkg;

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

jain.protocol.ip.mgcp.pkg
Class MgcpEvent

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.pkg.MgcpEvent

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class MgcpEvent
   extends java.lang.Object
   implements java.io.Serializable

   The MGCP event class (e.g., dialtone signals, hookup and hookdown
   events, etc.). Two static factory methods are provided: one that
   accepts the "name" of the event (a string) and an integer value to be
   associated uniquely with the named event; the other accepts only the
   name of the event, the unique integer value being supplied by the
   factory. Both factory methods return an object of class MGCPEvent. If
   the first of the two methods is invoked with a name that is already
   associated with another integer value, or with an integer value that
   is already associated with another name, the method throws an
   exception.

   For events/signals that are parameterized (e.g., the "adsi" signal), a
   "withParm" method is provided that allows a string-valued parameter to
   be supplied. The "withParm" method will return a reference to a new
   object of class MgcpEvent, which has the same event name and
   associated integer value as the object on which "withParm" was called,
   but with its own parameter string.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static MgcpEvent adsi
             The "adsi" event (ADSI_DISPLAY).
   static int ADSI_DISPLAY
             Represents the ADSI_DISPLAY event.
   static int ALERTING_TONE
             Represents the ALERTING_TONE event.
   static int ALL_EVENTS
             Represents the ALL_EVENTS wildcard event.
   static MgcpEvent AllEvents
             The "AllEvents" event.
   static MgcpEvent ann
             The "ann" event (PLAY_AN_ANNOUNCEMENT).
   static int ANSWER_SUPERVISION
             Represents the ANSWER_SUPERVISION event.
   static int ANSWER_TONE
             Represents the ANSWER_TONE event.
   static MgcpEvent as
             The "as" event (ANSWER_SUPERVISION).
   static MgcpEvent au
             The "au" event (AUTHORIZATION_SUCCEEDED).
   static int AUTHORIZATION_DENIED
             Represents the AUTHORIZATION_DENIED event.
   static int AUTHORIZATION_SUCCEEDED
             Represents the AUTHORIZATION_SUCCEEDED event.
   static MgcpEvent aw
             The "aw" event (ANSWER_TONE).
   static MgcpEvent ax
             The "ax" event (AUTHORIZATION_DENIED).
   static MgcpEvent bl
             The "bl" event (BLOCKING).
   static int BLOCKING
             Represents the BLOCKING event.
   static int BUSY_TONE
             Represents the BUSY_TONE event.
   static MgcpEvent bz
             The "bz" event (BUSY_TONE).
   static MgcpEvent c01
             The "c01" event (CONTINUITY_TONE).
   static MgcpEvent c02
             The "c02" event (CONTINUITY_TEST).
   static int CALL_BACK_REQUEST
             Represents the CALL_BACK_REQUEST event.
   static int CALL_WAITING_TONE
             Represents the CALL_WAITING_TONE event.
   static int CALL_WAITING_TONE_1
             Represents the CALL_WAITING_TONE_1 event.
   static int CALL_WAITING_TONE_2
             Represents the CALL_WAITING_TONE_2 event.
   static int CALL_WAITING_TONE_3
             Represents the CALL_WAITING_TONE_3 event.
   static int CALL_WAITING_TONE_4
             Represents the CALL_WAITING_TONE_4 event.
   static int CALLER_ID
             Represents the CALLER_ID event.
   static int CALLING_CARD_SERVICE_TONE
             Represents the CALLING_CARD_SERVICE_TONE event.
   static int CARRIER_LOST
             Represents the CARRIER_LOST event.
   static MgcpEvent cbk
             The "cbk" event (CALL_BACK_REQUEST).
   static MgcpEvent cf
             The "cf" event (CONFIRM_TONE).
   static MgcpEvent cg
             The "cg" event (NETWORK_CONGESTION_TONE).
   static MgcpEvent ci
             The "ci" event (CALLER_ID).
   static MgcpEvent cl
             The "cl" event (CARRIER_LOST).
   static int CONFIRM_TONE
             Represents the CONFIRM_TONE event.
   static int CONTINUITY_TEST
             Represents the CONTINUITY_TEST event.
   static int CONTINUITY_TONE
             Represents the CONTINUITY_TONE event.
   static int DIAL_TONE
             Represents the DIAL_TONE event.
   static int DISTINCTIVE_RINGING_0
             Represents the DISTINCTIVE_RINGING_0 event.
   static int DISTINCTIVE_RINGING_1
             Represents the DISTINCTIVE_RINGING_1 event.
   static int DISTINCTIVE_RINGING_2
             Represents the DISTINCTIVE_RINGING_2 event.
   static int DISTINCTIVE_RINGING_3
             Represents the DISTINCTIVE_RINGING_3 event.
   static int DISTINCTIVE_RINGING_4
             Represents the DISTINCTIVE_RINGING_4 event.
   static int DISTINCTIVE_RINGING_5
             Represents the DISTINCTIVE_RINGING_5 event.
   static int DISTINCTIVE_RINGING_6
             Represents the DISTINCTIVE_RINGING_6 event.
   static int DISTINCTIVE_RINGING_7
             Represents the DISTINCTIVE_RINGING_7 event.
   static int DISTINCTIVE_TONE_PATTERN
             Represents the DISTINCTIVE_TONE_PATTERN event.
   static MgcpEvent dl
             The "dl" event (DIAL_TONE).
   static int DTMF_0
             Represents the DTMF_0 event.
   static int DTMF_1
             Represents the DTMF_1 event.
   static int DTMF_2
             Represents the DTMF_2 event.
   static int DTMF_3
             Represents the DTMF_3 event.
   static int DTMF_4
             Represents the DTMF_4 event.
   static int DTMF_5
             Represents the DTMF_5 event.
   static int DTMF_6
             Represents the DTMF_6 event.
   static int DTMF_7
             Represents the DTMF_7 event.
   static int DTMF_8
             Represents the DTMF_8 event.
   static int DTMF_9
             Represents the DTMF_9 event.
   static int DTMF_A
             Represents the DTMF_A event.
   static int DTMF_B
             Represents the DTMF_B event.
   static int DTMF_C
             Represents the DTMF_C event.
   static int DTMF_D
             Represents the DTMF_D event.
   static int DTMF_HASH
             Represents the DTMF_HASH event.
   static int DTMF_STAR
             Represents the DTMF_STAR event.
   static MgcpEvent dtmf0
             The "dtmf0" event (DTMF_0).
   static MgcpEvent dtmf1
             The "dtmf1" event (DTMF_1).
   static MgcpEvent dtmf2
             The "dtmf2" event (DTMF_2).
   static MgcpEvent dtmf3
             The "dtmf3" event (DTMF_3).
   static MgcpEvent dtmf4
             The "dtmf4" event (DTMF_4).
   static MgcpEvent dtmf5
             The "dtmf5" event (DTMF_5).
   static MgcpEvent dtmf6
             The "dtmf6" event (DTMF_6).
   static MgcpEvent dtmf7
             The "dtmf7" event (DTMF_7).
   static MgcpEvent dtmf8
             The "dtmf8" event (DTMF_8).
   static MgcpEvent dtmf9
             The "dtmf9" event (DTMF_9).
   static MgcpEvent dtmfA
             The "dtmfA" event (DTMF_A).
   static MgcpEvent dtmfB
             The "dtmfB" event (DTMF_B).
   static MgcpEvent dtmfC
             The "dtmfC" event (DTMF_C).
   static MgcpEvent dtmfD
             The "dtmfD" event (DTMF_D).
   static MgcpEvent dtmfHash
             The "dtmfHash" event (DTMF_HASH).
   static MgcpEvent dtmfStar
             The "dtmfStar" event (DTMF_STAR).
   static MgcpEvent e
             The "e" event (ERROR_TONE).
   static int ERROR_TONE
             Represents the ERROR_TONE event.
   static int FAX_TONE_DETECTED
             Represents the FAX_TONE_DETECTED event.
   static int FLASH_HOOK
             Represents the FLASH_HOOK event.
   static MgcpEvent ft
             The "ft" event (FAX_TONE_DETECTED).
   static MgcpEvent hd
             The "hd" event (OFF_HOOK_TRANSITION).
   static MgcpEvent hf
             The "hf" event (FLASH_HOOK).
   static MgcpEvent hu
             The "hu" event (ON_HOOK_TRANSITION).
   static int INCOMING_SEIZURE
             Represents the INCOMING_SEIZURE event.
   static int INTERCEPT_TONE
             Represents the INTERCEPT_TONE event.
   static int INTERDIGIT_TIMER
             Represents the INTERDIGIT_TIMER event.
   static MgcpEvent is
             The "is" event (INCOMING_SEIZURE).
   static MgcpEvent it
             The "it" event (INTERCEPT_TONE).
   static MgcpEvent java
             The "java" event (LOAD_JAVA_SCRIPT).
   static MgcpEvent JI
             The "JI" event (JITTER_BUFFER_SIZE_CHANGED).
   static int JITTER_BUFFER_SIZE_CHANGED
             Represents the JITTER_BUFFER_SIZE_CHANGED event.
   static MgcpEvent K0
             The "K0" event (MF_K0_OR_KP).
   static MgcpEvent K1
             The "K1" event (MF_K1).
   static MgcpEvent K2
             The "K2" event (MF_K2).
   static MgcpEvent L
             The "L" event (LONG_DURATION_INDICATOR).
   static MgcpEvent lb
             The "lb" event (LOOPBACK).
   static MgcpEvent ld
             The "ld" event (LONG_DURATION_CONNECTION).
   static int LOAD_JAVA_SCRIPT
             Represents the LOAD_JAVA_SCRIPT event.
   static int LOAD_PERL_SCRIPT
             Represents the LOAD_PERL_SCRIPT event.
   static int LOAD_TCL_SCRIPT
             Represents the LOAD_TCL_SCRIPT event.
   static int LOAD_XML_SCRIPT
             Represents the LOAD_XML_SCRIPT event.
   static int LONG_DURATION_CONNECTION
             Represents the LONG_DURATION_CONNECTION event.
   static int LONG_DURATION_INDICATOR
             Represents the LONG_DURATION_INDICATOR event.
   static int LOOPBACK
             Represents the LOOPBACK event.
   static int MATCH_ANY_DIGIT_WILDCARD
             Represents the MATCH_ANY_DIGIT_WILDCARD event.
   static int MESSAGE_WAITING_INDICATOR
             Represents the MESSAGE_WAITING_INDICATOR event.
   static int MF_0
             Represents the MF_0 event.
   static int MF_1
             Represents the MF_1 event.
   static int MF_2
             Represents the MF_2 event.
   static int MF_3
             Represents the MF_3 event.
   static int MF_4
             Represents the MF_4 event.
   static int MF_5
             Represents the MF_5 event.
   static int MF_6
             Represents the MF_6 event.
   static int MF_7
             Represents the MF_7 event.
   static int MF_8
             Represents the MF_8 event.
   static int MF_9
             Represents the MF_9 event.
   static int MF_K0_OR_KP
             Represents the MF_K0_OR_KP event.
   static int MF_K1
             Represents the MF_K1 event.
   static int MF_K2
             Represents the MF_K2 event.
   static int MF_S0_OR_ST
             Represents the MF_S0_OR_ST event.
   static int MF_S1
             Represents the MF_S1 event.
   static int MF_S2
             Represents the MF_S2 event.
   static int MF_S3
             Represents the MF_S3 event.
   static MgcpEvent mf0
             The "mf0" event (MF_0).
   static MgcpEvent mf1
             The "mf1" event (MF_1).
   static MgcpEvent mf2
             The "mf2" event (MF_2).
   static MgcpEvent mf3
             The "mf3" event (MF_3).
   static MgcpEvent mf4
             The "mf4" event (MF_4).
   static MgcpEvent mf5
             The "mf5" event (MF_5).
   static MgcpEvent mf6
             The "mf6" event (MF_6).
   static MgcpEvent mf7
             The "mf7" event (MF_7).
   static MgcpEvent mf8
             The "mf8" event (MF_8).
   static MgcpEvent mf9
             The "mf9" event (MF_9).
   static int MODEM_DETECTED
             Represents the MODEM_DETECTED event.
   static MgcpEvent mt
             The "mt" event (MODEM_DETECTED).
   static MgcpEvent mwi
             The "mwi" event (MESSAGE_WAITING_INDICATOR).
   static MgcpEvent nbz
             The "nbz" event (NETWORK_BUSY_TONE).
   static int NETWORK_BUSY_TONE
             Represents the NETWORK_BUSY_TONE event.
   static int NETWORK_CONGESTION_TONE
             Represents the NETWORK_CONGESTION_TONE event.
   static int NEW_MILLIWATT_TONE
             Represents the NEW_MILLIWATT_TONE event.
   static MgcpEvent nm
             The "nm" event (NEW_MILLIWATT_TONE).
   static int NO_CIRCUIT
             Represents the NO_CIRCUIT event.
   static MgcpEvent oc
             The "oc" event (REPORT_ON_COMPLETION).
   static MgcpEvent of
             The "of" event (REPORT_FAILURE).
   static int OFF_HOOK_TRANSITION
             Represents the OFF_HOOK_TRANSITION event.
   static int OFFHOOK_WARNING_TONE
             Represents the OFFHOOK_WARNING_TONE event.
   static int OLD_MILLIWATT_TONE
             Represents the OLD_MILLIWATT_TONE event.
   static MgcpEvent om
             The "om" event (OLD_MILLIWATT_TONE).
   static int ON_HOOK_TRANSITION
             Represents the ON_HOOK_TRANSITION event.
   static MgcpEvent ot
             The "ot" event (OFFHOOK_WARNING_TONE).
   static MgcpEvent p
             The "p" event (PROMPT_TONE).
   static MgcpEvent pa
             The "pa" event (PACKET_ARRIVAL).
   static int PACKET_ARRIVAL
             Represents the PACKET_ARRIVAL event.
   static int PACKET_LOSS_EXCEEDED
             Represents the PACKET_LOSS_EXCEEDED event.
   static MgcpEvent pat
             The "pat" event (PATTERN_DETECTED).
   static int PATTERN_DETECTED
             Represents the PATTERN_DETECTED event.
   static MgcpEvent perl
             The "perl" event (LOAD_PERL_SCRIPT).
   static MgcpEvent PL
             The "PL" event (PACKET_LOSS_EXCEEDED).
   static int PLAY_AN_ANNOUNCEMENT
             Represents the PLAY_AN_ANNOUNCEMENT signal.
   static int PREEMPTION_TONE
             Represents the PREEMPTION_TONE event.
   static int PROMPT_TONE
             Represents the PROMPT_TONE event.
   static MgcpEvent pt
             The "pt" event (PREEMPTION_TONE).
   static MgcpEvent qa
             The "qa" event (QUALITY_ALERT).
   static int QUALITY_ALERT
             Represents the QUALITY_ALERT event.
   static MgcpEvent r0
             The "r0" event (DISTINCTIVE_RINGING_0).
   static MgcpEvent r1
             The "r1" event (DISTINCTIVE_RINGING_1).
   static MgcpEvent r2
             The "r2" event (DISTINCTIVE_RINGING_2).
   static MgcpEvent r3
             The "r3" event (DISTINCTIVE_RINGING_3).
   static MgcpEvent r4
             The "r4" event (DISTINCTIVE_RINGING_4).
   static MgcpEvent r5
             The "r5" event (DISTINCTIVE_RINGING_5).
   static MgcpEvent r6
             The "r6" event (DISTINCTIVE_RINGING_6).
   static MgcpEvent r7
             The "r7" event (DISTINCTIVE_RINGING_7).
   static MgcpEvent rbk
             The "rbk" event (RINGBACK_ON_CONNECTION).
   static int RECORDER_WARNING_TONE
             Represents the RECORDER_WARNING_TONE event.
   static int REORDER_TONE
             Represents the REORDER_TONE event.
   static int REPORT_FAILURE
             Represents the REPORT_FAILURE event.
   static int REPORT_ON_COMPLETION
             Represents the REPORT_ON_COMPLETION event.
   static int RETURN_SEIZURE
             Represents the RETURN_SEIZURE event.
   static MgcpEvent rg
             The "rg" event (RINGING).
   static int RINGBACK_ON_CONNECTION
             Represents the RINGBACK_ON_CONNECTION event.
   static int RINGBACK_TONE
             Represents the RINGBACK_TONE event.
   static int RINGING
             Represents the RINGING event.
   static int RINGSPLASH
             Represents the RINGSPLASH event.
   static MgcpEvent ro
             The "ro" event (REORDER_TONE).
   static MgcpEvent rs
             The "rs" event (RETURN_SEIZURE).
   static MgcpEvent rsp
             The "rsp" event (RINGSPLASH).
   static MgcpEvent rt
             The "rt" event (RINGBACK_TONE).
   static MgcpEvent s
             The "s" event (DISTINCTIVE_TONE_PATTERN).
   static MgcpEvent S0
             The "S0" event (MF_S0_OR_ST).
   static MgcpEvent S1
             The "S1" event (MF_S1).
   static MgcpEvent S2
             The "S2" event (MF_S2).
   static MgcpEvent S3
             The "S3" event (MF_S3).
   static int SAMPLING_RATE_CHANGED
             Represents the SAMPLING_RATE_CHANGED event.
   static MgcpEvent sdl
             The "sdl" event (STUTTER_DIALTONE_HANDSET).
   static MgcpEvent sit
             The "sit" event (SIT_TONE).
   static int SIT_TONE
             Represents the SIT_TONE event.
   static int SIT_TONE_HANDSET
             Represents the SIT_TONE_HANDSET event.
   static MgcpEvent sl
             The "sl" event (STUTTER_DIALTONE).
   static MgcpEvent SR
             The "SR" event (SAMPLING_RATE_CHANGED).
   static int STUTTER_DIALTONE
             Represents the STUTTER_DIALTONE event.
   static int STUTTER_DIALTONE_HANDSET
             Represents the STUTTER_DIALTONE_HANDSET event.
   static MgcpEvent t
             The "t" event (SIT_TONE_HANDSET).
   static MgcpEvent T
             The "T" event (INTERDIGIT_TIMER).
   static MgcpEvent tcl
             The "tcl" event (LOAD_TCL_SCRIPT).
   static MgcpEvent tdd
             The "tdd" event (TDD).
   static int TDD
             Represents the TDD event.
   static int TEST_LINE
             Represents the TEST_LINE event.
   static MgcpEvent tl
             The "tl" event (TEST_LINE).
   static MgcpEvent UC
             The "UC" event (USED_CODEC_CHANGED).
   static int UNSEIZE_CIRCUIT
             Represents the UNSEIZE_CIRCUIT event.
   static MgcpEvent us
             The "us" event (UNSEIZE_CIRCUIT).
   static int USED_CODEC_CHANGED
             Represents the USED_CODEC_CHANGED event.
   static int USER_DEFINED_EVENT_START_VALUE
             Represents the initial integer value that can be used for
   user-defined events.
   static MgcpEvent v
             The "v" event (ALERTING_TONE).
   static int VISUAL_MESSAGE_WAITING_INDICATOR
             Represents the VISUAL_MESSAGE_WAITING_INDICATOR event.
   static MgcpEvent vmwi
             The "vmwi" event (VISUAL_MESSAGE_WAITING_INDICATOR).
   static int WINK
             Represents the WINK event.
   static int WINK_OFF
             Represents the WINK_OFF event.
   static MgcpEvent wk
             The "wk" event (WINK).
   static MgcpEvent wko
             The "wko" event (WINK_OFF).
   static MgcpEvent wt
             The "wt" event (CALL_WAITING_TONE).
   static MgcpEvent wt1
             The "wt1" event (CALL_WAITING_TONE_1).
   static MgcpEvent wt2
             The "wt2" event (CALL_WAITING_TONE_2).
   static MgcpEvent wt3
             The "wt3" event (CALL_WAITING_TONE_3).
   static MgcpEvent wt4
             The "wt4" event (CALL_WAITING_TONE_4).
   static MgcpEvent X
             The "X" event (MATCH_ANY_DIGIT_WILDCARD).
   static MgcpEvent xml
             The "xml" event (LOAD_XML_SCRIPT).
   static MgcpEvent y
             The "y" event (RECORDER_WARNING_TONE).
   static MgcpEvent z
             The "z" event (CALLING_CARD_SERVICE_TONE).
   static MgcpEvent zz
             The "zz" event (NO_CIRCUIT).



   Method Summary
   static MgcpEvent factory(java.lang.String eventName)
             The factory method for generating a new MGCP event/signal
   where the integer value to be associated with the event/signal is left
   to the method to supply.
   static MgcpEvent factory(java.lang.String eventName, int eventValue)
             The factory method for generating a new MGCP event/signal
   where an integer value to be associated with the event/signal is
   supplied.
   static int getCurrentLargestEventValue()
             Gets the largest event value that has been used thus far.
    java.lang.String getName()
             Gets the name of the event.
    java.lang.String getParms()
             Gets the parameter string.
    int intValue()
             Gets the integer value that identifies the event uniquely.
    java.lang.String toString()
             Overrides java.lang.Object.toString().
    MgcpEvent withParm(java.lang.String eventParm)



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  adsi

public static final MgcpEvent adsi

          The "adsi" event (ADSI_DISPLAY).
     _________________________________________________________________

  ADSI_DISPLAY

public static final int ADSI_DISPLAY

          Represents the ADSI_DISPLAY event.
     _________________________________________________________________

  ALERTING_TONE

public static final int ALERTING_TONE

          Represents the ALERTING_TONE event.
     _________________________________________________________________

  ALL_EVENTS

public static final int ALL_EVENTS

          Represents the ALL_EVENTS wildcard event.
     _________________________________________________________________

  AllEvents

public static final MgcpEvent AllEvents

          The "AllEvents" event.
     _________________________________________________________________

  ann

public static final MgcpEvent ann

          The "ann" event (PLAY_AN_ANNOUNCEMENT).
     _________________________________________________________________

  ANSWER_SUPERVISION

public static final int ANSWER_SUPERVISION

          Represents the ANSWER_SUPERVISION event.
     _________________________________________________________________

  ANSWER_TONE

public static final int ANSWER_TONE

          Represents the ANSWER_TONE event.
     _________________________________________________________________

  as

public static final MgcpEvent as

          The "as" event (ANSWER_SUPERVISION).
     _________________________________________________________________

  au

public static final MgcpEvent au

          The "au" event (AUTHORIZATION_SUCCEEDED).
     _________________________________________________________________

  AUTHORIZATION_DENIED

public static final int AUTHORIZATION_DENIED

          Represents the AUTHORIZATION_DENIED event.
     _________________________________________________________________

  AUTHORIZATION_SUCCEEDED

public static final int AUTHORIZATION_SUCCEEDED

          Represents the AUTHORIZATION_SUCCEEDED event.
     _________________________________________________________________

  aw

public static final MgcpEvent aw

          The "aw" event (ANSWER_TONE).
     _________________________________________________________________

  ax

public static final MgcpEvent ax

          The "ax" event (AUTHORIZATION_DENIED).
     _________________________________________________________________

  bl

public static final MgcpEvent bl

          The "bl" event (BLOCKING).
     _________________________________________________________________

  BLOCKING

public static final int BLOCKING

          Represents the BLOCKING event.
     _________________________________________________________________

  BUSY_TONE

public static final int BUSY_TONE

          Represents the BUSY_TONE event.
     _________________________________________________________________

  bz

public static final MgcpEvent bz

          The "bz" event (BUSY_TONE).
     _________________________________________________________________

  c01

public static final MgcpEvent c01

          The "c01" event (CONTINUITY_TONE).
     _________________________________________________________________

  c02

public static final MgcpEvent c02

          The "c02" event (CONTINUITY_TEST).
     _________________________________________________________________

  CALL_BACK_REQUEST

public static final int CALL_BACK_REQUEST

          Represents the CALL_BACK_REQUEST event.
     _________________________________________________________________

  CALL_WAITING_TONE

public static final int CALL_WAITING_TONE

          Represents the CALL_WAITING_TONE event.
     _________________________________________________________________

  CALL_WAITING_TONE_1

public static final int CALL_WAITING_TONE_1

          Represents the CALL_WAITING_TONE_1 event.
     _________________________________________________________________

  CALL_WAITING_TONE_2

public static final int CALL_WAITING_TONE_2

          Represents the CALL_WAITING_TONE_2 event.
     _________________________________________________________________

  CALL_WAITING_TONE_3

public static final int CALL_WAITING_TONE_3

          Represents the CALL_WAITING_TONE_3 event.
     _________________________________________________________________

  CALL_WAITING_TONE_4

public static final int CALL_WAITING_TONE_4

          Represents the CALL_WAITING_TONE_4 event.
     _________________________________________________________________

  CALLER_ID

public static final int CALLER_ID

          Represents the CALLER_ID event.
     _________________________________________________________________

  CALLING_CARD_SERVICE_TONE

public static final int CALLING_CARD_SERVICE_TONE

          Represents the CALLING_CARD_SERVICE_TONE event.
     _________________________________________________________________

  CARRIER_LOST

public static final int CARRIER_LOST

          Represents the CARRIER_LOST event.
     _________________________________________________________________

  cbk

public static final MgcpEvent cbk

          The "cbk" event (CALL_BACK_REQUEST).
     _________________________________________________________________

  cf

public static final MgcpEvent cf

          The "cf" event (CONFIRM_TONE).
     _________________________________________________________________

  cg

public static final MgcpEvent cg

          The "cg" event (NETWORK_CONGESTION_TONE).
     _________________________________________________________________

  ci

public static final MgcpEvent ci

          The "ci" event (CALLER_ID).
     _________________________________________________________________

  cl

public static final MgcpEvent cl

          The "cl" event (CARRIER_LOST).
     _________________________________________________________________

  CONFIRM_TONE

public static final int CONFIRM_TONE

          Represents the CONFIRM_TONE event.
     _________________________________________________________________

  CONTINUITY_TEST

public static final int CONTINUITY_TEST

          Represents the CONTINUITY_TEST event.
     _________________________________________________________________

  CONTINUITY_TONE

public static final int CONTINUITY_TONE

          Represents the CONTINUITY_TONE event.
     _________________________________________________________________

  DIAL_TONE

public static final int DIAL_TONE

          Represents the DIAL_TONE event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_0

public static final int DISTINCTIVE_RINGING_0

          Represents the DISTINCTIVE_RINGING_0 event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_1

public static final int DISTINCTIVE_RINGING_1

          Represents the DISTINCTIVE_RINGING_1 event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_2

public static final int DISTINCTIVE_RINGING_2

          Represents the DISTINCTIVE_RINGING_2 event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_3

public static final int DISTINCTIVE_RINGING_3

          Represents the DISTINCTIVE_RINGING_3 event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_4

public static final int DISTINCTIVE_RINGING_4

          Represents the DISTINCTIVE_RINGING_4 event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_5

public static final int DISTINCTIVE_RINGING_5

          Represents the DISTINCTIVE_RINGING_5 event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_6

public static final int DISTINCTIVE_RINGING_6

          Represents the DISTINCTIVE_RINGING_6 event.
     _________________________________________________________________

  DISTINCTIVE_RINGING_7

public static final int DISTINCTIVE_RINGING_7

          Represents the DISTINCTIVE_RINGING_7 event.
     _________________________________________________________________

  DISTINCTIVE_TONE_PATTERN

public static final int DISTINCTIVE_TONE_PATTERN

          Represents the DISTINCTIVE_TONE_PATTERN event.
     _________________________________________________________________

  dl

public static final MgcpEvent dl

          The "dl" event (DIAL_TONE).
     _________________________________________________________________

  DTMF_0

public static final int DTMF_0

          Represents the DTMF_0 event.
     _________________________________________________________________

  DTMF_1

public static final int DTMF_1

          Represents the DTMF_1 event.
     _________________________________________________________________

  DTMF_2

public static final int DTMF_2

          Represents the DTMF_2 event.
     _________________________________________________________________

  DTMF_3

public static final int DTMF_3

          Represents the DTMF_3 event.
     _________________________________________________________________

  DTMF_4

public static final int DTMF_4

          Represents the DTMF_4 event.
     _________________________________________________________________

  DTMF_5

public static final int DTMF_5

          Represents the DTMF_5 event.
     _________________________________________________________________

  DTMF_6

public static final int DTMF_6

          Represents the DTMF_6 event.
     _________________________________________________________________

  DTMF_7

public static final int DTMF_7

          Represents the DTMF_7 event.
     _________________________________________________________________

  DTMF_8

public static final int DTMF_8

          Represents the DTMF_8 event.
     _________________________________________________________________

  DTMF_9

public static final int DTMF_9

          Represents the DTMF_9 event.
     _________________________________________________________________

  DTMF_A

public static final int DTMF_A

          Represents the DTMF_A event.
     _________________________________________________________________

  DTMF_B

public static final int DTMF_B

          Represents the DTMF_B event.
     _________________________________________________________________

  DTMF_C

public static final int DTMF_C

          Represents the DTMF_C event.
     _________________________________________________________________

  DTMF_D

public static final int DTMF_D

          Represents the DTMF_D event.
     _________________________________________________________________

  DTMF_HASH

public static final int DTMF_HASH

          Represents the DTMF_HASH event.
     _________________________________________________________________

  DTMF_STAR

public static final int DTMF_STAR

          Represents the DTMF_STAR event.
     _________________________________________________________________

  dtmf0

public static final MgcpEvent dtmf0

          The "dtmf0" event (DTMF_0).
     _________________________________________________________________

  dtmf1

public static final MgcpEvent dtmf1

          The "dtmf1" event (DTMF_1).
     _________________________________________________________________

  dtmf2

public static final MgcpEvent dtmf2

          The "dtmf2" event (DTMF_2).
     _________________________________________________________________

  dtmf3

public static final MgcpEvent dtmf3

          The "dtmf3" event (DTMF_3).
     _________________________________________________________________

  dtmf4

public static final MgcpEvent dtmf4

          The "dtmf4" event (DTMF_4).
     _________________________________________________________________

  dtmf5

public static final MgcpEvent dtmf5

          The "dtmf5" event (DTMF_5).
     _________________________________________________________________

  dtmf6

public static final MgcpEvent dtmf6

          The "dtmf6" event (DTMF_6).
     _________________________________________________________________

  dtmf7

public static final MgcpEvent dtmf7

          The "dtmf7" event (DTMF_7).
     _________________________________________________________________

  dtmf8

public static final MgcpEvent dtmf8

          The "dtmf8" event (DTMF_8).
     _________________________________________________________________

  dtmf9

public static final MgcpEvent dtmf9

          The "dtmf9" event (DTMF_9).
     _________________________________________________________________

  dtmfA

public static final MgcpEvent dtmfA

          The "dtmfA" event (DTMF_A).
     _________________________________________________________________

  dtmfB

public static final MgcpEvent dtmfB

          The "dtmfB" event (DTMF_B).
     _________________________________________________________________

  dtmfC

public static final MgcpEvent dtmfC

          The "dtmfC" event (DTMF_C).
     _________________________________________________________________

  dtmfD

public static final MgcpEvent dtmfD

          The "dtmfD" event (DTMF_D).
     _________________________________________________________________

  dtmfHash

public static final MgcpEvent dtmfHash

          The "dtmfHash" event (DTMF_HASH).
     _________________________________________________________________

  dtmfStar

public static final MgcpEvent dtmfStar

          The "dtmfStar" event (DTMF_STAR).
     _________________________________________________________________

  e

public static final MgcpEvent e

          The "e" event (ERROR_TONE).
     _________________________________________________________________

  ERROR_TONE

public static final int ERROR_TONE

          Represents the ERROR_TONE event.
     _________________________________________________________________

  FAX_TONE_DETECTED

public static final int FAX_TONE_DETECTED

          Represents the FAX_TONE_DETECTED event.
     _________________________________________________________________

  FLASH_HOOK

public static final int FLASH_HOOK

          Represents the FLASH_HOOK event.
     _________________________________________________________________

  ft

public static final MgcpEvent ft

          The "ft" event (FAX_TONE_DETECTED).
     _________________________________________________________________

  hd

public static final MgcpEvent hd

          The "hd" event (OFF_HOOK_TRANSITION).
     _________________________________________________________________

  hf

public static final MgcpEvent hf

          The "hf" event (FLASH_HOOK).
     _________________________________________________________________

  hu

public static final MgcpEvent hu

          The "hu" event (ON_HOOK_TRANSITION).
     _________________________________________________________________

  INCOMING_SEIZURE

public static final int INCOMING_SEIZURE

          Represents the INCOMING_SEIZURE event.
     _________________________________________________________________

  INTERCEPT_TONE

public static final int INTERCEPT_TONE

          Represents the INTERCEPT_TONE event.
     _________________________________________________________________

  INTERDIGIT_TIMER

public static final int INTERDIGIT_TIMER

          Represents the INTERDIGIT_TIMER event.
     _________________________________________________________________

  is

public static final MgcpEvent is

          The "is" event (INCOMING_SEIZURE).
     _________________________________________________________________

  it

public static final MgcpEvent it

          The "it" event (INTERCEPT_TONE).
     _________________________________________________________________

  java

public static final MgcpEvent java

          The "java" event (LOAD_JAVA_SCRIPT).
     _________________________________________________________________

  JI

public static final MgcpEvent JI

          The "JI" event (JITTER_BUFFER_SIZE_CHANGED).
     _________________________________________________________________

  JITTER_BUFFER_SIZE_CHANGED

public static final int JITTER_BUFFER_SIZE_CHANGED

          Represents the JITTER_BUFFER_SIZE_CHANGED event.
     _________________________________________________________________

  K0

public static final MgcpEvent K0

          The "K0" event (MF_K0_OR_KP).
     _________________________________________________________________

  K1

public static final MgcpEvent K1

          The "K1" event (MF_K1).
     _________________________________________________________________

  K2

public static final MgcpEvent K2

          The "K2" event (MF_K2).
     _________________________________________________________________

  L

public static final MgcpEvent L

          The "L" event (LONG_DURATION_INDICATOR).
     _________________________________________________________________

  lb

public static final MgcpEvent lb

          The "lb" event (LOOPBACK).
     _________________________________________________________________

  ld

public static final MgcpEvent ld

          The "ld" event (LONG_DURATION_CONNECTION).
     _________________________________________________________________

  LOAD_JAVA_SCRIPT

public static final int LOAD_JAVA_SCRIPT

          Represents the LOAD_JAVA_SCRIPT event.
     _________________________________________________________________

  LOAD_PERL_SCRIPT

public static final int LOAD_PERL_SCRIPT

          Represents the LOAD_PERL_SCRIPT event.
     _________________________________________________________________

  LOAD_TCL_SCRIPT

public static final int LOAD_TCL_SCRIPT

          Represents the LOAD_TCL_SCRIPT event.
     _________________________________________________________________

  LOAD_XML_SCRIPT

public static final int LOAD_XML_SCRIPT

          Represents the LOAD_XML_SCRIPT event.
     _________________________________________________________________

  LONG_DURATION_CONNECTION

public static final int LONG_DURATION_CONNECTION

          Represents the LONG_DURATION_CONNECTION event.
     _________________________________________________________________

  LONG_DURATION_INDICATOR

public static final int LONG_DURATION_INDICATOR

          Represents the LONG_DURATION_INDICATOR event.
     _________________________________________________________________

  LOOPBACK

public static final int LOOPBACK

          Represents the LOOPBACK event.
     _________________________________________________________________

  MATCH_ANY_DIGIT_WILDCARD

public static final int MATCH_ANY_DIGIT_WILDCARD

          Represents the MATCH_ANY_DIGIT_WILDCARD event.
     _________________________________________________________________

  MESSAGE_WAITING_INDICATOR

public static final int MESSAGE_WAITING_INDICATOR

          Represents the MESSAGE_WAITING_INDICATOR event.
     _________________________________________________________________

  MF_0

public static final int MF_0

          Represents the MF_0 event.
     _________________________________________________________________

  MF_1

public static final int MF_1

          Represents the MF_1 event.
     _________________________________________________________________

  MF_2

public static final int MF_2

          Represents the MF_2 event.
     _________________________________________________________________

  MF_3

public static final int MF_3

          Represents the MF_3 event.
     _________________________________________________________________

  MF_4

public static final int MF_4

          Represents the MF_4 event.
     _________________________________________________________________

  MF_5

public static final int MF_5

          Represents the MF_5 event.
     _________________________________________________________________

  MF_6

public static final int MF_6

          Represents the MF_6 event.
     _________________________________________________________________

  MF_7

public static final int MF_7

          Represents the MF_7 event.
     _________________________________________________________________

  MF_8

public static final int MF_8

          Represents the MF_8 event.
     _________________________________________________________________

  MF_9

public static final int MF_9

          Represents the MF_9 event.
     _________________________________________________________________

  MF_K0_OR_KP

public static final int MF_K0_OR_KP

          Represents the MF_K0_OR_KP event.
     _________________________________________________________________

  MF_K1

public static final int MF_K1

          Represents the MF_K1 event.
     _________________________________________________________________

  MF_K2

public static final int MF_K2

          Represents the MF_K2 event.
     _________________________________________________________________

  MF_S0_OR_ST

public static final int MF_S0_OR_ST

          Represents the MF_S0_OR_ST event.
     _________________________________________________________________

  MF_S1

public static final int MF_S1

          Represents the MF_S1 event.
     _________________________________________________________________

  MF_S2

public static final int MF_S2

          Represents the MF_S2 event.
     _________________________________________________________________

  MF_S3

public static final int MF_S3

          Represents the MF_S3 event.
     _________________________________________________________________

  mf0

public static final MgcpEvent mf0

          The "mf0" event (MF_0).
     _________________________________________________________________

  mf1

public static final MgcpEvent mf1

          The "mf1" event (MF_1).
     _________________________________________________________________

  mf2

public static final MgcpEvent mf2

          The "mf2" event (MF_2).
     _________________________________________________________________

  mf3

public static final MgcpEvent mf3

          The "mf3" event (MF_3).
     _________________________________________________________________

  mf4

public static final MgcpEvent mf4

          The "mf4" event (MF_4).
     _________________________________________________________________

  mf5

public static final MgcpEvent mf5

          The "mf5" event (MF_5).
     _________________________________________________________________

  mf6

public static final MgcpEvent mf6

          The "mf6" event (MF_6).
     _________________________________________________________________

  mf7

public static final MgcpEvent mf7

          The "mf7" event (MF_7).
     _________________________________________________________________

  mf8

public static final MgcpEvent mf8

          The "mf8" event (MF_8).
     _________________________________________________________________

  mf9

public static final MgcpEvent mf9

          The "mf9" event (MF_9).
     _________________________________________________________________

  MODEM_DETECTED

public static final int MODEM_DETECTED

          Represents the MODEM_DETECTED event.
     _________________________________________________________________

  mt

public static final MgcpEvent mt

          The "mt" event (MODEM_DETECTED).
     _________________________________________________________________

  mwi

public static final MgcpEvent mwi

          The "mwi" event (MESSAGE_WAITING_INDICATOR).
     _________________________________________________________________

  nbz

public static final MgcpEvent nbz

          The "nbz" event (NETWORK_BUSY_TONE).
     _________________________________________________________________

  NETWORK_BUSY_TONE

public static final int NETWORK_BUSY_TONE

          Represents the NETWORK_BUSY_TONE event.
     _________________________________________________________________

  NETWORK_CONGESTION_TONE

public static final int NETWORK_CONGESTION_TONE

          Represents the NETWORK_CONGESTION_TONE event.
     _________________________________________________________________

  NEW_MILLIWATT_TONE

public static final int NEW_MILLIWATT_TONE

          Represents the NEW_MILLIWATT_TONE event.
     _________________________________________________________________

  nm

public static final MgcpEvent nm

          The "nm" event (NEW_MILLIWATT_TONE).
     _________________________________________________________________

  NO_CIRCUIT

public static final int NO_CIRCUIT

          Represents the NO_CIRCUIT event.
     _________________________________________________________________

  oc

public static final MgcpEvent oc

          The "oc" event (REPORT_ON_COMPLETION).
     _________________________________________________________________

  of

public static final MgcpEvent of

          The "of" event (REPORT_FAILURE).
     _________________________________________________________________

  OFF_HOOK_TRANSITION

public static final int OFF_HOOK_TRANSITION

          Represents the OFF_HOOK_TRANSITION event.
     _________________________________________________________________

  OFFHOOK_WARNING_TONE

public static final int OFFHOOK_WARNING_TONE

          Represents the OFFHOOK_WARNING_TONE event.
     _________________________________________________________________

  OLD_MILLIWATT_TONE

public static final int OLD_MILLIWATT_TONE

          Represents the OLD_MILLIWATT_TONE event.
     _________________________________________________________________

  om

public static final MgcpEvent om

          The "om" event (OLD_MILLIWATT_TONE).
     _________________________________________________________________

  ON_HOOK_TRANSITION

public static final int ON_HOOK_TRANSITION

          Represents the ON_HOOK_TRANSITION event.
     _________________________________________________________________

  ot

public static final MgcpEvent ot

          The "ot" event (OFFHOOK_WARNING_TONE).
     _________________________________________________________________

  p

public static final MgcpEvent p

          The "p" event (PROMPT_TONE).
     _________________________________________________________________

  pa

public static final MgcpEvent pa

          The "pa" event (PACKET_ARRIVAL).
     _________________________________________________________________

  PACKET_ARRIVAL

public static final int PACKET_ARRIVAL

          Represents the PACKET_ARRIVAL event.
     _________________________________________________________________

  PACKET_LOSS_EXCEEDED

public static final int PACKET_LOSS_EXCEEDED

          Represents the PACKET_LOSS_EXCEEDED event.
     _________________________________________________________________

  pat

public static final MgcpEvent pat

          The "pat" event (PATTERN_DETECTED).
     _________________________________________________________________

  PATTERN_DETECTED

public static final int PATTERN_DETECTED

          Represents the PATTERN_DETECTED event.
     _________________________________________________________________

  perl

public static final MgcpEvent perl

          The "perl" event (LOAD_PERL_SCRIPT).
     _________________________________________________________________

  PL

public static final MgcpEvent PL

          The "PL" event (PACKET_LOSS_EXCEEDED).
     _________________________________________________________________

  PLAY_AN_ANNOUNCEMENT

public static final int PLAY_AN_ANNOUNCEMENT

          Represents the PLAY_AN_ANNOUNCEMENT signal.
     _________________________________________________________________

  PREEMPTION_TONE

public static final int PREEMPTION_TONE

          Represents the PREEMPTION_TONE event.
     _________________________________________________________________

  PROMPT_TONE

public static final int PROMPT_TONE

          Represents the PROMPT_TONE event.
     _________________________________________________________________

  pt

public static final MgcpEvent pt

          The "pt" event (PREEMPTION_TONE).
     _________________________________________________________________

  qa

public static final MgcpEvent qa

          The "qa" event (QUALITY_ALERT).
     _________________________________________________________________

  QUALITY_ALERT

public static final int QUALITY_ALERT

          Represents the QUALITY_ALERT event.
     _________________________________________________________________

  r0

public static final MgcpEvent r0

          The "r0" event (DISTINCTIVE_RINGING_0).
     _________________________________________________________________

  r1

public static final MgcpEvent r1

          The "r1" event (DISTINCTIVE_RINGING_1).
     _________________________________________________________________

  r2

public static final MgcpEvent r2

          The "r2" event (DISTINCTIVE_RINGING_2).
     _________________________________________________________________

  r3

public static final MgcpEvent r3

          The "r3" event (DISTINCTIVE_RINGING_3).
     _________________________________________________________________

  r4

public static final MgcpEvent r4

          The "r4" event (DISTINCTIVE_RINGING_4).
     _________________________________________________________________

  r5

public static final MgcpEvent r5

          The "r5" event (DISTINCTIVE_RINGING_5).
     _________________________________________________________________

  r6

public static final MgcpEvent r6

          The "r6" event (DISTINCTIVE_RINGING_6).
     _________________________________________________________________

  r7

public static final MgcpEvent r7

          The "r7" event (DISTINCTIVE_RINGING_7).
     _________________________________________________________________

  rbk

public static final MgcpEvent rbk

          The "rbk" event (RINGBACK_ON_CONNECTION).
     _________________________________________________________________

  RECORDER_WARNING_TONE

public static final int RECORDER_WARNING_TONE

          Represents the RECORDER_WARNING_TONE event.
     _________________________________________________________________

  REORDER_TONE

public static final int REORDER_TONE

          Represents the REORDER_TONE event.
     _________________________________________________________________

  REPORT_FAILURE

public static final int REPORT_FAILURE

          Represents the REPORT_FAILURE event.
     _________________________________________________________________

  REPORT_ON_COMPLETION

public static final int REPORT_ON_COMPLETION

          Represents the REPORT_ON_COMPLETION event.
     _________________________________________________________________

  RETURN_SEIZURE

public static final int RETURN_SEIZURE

          Represents the RETURN_SEIZURE event.
     _________________________________________________________________

  rg

public static final MgcpEvent rg

          The "rg" event (RINGING).
     _________________________________________________________________

  RINGBACK_ON_CONNECTION

public static final int RINGBACK_ON_CONNECTION

          Represents the RINGBACK_ON_CONNECTION event.
     _________________________________________________________________

  RINGBACK_TONE

public static final int RINGBACK_TONE

          Represents the RINGBACK_TONE event.
     _________________________________________________________________

  RINGING

public static final int RINGING

          Represents the RINGING event.
     _________________________________________________________________

  RINGSPLASH

public static final int RINGSPLASH

          Represents the RINGSPLASH event.
     _________________________________________________________________

  ro

public static final MgcpEvent ro

          The "ro" event (REORDER_TONE).
     _________________________________________________________________

  rs

public static final MgcpEvent rs

          The "rs" event (RETURN_SEIZURE).
     _________________________________________________________________

  rsp

public static final MgcpEvent rsp

          The "rsp" event (RINGSPLASH).
     _________________________________________________________________

  rt

public static final MgcpEvent rt

          The "rt" event (RINGBACK_TONE).
     _________________________________________________________________

  s

public static final MgcpEvent s

          The "s" event (DISTINCTIVE_TONE_PATTERN).
     _________________________________________________________________

  S0

public static final MgcpEvent S0

          The "S0" event (MF_S0_OR_ST).
     _________________________________________________________________

  S1

public static final MgcpEvent S1

          The "S1" event (MF_S1).
     _________________________________________________________________

  S2

public static final MgcpEvent S2

          The "S2" event (MF_S2).
     _________________________________________________________________

  S3

public static final MgcpEvent S3

          The "S3" event (MF_S3).
     _________________________________________________________________

  SAMPLING_RATE_CHANGED

public static final int SAMPLING_RATE_CHANGED

          Represents the SAMPLING_RATE_CHANGED event.
     _________________________________________________________________

  sdl

public static final MgcpEvent sdl

          The "sdl" event (STUTTER_DIALTONE_HANDSET).
     _________________________________________________________________

  sit

public static final MgcpEvent sit

          The "sit" event (SIT_TONE).
     _________________________________________________________________

  SIT_TONE

public static final int SIT_TONE

          Represents the SIT_TONE event.
     _________________________________________________________________

  SIT_TONE_HANDSET

public static final int SIT_TONE_HANDSET

          Represents the SIT_TONE_HANDSET event.
     _________________________________________________________________

  sl

public static final MgcpEvent sl

          The "sl" event (STUTTER_DIALTONE).
     _________________________________________________________________

  SR

public static final MgcpEvent SR

          The "SR" event (SAMPLING_RATE_CHANGED).
     _________________________________________________________________

  STUTTER_DIALTONE

public static final int STUTTER_DIALTONE

          Represents the STUTTER_DIALTONE event.
     _________________________________________________________________

  STUTTER_DIALTONE_HANDSET

public static final int STUTTER_DIALTONE_HANDSET

          Represents the STUTTER_DIALTONE_HANDSET event.
     _________________________________________________________________

  t

public static final MgcpEvent t

          The "t" event (SIT_TONE_HANDSET).
     _________________________________________________________________

  T

public static final MgcpEvent T

          The "T" event (INTERDIGIT_TIMER).
     _________________________________________________________________

  tcl

public static final MgcpEvent tcl

          The "tcl" event (LOAD_TCL_SCRIPT).
     _________________________________________________________________

  tdd

public static final MgcpEvent tdd

          The "tdd" event (TDD).
     _________________________________________________________________

  TDD

public static final int TDD

          Represents the TDD event.
     _________________________________________________________________

  TEST_LINE

public static final int TEST_LINE

          Represents the TEST_LINE event.
     _________________________________________________________________

  tl

public static final MgcpEvent tl

          The "tl" event (TEST_LINE).
     _________________________________________________________________

  UC

public static final MgcpEvent UC

          The "UC" event (USED_CODEC_CHANGED).
     _________________________________________________________________

  UNSEIZE_CIRCUIT

public static final int UNSEIZE_CIRCUIT

          Represents the UNSEIZE_CIRCUIT event.
     _________________________________________________________________

  us

public static final MgcpEvent us

          The "us" event (UNSEIZE_CIRCUIT).
     _________________________________________________________________

  USED_CODEC_CHANGED

public static final int USED_CODEC_CHANGED

          Represents the USED_CODEC_CHANGED event.
     _________________________________________________________________

  USER_DEFINED_EVENT_START_VALUE

public static final int USER_DEFINED_EVENT_START_VALUE

          Represents the initial integer value that can be used for
          user-defined events.
     _________________________________________________________________

  v

public static final MgcpEvent v

          The "v" event (ALERTING_TONE).
     _________________________________________________________________

  VISUAL_MESSAGE_WAITING_INDICATOR

public static final int VISUAL_MESSAGE_WAITING_INDICATOR

          Represents the VISUAL_MESSAGE_WAITING_INDICATOR event.
     _________________________________________________________________

  vmwi

public static final MgcpEvent vmwi

          The "vmwi" event (VISUAL_MESSAGE_WAITING_INDICATOR).
     _________________________________________________________________

  WINK

public static final int WINK

          Represents the WINK event.
     _________________________________________________________________

  WINK_OFF

public static final int WINK_OFF

          Represents the WINK_OFF event.
     _________________________________________________________________

  wk

public static final MgcpEvent wk

          The "wk" event (WINK).
     _________________________________________________________________

  wko

public static final MgcpEvent wko

          The "wko" event (WINK_OFF).
     _________________________________________________________________

  wt

public static final MgcpEvent wt

          The "wt" event (CALL_WAITING_TONE).
     _________________________________________________________________

  wt1

public static final MgcpEvent wt1

          The "wt1" event (CALL_WAITING_TONE_1).
     _________________________________________________________________

  wt2

public static final MgcpEvent wt2

          The "wt2" event (CALL_WAITING_TONE_2).
     _________________________________________________________________

  wt3

public static final MgcpEvent wt3

          The "wt3" event (CALL_WAITING_TONE_3).
     _________________________________________________________________

  wt4

public static final MgcpEvent wt4

          The "wt4" event (CALL_WAITING_TONE_4).
     _________________________________________________________________

  X

public static final MgcpEvent X

          The "X" event (MATCH_ANY_DIGIT_WILDCARD).
     _________________________________________________________________

  xml

public static final MgcpEvent xml

          The "xml" event (LOAD_XML_SCRIPT).
     _________________________________________________________________

  y

public static final MgcpEvent y

          The "y" event (RECORDER_WARNING_TONE).
     _________________________________________________________________

  z

public static final MgcpEvent z

          The "z" event (CALLING_CARD_SERVICE_TONE).
     _________________________________________________________________

  zz

public static final MgcpEvent zz

          The "zz" event (NO_CIRCUIT).

   Method Detail

  factory

public static MgcpEvent factory(java.lang.String eventName)

          The factory method for generating a new MGCP event/signal where
          the integer value to be associated with the event/signal is
          left to the method to supply.

        Parameters:
                eventName - The string that represents the name of the
                event.
     _________________________________________________________________

  factory

public static MgcpEvent factory(java.lang.String eventName,
                                int eventValue)
                         throws java.lang.IllegalArgumentException

          The factory method for generating a new MGCP event/signal where
          an integer value to be associated with the event/signal is
          supplied.

        Parameters:
                eventName - The string that represents the name of the
                event.
                eventValue - The integer value to be uniquely associated
                with the name of the event.

        Throws:
                java.lang.IllegalArgumentException - Thrown if another
                event name is already associated with the specified event
                value, or if the specified event name is already
                associated with another event value.
     _________________________________________________________________

  getCurrentLargestEventValue

public static int getCurrentLargestEventValue()

          Gets the largest event value that has been used thus far. When
          calling MgcpEvent.factory(eventName, eventValue), can use
          MgcpEvent.getCurrentLargestEventValue() + 1 for eventValue to
          ensure that a non-conflicting event value will be used.
     _________________________________________________________________

  getName

public java.lang.String getName()

          Gets the name of the event.
     _________________________________________________________________

  getParms

public java.lang.String getParms()

          Gets the parameter string.
     _________________________________________________________________

  intValue

public int intValue()

          Gets the integer value that identifies the event uniquely.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().

        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

  withParm

public MgcpEvent withParm(java.lang.String eventParm)
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
