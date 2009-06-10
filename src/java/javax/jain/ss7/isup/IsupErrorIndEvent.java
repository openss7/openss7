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

/** An ISUP EVENT: The IsupErrorIndEvent class is a sub class of the IsupEvent class
  * and is used to indicate a protocol error to the Listener application, the protocol
  * error relates to incorrectly formatted message being received by the protocol
  * stack.
  * For example, unrecognized message being received etc. This information is filled
  * in the error cause. It is also used to indicate any stack related faults happening
  * in the system. eg. Stack Initialization failure etc.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class IsupErrorIndEvent extends IsupEvent {
    /** Error Cause for unknown message types */
    public static final int ISUP_ERROR_UNKNOWN_MSGTYPE = 1;
    /** Error Cause for Stack initialization failure */
    public static final int ISUP_ERROR_STACK_INIT_FAILURE = 2;
    /** Error Cause for invalid ISUP message received from the user */
    public static final int ISUP_ERROR_INVALID_ISUP_MESSAGE = 3;
    /** Constructor for initializing the IsupErrorInd Event with the information
      * relevant for error processing.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  Not used as it is a locally generated event.
      * @param cic  CIC on which the error has been raised.
      * @param congestionPriority  Not used, filled as 0. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param eCause  Error cause.
      * @param eString  Error string.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public IsupErrorIndEvent(Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority, int
            eCause, java.lang.String eString)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setErrorCause(eCause);
        this.setErrorString(eString);
    }
    /** Gets the ISUP ERROR INDICATOR primtive value.
      * @return The ISUP ERROR INDICATOR primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_ISUP_ERROR_IND;
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory
      * parameter is not set.  */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        if (m_eCauseIsSet)
            throw new MandatoryParameterNotSetException("eCause not set.");
        if (m_eString == null)
            throw new MandatoryParameterNotSetException("eString not set.");
    }
    /** Gets the cause value relating to the error.
      * @return Error Cause can be one of; <ul> <ul>ISUP_ERROR_STACK_INIT_FAILURE or
      * <ul>ISUP_ERROR_UNKNOWN_MSGTYPE. </ul> */
    public int getErrorCause() {
        return m_eCause;
    }
    /** setErrorCause method sets the value for cause value relating to the error.
      * Refer to getErrorCause method for details on error cause.
      * @param eCause  Error cause value.  */
    public void setErrorCause(int eCause) {
        m_eCause = eCause;
        m_eCauseIsSet = true;
    }
    /** getErrorString method returns the string corresponding to the error cause.
      * The value of the string is implementation dependent.
      * @return String describing the error cause.
      * @exception MandatoryParameterException  Thrown if the Mandatory Parameter is
      * not set. */
    public java.lang.String getErrorString()
        throws MandatoryParameterNotSetException {
        if (m_eString != null)
            return m_eString;
        throw new MandatoryParameterNotSetException("eString is not set.");
    }
    /** setErrorString method sets the string corresponding to the error cause the
      * value of the string is implementation dependent.
      * @param eString  Error string describing the error cause.  */
    public void setErrorString(java.lang.String eString) {
        m_eString = eString;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the IsupErrorIndEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.IsupErrorIndEvent");
        if (m_eCauseIsSet)
            b.append("\n\tm_eCause: " + m_eCause);
        b.append("\n\tm_eString: " + m_eString);
        return b.toString();
    }
    protected int m_eCause;
    protected boolean m_eCauseIsSet = false;
    protected String m_eString = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
