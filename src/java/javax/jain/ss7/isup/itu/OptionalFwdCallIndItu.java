/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/


package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ITU ISUP Optional Forward Call Indicators.
  * This class provides the methods to access the ITU only sub-fields in the optional
  * forward call indicators parameter.
  * @author Monavacon LImited
  * @version 1.2.2
  */
public class OptionalFwdCallIndItu extends java.lang.Object implements java.io.Serializable {
    public static final byte CUGCI_NON_CUG_CALL = 0;
    public static final byte CUGCI_CUG_CALL_OUTGOING_ACCESS_ALLOWED = 2;
    public static final byte CUGCI_CUG_CALL_OUTGOING_ACCESS_NOT_ALLOWED = 3;
    public static final boolean SSI_NO_ADDITIONAL_INFORMATION = false;
    public static final boolean SSI_ADDITIONAL_INFORMATION_SENT_IN_SEGMENTATION = true;
    public static final boolean CLIRI_NOT_REQUESTED = false;
    public static final boolean CLIRI_REQUESTED = true;
    /** Constructs a new Itu Optional Forward Call Indicators.  */
    public OptionalFwdCallIndItu() {
    }
    /** Constructs an ITU OptionalBwdInd class from the input parameters specified.
      * @param in_cugCallInd  The closed user group call indicator, range 0 to 3. <ul>
      * <li>CUGCI_NON_CUG_CALL, <li>CUGCI_CUG_CALL_OUTGOING_ACCESS_ALLOWED and
      * <li>CUGCI_CUG_CALL_OUTGOING_ACCESS_NOT_ALLOWED. </ul>
      * @param in_segInd  The simple segmentation indicator. <ul>
      * <li>SSI_NO_ADDITIONAL_INFORMATION and
      * <li>SSI_ADDITIONAL_INFORMATION_SENT_IN_SEGMENTATION. </ul>
      * @param in_colrInd  The the connected line identity request indicator. <ul>
      * <li>CLIRI_NOT_REQUESTED and <li>CLIRI_REQUESTED. </ul>
      * @exception ParameterRangeInvalidException - Thrown when value is out of range.
      */
    public OptionalFwdCallIndItu(byte in_cugCallInd, boolean in_segInd,
            boolean in_colrInd)
        throws ParameterRangeInvalidException {
        this.setClosedUserGroupCallInd(in_cugCallInd);
        this.setSegmentationInd(in_segInd);
        this.setConnectedLineIdentityReqInd(in_colrInd);
    }
    /** Gets the Closed User Group Call Indicator field of the parameter.
      * @return The Closed User Group Call Indicator value, range 0 to 3, see
      * OptionalFwdCallIndItu().
      */
    public byte getClosedUserGroupCallInd() {
        return m_CUGcallInd;
    }
    /** Sets the Closed User Group Call Indicator field of the parameter.
      * @param CUGcallInd  The Closed User Group call Indicator value, range 0 to 3, see
      * OptionalFwdCallIndItu().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setClosedUserGroupCallInd(byte CUGcallInd)
        throws ParameterRangeInvalidException {
        if (0 <= CUGcallInd && CUGcallInd <= 3) {
            m_CUGcallInd = CUGcallInd;
            return;
        }
        throw new ParameterRangeInvalidException("ClosedUserGroupCallInd value " + CUGcallInd + " out of range.");
    }
    /** Gets the Simple Segmentation Indicator field of the parameter.
      * @return Boolean containing the SimpleSegmentation value, see
      * OptionalFwdCallIndItu().
      */
    public boolean getSegmentationInd() {
        return m_simpleSegmentation;
    }
    /** Sets the Simple Segmentation Indicator field of the parameter.
      * @param aSimpleSegmentation  The SimpleSegmentation value, see
      * OptionalFwdCallIndItu().
      */
    public void setSegmentationInd(boolean aSimpleSegmentation) {
        m_simpleSegmentation = aSimpleSegmentation;
    }
    /** Gets the Connected Line Identity Request Indicator field of the parameter.
      * @return The COLR Ind value, see OptionalFwdCallIndItu().
      */
    public boolean getConnectedLineIdentityReqInd() {
        return m_COLRInd;
    }
    /** Sets the Connected Line Identity Request Indicator field of the parameter.
      * @param aCOLRInd  The Connected Line Identity Request Indicator value, see
      * OptionalFwdCallIndItu().
      */
    public void setConnectedLineIdentityReqInd(boolean aCOLRInd) {
        m_COLRInd = aCOLRInd;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * OptionalFwdCallIndItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.OptionalFwdCallIndItu");
        b.append("\n\tm_CUGcallInd: " + m_CUGcallInd);
        b.append("\n\tm_simpleSegmentation: " + m_simpleSegmentation);
        b.append("\n\tm_COLRInd: " + m_COLRInd);
        return b.toString();
    }
    protected byte m_CUGcallInd;
    protected boolean m_simpleSegmentation;
    protected boolean m_COLRInd;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
