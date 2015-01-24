/*
 @(#) src/java/jain/protocol/ss7/oam/mtp3/Mtp3TimerProfileMBean.java <p>
 
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

package jain.protocol.ss7.oam.mtp3;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the specified timers for the MTP3 Protocol Layer. When this
  * Mtp3TimerProfileMBean is created, the corresponding OamTimerVals should be created for
  * each Timer with the correct min, max and default values set. The Timer values are
  * based on the ITU (Q.704) and ANSI (T1.111.4) Standards.
  *
  * Whenever the Protocol variant of the Protocol Layer is changed, the max, min, and
  * default values should be automatically changed.
  *
  * The comments for each Timer defined within this Timer Profile identify the default
  * values for that Timer.
  *
  * When the value of a timer (t) defined for this Mtp3TimerProfile is changed, an
  * Mtp3AlarmNotification should be sent to all registered Mtp3AlarmNotificationListeners.
  *
  * The Mtp3AlarmNotification (event) should have the following characterics: <ul>
  * <li>event.getSource().getClass().getName() == "jain.protocol.ss7.oam.mtp3.Mtp3TimerProfileMBean"
  * <li>event.getType() == OamAlarmNotification.ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>event.getTimerId() == t.getTimerID() </ul>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>ALARM_TIMER_EXPIRED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  * <li>NON_OAM_RELATED_ERROR
  * <li>MTP3_ERROR_ILLEGAL_TIMER_EXPIRY </ul>
  *
  * <center> [TimerProfiles.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Timer Profiles </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface Mtp3TimerProfileMBean extends OamTimerProfileMBean, Mtp3ManagedObjectMBean {
    /**
      * Returns the ObjectName of the Timer: Delay to avoid message mis-sequencing on
      * changeover [ITU and ANSI].
      * <br> Timer ID: 201
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      * <br> ANSI - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      */
    public ObjectName getMtp3TimerT1();
    /**
      * Returns the ObjectName of the Timer: Waiting for changeover acknowledgement [ITU
      * and ANSI].
      * <br> Timer ID: 202
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 700 ms, Max: 2000 ms, Default: 1350 ms
      * <br> ANSI - Min: 700 ms, Max: 2000 ms, Default: 1350 ms
      */
    public ObjectName getMtp3TimerT2();
    /**
      * Returns the ObjectName of the Timer: Time controlled diversion-delay to avoid
      * mis-sequencing on changeback [ITU and ANSI].
      * <br> Timer ID: 203
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      * <br> ANSI - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      */
    public ObjectName getMtp3TimerT3();
    /**
      * Returns the ObjectName of the Timer: Waiting for changeback acknowledgement (first
      * attempt) [ITU and ANSI].
      * <br> Timer ID: 204
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      * <br> ANSI - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      */
    public ObjectName getMtp3TimerT4();
    /**
      * Returns the ObjectName of the Timer: Waiting for changeback acknowledgement
      * (second attempt) [ITU and ANSI].
      * <br> Timer ID: 205
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      * <br> ANSI - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      */
    public ObjectName getMtp3TimerT5();
    /**
      * Returns the ObjectName of the Timer: Delay to avoid message mis-sequencing on
      * controlled rerouting [ITU and ANSI].
      * <br> Timer ID: 206
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      * <br> ANSI - Min: 500 ms, Max: 1200 ms, Default: 850 ms
      */
    public ObjectName getMtp3TimerT6();
    /**
      * Returns the ObjectName of the Timer: Waiting for signalling data link connection
      * acknowledgement [ITU and ANSI].
      * <br> Timer ID: 207
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 1000 ms, Max: 2000 ms, Default: 1500 ms
      * <br> ANSI - Min: 1000 ms, Max: 2000 ms, Default: 1500 ms
      */
    public ObjectName getMtp3TimerT7();
    /**
      * Returns the ObjectName of the Timer: Transfer prohibited inhibition timer
      * (transient solution) [ITU and ANSI].
      * <br> Timer ID: 208
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 800 ms, Max: 1200 ms, Default: 1000 ms
      * <br> ANSI - Min: 800 ms, Max: 1200 ms, Default: 1000 ms
      */
    public ObjectName getMtp3TimerT8();
    /**
      * Returns the ObjectName of the Timer: Waiting to repeat signalling routeset test
      * message [ITU and ANSI].
      * <br> Timer ID: 210
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 30000 ms, Max: 60000 ms, Default: 4500 ms
      * <br> ANSI - Min: 30000 ms, Max: 60000 ms, Default: 4500 ms
      */
    public ObjectName getMtp3TimerT10();
    /**
      * Returns the ObjectName of the Timer: Transfer restricted timer [ITU and ANSI].
      * <br> Timer ID: 211
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 30000 ms, Max: 90000 ms, Default: 6000 ms
      * <br> ANSI - Min: 30000 ms, Max: 90000 ms, Default: 6000 ms
      */
    public ObjectName getMtp3TimerT11();
    /**
      * Returns the ObjectName of the Timer: Waiting for uninhibit acknowledgement [ITU
      * and ANSI].
      * <br> Timer ID: 212
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 800 ms, Max: 1500 ms, Default: 1150 ms
      * <br> ANSI - Min: 800 ms, Max: 1500 ms, Default: 1150 ms
      */
    public ObjectName getMtp3TimerT12();
    /**
      * Returns the ObjectName of the Timer: Waiting for force uninhibit [ITU and ANSI].
      * <br> Timer ID: 213
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 800 ms, Max: 1500 ms, Default: 1150 ms
      * <br> ANSI - Min: 800 ms, Max: 1500 ms, Default: 1150 ms
      */
    public ObjectName getMtp3TimerT13();
    /**
      * Waiting for inhibition acknowledgement [ITU and ANSI].
      * <br> Timer ID: 214
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 2000 ms, Max: 3000 ms, Default: 2500 ms
      * <br> ANSI - Min: 2000 ms, Max: 3000 ms, Default: 2500 ms
      */
    public ObjectName getMtp3TimerT14();
    /**
      * Returns the ObjectName of the Timer: Waiting to start signalling routeset
      * congestion test [ITU and ANSI].
      * <br> Timer ID: 215
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 2000 ms, Max: 3000 ms, Default: 2500 ms
      * <br> ANSI - Min: 2000 ms, Max: 3000 ms, Default: 2500 ms
      */
    public ObjectName getMtp3TimerT15();
    /**
      * Returns the ObjectName of the Timer: Waiting for routeset congestion status update
      * [ITU and ANSI].
      * <br> Timer ID: 216
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 1400 ms, Max: 2000 ms, Default: 1700 ms
      * <br> ANSI - Min: 1400 ms, Max: 2000 ms, Default: 1700 ms
      */
    public ObjectName getMtp3TimerT16();
    /**
      * Returns the ObjectName of the Timer: Delay to avoid oscillation of initial
      * alignment failure and link restart [ITU and ANSI].
      * <br> Timer ID: 217
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 800 ms, Max: 1500 ms, Default: 1150 ms
      * <br> ANSI - Min: 800 ms, Max: 1500 ms, Default: 1150 ms
      */
    public ObjectName getMtp3TimerT17();
    /**
      * Returns the ObjectName of the Timer: Timer within a signalling point whose MTP
      * restarts for supervising link and link set activation as well as the receipt of
      * routing information [ITU].  Not Used [ANSI].
      * <br> Timer ID: 218
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Recomended Range: The value is implementation and network dependent. Criteria
      * to choose T18 are given in ITU-T Rec. Q.704 9.2. [ITU] The max, min and default
      * values are set initially to 0. These should then be set to their correct values.
      */
    public ObjectName getMtp3TimerT18();
    /**
      * Returns the ObjectName of the Timer: Supervision timer during MTP restart to avoid
      * possible ping-pong of TFP, TFR1 and TRA messages [ITU].  Failed link craft
      * referral timer [ANSI].
      * <br> Timer ID: 219
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 67000 ms, Max: 69000 ms, Default: 68000 ms
      * <br> ANSI - Min: 480000 ms, Max: 600000 ms, Default: 540000 ms
      */
    public ObjectName getMtp3TimerT19();
    /**
      * Returns the ObjectName of the Timer: Overall MTP restart timer at the signalling
      * point whose MTP restarts [ITU].  Waiting to repeat local inhibit test [ANSI].
      * <br> Timer ID: 220
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 59000 ms, Max: 61000 ms, Default: 60000 ms
      * <br> ANSI - Min: 90 ms, Max: 120 ms, Default: 105 ms
      */
    public ObjectName getMtp3TimerT20();
    /**
      * Returns the ObjectName of the Timer: Overall MTP restart timer at a signalling
      * point adjacent to one whose MTP restarts [ITU].  Waiting to repeat remote inhibit
      * test [ANSI].
      * <br> Timer ID: 221
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 63000 ms, Max: 65000 ms, Default: 64000 ms
      * <br> ANSI - Min: 90000 ms, Max: 120000 ms, Default: 105000 ms
      */
    public ObjectName getMtp3TimerT21();
    /**
      * Returns the ObjectName of the Timer: Local inhibit test timer [ITU].  Timer at
      * starting SP, waiting for signalling links to become available [ANSI].
      * <br> Timer ID: 222
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 180000 ms, Max: 360000 ms, Default: 240000 ms
      * <br> ANSI - Network dependent: The max, min and default values are set initially
      * to 0. These should then be set to their correct values.
      */
    public ObjectName getMtp3TimerT22();
    /**
      * Returns the ObjectName of the Timer: Remote inhibit test timer [ITU].  Timer at
      * restating SP, started after Mtp3Timer22, waiting to receive all traffic restart
      * allowed messages [ANSI].
      * <br> Timer ID: 223
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 180000 ms, Max: 360000 ms, Default: 240000 ms
      * <br> ANSI - Network dependent: The max, min and default values are set initially
      * to 0. These should then be set to their correct values.
      */
    public ObjectName getMtp3TimerT23();
    /**
      * Returns the ObjectName of the Timer: Stabilising timer after removal of local
      * processor outage, used in LPO latching to RPO (national option) [ITU].  Timer at
      * restarting SP, started after Mtp3Timer22, waiting to broadcast all traffic restart
      * allowed messages [ANSI].
      * <br> Timer ID: 224
      * <br> Reference: ITU Q.704, Section 16.8. T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 500 ms, Max: 500 ms, Default: 500 ms
      * <br> ANSI - Network dependent: The max, min and default values are set initially
      * to 0. These should then be set to their correct values.
      */
    public ObjectName getMtp3TimerT24();
    /**
      * Returns the ObjectName of the Timer: Timer at SP adjacent to restarting SP,
      * waiting for traffic restart allowed message [ANSI].
      * <br> Timer ID: 225
      * <br> Reference: T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: 30000 ms, Max: 35000 ms, Default: 32500 ms
      */
    public ObjectName getMtp3TimerT25();
    /**
      * Returns the ObjectName of the Timer: Timer at restarting SP, waiting to repeat
      * traffic restart waiting message [ANSI].
      * <br> Timer ID: 226
      * <br> Reference: T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: 12000 ms, Max: 15000 ms, Default: 13500 ms
      */
    public ObjectName getMtp3TimerT26();
    /**
      * Returns the ObjectName of the Timer: Minimum duration of unavailability for full
      * restart [ANSI].
      * <br> Timer ID: 227
      * <br> Reference: T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: 2000 ms, Max: 5000 ms, Default: 3500 ms
      */
    public ObjectName getMtp3TimerT27();
    /**
      * Returns the ObjectName of the Timer: Timer at SP adjacent to restarting SP,
      * waiting for traffic restart waiting message [ANSI].
      * <br> Timer ID: 228
      * <br> Reference: T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: 3000 ms, Max: 35000 ms, Default: 19000 ms
      */
    public ObjectName getMtp3TimerT28();
    /**
      * Returns the ObjectName of the Timer: Timer started when TRA sent in response to
      * unexpected TRA or TRW [ANSI].
      * <br> Timer ID: 229
      * <br> Reference: T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: 60000 ms, Max: 65000 ms, Default: 62500 ms
      */
    public ObjectName getMtp3TimerT29();
    /**
      * Returns the ObjectName of the Timer: Timer to limit sending of TFPs and TFRs in
      * response to unexpected TRA or TRW [ANSI].
      * <br> Timer ID: 230
      * <br> Reference: T1.111.4, Section 16.7
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: 30000 ms, Max: 35000 ms, Default: 32500 ms
      */
    public ObjectName getMtp3TimerT30();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
