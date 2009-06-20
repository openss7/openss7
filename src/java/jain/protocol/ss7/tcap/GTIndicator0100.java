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

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This class represents a Global Title Indicator-0100, it can be used
  * to store, create and retrieve GlobalTitle-0100 entries. It is
  * specific only to ITU, as it not assigned in US Networks.
  * @version 1.2.2
  * @author Monavacon Limited
  * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as
  * a result of the addition of the GTIndicator0100 class.
  */
public final class GTIndicator0100 extends GlobalTitle {
    /** @deprecated
      * As of JAIN TCAP v1.1, this empty constructor should no longer be
      * used as all paramters are mandatory in the GT Indicators.  */
    public GTIndicator0100() {
    }
    /** @deprecated
      * Constructs a new Global Title Indicator-0100 with the relevent
      * mandatory parameters.
      * @param translationType
      * The new Translation Type supplied to the constructor.
      * @param numberingPlan
      * The new Numbering Plan supplied to the constructor.
      * @param encodingScheme
      * The new Encoding Scheme supplied to the constructor.
      * @param natureOfAddrInd
      * The new Nature Of Address Indicator supplied to the constructor.
      * @param addressInformation
      * The new Address Information supplied to the constructor.
      * @since JAIN TCAP v1.1 */
    public GTIndicator0100(byte translationType, int numberingPlan,
            int encodingScheme, int natureOfAddrInd,
            byte[] addressInformation) {
        setAddressInformation(addressInformation);
        setTranslationType(translationType);
        setNumberingPlan(numberingPlan);
        setEncodingScheme(encodingScheme);
        setNatureOfAddrInd(natureOfAddrInd);
    }
    /** @deprecated
      * Sets the Translation Type of this Global Title Indicator-0001,
      * which directs the message to the appropriate global title
      * translator. The coding and definition of the translation type
      * for GTIndicator-0011 is for further study.
      * @param translationType
      * The new Translation Type value.  */
    public void setTranslationType(byte translationType) {
        m_translationType = translationType;
        m_translationTypePresent = true;
    }
    /** @deprecated
      * Sets the Numbering Plan of this Global Title Indicator-0100.
      * @param numberingPlan
      * One of the following:- <ul>
      * <li>NP_UNKNOWN - Indicates an unknown Numbering Plan
      * <li>NP_ISDN_TEL - Indicates ISDN/Telephony Numbering Plan
      * <li>NP_RESERVED - Indicates a reserved Numbering plan in ANSI.
      * <li>NP_DATA - Indicates Data Numbering Plan
      * <li>NP_TELEX - Indicates a Telex Numbering Plan
      * <li>NP_MARITIME_MOBILE - Indicates a Maritime Mobile Numbering Plan
      * <li>NP_LAND_MOBILE - Indicates a Land Mobile Numbering Plan
      * <li>NP_ISDN_MOBILE - Indicates an ISDN Mobile Numbering Plan </ul>
      * @see TcapConstants */
    public void setNumberingPlan(int numberingPlan) {
        m_numberingPlan = numberingPlan;
        m_numberingPlanPresent = true;
    }
    /** @deprecated
      * Sets the Encoding Scheme of this Global Title Indicator-0100.
      * @param encodingScheme
      * The new Encoding Scheme value.  */
    public void setEncodingScheme(int encodingScheme) {
        m_encodingScheme = encodingScheme;
        m_encodingSchemePresent = true;
    }
    /** @deprecated
      * Sets the Nature of Address Indicator of this Global Title
      * Indicator-0100.
      * @param natureOfAddrInd
      * One of the following: <ul>
      * <li>NA_UNKNOWN - Indicates a unknown Nature of Address
      * <li>NA_SUBSCRIBER - Indicates a subscriber number
      * <li>NA_RESERVED - Reserved for national use Nature of Address Indicator
      * <li>NA_NATIONAL_SIGNIFICANT - Indicates a National Significant Number
      * <li>NA_INTERNATIONAL - Indicates an international number </ul>
      * @see TcapConstants */
    public void setNatureOfAddrInd(int natureOfAddrInd) {
        m_natureOfAddrInd = natureOfAddrInd;
        m_natureOfAddrIndPresent = true;
    }
    /** @deprecated
      * As of JAIN TCAP API v1.1, the translation type is mandatory
      * therefore no longer needs an isPresent method .  */
    public boolean isTranslationTypePresent() {
        return m_translationTypePresent;
    }
    /** @deprecated
      * Gets the Translation Type of this Global Title Indicator-0100,
      * which directs the message to the appropriate global title
      * translator. The coding and definition of the translation type
      * for GTIndicator-0100 is for further study.
      * @return
      * The Translation Type value.
      * @exception MandatoryParameterNotSetException
      * Thrown if this mandatory parameter has not been set.
      * @since JAIN TCAP v1.1 */
    public byte getTranslationType()
        throws MandatoryParameterNotSetException {
        if (m_translationTypePresent)
            return m_translationType;
        throw new MandatoryParameterNotSetException("Translation Type: not set.");
    }
    /** @deprecated
      * As of JAIN TCAP API v1.1, the numbering Plan is mandatory
      * therefore no longer needs an isPresent method.  */
    public boolean isNumberingPlanPresent() {
        return m_numberingPlanPresent;
    }
    /** @deprecated
      * Gets the Numbering Plan of this Global Title Indicator-0100.  
      * @return
      * The Numbering Plan - one of the following:- <ul>
      * <li>NP_UNKNOWN - Indicates an unknown Numbering Plan
      * <li>NP_ISDN_TEL - Indicates ISDN/Telephony Numbering Plan
      * <li>NP_RESERVED - Indicates a reserved Numbering plan in ANSI.
      * <li>NP_DATA - Indicates Data Numbering Plan
      * <li>NP_TELEX - Indicates a Telex Numbering Plan
      * <li>NP_MARITIME_MOBILE - Indicates a Maritime Mobile Numbering Plan
      * <li>NP_LAND_MOBILE - Indicates a Land Mobile Numbering Plan
      * <li>NP_ISDN_MOBILE - Indicates an ISDN Mobile Numbering Plan </ul>
      * @exception MandatoryParameterNotSetException
      * Thrown if this mandatory parameter has not been set.
      * @since JAIN TCAP v1.1
      * @see TcapConstants */
    public int getNumberingPlan()
        throws MandatoryParameterNotSetException {
        if (m_numberingPlanPresent)
            return m_numberingPlan;
        throw new MandatoryParameterNotSetException("Numbering Plan: not set.");
    }
    /** @deprecated
      * As of JAIN TCAP API v1.1, the encoding scheme is mandatory
      * therefore no longer needs an isPresent method.  */
    public boolean isEncodingSchemePresent() {
        return m_encodingSchemePresent;
    }
    /** @deprecated
      * Gets the Encoding Scheme of this Global Title Indicator-0100.
      * @return
      * The Encoding Scheme - one of the following:- <ul>
      * <li>ES_UNKNOWN - Indicates an unknown encoding scheme.
      * <li>ES_ODD - Indicates BCD, odd number of digits
      * <li>ES_EVEN - Indicates BCD, even number of digits
      * <li>ES_NATIONAL_SPECIFIC - Indicates a national specific </ul>
      * @exception MandatoryParameterNotSetException
      * Thrown if this mandatory parameter has not been set.
      * @since JAIN TCAP v1.1
      * @see TcapConstants */
    public int getEncodingScheme()
        throws MandatoryParameterNotSetException {
        if (m_encodingSchemePresent)
            return m_encodingScheme;
        throw new MandatoryParameterNotSetException("Encoding Scheme: not set.");
    }
    /** @deprecated
      * As of JAIN TCAP API v1.1, the nature of address indicator is
      * mandatory therefore no longer needs an isPresent method.  */
    public boolean isNatureOfAddrIndPresent() {
	return m_natureOfAddrIndPresent;
    }
    /** @deprecated
      * Gets the Nature of Address Indicator of this Global Title
      * Indicator-0100.
      * @return
      * The NatureOfAddrInd value - one of the following: <ul>
      * <li>NA_UNKNOWN - Indicates a unknown Nature of Address
      * <li>NA_SUBSCRIBER - Indicates a subscriber number
      * <li>NA_RESERVED - Reserved for national use Nature of Address Indicator
      * <li>NA_NATIONAL_SIGNIFICANT - Indicates a National Significant Number
      * <li>NA_INTERNATIONAL - Indicates an international number </ul>
      * @exception MandatoryParameterNotSetException
      * Thrown if this mandatory parameter has not been set.
      * @since JAIN TCAP v1.1 */
    public int getNatureOfAddrInd()
        throws MandatoryParameterNotSetException {
        if (m_natureOfAddrIndPresent)
            return m_natureOfAddrInd;
        throw new MandatoryParameterNotSetException();
    }
    /** @deprecated
      * This method returns the format of this Global Title.
      * @return
      * The Global Title Indicator value.  */
    public int getGTIndicator() {
        return jain.protocol.ss7.tcap.TcapConstants.GTINDICATOR_0100;
    }
    /** @deprecated
      * Clears all previously set parameters.  */
    public void clearAllParameters() {
        super.clearAllParameters();
        m_translationTypePresent = false;
        m_numberingPlanPresent = false;
        m_encodingSchemePresent = false;
        m_natureOfAddrIndPresent = false;
    }
    /** @deprecated
      * java.lang.String representation of class GTIndicator0100.
      * @return
      * java.lang.String provides description of class GTIndicator0100.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nGlobal Title Indicator = GTINDICATOR_0100");
        b.append(super.toString());
        b.append("\n\tm_translationType = " + m_translationType);
        b.append("\n\tm_translationTypePresent = " + m_translationTypePresent);
        b.append("\n\tm_numberingPlan = " + m_numberingPlan);
        b.append("\n\tm_numberingPlanPresent = " + m_numberingPlanPresent);
        b.append("\n\tm_natureOfAddrInd = " + m_natureOfAddrInd);
        b.append("\n\tm_natureOfAddrIndPresent = " + m_natureOfAddrIndPresent);
        b.append("\n\tm_encodingScheme = " + m_encodingScheme);
        b.append("\n\tm_encodingSchemePresent = " + m_encodingSchemePresent);
        return b.toString();
    }
    /** The Translation Type of this Global Title Indicator.
      * @serial m_translationType
      * - a default serializable field.  */
    private byte m_translationType = 0;
    /** Indicates if the translation type parameter has been set.
      * @serial m_translationTypePresent
      * - a default serializable field.  */
    private boolean m_translationTypePresent = false;
    /** The Numbering Plan of this Global Title Indicator.
      * @serial m_numberingPlan
      * - a default serializable field.  */
    private int m_numberingPlan = 0;
    /** Indicates if the numbering Plan parameter has been set.
      * @serial m_numberingPlanPresent
      * - a default serializable field.  */
    private boolean m_numberingPlanPresent = false;
    /** The Nature of Address Indicator of this Global Title Indicator.
      * @serial m_natureOfAddrInd
      * - a default serializable field.  */
    private int m_natureOfAddrInd = 0;
    /** Indcates if the Nature of Address Indicator parameter has been set.
      * @serial m_natureOfAddrIndPresent
      * - a default serializable field.  */
    private boolean m_natureOfAddrIndPresent = false;
    /** The Encoding Scheme of this Global Title Indicator.
      * @serial m_encodingScheme
      * - a default serializable field.  */
    private int m_encodingScheme = 0;
    /** Indcates if the Encoding Scheme parameter has been set.
      * @serial m_encodingSchemePresent
      * - a default serializable field.  */
    private boolean m_encodingSchemePresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
