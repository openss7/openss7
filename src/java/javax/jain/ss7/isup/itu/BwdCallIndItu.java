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

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing an ISUP Backward Call Indicators for ITU variant.
    This class inherits from the base BwdCallInd parameter class for adding the ITU
    sub-fields to the parameter.

    @author Monavacon Limited
    @version 1.2.2
  */
public class BwdCallIndItu extends BwdCallInd {
    public static final boolean EII_NO_END_TO_END_INFORMATION_AVAILABLE = false;
    public static final boolean EII_END_TO_END_INFORMATION_AVAILABLE = true;
    /** Constructs a new BwdCallIndItu class, parameters with default values.  */
    public BwdCallIndItu() {
    }
    /** Constructs a BwdCallIndItu class from the input parameters specified.
        @param in_chargeInd  The charge indicator, range 0-3, see BwdCallInd.
        @param in_calledStatusInd  The called party's status indicator, range 0-3, see
        BwdCallInd.
        @param in_calledCateInd  The called party's category indicator, range 0-3, see
        BwdCallInd.
        @param in_eteMethodInd  The end-to-end method indicator, range 0-3, see
        BwdCallInd.
        @param in_iwInd  The interworking indicator, see BwdCallInd.
        @param in_isupInd  The ISDN User Part indicator, see BwdCallInd.
        @param in_holdInd  The holding indicator, see BwdCallInd.
        @param in_isdnAccessInd  The ISDB access indicator, see BwdCallInd.
        @param in_echoDevInd  The echo control device indicator, see BwdCallInd.
        @param in_sccpMethodInd  The SCCP method indicator, range 0-3, see BwdCallInd.
        @param in_iamSegInd  The IAM segmentation indicator, see BwdCallInd.
        @param in_eteInfoInd  The end-to-end information indicator. <ul>
        <li>EII_NO_END_TO_END_INFORMATION_AVAILABLE and
        <li>EII_END_TO_END_INFORMATION_AVAILABLE. </ul>
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public BwdCallIndItu(byte in_chargeInd, byte in_calledStatusInd, byte in_calledCatInd,
            byte in_eteMethodInd, boolean in_iwInd, boolean in_isupInd,
            boolean in_holdInd, boolean in_isdnAccessInd, boolean in_echoDevInd,
            byte in_sccpMethodInd, boolean in_eteInfoInd)
        throws ParameterRangeInvalidException {
        super(in_chargeInd, in_calledStatusInd, in_calledCatInd, in_eteMethodInd,
                in_iwInd, in_isupInd, in_holdInd, in_isdnAccessInd, in_echoDevInd,
                in_sccpMethodInd);
         this.setETEInfoInd(in_eteInfoInd);
    }
    /** Gets the End To End Information Indicator field of the parameter.
        @return Boolean, the EndToEndInformationIndicator value, see BwdCallIndItu().
      */
    public boolean getETEInfoInd() {
        return m_endToEndInformationIndicator;
    }
    /** Sets the End To End Information Indicator field of the parameter.
        @param aEndToEndInformationIndicator  Value of End To End Information Indicator
        field, see BwdCallIndItu().
      */
    public void setETEInfoInd(boolean aEndToEndInformationIndicator) {
        m_endToEndInformationIndicator = aEndToEndInformationIndicator;
    }
    /** The toString method retrieves a string containing the values of the members of the
        BwdCallIndItu class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.BwdCallIndItu");
        b.append("\n\tm_endToEndInformationIndicator: " + m_endToEndInformationIndicator);
        return b.toString();
    }
    protected boolean m_endToEndInformationIndicator;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
