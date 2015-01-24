/*
 @(#) src/java/javax/jain/ss7/isup/NwStatusIndEvent.java <p>
 
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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** This class is a sub class of the IsupEvent class which is used to indicate one of
  * the following; <ul> <li>Change in remote node status. The different node status
  * relate to node accessibility, inaccessibility and congestion.  <li>Change in
  * remote node user part status. The remote node user part can be either available or
  * unavailable at any point in time. </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class NwStatusIndEvent extends IsupEvent {
    /** User Part status unequipped remote user */
    public static final int ISUP_UP_USER_UNEQUIPPED = 1;
    /** User Part status inaccessible remote user */
    public static final int ISUP_UP_USER_INACCESSIBLE = 2;
    /** User Part status unknown */
    public static final int ISUP_UP_UNKNOWN = 0;
    /** Constructor for initializing the NwStatusInd Event.
      * @param source  The source of this event.
      * @param primitive  Primitive value for NwStatusInd event; <ul>
      * <li>ISUP_PRIMITIVE_MTP_PAUSE, <li>ISUP_PRIMITIVE_MTP_RESUME,
      * <li>ISUP_PRIMITIVE_MTP_STATUS_CONGESTION and
      * <li>ISUP_PRIMITIVE_MTP_STATUS_USER_PART.  </ul>
      * @param dpc  This field is not used.
      * @param opc  This field is not used.
      * @param sls  This field is not used.
      * @param cic  This field is not used.
      * @param congestionPriority  This field is not used.
      * @param spc  Affected signaling point code  This field is used to indicate
      * the Signaling Point Code of the Affected node.
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * set correctly.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public NwStatusIndEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority, int
            primitive, SignalingPointCode spc)
        throws PrimitiveInvalidException, ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setIsupPrimitive(primitive);
        this.setSignalingPointCode(spc);
    }
    /** Gets the ISUP primtive value.
      * @return One of the following ISUP primitive values. If ISUP_PRIMITIVE_UNSET is
      * returned, it indicates the primitive value is not set, hence this is an
      * invalid event. <ul> <li>ISUP_PRIMITIVE_UNSET, <li>ISUP_PRIMITIVE_MTP_PAUSE,
      * <li>ISUP_PRIMITIVE_MTP_RESUME, <li>ISUP_PRIMITIVE_MTP_STATUS_CONGESTION and
      * <li>ISUP_PRIMITIVE_MTP_STATUS_USER_PART. </ul> */
    public int getIsupPrimitive() {
        return m_isupPrimitive;
    }
    /** Sets the Isup primtive value.
      * @param i_isupPrimitive  The value of isup primitive from; <ul>
      * <li>ISUP_PRIMITIVE_MTP_PAUSE, <li>ISUP_PRIMITIVE_MTP_RESUME,
      * <li>ISUP_PRIMITIVE_MTP_STATUS_CONGESTION and
      * <li>ISUP_PRIMITIVE_MTP_STATUS_USER_PART. </ul>
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * set correctly.  */
    public void setIsupPrimitive(int i_isupPrimitive)
                      throws PrimitiveInvalidException {
        switch (i_isupPrimitive) {
            case IsupConstants.ISUP_PRIMITIVE_MTP_PAUSE:
            case IsupConstants.ISUP_PRIMITIVE_MTP_RESUME:
            case IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_CONGESTION:
            case IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_USER_PART:
                m_isupPrimitive = i_isupPrimitive;
                return;
        }
        throw new PrimitiveInvalidException("Primitive value " + i_isupPrimitive + " invalid.");
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory
      * parameter is not set.  */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        if (m_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_UNSET)
            throw new MandatoryParameterNotSetException("IsupPrimitive not set.");
        if (m_spc == null)
            throw new MandatoryParameterNotSetException("AffectedPointCode not set.");
    }
    /** Gets the user part status in the remote node.
      * The user part status is present only when the primitive is
      * ISUP_PRIMITIVE_STATUS_USER_PART in the NwStatusInd event object. This is not
      * present in ANSI variant.
      * @return User Part Status can be one of; <ul> <li>ISUP_UP_USER_UNEQUIPPED,
      * <li>ISUP_UP_USER_INACCESSIBLE and <li>ISUP_UP_UNKNOWN. </ul>
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set. */
    public int getUserPartStatus()
        throws ParameterNotSetException {
        if (m_userPartStatusIsSet)
            return m_userPartStatus;
        throw new ParameterNotSetException("UserPartStatus not set.");
    }
    /** Sets the status of the user part in the remote node.
      * The user part status is present only when the primitive is
      * ISUP_PRIMITIVE_STATUS_USER_PART in the NwStatusInd event object. This is not
      * present in ANSI variant. <p>
      * Implementation Notes:
      * The signature of this method is not right.  It should throw
      * ParameterRangeInvalidException when a user part status is proposed that is
      * different from the allowable values.
      * @param userPartStatus  Status of the user part. Refer to getUserPartStatus
      * method for the value of userPartStatus.  */
    public void setUserPartStatus(int userPartStatus)
        throws ParameterRangeInvalidException {
        switch (userPartStatus) {
            case ISUP_UP_USER_UNEQUIPPED:
            case ISUP_UP_USER_INACCESSIBLE:
            case ISUP_UP_UNKNOWN:
                m_userPartStatus = userPartStatus;
                m_userPartStatusIsSet = true;
                return;
        }
        throw new ParameterRangeInvalidException("userPartStatus value " + userPartStatus + " out of range.");
    }
    /** Indicates whether the user part status field is present or not.
      * The user part status is present only when the primitive is
      * ISUP_PRIMITIVE_STATUS_USER_PART in the NwStatusInd event object. This is not
      * present in ANSI variant.
      * @return True if present else false.  */
    public boolean isUserPartStatusPresent() {
        return m_userPartStatusIsSet;
    }
    /** Gets the Signaling Point Code of the Affected node. <p>
      * Implementation Notes:
      * The signature of this method is not right.  It whould throw
      * ParameterNotSetException when an attempt to read an unset parameter occurs.
      * @return Affected signaling point code. For more details refer to JAIN ISUP
      * Signaling Point Code format.  */
    public SignalingPointCode getSignalingPointCode()
        throws ParameterNotSetException {
        if (m_spc != null)
            return m_spc;
        throw new ParameterNotSetException("AffectedPointCode not set.");
    }
    /** Sets the Signaling Point Code of the Affected node.
      * @param spc  Affected signaling point code. For more details refer to JAIN ISUP
      * Signaling Point Code format
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setSignalingPointCode(SignalingPointCode spc)
        throws ParameterRangeInvalidException {
        m_spc = spc;
    }
    /** Gets the congestion level at the affected point code.
      * The Congestion level range is between 0 and 3. This field is only used with
      * primitive ISUP_PRIMITIVE_MTP_STATUS_CONGESTION.
      * @return Congestion level - range 0 to 3.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public int getCongLevel()
        throws ParameterNotSetException {
        if (m_congestionLevelIsSet)
            return m_congestionLevel;
        throw new ParameterNotSetException("CongLevel parameter not set.");
    }
    /** Sets the congestion level at the affected point code.
      * The Congestion level range is between 0 and 3. This field is only used with
      * primitive ISUP_PRIMITIVE_MTP_STATUS_CONGESTION.
      * @param congestionLevel  Congestion level of the remote node Refer to
      * getCongLevel method for the value of congestionLevel.  */
    public void setCongLevel(int congestionLevel) {
        m_congestionLevel = congestionLevel;
        m_congestionLevelIsSet = true;
    }
    /** isCongLevelPresent method indicates whether this is present in the network
      * status indication event or not.
      * @return True if present else false.  */
    public boolean isCongLevelPresent() {
        return m_congestionLevelIsSet;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the NwStatusIndEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.NwStatusIndEvent");
        b.append("\n\tm_isupPrimitive: " + m_isupPrimitive);
        if (m_userPartStatusIsSet)
            b.append("\n\tm_userPartStatus: " + m_userPartStatus);
        b.append("\n\tm_spc : " + m_spc );
        b.append("\n\tm_congestionLevel: " + m_congestionLevel);
        b.append("\n\tm_congestionLevelIsSet : " + m_congestionLevelIsSet );
        return b.toString();
    }
    protected int m_isupPrimitive = IsupConstants.ISUP_PRIMITIVE_UNSET;
    protected int m_userPartStatus;
    protected boolean m_userPartStatusIsSet = false;
    protected SignalingPointCode m_spc = null;
    protected int m_congestionLevel;
    protected boolean m_congestionLevelIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
