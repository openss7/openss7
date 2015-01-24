/*
 @(#) src/java/jain/protocol/ss7/oam/OamTimerValMBean.java <p>
 
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

package jain.protocol.ss7.oam;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface represents an individual SS7 timer. The maximum and minimum range for a
  * particular timer can be set at construction. Once created, the maximum and minimum
  * range for the timer value cannot be changed. When changing the current value of the
  * timer, the supplied value is checked to see if it falls within the maximum and minimum
  * range. A collection of these timer values can then be stored for each Protocol Layer
  * in the OAM Timer ProfileMBean for that particular layer. <p>
  *
  * <center> [TimerProfiles.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Timer </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface OamTimerValMBean {
    /** This method gets the maximum value for this timer in milliseconds. */
    public Long getMaxVal();
    /** This method gets the minimum value for this timer in milliseconds. */
    public Long getMinVal();
    /**
      * This method sets the current value for this timer in milliseconds. If the supplied
      * value falls within the minimum and maximum range for the timer, the value is
      * changed and the method returns true. If the supplied value lies outside the range,
      * then the value remains unchanged and the method returns false.
      *
      * @return <ul>
      * <li>true if the value has been changed
      * <li>false if the supplied value lies outside the range of this timer (the value
      * remains unchanged). </ul>
      */
    public Boolean setCurrentVal(Long currentVal);
    /** This method gets the current value for this timer in milliseconds. */
    public Long getCurrentVal();
    /** Returns the ID of the this timer value. The IDs of each timer are defined in the
      * appropriate OamTimerProfile. */
    public Integer getTimerId();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
