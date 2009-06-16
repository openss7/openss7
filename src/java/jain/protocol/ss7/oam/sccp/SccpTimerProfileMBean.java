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

package jain.protocol.ss7.oam.sccp;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the specified timers for the SCCP Protocol Layer. When this
  * SccpTimerProfile is created, the corresponding OamTimerVals should be created for each
  * Timer with the correct min, max and default values set. The Timer values are based on
  * the ITU and ANSI Standards. <p>
  *
  * Whenever the Protocol variant of the Protocol Layer is changed, the max, min, and
  * default values should be automatically changed. <p>
  *
  * The comments for each Timer defined within this Timer Profile identify the default
  * values for that Timer. <p>
  *
  * When the value of any of a timer (t) defined for this SccpTimerProfile is changed, an
  * SccpAlarmNotification should be sent to all registered SccpAlarmNotificationListeners.
  * <p>
  *
  * The SccpAlarmNotification (event) should have the following characterics: <ul>
  * <li>event.getSource().getClass().getName() == "jain.protocol.ss7.oam.sccp.SccpTimerProfileMBean"
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
  * <li>SCCP_ERROR_ILLEGAL_TIMER_EXPIRY </ul>
  *
  * <center> [TimerProfiles.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Timer </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface SccpTimerProfileMBean extends OamTimerProfileMBean, SccpManagedObjectMBean {
    /**
      * Returns the ObjectName of the Timer: Timer Connect - Waiting for connection
      * confirm message [ITU and ANSI]. <p>
      * <br> Timer ID: 301
      * <br> Reference: ITU Q.714, Section C.4
      * <br> Default Values: Not Specified
      * <br> ITU and ANSI - Min: 360000 ms, Max: 720000 ms, Default: 540000 ms
      */
    public ObjectName getSccpTimerConnect();
    /**
      * Returns the ObjectName of the Timer: Timer Guard - Waiting to resume normal
      * procedure for temporary connection sections during the restart procedure [ITU and
      * ANSI]. <p>
      * <br> Timer ID: 302
      * <br> Reference: ITU Q.714, Section C.4, T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ITU and ANSI - Min: 82000000 ms, Max: 90000000 ms, Default: 86000000 ms
      */
    public ObjectName getSccpTimerGuard();
    /**
      * Returns the ObjectName of the Timer: Timer IAR - Waiting to receive a message on a
      * connection section [ITU and ANSI]. <p>
      * <br> Timer ID: 303
      * <br> Reference: ITU Q.714, Section C.4, T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ITU and ANSI - Min: 39600000 ms, Max: 75600000 ms, Default: 62600000 ms
      */
    public ObjectName getSccpTimerIAR();
    /**
      * Returns the ObjectName of the Timer: Timer IAS - Delay to send a message on a conn
      * IT on a connection section when there are no messages to send [ITU and ANSI]. <p>
      * <br> Timer ID: 304
      * <br> Reference: ITU Q.714, Section C.4, T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ITU and ANSI - Min: 1800000 ms, Max: 3600000 ms, Default: 2400000 ms
      */
    public ObjectName getSccpTimerIAS();
    /**
      * Returns the ObjectName of the Timer: Timer Int - Waiting for release complete
      * message; or to release connection resources, freeze the LRN and alert a
      * maintenance function after the initial expiry [ITU]. The timer returned
      * bySccpTimerReleasehas expired. <p>
      * <br> Timer ID: 305
      * <br> Reference: ITU Q.714, Section C.4
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 0 ms, Max: 1000 ms, Default: 500 ms
      */
    public ObjectName getSccpTimerInt();
    /**
      * Returns the ObjectName of the Timer: Reassembly Timer - Waiting to receive all the
      * segments of the remaining segments single segmented message after receiving the
      * first segment [ITU]. <p>
      * <br> Timer ID: 306
      * <br> Reference: ITU Q.714, Section C.4, T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 10000 ms, Max: 20000 ms, Default: 15000 ms
      */
    public ObjectName getSccpTimerReassembly();
    /**
      * Returns the ObjectName of the Timer: Timer Release - Waiting for release complete
      * message [ITU and ANSI]. <p>
      * <br> Timer ID: 307
      * <br> Reference: ITU Q.714, Section C.4
      * <br> Default Values: Not Specified
      * <br> ITU and ANSI - Min: 10000 ms, Max: 20000 ms, Default: 1500 ms
      */
    public ObjectName getSccpTimerRelease();
    /**
      * Returns the ObjectName of the Timer: Timer Repeat Release - Waiting for release
      * complete message; or to repeat sending released message after the initial expiry
      * [ITU]. <p>
      * <br> Timer ID: 308
      * <br> Reference: ITU Q.714, Section C.4
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 10000 ms, Max: 20000 ms, Default: 1500 ms
      */
    public ObjectName getSccpTimerRepeatRelease();
    /**
      * Returns the ObjectName of the Timer: Reset Timer - Waiting to release temporary
      * connection section or alert maintenance function after reset request message is
      * sent [ITU and ANSI]. <p>
      * <br> Timer ID: 309
      * <br> Reference: ITU Q.714, Section C.4, T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ITU and ANSI - Min: 10000 ms, Max: 20000 ms, Default: 15000 ms
      */
    public ObjectName getSccpTimerReset();
    /**
      * Returns the ObjectName of the Timer: Out-of-service Grant Timer - Waiting for
      * grant for subsystem to go out-of-service [ANSI].< p>
      * <br> Timer ID: 309
      * <br> Reference: T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: not specified, Max: not specified, Default: 30,000 ms
      */
    public ObjectName getSccpTimerCoordChg();
    /**
      * Returns the ObjectName of the Timer: Ignore SST Timer - Delay for subsystem
      * between receiving grant to go out-of-service and actually going out of service
      * [ANSI]. <p>
      * <br> Timer ID: 309
      * <br> Reference: T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: not specified, Max: not specified, Default: 30,000 ms
      */
    public ObjectName getSccpTimerIgnoreSST();
    /**
      * Returns the ObjectName of the Timer: Routing Stat Info Timer - Delay between
      * requests for subsystem routing status information [ANSI]. <p>
      * <br> Timer ID: 309
      * <br> Reference: T1.112.4, Section 6.8
      * <br> Default Values: Not Specified
      * <br> ANSI - Min: not specified, Max: not specified, Default: 30,000 ms
      */
    public ObjectName getSccpTimerRtgStatInfo();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
