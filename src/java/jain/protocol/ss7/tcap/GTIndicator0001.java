/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : GTIndicator0001.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     02/11/2000  Phelim O'Doherty    Added isOddIndicator and setOddIndicator
 *                                         methods for Nature of Address Indicator.
 *                                         Deprecated get methods for new get methods
 *                                         with MandantoryParameterNotSetException.
 *                                         Added new mandatory constructer for ITU
 *                                         and ANSI.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
/**
 *  This class represents a Global Title Indicator-0001, it can be used to
 *  store, create and retrieve GlobalTitle-0001 entries. It contains the
 *  accessor methods for the following parameters in order to accommodate the
 *  difference in the protocol variants supported:
 *  <UL>
 *    <LI> Address Information for ITU and ANSI
 *    <LI> Translation Type for ANSI
 *    <LI> Numbering Plan for ANSI
 *    <LI> Encoding Scheme for ANSI
 *    <LI> Nature of Address Indicator for ITU
 *    <LI> Odd Indicator (bit 8 of Address Indicator) for ITU
 *  </UL>
 *  <BR>
 *  For GT0001 address information it is expected that for non BCD numbers to be
 *  raw data and when BCD encoding, to be 2 BCD numbers per byte (i.e 2
 *  address_signals/digits/numbers per byte when BCD encoding). <BR>
 *  The nature of address indicator for GT0001, only uses bits 1 to 7 for its
 *  coding, therefore bit 8 of the octet contains the even/odd indicator which
 *  is coded as 0 for even or 1 for odd number of
 *  address_signals/digits/numbers. <BR>
 *
 *  <TABLE BORDER CELLSPACING=2 BORDERCOLOR="#000000" CELLPADDING=7 WIDTH=500>
 *    <CENTER>
 *    <TR>
 *
 *      <TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        Bit 8</B>
 *      </TD>
 *
 *      <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        Odd/Even</B>
 *      </TD>
 *
 *    </TR>
 *
 *    <TR>
 *
 *      <TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        0</B>
 *      </TD>
 *
 *      <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        Even number of address signals</B>
 *      </TD>
 *
 *    </TR>
 *
 *    <TR>
 *
 *      <TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        1</B>
 *      </TD>
 *
 *      <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        Odd number of address signals</B>
 *      </TD>
 *
 *    </TR>
 *
 *  </TABLE>
 *  </CENTER><br>
 *  The JAIN TCAP API provides a boolean oddIndicator, which can be used to
 *  determine the contents of the even/odd Indicator field. The <code>
 *  isOddIndicator</code> will return <var>true</var> when there is an odd
 *  number of address_signals/digits/numbers and <var>false</var> when there is
 *  an even number of address_signals/digits/numbers. <br>
 *  <B><FONT FACE="Courier New"><P>
 *  <CENTER><IMG SRC=../../../../doc-files/gt0001.jpg><br>
 *  Global title format for Indicator 0001</FONT></B></center>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        GlobalTitle
 *
 * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as a result
 * of the addition of the {@link jain.protocol.ss7.GTIndicator0001} class.
 */
public final class GTIndicator0001 extends GlobalTitle {

    /**
    * Constructs a new Global Title Indicator-0001.
    */
    public GTIndicator0001(){
    }

    /**
    * Indicates if the Translation Type is present.
    * @return  true if Translation Type has been set.
    */
    public boolean isTranslationTypePresent() {
      return translationTypePresent;
    }

    /**
    * Gets the Translation Type of this Global Title Indicator.
    * @return the Translation Type
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public byte getTranslationType() throws ParameterNotSetException {
        if(translationTypePresent == true) {
            return(translationType);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Sets the Translation Type of this Global Title Indicator
    * @param <var>translationType</var> the Translation Type
    */
    public void setTranslationType(byte translationType) {
        this.translationType = translationType;
        translationTypePresent=true;
    }

    /**
    * Indicates if the Numbering Plan is present.
    * @return  true if Numbering Plan has been set.
    */
    public boolean isNumberingPlanPresent() {
        return numberingPlanPresent;
    }

    /**
    * Gets the Numbering Plan of this Global Title Indicator-0001.
    * @return the Numbering Plan
    * <UL>
    * <LI><B>NP_UNKOWN</B> Indicates an unknown Numbering Plan
    * <LI><B>NP_ISDN_TEL</B>Indicates ISDN/Telephony Numbering Plan
    * <LI><B>NP_RESERVED<B> Indicates a reserved Numbering plan in ANSI.
    * <LI><B>NP_DATA</B> Indicates Data Numbering Plan
    * <LI><B>NP_TELEX</B> Indicates a Telex Numbering Plan
    * <LI><B>NP_MARITIME_MOBILE</B>Indicates a Maritime Mobile Numbering Plan
    * <LI><B>NP_LAND_MOBILE</B> Indicates a Land Mobile Numbering Plan
    * <LI><B>NP_ISDN_MOBILE</B> Indicates an ISDN Mobile Numbering Plan
    * </UL>
    */
    public void setNumberingPlan(int numberingPlan)  {
        this.numberingPlan = numberingPlan;
        numberingPlanPresent=true;
    }

    /**
    * Gets the Numbering Plan of this Global Title Indicator-0001.
    * @return the Numbering Plan
    * <UL>
    * <LI><B>NP_UNKOWN</B> Indicates an unknown Numbering Plan
    * <LI><B>NP_ISDN_TEL</B>Indicates ISDN/Telephony Numbering Plan
    * <LI><B>NP_RESERVED<B> Indicates a reserved Numbering plan in ANSI.
    * <LI><B>NP_DATA</B> Indicates Data Numbering Plan
    * <LI><B>NP_TELEX</B> Indicates a Telex Numbering Plan
    * <LI><B>NP_MARITIME_MOBILE</B>Indicates a Maritime Mobile Numbering Plan
    * <LI><B>NP_LAND_MOBILE</B> Indicates a Land Mobile Numbering Plan
    * <LI><B>NP_ISDN_MOBILE</B> Indicates an ISDN Mobile Numbering Plan
    * </UL>
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public int getNumberingPlan() throws ParameterNotSetException{
        if(numberingPlanPresent == true) {
            return(numberingPlan);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Sets the Encoding Scheme of this Global Title Indicator-0001.
    * @param <var>encodingScheme</var> the Encoding Scheme
    * <UL>
    * <LI><B>ES_UNKNOWN</B> Indicates an unknown encoding scheme.
    * <LI><B>ES_ODD</B>Indicates BCD, odd number of digits
    * <LI><B>ES_EVEN</B> Indicates BCD, even number of digits
    * <LI><B>ES_NATIONAL_SPECIFIC</B> Indicates a national specific
    * </UL>
    */
    public void setEncodingScheme(int encodingScheme) {
        this.encodingScheme = encodingScheme;
        encodingSchemePresent=true;
    }

    /**
    * Indicates if the Encoding Scheme is present.
    * @return  true if Encoding Scheme has been set.
    */
    public boolean isEncodingSchemePresent() {
        return encodingSchemePresent;
    }

    /**
    * Gets the Encoding Scheme of this Global Title Indicator-0001.
    * @return the Encoding Scheme
    * <UL>
    * <LI><B>ES_UNKNOWN</B> Indicates an unknown encoding scheme.
    * <LI><B>ES_ODD</B>Indicates BCD, odd number of digits
    * <LI><B>ES_EVEN</B> Indicates BCD, even number of digits
    * <LI><B>ES_NATIONAL_SPECIFIC</B> Indicates a national specific
    * </UL>
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public int getEncodingScheme()  throws ParameterNotSetException{
        if(encodingSchemePresent == true) {
            return(encodingScheme);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Sets the Nature of Address Indicator of this Global Title Indicator-0001.
    * The Nature of address indicator is only specific to the ITU variant.
    * @param <var>natureOfAddrInd</var> one of the following:
    * <UL>
    * <LI><B>NA_UNKNOWN</B> Indicates a unknown Nature of Address
    * <LI><B>NA_SUBSCRIBER</B>Indicates a subscriber number
    * <LI><B>NA_RESERVED</B> RESERVED for national use Nature of Address Indicator
    * <LI><B>NA_NATIONAL_SIGNIFICANT</B> Indicates a National Significant Number
    * <LI><B>NA_INTERNATIONAL</B> Indicates an international number
    * </UL>
    */
    public void setNatureOfAddrInd(int natureOfAddrInd) {
        this.natureOfAddrInd = natureOfAddrInd;
        natureOfAddrIndPresent=true;
    }

    /**
    * Gets the Nature of Address Indicator of this Global Title Indicator-0001.
    * The nature of address indicator is only specific to the ITU variant.
    * @return one of the following:
    * <UL>
    * <LI><B>NA_UNKNOWN</B> Indicates a unknown Nature of Address
    * <LI><B>NA_SUBSCRIBER</B>Indicates a subscriber number
    * <LI><B>NA_RESERVED</B> Reserved for national use Nature of Address Indicator
    * <LI><B>NA_NATIONAL_SIGNIFICANT</B> Indicates a National Significant Number
    * <LI><B>NA_INTERNATIONAL</B> Indicates an international number
    * </UL>
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public int getNatureOfAddrInd() throws ParameterNotSetException {
        if(natureOfAddrIndPresent == true) {
            return(natureOfAddrInd);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * This method returns the format of this Global Title.
    */
    public int getGTIndicator() {
        return (jain.protocol.ss7.tcap.TcapConstants.GTINDICATOR_0001);
    }

    /**
    * Clears all previously set parameters.
    */
    public void clearAllParameters() {
        translationTypePresent = false;
        numberingPlanPresent = false;
        encodingSchemePresent = false;
        natureOfAddrIndPresent = false;
    }

    /**
    * Returns a description of this class
    */
    public String toString() {

        String text = "GlobalTitle0001\n";


        try {
            String schemeName = null;

            switch(getEncodingScheme()) {
                case TcapConstants.ES_EVEN :  {
                    schemeName = "ES_EVEN";
                    break;
                }
                case TcapConstants.ES_NATIONAL_SPECIFIC :  {
                    schemeName = "ES_NATIONAL_SPECIFIC";
                    break;
                }
                case TcapConstants.ES_ODD :  {
                    schemeName = "ES_ODD";
                    break;
                }
                case TcapConstants.ES_UNKNOWN :  {
                    schemeName = "ES_UNKNOWN";
                    break;
                }
        }
        text += "\t\t\tEncoding Scheme: " + schemeName +"\n";

      } catch(ParameterNotSetException e) {}

      text += "\t\t\tGlobal Title Indicator: GTINDICATOR_0001\n";

      try {
          String nai = null;
          switch(getNatureOfAddrInd()) {
              case TcapConstants.NA_INTERNATIONAL :  {
                  nai = "NA_INTERNATIONAL";
                  break;
              }
              case TcapConstants.NA_NATIONAL_SIGNIFICANT :  {
                  nai = "NA_NATIONAL_SIGNIFICANT";
                  break;
              }
              case TcapConstants.NA_RESERVED :  {
                  nai = "NA_RESERVED";
                  break;
              }
              case TcapConstants.NA_SUBSCRIBER :  {
                  nai = "NA_SUBSCRIBER";
                  break;
              }
              case TcapConstants.NA_UNKNOWN :  {
                  nai = "NA_UNKNOWN";
                  break;
              }
          }
          text += "\t\t\t" + "Nature iof Address Indicator: " + nai +"\n";

      } catch(ParameterNotSetException e) {}

      try {
          String np = null;
          switch(getNumberingPlan()) {
              case TcapConstants.NP_DATA : {
                  np = "NP_DATA";
                  break;
              }
              case TcapConstants.NP_GENERIC : {
                  np = "NP_GENERIC";
                  break;
              }
              case TcapConstants.NP_ISDN_MOBILE : {
                  np = "NP_ISDN_MOBILE";
                  break;
              }
              case TcapConstants.NP_ISDN_TEL : {
                  np = "NP_ISDN_TEL";
                  break;
              }
              case TcapConstants.NP_LAND_MOBILE : {
                  np = "NP_LAND_MOBILE";
                  break;
              }case TcapConstants.NP_MARITIME_MOBILE: {
                  np = "NP_MARITIME_MOBILE";
                  break;
              }case TcapConstants.NP_NETWORK : {
                  np = "NP_NETWORK";
                  break;
              }
              case TcapConstants.NP_TELEX : {
                  np = "NP_TELEX";
                  break;
              }
          }
          text += "\t\t\tNumbering Plan: " + np +"\n";

      } catch(ParameterNotSetException e) {}

      try {
          text += "\t\t\tTranslation Type: " + getTranslationType() + "\n";
      } catch(ParameterNotSetException e) {}
      return text;
  }

  /**
  * The Translation Type of this Global Title Indicator
  * @serial translationType - a default serializable field
  */
  private byte translationType = 0;

  /**
  * Indcates if the translation type parameter has been set
  * @serial translationTypePresent - a default serializable field
  */
  private boolean translationTypePresent = false;

  /**
  * The Numbering Plan of this Global Title Indicator
  * @serial numberingPlan - a default serializable field
  */
  private int numberingPlan = 0;

  /**
  * Indcates if the numbering Plan parameter has been set
  * @serial numberingPlanPresent - a default serializable field
  */
  private boolean numberingPlanPresent = false;

  /**
  * The  Nature of Address Indicator of this Global Title Indicator
  * @serial natureOfAddrInd - a default serializable field
  */
  private int natureOfAddrInd = 0;

  /**
  * Indcates if the Nature of Address Indicator parameter has been set
  * @serial natureOfAddrIndPresent - a default serializable field
  */
  private boolean natureOfAddrIndPresent = false;

  /**
  * The  Encoding Scheme of this Global Title Indicator
  * @serial encodingScheme - a default serializable field
  */
  private int encodingScheme = 0;

  /**
  * Indcates if the Encoding Scheme parameter has been set
  * @serial encodingSchemePresent - a default serializable field
  */
  private boolean encodingSchemePresent = false;
}

