/*
 @(#) src/java/javax/jain/ss7/inap/datatype/ForwardCallIndicators.java <p>
 
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

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This Class defines the Forward Call Indicators Format (encoded as in Q.763).
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ForwardCallIndicators implements java.io.Serializable {
    /** Constructor For ForwardCallIndicators.  */
    public ForwardCallIndicators(boolean nationalInternationalCallIndicator,
            int endToEndMethodIndicator, boolean interworkingIndicator,
            boolean endToEndInformationIndicator,
            boolean iSDNUserPartIndicator,
            int iSDNUserPartPreferenceIndicator,
            boolean iSDNAccessIndicator, int sCCPMethodIndicator) {
        setNationalInternationalCallIndicator(nationalInternationalCallIndicator);
        setEndToEndMethodIndicator(endToEndMethodIndicator);
        setInterworkingIndicator(interworkingIndicator);
        setEndToEndInformationIndicator(endToEndInformationIndicator);
        setISDNUserPartIndicator(iSDNUserPartIndicator);
        setISDNUserPartPreferenceIndicator(iSDNUserPartPreferenceIndicator);
        setISDNAccessIndicator(iSDNAccessIndicator);
        setSCCPMethodIndicator(sCCPMethodIndicator);
    }
    /** Gets National International Call Indicator.  */
    public boolean getNationalInternationalCallIndicator() {
        return nationalInternationalCallIndicator;
    }
    /** Sets National International Call Indicator.  */
    public void setNationalInternationalCallIndicator(boolean nationalInternationalCallIndicator) {
        this.nationalInternationalCallIndicator = nationalInternationalCallIndicator;
    }
    /** Gets End To End Method Indicator.  */
    public int getEndToEndMethodIndicator() {
        return endToEndMethodIndicator;
    }
    /** Sets End To End Method Indicator.  */
    public void setEndToEndMethodIndicator(int endToEndMethodIndicator) {
        this.endToEndMethodIndicator = endToEndMethodIndicator;
    }
    /** Gets Interworking Indicator.  */
    public boolean getInterworkingIndicator() {
        return interworkingIndicator;
    }
    /** Sets Interworking Indicator.  */
    public void setInterworkingIndicator(boolean interworkingIndicator) {
        this.interworkingIndicator = interworkingIndicator;
    }
    /** Gets End To End Method Information Indicator.  */
    public boolean getEndToEndInformationIndicator() {
        return endToEndInformationIndicator;
    }
    /** Sets End To End Method Information Indicator.  */
    public void setEndToEndInformationIndicator(boolean endToEndInformationIndicator) {
        this.endToEndInformationIndicator = endToEndInformationIndicator;
    }
    /** Gets ISDN UserPart Indicator.  */
    public boolean getISDNUserPartIndicator() {
        return iSDNUserPartIndicator;
    }
    /** Sets ISDN UserPart Indicator.  */
    public void setISDNUserPartIndicator(boolean iSDNUserPartIndicator) {
        this.iSDNUserPartIndicator = iSDNUserPartIndicator;
    }
    /** Gets ISDN UserPart Preference Indicator.  */
    public int getISDNUserPartPreferenceIndicator() {
        return iSDNUserPartPreferenceIndicator;
    }
    /** Sets ISDN UserPart Preference Indicator.  */
    public void setISDNUserPartPreferenceIndicator(int iSDNUserPartPreferenceIndicator) {
        this.iSDNUserPartPreferenceIndicator = iSDNUserPartPreferenceIndicator;
    }
    /** Gets ISDN Access Indicator.  */
    public boolean getISDNAccessIndicator() {
        return iSDNAccessIndicator;
    }
    /** Sets ISDN Access Indicator.  */
    public void setISDNAccessIndicator(boolean iSDNAccessIndicator) {
        this.iSDNAccessIndicator = iSDNAccessIndicator;
    }
    /** Gets SCCP Method Indicator.  */
    public int getSCCPMethodIndicator() {
        return sCCPMethodIndicator;
    }
    /** Sets SCCP Method Indicator.  */
    public void setSCCPMethodIndicator(int sCCPMethodIndicator) {
        this.sCCPMethodIndicator = sCCPMethodIndicator;
    }
    private boolean nationalInternationalCallIndicator;
    private int endToEndMethodIndicator;
    private boolean interworkingIndicator;
    private boolean endToEndInformationIndicator;
    private boolean iSDNUserPartIndicator;
    private int iSDNUserPartPreferenceIndicator;
    private boolean iSDNAccessIndicator;
    private int sCCPMethodIndicator;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
