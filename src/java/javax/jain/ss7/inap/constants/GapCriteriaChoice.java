/*
 @(#) $RCSfile: GapCriteriaChoice.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:48 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:48 $ by $Author: brian $
 */

package javax.jain.ss7.inap.constants;

import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * This class indicates the GapCriteria choice.
  *
  * @version 1.2.2
  * @author Monavacon Limited.
  */
public class GapCriteriaChoice {
    /** Internal integer value: CALLED_ADDRESS_VALUE.  */
    public static final int M_CALLED_ADDRESS_VALUE = 0;
    /** Internal integer value: GAP_ON_SERVICE.  */
    public static final int M_GAP_ON_SERVICE = 2;
    /** Internal integer value: GAP_ALL_IN_TRAFFIC.  */
    public static final int M_GAP_ALL_IN_TRAFFIC = 3;
    /** Internal integer value: CALLED_ADDRESS_AND_SERVICE.  */
    public static final int M_CALLED_ADDRESS_AND_SERVICE = 29;
    /** Internal integer value: CALLING_ADDRESS_AND_SERVICE.  */
    public static final int M_CALLING_ADDRESS_AND_SERVICE = 30;
    /** Private internal integer value of constant class.  */
    private int gapCriteriaChoice;
    /** Private constructor for constant class.
      * @param gapCriteriaChoice
      * Internal integer value of constant class.  */
    private GapCriteriaChoice(int gapCriteriaChoice) {
        this.gapCriteriaChoice = gapCriteriaChoice;
    }
    /** GapCriteriaChoice :CALLED_ADDRESS_VALUE */
    public static final GapCriteriaChoice CALLED_ADDRESS_VALUE
        = new GapCriteriaChoice(M_CALLED_ADDRESS_VALUE);
    /** GapCriteriaChoice :GAP_ON_SERVICE */
    public static final GapCriteriaChoice GAP_ON_SERVICE
        = new GapCriteriaChoice(M_GAP_ON_SERVICE);
    /** GapCriteriaChoice : GAP_ALL_IN_TRAFFIC */
    public static final GapCriteriaChoice GAP_ALL_IN_TRAFFIC
        = new GapCriteriaChoice(M_GAP_ALL_IN_TRAFFIC);
    /** GapCriteriaChoice :CALLED_ADDRESS_AND_SERVICE */
    public static final GapCriteriaChoice CALLED_ADDRESS_AND_SERVICE
        = new GapCriteriaChoice(M_CALLED_ADDRESS_AND_SERVICE);
    /** GapCriteriaChoice : CALLING_ADDRESS_AND_SERVICE */
    public static final GapCriteriaChoice CALLING_ADDRESS_AND_SERVICE
        = new GapCriteriaChoice(M_CALLING_ADDRESS_AND_SERVICE);
    /** Gets the integer value representation of the Constant class.
      * @return
      * Internal integer value of constant class.  */
    public int getGapCriteriaChoice() {
        return gapCriteriaChoice;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
