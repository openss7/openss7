/*
 @(#) $RCSfile: UserPartTestInfoItuEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:09 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:09 $ by $Author: brian $
 */

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The UserPartTestInfoItuEvent class is a sub class of the IsupEvent
  * class and is exchanged between an ISUP Provider and an ISUP Listener for sending and
  * receiving the ISUP UPT and UPA Messages.
  * ISUP Provider would send an EventUserPartTestInfo object to the listener on the
  * reception of an UPT or UPA message from the stack for the user address handled by that
  * listener. The getPrimitive() method indicates whether it is User Part Test (UPT) or
  * User Part Available (UPA) message that was received. The mandatory parameters for
  * generating an ISUP message are set using the constructor itself. The optional
  * parameters may be set using get and set methods described below.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class UserPartTestInfoItuEvent extends IsupEvent {
    /** Constructs a new UserPartTestInfoItuEvent, with only the JAIN ISUP Mandatory
      * parameters being supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param primitive  Primitive value is ISUP_PRIMITIVE_USER_PART_TEST for a UPT
      * message and is ISUP_PRIMITIVE_USER_PART_AVAILABLE for a UPA message.  Refer to
      * IsupEvent class for more details.
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not set
      * correctly.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public UserPartTestInfoItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            int primitive)
        throws PrimitiveInvalidException, ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setIsupPrimitive(primitive);
    }
    /** Gets the ISUP primtive value.
      * @return One of the follwoing ISUP primitive values. If ISUP_PRIMITIVE_UNSET is
      * returned, it indicates the primitive value is not set, hence this is an invalid
      * event. <ul> <li>ISUP_PRIMITIVE_UNSET, <li>ISUP_PRIMITIVE_USER_PART_TEST and
      * <li>ISUP_PRIMITIVE_USER_PART_AVAILABLE. </ul>
      */
    public int getIsupPrimitive() {
        return m_primitive;
    }
    /** Sets the ISUP primtive value.
      * @param i_isupPrimitive  The value of isup primitive from <ul>
      * <li>ISUP_PRIMITIVE_USER_PART_TEST and <li>ISUP_PRIMITIVE_USER_PART_AVAILABLE.
      * </ul>
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not set
      * correctly.
      */
    public void setIsupPrimitive(int i_isupPrimitive)
        throws PrimitiveInvalidException {
        switch (i_isupPrimitive) {
            case IsupConstants.ISUP_PRIMITIVE_USER_PART_TEST:
            case IsupConstants.ISUP_PRIMITIVE_USER_PART_AVAILABLE:
                m_primitive = i_isupPrimitive;
                return;
        }
        throw new PrimitiveInvalidException("IsupPrimitive value " + i_isupPrimitive + " out of range.");
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
      * is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        if (getIsupPrimitive() == IsupConstants.ISUP_PRIMITIVE_UNSET)
            throw new MandatoryParameterNotSetException("IsupPrimitive not set.");
    }
    /** Gets the Parameter Compatibility Information parameter of the message.
      * Refer to Parameter Compatibility Information parameter for greater details.
      * @return Parameter Compatibility Information parameter.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public ParamCompatibilityInfoItu getParamCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_paramCompInf != null)
            return m_paramCompInf;
        throw new ParameterNotSetException("ParamCompatibilityInfoItu not set.");
    }
    /** Sets the Parameter Compatibility Information parameter of the message.
      * Refer to Parameter Compatibility Information parameter for greater details.
      * @param in_paramCompInfo  The Parameter Compatibility Information parameter of the
      * event.
      */
    public void setParamCompatibilityInfo(ParamCompatibilityInfoItu in_paramCompInfo) {
        m_paramCompInf = in_paramCompInfo;
    }
    /** Indicates if the Parameter Compatibility Information parameter is present in this
      * Event.
      * @return True if the parameter has been set.
      */
    public boolean isParamCompatibilityInfoPresent() {
        return (m_paramCompInf != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * UserPartTestInfoItuEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.UserPartTestInfoItuEvent");
        b.append("\n\tm_primitive: " + m_primitive);
        b.append("\n\tm_paramCompInf: " + m_paramCompInf);
        return b.toString();
    }
    protected int m_primitive = IsupConstants.ISUP_PRIMITIVE_UNSET;
    protected ParamCompatibilityInfoItu m_paramCompInf = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
