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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The ContinuityEvent class is a sub class of the IsupEvent class and is
    exchanged between an ISUP Provider and an ISUP Listener for sending and receiving the
    ISUP Continuity message.
    Listener would send a ContinuityEvent object to the provider for sending a COT message
    to the ISUP stack. ISUP Provider would send a ContinuityEvent object to the listener
    on the reception of a COT message from the stack for the user address handled by that
    listener.  This class is common to both ANSI and ITU variants. The mandatory
    parameters are supplied to the constructor. Optional parameters may then be set using
    the set methods The primitive field is filled as ISUP_PRIMITIVE_CONTINUITY.

    @author Monavacon Limited
    @version 1.2.2
  */
public class ContinuityEvent extends IsupEvent {
    /** Constructs a new ContinuityEvent, with only the JAIN ISUP Mandatory parameters
        being supplied to the constructor.
        @param source  The source of this event.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param in_continuityInd  Continuity indicators, 1 for continuity check success and
        0 otherwise.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public ContinuityEvent(Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            byte in_continuityInd)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setContinuityInd(in_continuityInd);
    }
    /** Gets the ISUP CONTINUITY primtive value.
        @return The ISUP CONTINUITY primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_CONTINUITY;
    }
    /** Gets the Continuity Indicator parameter of the message.
        @return The ContinuityInd parameter of the event, 1 if continuity check successful
        0 otherwise.
      */
    public byte getContinuityInd() {
        return m_continuityInd;
    }
    /** Sets the Continuity Indicator parameter of the message.
        @param in_continuityInd  The ContinuityInd parameter of the event 1 if continuity
        check successful 0 otherwise.
        @return ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setContinuityInd(byte in_continuityInd)
        throws ParameterRangeInvalidException {
        switch (in_continuityInd) {
            case 0: case 1: m_continuityInd = in_continuityInd; return;
        }
        throw new ParameterRangeInvalidException("ContinuityInd value " + in_continuityInd + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        ContinuityEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ContinuityEvent");
        b.append("\n\tm_continuityInd: " + m_continuityInd);
        return b.toString();
    }
    protected byte m_continuityInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
