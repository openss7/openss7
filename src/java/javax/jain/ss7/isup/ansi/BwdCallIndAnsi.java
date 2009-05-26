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

package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

/**
  * A class representing an ISUP Backward Call Indicators for ANSI variant. This
  * class inherits from the base BwdCallInd parameter class for adds the ANSI
  * sub-fields to the parameter.
  */
public class BwdCallIndAnsi extends BwdCallInd {
    public static final boolean ISI_NO_IAM_SEGMENTATION_IND = false;
    public static final boolean ISI_IAM_SEGMENTATION_USED = true;

    /**
      * Constructs a new ANSI BwdCallInd class, parameters with default values.
      */
    public BwdCallIndAnsi() {
    }
    /**
      * Constructs an ANSI BwdCallInd class from the input parameters specified.
      * @param in_chargeInd  The charge indicator, range 0-3, see BwdCallInd.
      * @param in_calledStatusInd  The called party's status indicator, range 0-3, see BwdCallInd.
      * @param in_calledCateInd  The called party's category indicator, range 0-3, see BwdCallInd.
      * @param in_eteMethodInd  The end-to-end method indicator, range 0-3, see BwdCallInd.
      * @param in_iwInd  The interworking indicator, see BwdCallInd.
      * @param in_isupInd  The ISDN User Part indicator, see BwdCallInd.
      * @param in_holdInd  The holding indicator, see BwdCallInd.
      * @param in_isdnAccessInd  The ISDB access indicator, see BwdCallInd.
      * @param in_echoDevInd  The echo control device indicator, see BwdCallInd.
      * @param in_sccpMethodInd  The SCCP method indicator, range 0-3, see BwdCallInd.
      * @param in_iamSegInd  The IAM segmentation indicator, see BwdCallInd.
      * <li>ISI_NO_IAM_SEGMENTATION_IND
      * <li>ISI_ADDITIONAL_INFORMATION_RECEIVED_INCOPORATED
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public BwdCallIndAnsi(byte in_chargeInd, byte in_calledStatusInd,
            byte in_calledCatInd, byte in_eteMethodInd, boolean in_iwInd,
            boolean in_isupInd, boolean in_holdInd, boolean in_isdnAccessInd,
            boolean in_echoDevInd, byte in_sccpMethodInd, boolean in_iamSegInd)
        throws ParameterRangeInvalidException {
        super(in_chargeInd, in_calledStatusInd, in_calledCatInd,
                in_eteMethodInd, in_iwInd, in_isupInd, in_holdInd,
                in_isdnAccessInd, in_echoDevInd, in_sccpMethodInd);
        m_iamSegInd = in_iamSegInd;	
    }
    /**
      * Gets the IAM segmentation Indicator field of the parameter.
      * @return The IAMSegmentationIndicator value, see BwdCallIndAnsi().
      */
    public boolean getIAMSegInd() {
        return m_iamSegInd;
    }
    /**
      * Sets the IAM segmentation Indicator field of the parameter.
      * @param aIAMSegmentationIndicator  The IAM segmentation indicator value, see BwdCallIndAnsi().
      */
    public void setIAMSegInd(boolean aIAMSegmentationIndicator) {
        m_iamSegInd = aIAMSegmentationIndicator;
    }
    /**
      * The toString method retrieves a string containing the values of the members of the BwdCallIndAnsi class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\nBwdCallIndAnsi:");
        buffer.append("\n\tIAM Segmentation Indicator: " + m_iamSegInd);
        return buffer.toString();
    }
    private boolean m_iamSegInd = ISI_NO_IAM_SEGMENTATION_IND;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
