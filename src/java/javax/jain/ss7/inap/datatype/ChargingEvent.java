/*
 @(#) $RCSfile: ChargingEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:52 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:52 $ by $Author: brian $
 */

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class defines the ChargingEvent datatype.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ChargingEvent implements java.io.Serializable {
    /** Constructor For ChargingEvent.  */
    public ChargingEvent(java.lang.String eventTypeCharging, MonitorMode monitorMode) {
        setEventTypeCharging(eventTypeCharging);
        setMonitorMode(monitorMode);
    }
    /** Gets Event Type Charging.  */
    public java.lang.String getEventTypeCharging() {
        return eventTypeCharging;
    }
    /** Sets Event Type Charging.  */
    public void setEventTypeCharging(java.lang.String eventTypeCharging) {
        this.eventTypeCharging = eventTypeCharging;
    }
    /** Gets Monitor Mode.  This parameter gets the information of an
      * event being relayed and/or processed by the SSP. In the context
      * of charging events, the following explanantions hold good. <ul>
      * <li>INTERRUPTED - Means that the SSF has notified the SCF of the
      * charging event; does not process the event but discards it.
      * <li>NOTIFY_AND_CONTINUE - Means that the SSF has notified the
      * SCF of the charging event and continues processing the event
      * without waiting for SCF instructions.
      * <li>TRANSPARENT - Means that the SSF does not notify the SCF of
      * the event. Monitoring the charging of a previously requested
      * charging event is ended. </ul> */
    public MonitorMode getMonitorMode() {
        return monitorMode;
    }
    /** Sets Monitor Mode.  */
    public void setMonitorMode(MonitorMode monitorMode) {
        this.monitorMode = monitorMode;
    }
    /** Gets Leg ID.  */
    public LegID getLegID() throws ParameterNotSetException {
        if (isLegID)
            return legID;
        throw new ParameterNotSetException("Leg Id: not set.");
    }
    /** Sets Leg ID.  */
    public void setLegID(LegID legID) {
        this.legID = legID;
        isLegID = true;  
    }
    /** Indicates if the Leg ID optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isLegIDPresent() {
        return isLegID;
    }
    private java.lang.String eventTypeCharging;
    private MonitorMode monitorMode;
    private LegID legID;
    private boolean isLegID = false;
} 

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
