/*
 @(#) $RCSfile: Imsi.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:38 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:38 $ by $Author: brian $
 */

package javax.jain.protocol.ss7.map;

import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This parameter class represents an Internation Mobile Subscriber Identity
  * (IMSI) as defined in the sepcification 3G TS 23.003 and ITU-T E.212. <p>
  *
  * The following rules applies for the setting of parameter components: <p>
  *
  * <li><i>Mandatory component</i> with <i>no default value</i> must be
  * provided to the constructor.
  * <li><i>Mandatory component</i> with <i>default value</i> need not be set.
  * <li><i>Optional/conditional component</i> is by default not present.  Such
  * a component becomes present by setting it.
  * <p>
  *
  * <h4>Parameter components:-</h4> <ul>
  * <li><var>mcc</var>, Mobile Country Code, mandatory component with no default.
  * <li><var>mnc</var>, Mobile Network Code, mandatory component with no
  * default.
  * <li><var>msin</var>, Mobile Subscriber Identification Number, mandatory
  * component with no default.
  * <p>
  *
  * Format rule: the length of the whole IMSI must not be more than 15 digits.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class Imsi extends SS7Parameter {
    /**
      * Empty constructor; needed for serializable objects and beans.
      */
    public Imsi() throws SS7InvalidParamException {
    }
    /**
      * The constructor.
      */
    public Imsi(java.lang.String mcc, java.lang.String mnc, java.lang.String msin) throws SS7InvalidParamException {
        this();
        this.setMobileCountryCode(mcc);
        this.setMobileNetworkCode(mnc);
        this.setMobileSubscriberNr(msin);
    }
    /**
      * Change the value of the Mobile Country Code (MCC).
      * @param mcc java.lang.String with 3 digits MCC.
      */
    public void setMobileCountryCode(java.lang.String mcc) throws SS7InvalidParamException {
        int len = mcc.length();
        int max = 15 - length() + getMobileCountryCode().length();
        if (max > 3)
            max = 3;
        if (len > max)
            throw new SS7InvalidParamException("Mobile Country Code is too long by " + (len - max) + " digits.");
        for (int i=0; i < len; i++) {
            switch (mcc.charAt(i)) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
                    break;
                default:
                    throw new SS7InvalidParamException("Mobile Country Mobile Code bad digit '" + mcc.charAt(i) + "' at position " + i + ".");
            }
        }
        m_mcc = mcc;
        m_mcc_is_set = true;
    }
    /**
      * Get the value of thie Mobile Country Code (MCC).
      * @return java.lang.String with 3 digit MCC.
      */
    public java.lang.String getMobileCountryCode() {
        java.lang.String mcc = "";
        if (m_mcc_is_set)
            mcc = m_mcc;
        return mcc;
    }
    /**
      * Change the value of the Mobile Network Code (MNC).
      * @param java.lang.String with 2 or 3 digits MNC.
      */
    public void setMobileNetworkCode(java.lang.String mnc) throws SS7InvalidParamException {
        int len = mnc.length();
        int max = 15 - length() + getMobileNetworkCode().length();
        if (max > 3)
            max = 3;
        if (len > max)
            throw new SS7InvalidParamException("Mobile Network Code is too long by " + (len - max) + " digits.");
        for (int i=0; i < len; i++) {
            switch (mnc.charAt(i)) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
                    break;
                default:
                    throw new SS7InvalidParamException("Mobile Network Code bad digit '" + mnc.charAt(i) + "' at position " + i + ".");
            }
        }
        m_mnc = mnc;
        m_mnc_is_set = true;
    }
    /**
      * Get the value of the Mobile Network Code (MNC).
      * @return java.lang.String with 2 or 3 digits MNC.
      */
    public java.lang.String getMobileNetworkCode() {
        java.lang.String mnc = "";
        if (m_mnc_is_set)
            mnc = m_mnc;
        return mnc;
    }
    /**
      * Change the value of the Mobile Subscriber Identification Number (MSIN).
      * @param msin java.lang.String with MSIN.
      */
    public void setMobileSubscriberNr(java.lang.String msin) throws SS7InvalidParamException {
        int len = msin.length();
        int max = 15 - length() + getMobileSubscriberNr().length();
        if (len > max)
            throw new SS7InvalidParamException("Mobile Subscriber Nr is too long by " + (len - max) + " digits.");
        for (int i=0; i < len; i++) {
            switch (msin.charAt(i)) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
                    break;
                default:
                    throw new SS7InvalidParamException("Mobile Subscriber Nr bad digit '" + msin.charAt(i) + "' at position " + i + ".");
            }
        }
        m_msin = msin;
        m_msin_is_set = true;
    }
    /**
      * Get the value fo the Mobile Subscriber Identification Number (MSIN).
      * @return java.lang.String with MSIN.
      */
    public java.lang.String getMobileSubscriberNr() {
        java.lang.String msin = "";
        if (m_msin_is_set)
            msin = m_msin;
        return msin;
    }
    /**
      * Get the whole IMSI (MCC+MNC+MSIN) as a java.lang.String.
      * @return java.lang.String cntaining the E.164 number.
      */
    public java.lang.String toString() {
        java.lang.String text = "Imsi\n";
        text += "\t\t\tMobile Country Code: " + getMobileCountryCode() + "\n";
        text += "\t\t\tMobile Network Code: " + getMobileNetworkCode() + "\n";
        text += "\t\t\tMobile Subscriber Nr: " + getMobileSubscriberNr() + "\n";
        return text;
    }
    /**
      * Compares this IMSI with another.
      * @param imsi IMSI to be compared with this IMSI.
      * @return True if equal, otherwise false.
      */
    public boolean equals(java.lang.Object object) {
        try {
            if (object != null && object instanceof Imsi) {
                Imsi imsi = (Imsi) object;
                if (m_mcc_is_set != imsi.m_mcc_is_set)
                    return false;
                if (m_mnc_is_set != imsi.m_mnc_is_set)
                    return false;
                if (m_msin_is_set != imsi.m_msin_is_set)
                    return false;
                if (!m_mcc.equals(imsi.m_mcc))
                    return false;
                if (!m_mnc.equals(imsi.m_mnc))
                    return false;
                if (!m_msin.equals(imsi.m_msin))
                    return false;
                return true;
            }
            return false;
        } catch (Exception e) {
            return false;
        }
    }
    /**
      * Get the length of the IMSI; should be 15 digits or less.
      * @return Number of digits.
      */
    public int length() {
        int len = 0;
        len += getMobileCountryCode().length();
        len += getMobileNetworkCode().length();
        len += getMobileSubscriberNr().length();
        return len;
    }
    private java.lang.String m_mcc = "";
    private boolean m_mcc_is_set = false;
    private java.lang.String m_mnc = "";
    private boolean m_mnc_is_set = false;
    private java.lang.String m_msin = "";
    private boolean m_msin_is_set = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
