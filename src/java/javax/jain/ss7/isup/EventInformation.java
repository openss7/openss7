/*
 @(#) $RCSfile: EventInformation.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:04 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:04 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ISUP Event Information parameter.
    This class is common to the ITU and ANSI variants and provides the methods to
    access the sub-fields of this parameter.

    @author Monavacon Limited
    @version 1.2.2
  */
public class EventInformation implements java.io.Serializable {
    public static final byte EI_ALERTING = 1;
    public static final byte EI_PROGRESS = 2;
    public static final byte EI_INBAND_INFORMATION_OR_PATTERN_AVAILABLE = 3;
    public static final byte EI_CALL_FORWARDED_ON_BUSY = 4;
    public static final byte EI_CALL_FORWARDED_ON_NO_REPLY = 5;
    public static final byte EI_CALL_FORWARDED_UNCONDITIONAL = 5;
    public static final boolean EPRI_NO_INDICATION = false;
    public static final boolean EPRI_PRESENTATION_RESTRICTED = true;
    /** Constructs a new EventInformation class, parameters with default values. */
    public EventInformation() {
    }
    /** Constructs a new EventInformation class from the input parameters specified.
      * @param in_eventInd  The event indicator, range 0 to 127; <ul> <li>EI_ALERTING,
      * <li>EI_PROGRESS, <li>EI_INBAND_INFORMATION_OR_PATTERN_AVAILABLE,
      * <li>EI_CALL_FORWARDED_ON_BUSY, <li>EI_CALL_FORWARDED_ON_NO_REPLY and
      * <li>EI_CALL_FORWARDED_UNCONDITIONAL. </ul>
      * @param in_eventPresRestInd  The event presentation restricted indicator; <ul>
      * <li>EPRI_NO_INDICATION and <li>EPRI_PRESENTATION_RESTRICTED. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public EventInformation(byte in_eventInd, boolean in_eventPresRestInd)
        throws ParameterRangeInvalidException {
        this.setEventInd(in_eventInd);
        this.setEventPresRestInd(in_eventPresRestInd);
    }
    /** Gets the Event Indicator field of the parameter.
      * @return Event Indication subfield, range 0 to 127, see EventInformation().  */
    public byte getEventInd() {
        return m_eventInd;
    }
    /** Sets the Event Indicator field of the parameter.
      * @param in_eventInd  The Event Indicator subfield, range 0-127, see
      * EventInformation().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setEventInd(byte in_eventInd)
        throws ParameterRangeInvalidException {
        switch (in_eventInd) {
            case EI_ALERTING:
            case EI_PROGRESS:
            case EI_INBAND_INFORMATION_OR_PATTERN_AVAILABLE:
            case EI_CALL_FORWARDED_ON_BUSY:
            case EI_CALL_FORWARDED_ON_NO_REPLY:
            //case EI_CALL_FORWARDED_UNCONDITIONAL:
                m_eventInd = in_eventInd;
                return;
        }
        throw new ParameterRangeInvalidException("EventInd value " + in_eventInd + " out of range.");
    }
    /** Gets the Event Presentation Restricted Indicator field of the parameter.
      * @return boolean  The Event Presentation Restricted Indicator value, see
      * EventInformation() if the Event Presentation Restricted Indicator is true,
      * then presentation is restricted, false otherwise.  */
    public boolean getEventPresRestInd() {
        return m_eventPresRestInd;
    }
    /** Sets the Event Presentation Restricted Indicator field of the parameter.
      * @param in_eventPresRestInd  The Event Presentation Restricted Indicator value,
      * see EventInformation().  */
    public void setEventPresRestInd(boolean in_eventPresRestInd) {
        m_eventPresRestInd = in_eventPresRestInd;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the EventInformation class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.EventInformation");
        b.append("\n\tm_eventInd: " + m_eventInd);
        b.append("\n\tm_eventPresRestInd: " + m_eventPresRestInd);
        return b.toString();
    }
    protected byte m_eventInd;
    protected boolean m_eventPresRestInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
