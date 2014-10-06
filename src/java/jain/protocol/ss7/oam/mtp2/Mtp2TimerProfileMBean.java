/*
 @(#) $RCSfile: Mtp2TimerProfileMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:25 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:25 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.mtp2;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the specified timers for the MTP2 Protocol Layer. When this
  * Mtp2TimerProfileMBean is created, the corresponding OamTimerValsMBean should be
  * created for each Timer with the correct min, max and default values set. <p>
  *
  * Whenever the Protocol variant of the Protocol Layer is changed, the max, min, and
  * default values should be automatically changed. <p>
  *
  * The Timer values are based on the ITU and ANSI Standards. <p>
  *
  * Timer Default values are rate-dependent (ITU Q.703 , ANSI T1.111) and therefore should
  * be defaulted to the middle of the min and max range.  The comments for each Timer
  * defined within this Timer Profile identify the default values for that Timer. <p>
  *
  * When the value of a timer (t) defined for this Mtp2TimerProfileMBean is changed, an
  * Mtp2AlarmNotification should be sent to all registered Mtp2AlarmNotificationListeners.
  * The Mtp2AlarmNotification (Notification) should have the following characterics: <ul>
  *
  * <li>Notification.getSource().getClass().getName() ==
  * "jain.protocol.ss7.oam.mtp2.Mtp2TimerProfileMBean"
  *
  * <li>Notification.getType() == OamAlarmNotification.ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  *
  * <li>Notification.getTimerId() == t.getTimerID() </ul>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  *
  * <li>ALARM_TIMER_EXPIRED
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  *
  * <li>NON_OAM_RELATED_ERROR
  * <li>MTP2_ERROR_INVALID_TIMER_EXPIRED </ul>
  *
  * <center> [TimerProfiles.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Timer </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface Mtp2TimerProfileMBean extends OamTimerProfileMBean, Mtp2ManagedObjectMBean {
    /**
      * Returns the ObjectName of the Timer: Alignment ready [ITU and ANSI].
      * <br> Timer ID: 101
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 40000 ms, Max: 50000 ms, Default: 45000 ms
      * <br> ANSI - Min: 12500 ms, Max: 13500 ms, Default: 13000 ms
      */
    public ObjectName getMtp2TimerT1();
    /**
      * Returns the ObjectName of the Timer: Not aligned [ITU and ANSI].
      * <br> Timer ID: 102
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 50000 ms, Max: 150000 ms, Default: 100000 ms
      * <br> ANSI - Min: 11000 ms, Max: 12000 ms. Default: 11500 ms
      */
    public ObjectName getMtp2TimerT2();
    /**
      * Returns the ObjectName of the Timer: Aligned [ITU and ANSI].
      * <br> Timer ID: 103
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 1000 ms, Max: 1500 ms, Default: 1250 ms
      * <br> ANSI - Min: 11000 ms, Max: 12000 ms, Default: 11500 ms
      */
    public ObjectName getMtp2TimerT3();
    /**
      * Returns the ObjectName of the Timer: Emergency proving period [ITU and ANSI].
      * <br> Timer ID: 104
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 400 ms, Max: 600, Default: 500 ms
      * <br> ANSI - Min: 540 ms, Max: 600, Default: 580 ms
      */
    public ObjectName getMtp2TimerT4E();
    /**
      * Returns the ObjectName of the Timer: Normal proving period [ITU and ANSI].
      * <br> Timer ID: 105
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 75000 ms, Max: 95000, Default: 85000 ms
      * <br> ANSI - Min: 2000 ms, Max: 2500, Default: 2250 ms
      */
    public ObjectName getMtp2TimerT4N();
    /**
      * Returns the ObjectName of the Timer: Sending SIB [ITU and ANSI].
      * <br> Timer ID: 106
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 80 ms, Max: 120 ms, Default: 100 ms
      * <br> ANSI - Min: 80 ms, Max: 120 ms, Default: 100 ms
      */
    public ObjectName getMtp2TimerT5();
    /**
      * Returns the ObjectName of the Timer: Remote congestion [ITU and ANSI].
      * <br> Timer ID: 107
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 3000 ms, Max: 6000 ms, Default: 4500 ms
      * <br> ANSI - Min: 3000 ms, Max: 6000 ms, Default: 4500 ms
      */
    public ObjectName getMtp2TimerT6();
    /**
      * Returns the ObjectName of the Timer: Excessive delay of acknowledgement [ITU and
      * ANSI].
      * <br> Timer ID: 108
      * <br> Reference: ITU Q.703, Section 12.3 T1.111.3, Section 12.3
      * <br> Default Values: Not Specified
      * <br> ITU - Min: 500 ms, Max: 2000 ms, Default: 1250 ms
      * <br> ANSI - Min: 500 ms, Max: 2000 ms, Default: 1250 ms
      */
    public ObjectName getMtp2TimerT7();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
