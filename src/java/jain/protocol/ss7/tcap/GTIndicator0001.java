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
  * This class represents a Global Title Indicator-0001, it can be used
  * to store, create and retrieve GlobalTitle-0001 entries. It contains
  * the accessor methods for the following parameters in order to
  * accommodate the difference in the protocol variants supported: <ul>
  * <li>Address Information for ITU and ANSI
  * <li>Translation Type for ANSI
  * <li>Numbering Plan for ANSI
  * <li>Encoding Scheme for ANSI
  * <li>Nature of Address Indicator for ITU
  * <li>Odd Indicator (bit 8 of Address Indicator) for ITU </ul>
  * For GT0001 address information it is expected that for non BCD
  * numbers to be raw data and when BCD encoding, to be 2 BCD numbers
  * per byte (i.e 2 address_signals/digits/numbers per byte when BCD
  * encoding).  The nature of address indicator for GT0001, only uses
  * bits 1 to 7 for its coding, therefore bit 8 of the octet contains
  * the even/odd indicator which is coded as 0 for even or 1 for odd
  * number of address_signals/digits/numbers.
  * <table align="center" border=1 cellpadding=1>
  * <tr>
  * <th>Bit 8</th>
  * <th>Odd/Event</th>
  * </tr>
  * <tr>
  * <td align="center">0</td>
  * <td>Even number of address signals.</td>
  * </tr>
  * <tr>
  * <td align="center">1</td>
  * <td>Odd number of address signals.</td>
  * </tr>
  * </table> <p>
  * The JAIN TCAP API provides a boolean oddIndicator, which can be used
  * to determine the contents of the even/odd Indicator field. The
  * isOddIndicator will return true when there is an odd number of
  * address_signals/digits/numbers and false when there is an even
  * number of address_signals/digits/numbers.
  * <center> [gt0001.jpg] </center><br>
  * <center> Global title format for Indicator 0001 </center><br>
  * @version 1.2.2
  * @author Monavacon Limited
  * @see GlobalTitle
  * @deprecated
  * As of JAIN TCAP v1.1. This class is no longer needed as a result of
  * the addition of the GTIndicator0001 class.
  *
  */
public final class GTIndicator0001 extends GlobalTitle {
    /** @deprecated
      * Constructs a new Global Title Indicator-0001.  */
    public GTIndicator0001() {
    }
    /** @deprecated
      * Indicates if the Translation Type is present.
      * @return
      * True if Translation Type has been set.  */
    public boolean isTranslationTypePresent() {
        return translationTypePresent;
    }
    /** @deprecated
      * Gets the Translation Type of this Global Title Indicator.
      * @return
      * The Translation Type.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public byte getTranslationType()
        throws ParameterNotSetException {
        if(translationTypePresent)
            return translationType;
        throw new ParameterNotSetException("Translation Type: not set.");
    }
    /** @deprecated
      * Sets the Translation Type of this Global Title Indicator.
      * @param translationType
      * The Translation Type.  */
    public void setTranslationType(byte translationType) {
        this.translationType = translationType;
        this.translationTypePresent = true;
    }
    /** @deprecated
      * Indicates if the Numbering Plan is present.
      * @return
      * True if Numbering Plan has been set.  */
    public boolean isNumberingPlanPresent() {
        return numberingPlanPresent;
    }
    /** @deprecated
      * Gets the Numbering Plan of this Global Title Indicator-0001.
      * @return
      * The Numbering Plan <ul>
      * <li>NP_UNKOWN Indicates an unknown Numbering Plan
      * <li>NP_ISDN_TELIndicates ISDN/Telephony Numbering Plan
      * <li>NP_RESERVED Indicates a reserved Numbering plan in ANSI.
      * <li>NP_DATA Indicates Data Numbering Plan 
      * <li>NP_TELEX Indicates a Telex Numbering Plan
      * <li>NP_MARITIME_MOBILEIndicates a Maritime Mobile Numbering Plan
      * <li>NP_LAND_MOBILE Indicates a Land Mobile Numbering Plan
      * <li>NP_ISDN_MOBILE Indicates an ISDN Mobile Numbering Plan </ul> */
    public void setNumberingPlan(int numberingPlan) {
        this.numberingPlan = numberingPlan;
        this.numberingPlanPresent = true;
    }
    /** @deprecated
      * Gets the Numbering Plan of this Global Title Indicator-0001.
      * @return
      * The Numbering Plan <ul>
      * <li>NP_UNKOWN Indicates an unknown Numbering Plan
      * <li>NP_ISDN_TELIndicates ISDN/Telephony Numbering Plan
      * <li>NP_RESERVED Indicates a reserved Numbering plan in ANSI.
      * <li>NP_DATA Indicates Data Numbering Plan 
      * <li>NP_TELEX Indicates a Telex Numbering Plan
      * <li>NP_MARITIME_MOBILEIndicates a Maritime Mobile Numbering Plan
      * <li>NP_LAND_MOBILE Indicates a Land Mobile Numbering Plan
      * <li>NP_ISDN_MOBILE Indicates an ISDN Mobile Numbering Plan </ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public int getNumberingPlan()
        throws ParameterNotSetException {
        if (numberingPlanPresent)
            return numberingPlan;
        throw new ParameterNotSetException("Numbering Plan: not set.");
    }
    /** @deprecated
      * Sets the Encoding Scheme of this Global Title Indicator-0001.
      * @param encodingScheme
      * The Encoding Scheme <ul>
      * <li>ES_UNKNOWN Indicates an unknown encoding scheme.
      * <li>ES_ODDIndicates BCD, odd number of digits
      * <li>ES_EVEN Indicates BCD, even number of digits
      * <li>ES_NATIONAL_SPECIFIC Indicates a national specific </ul> */
    public void setEncodingScheme(int encodingScheme) {
        this.encodingScheme = encodingScheme;
        this.encodingSchemePresent = true;
    }
    /** @deprecated
      * Indicates if the Encoding Scheme is present.
      * @return
      * True if Encoding Scheme has been set.  */
    public boolean isEncodingSchemePresent() {
        return encodingSchemePresent;
    }
    /** @deprecated
      * Gets the Encoding Scheme of this Global Title Indicator-0001.
      * @return
      * The Encoding Scheme <ul>
      * <li>ES_UNKNOWN Indicates an unknown encoding scheme.
      * <li>ES_ODDIndicates BCD, odd number of digits
      * <li>ES_EVEN Indicates BCD, even number of digits
      * <li>ES_NATIONAL_SPECIFIC Indicates a national specific </ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public int getEncodingScheme()
        throws ParameterNotSetException {
        if (encodingSchemePresent)
            return encodingScheme;
        throw new ParameterNotSetException("Encoding Scheme: not set.");
    }
    /** @deprecated
      * Sets the Nature of Address Indicator of this Global Title
      * Indicator-0001. The Nature of address indicator is only specific
      * to the ITU variant.
      * @param natureOfAddrInd
      * One of the following: <ul>
      * <li>NA_UNKNOWN Indicates a unknown Nature of Address
      * <li>NA_SUBSCRIBERIndicates a subscriber number
      * <li>NA_RESERVED RESERVED for national use Nature of Address Indicator
      * <li>NA_NATIONAL_SIGNIFICANT Indicates a National Significant Number
      * <li>NA_INTERNATIONAL Indicates an international number </ul> */
    public void setNatureOfAddrInd(int natureOfAddrInd) {
        this.natureOfAddrInd = natureOfAddrInd;
        this.natureOfAddrIndPresent = true;
    }
    /** @deprecated
      * Gets the Nature of Address Indicator of this Global Title
      * Indicator-0001. The nature of address indicator is only specific
      * to the ITU variant.
      * @return
      * One of the following: <ul>
      * <li>NA_UNKNOWN Indicates a unknown Nature of Address
      * <li>NA_SUBSCRIBERIndicates a subscriber number
      * <li>NA_RESERVED Reserved for national use Nature of Address Indicator
      * <li>NA_NATIONAL_SIGNIFICANT Indicates a National Significant Number
      * <li>NA_INTERNATIONAL Indicates an international number </ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public int getNatureOfAddrInd()
        throws ParameterNotSetException {
        if (natureOfAddrIndPresent)
            return natureOfAddrInd;
        throw new ParameterNotSetException("Nature of Addres Indicator: not set.");
    }
    /** @deprecated
      * This method returns the format of this Global Title.  */
    public int getGTIndicator() {
        return jain.protocol.ss7.tcap.TcapConstants.GTINDICATOR_0001;
    }
    /** @deprecated
      * Clears all previously set parameters.  */
    public void clearAllParameters() {
        translationTypePresent = false;
        numberingPlanPresent = false;
        encodingSchemePresent = false;
        natureOfAddrIndPresent = false;
    }
    /** @deprecated
      * Returns a description of this class.  */
    public java.lang.String toString() {
        String text = "GlobalTitle0001\n";
        try {
            String schemeName = null;
            switch (getEncodingScheme()) {
                case TcapConstants.ES_EVEN:
                    schemeName = "ES_EVEN";
                    break;
                case TcapConstants.ES_NATIONAL_SPECIFIC:
                    schemeName = "ES_NATIONAL_SPECIFIC";
                    break;
                case TcapConstants.ES_ODD:
                    schemeName = "ES_ODD";
                    break;
                case TcapConstants.ES_UNKNOWN:
                    schemeName = "ES_UNKNOWN";
                    break;
            }
            text += "\t\t\tEncoding Scheme: " + schemeName +"\n";
        } catch (ParameterNotSetException e) {
        }
        text += "\t\t\tGlobal Title Indicator: GTINDICATOR_0001\n";
        try {
            String nai = null;
            switch (getNatureOfAddrInd()) {
                case TcapConstants.NA_INTERNATIONAL:
                    nai = "NA_INTERNATIONAL";
                    break;
                case TcapConstants.NA_NATIONAL_SIGNIFICANT:
                    nai = "NA_NATIONAL_SIGNIFICANT";
                    break;
                case TcapConstants.NA_RESERVED:
                    nai = "NA_RESERVED";
                    break;
                case TcapConstants.NA_SUBSCRIBER:
                    nai = "NA_SUBSCRIBER";
                    break;
                case TcapConstants.NA_UNKNOWN :  {
                  nai = "NA_UNKNOWN";
                  break;
              }
          }
          text += "\t\t\t" + "Nature iof Address Indicator: " + nai +"\n";
        } catch (ParameterNotSetException e) {
        }
        try {
            String np = null;
            switch (getNumberingPlan()) {
                case TcapConstants.NP_DATA:
                    np = "NP_DATA";
                    break;
                case TcapConstants.NP_GENERIC:
                    np = "NP_GENERIC";
                    break;
                case TcapConstants.NP_ISDN_MOBILE:
                    np = "NP_ISDN_MOBILE";
                    break;
                case TcapConstants.NP_ISDN_TEL:
                    np = "NP_ISDN_TEL";
                    break;
                case TcapConstants.NP_LAND_MOBILE:
                    np = "NP_LAND_MOBILE";
                    break;
                case TcapConstants.NP_MARITIME_MOBILE:
                    np = "NP_MARITIME_MOBILE";
                    break;
                case TcapConstants.NP_NETWORK:
                    np = "NP_NETWORK";
                    break;
                case TcapConstants.NP_TELEX:
                    np = "NP_TELEX";
                    break;
            }
            text += "\t\t\tNumbering Plan: " + np +"\n";
        } catch (ParameterNotSetException e) {
        }
        try {
            text += "\t\t\tTranslation Type: " + getTranslationType() + "\n";
        } catch (ParameterNotSetException e) {
        }
        return text;
    }
    /** The Translation Type of this Global Title Indicator.
      * @serial translationType
      * - a default serializable field.  */
    private byte translationType = 0;
    /** Indcates if the translation type parameter has been set.
      * @serial translationTypePresent
      * - a default serializable field.  */
    private boolean translationTypePresent = false;
    /** The Numbering Plan of this Global Title Indicator.
      * @serial numberingPlan
      * - a default serializable field.  */
    private int numberingPlan = 0;
    /** Indcates if the numbering Plan parameter has been set.
      * @serial numberingPlanPresent
      * - a default serializable field.  */
    private boolean numberingPlanPresent = false;
    /** The  Nature of Address Indicator of this Global Title Indicator.
      * @serial natureOfAddrInd
      * - a default serializable field.  */
    private int natureOfAddrInd = 0;
    /** Indcates if the Nature of Address Indicator parameter has been set.
      * @serial natureOfAddrIndPresent
      * - a default serializable field.  */
    private boolean natureOfAddrIndPresent = false;
    /** The Encoding Scheme of this Global Title Indicator.
      * @serial encodingScheme
      * - a default serializable field.  */
    private int encodingScheme = 0;
    /** Indcates if the Encoding Scheme parameter has been set.
      * @serial encodingSchemePresent
      * - a default serializable field.  */
    private boolean encodingSchemePresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
