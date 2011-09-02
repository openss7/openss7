/*
 @(#) $RCSfile: InfoReqIndAnsi.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:48 $ <p>
 
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
 
 Last Modified $Date: 2011-09-02 08:46:48 $ by $Author: brian $
 */

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI ISUP Information Request Indicators.
    This class inherits from the core class and adds methods to access the ANSI specific
    sub-fields of Information Request Indicator parameter class.

    @author Monavacon Limited
    @version 1.2.2
  */
public class InfoReqIndAnsi extends InfoReqInd {
    public static final boolean MLBGII_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_NOT_REQUESTED = false;
    public static final boolean MLBGII_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_REQUESTED = true;
    /** Constructs a new ANSI InfoReqInd class, parameters with default values.  */
    public InfoReqIndAnsi() {
    }
    /** Constructs an ANSI InfoReqInd class from the input parameters specified.
        @param in_callingAddrReqInd  The calling party address request indicator, see
        InfoReqInd().
        @param in_holdingInd  The holding indicator, see InfoReqInd().
        @param in_callingCatReqInd  The calling party's category request indicator, see
        InfoReqInd().
        @param in_chargeInfoReqInd  The charge information request indicator, see
        InfoReqInd().
        @param in_mciReqInd  The malicious call identification request indicator, see
        InfoReqInd().
        @param in_multiLocBGInfoInd  The multi-location business group information
        response indicator.  <ul>
        <li>MLBGII_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_NOT_REQUESTED and
        <li>MLBGII_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_REQUESTED. </ul>
      */
    public InfoReqIndAnsi(boolean in_callingAddrReqInd, boolean in_holdingInd,
            boolean in_callingCatReqInd, boolean in_chargeInfoReqInd,
            boolean in_mciReqInd, boolean in_multiLocBGInfoInd) {
        super(in_callingAddrReqInd, in_holdingInd, in_callingCatReqInd,
                in_chargeInfoReqInd, in_mciReqInd);
        this.setMultiLocBusinessGroupInfoReqInd(in_multiLocBGInfoInd);
    }
    /** Gets the Multi-location Business Group Information Ind field of the parameter.
        @return Boolean  The Multi-Location Business Group Information Request Indicator
        value, see InfoReqIndAnsi().
      */
    public boolean getMultiLocBusinessGroupInfoReqInd() {
        return m_multiLocBGInfoInd;
    }
    /** Sets the Multi-location Business Group Information Ind field of the parameter.
        @param multiLocBGInfoInd  The
        MultiLocationBusinessGroupInformationResponseIndicator value, see
        InfoReqIndAnsi().
      */
    public void setMultiLocBusinessGroupInfoReqInd(boolean multiLocBGInfoInd) {
        m_multiLocBGInfoInd = multiLocBGInfoInd;
    }
    /** The toString method retrieves a string containing the values of the members of the
        InfoReqIndAnsi class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.InfoReqIndAnsi");
        b.append("\n\tm_multiLocBGInfoInd: " + m_multiLocBGInfoInd);
        return b.toString();
    }
    protected boolean m_multiLocBGInfoInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
