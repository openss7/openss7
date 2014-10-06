/*
 @(#) $RCSfile: GTIndicator0010.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:29 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:29 $ by $Author: brian $
 */

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This class represents a Global Title Indicator-0010, it can be used
  * to store, create and retrieve GlobalTitle-0010 entries.
  * @version 1.2.2
  * @author Monavacon Limited
  * @see GlobalTitle
  * @deprecated
  * As of JAIN TCAP v1.1. This class is no longer needed as a result of
  * the addition of the GTIndicator0010 class.
  */
public final class GTIndicator0010 extends GlobalTitle {
    /** @deprecated
      * As of JAIN TCAP v1.1, this empty constructor should no longer be
      * used as all paramters are mandatory in the GT Indicators.  */
    public GTIndicator0010() {
    }
    /** @deprecated
      * Constructs a new Global Title Indicator-0010 with the mandatory
      * fields required for an ANSI and ITU implementation of the
      * GTIndicator-0010.
      * @param translationType
      * The new translation type supplied to the constructor.
      * @param addressInformation
      * The new address Information supplied to the constructor.
      * @since JAIN TCAP v1.1 */
    public GTIndicator0010(byte translationType, byte[] addressInformation) {
        setAddressInformation(addressInformation);
        setTranslationType(translationType);
    }
    /** @deprecated
      * Sets the Translation Type of this Global Title Indicator-0010,
      * which directs the message to the appropriate global title
      * translator. In the case of GTIndicator-0010 the translation type
      * may also imply the encoding scheme, used to encode the address
      * information and the numbering plan.
      * @param translationType
      * The new Translation Type value.  */
    public void setTranslationType(byte translationType) {
        m_translationType = translationType;
        m_translationTypePresent = true;
    }
    /** @deprecated
      * As of JAIN TCAP API v1.1, the translation type is mandatory
      * therefore no longer needs an isPresent method .  */
    public boolean isTranslationTypePresent() {
        return m_translationTypePresent;
    }
    /** @deprecated
      * Gets the Translation Type of this Global Title Indicator-0010,
      * which directs the message to the appropriate global title
      * translator. In the case of GTIndicator-0010 the translation type
      * may also imply the encoding scheme, used to encode the address
      * information and the numbering plan.
      * @return
      * The Translation Type value.
      * @exception MandatoryParameterNotSetException
      * Thrown when this mandatory parameter has not been set.
      * @since JAIN TCAP v1.1 */
    public byte getTranslationType()
        throws MandatoryParameterNotSetException {
        if (m_translationTypePresent)
            return m_translationType;
        throw new MandatoryParameterNotSetException("Translation Type: not set.");
    }
    /** @deprecated
      * This method returns the format of this Global Title.
      * @return
      * The Global Title Indicator value.  */
    public int getGTIndicator() {
        return jain.protocol.ss7.tcap.TcapConstants.GTINDICATOR_0010;
    }
    /** @deprecated
      * Clears all previously set parameters.  */
    public void clearAllParameters() {
        super.clearAllParameters();
        m_translationTypePresent = false;
    }
    /** @deprecated
      * java.lang.String representation of class GTIndicator0010.
      * @return
      * java.lang.String provides description of class GTIndicator0010.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nGlobal Title Indicator = GTINDICATOR_0010");
        b.append(super.toString());
        b.append("\n\tm_translationType = " + m_translationType);
        b.append("\n\tm_translationTypePresent = " + m_translationTypePresent);
        return b.toString();
    }
    /** The Translation Type of this Global Title Indicator.
      * @serial m_translationType
      * - a default serializable field.  */
    private byte m_translationType = 0;
    /** Indcates if the translation type parameter has been set.
      * @serial m_translationTypePresent
      * - a default serializable field.  */
    private boolean m_translationTypePresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
