/*
 @(#) $RCSfile: FwdCallIndAnsi.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:07 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:07 $ by $Author: brian $
 */

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI Forward Call Indicator parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class FwdCallIndAnsi extends FwdCallInd {
    public static final boolean ISI_NO_IAM_SEGMENTATION_IND = false;
    public static final boolean ISI_IAM_SEGMENTATION_USED = true;
    /** Constructs a new FwdCallInd taking no parameters.
        This constructor sets all the sub-field values to their spare value (which is
        zero).
      */
    public FwdCallIndAnsi() {
    }
    /** Constructs a new FwdCallIndAnsi with the parameters specified.
        @param endToEndMethodIndicator - The end to end method indicator, range 0-3, see
        FwdCallInd().
        @param interworkingIndicator - The interworking indicator, see FwdCallInd().
        @param isdnUserPartIndicator - The ISDN User part indicator, see FwdCallInd().
        @param isdnUserPartPreferenceIndicator - The ISDN User part preference indicator,
        range 0-3, see FwdCallInd().
        @param isdnAccessIndicator - The ISDN access indicator, see FwdCallInd().
        @param sccpMethodIndicator - The SCCP method indicator, range 0-3, see
        FwdCallInd().
        @param typeOfCallInd - The national/international call indicator, see FwdCallInd().
        @param iamSegmentationIndicator - The IAM Segmentation Indicator. <ul>
        <li>ISI_NO_IAM_SEGMENTATION_IND and <li>ISI_IAM_SEGMENTATION_USED. </ul>
        @exception ParameterRangeInvalidException - Thrown when value is out of range.
      */
    public FwdCallIndAnsi(byte endToEndMethodIndicator, boolean interworkingIndicator,
            boolean isdnUserPartIndicator, byte isdnUserPartPreferenceIndicator,
            boolean isdnAccessIndicator, byte sccpMethodIndicator,
            boolean iamSegmentationIndicator, boolean typeOfCallInd)
        throws ParameterRangeInvalidException {
        super(endToEndMethodIndicator, interworkingIndicator, isdnUserPartIndicator,
                isdnUserPartPreferenceIndicator, isdnAccessIndicator, sccpMethodIndicator,
                typeOfCallInd);
        this.setIAMSegInd(iamSegmentationIndicator);
    }
    /** Gets the IAM Segmentation Indicator field of the parameter.
        @return boolean  The IamSegInd value, see FwdCallIndAnsi().
      */
    public boolean getIAMSegInd() {
        return m_iamSegmentationIndicator;
    }
    /** Sets the IAM Segmentation Indicator field of the parameter.
        @param iamSegmentationIndicator - The IamSegInd value, see FwdCallIndAnsi().
      */
    public void setIAMSegInd(boolean iamSegmentationIndicator) {
        m_iamSegmentationIndicator = iamSegmentationIndicator;
    }
    /** The toString method retrieves a string containing the values of the members of the
        FwdCallIndAnsi class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.FwdCallIndAnsi");
        b.append("\n\tm_iamSegmentationIndicator: " + m_iamSegmentationIndicator);
        return b.toString();
    }
    protected boolean m_iamSegmentationIndicator;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
