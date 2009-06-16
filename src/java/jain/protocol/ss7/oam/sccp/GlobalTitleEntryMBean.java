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

package jain.protocol.ss7.oam.sccp;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This class represents a Global Title entry (a combination of the global title
  * translator and global title rule). The Global Title Entry is used to translate dialed
  * digits into the appropriate user address based on the Global Title indicator and
  * encoding schemes. The Global Title Indicator attribute is derived from the combination
  * of Nature of Address, the Translation Type, and Numbering Plan used and therefore
  * indicates which values are present. <p>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  *
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  *
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [SccpMBeans.jpg] </center><br>
  * <center> Relationship between the SCCP JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface GlobalTitleEntryMBean extends SccpManagedObjectMBean {
    /** A Global title indicator constant: indicator = 0001 */
    public static final int GTINDICATOR_0001 = 1;
    /** A Global title indicator constant: indicator = 0010 */
    public static final int GTINDICATOR_0010 = 2;
    /** A Global title indicator constant: indicator = 0011 */
    public static final int GTINDICATOR_0011 = 3;
    /** A Global title indicator constant: indicator = 0100 */
    public static final int GTINDICATOR_0100 = 4;
    /** Numbering Plan Constant: Unkown Numbering Plan.  */
    public static final int NP_UNKOWN = 0;
    /** Numbering Plan Constant: ISDN/Telephony Numbering Plan.  */
    public static final int NP_ISDN_TEL = 1;
    /** Numbering Plan Constant: Generic Numbering Plan.  */
    public static final int NP_GENERIC = 2;
    /** Numbering Plan Constant: Data Numbering Plan.  */
    public static final int NP_DATA = 3;
    /** Numbering Plan Constant: Telex Numbering Plan.  */
    public static final int NP_TELEX = 4;
    /** Numbering Plan Constant: Maritime Mobile Numbering Plan.  */
    public static final int NP_MARITIME_MOBILE = 5;
    /** Numbering Plan Constant: Land/Mobile Numbering Plan.  */
    public static final int NP_LAND_MOBILE = 6;
    /** Numbering Plan Constant: ISDN/Mobile Numbering Plan.  */
    public static final int NP_ISDN_MOBILE = 7;
    /** Numbering Plan Constant: Private Network or Network Specific Numbering Plan.  */
    public static final int NP_NETWORK = 8;
    /** Nature of Address Indicator constant: Indicates a unknown Nature of Address
      * Indicator.  */
    public static final int NA_UNKNOWN = 0;
    /** Nature of Address Indicator constant: Indicates a subsciber number.  */
    public static final int NA_SUBSCRIBER = 1;
    /** Nature of Address Indicator constant: Reserved for national use.  */
    public static final int NA_RESERVED = 2;
    /** Nature of Address Indicator constant: Indicates a National Significant Number.  */
    public static final int NA_NATIONAL_SIGNIFICANT = 3;
    /** Nature of Address Indicator constant: Indicates an international number.  */
    public static final int NA_INTERNATIONAL = 4;
    /** Encoding Scheme constant: Indicates an unknown encoding scheme.  */
    public static final int ES_UNKNOWN = 0;
    /** Encoding Scheme constant:Indicates BCD, odd number of digits.  */
    public static final int ES_ODD = 1;
    /** Encoding Scheme constant:Encoding Scheme constants: Indicates a BCD, with an even
      * number of digits.  */
    public static final int ES_EVEN = 2;
    /** Encoding Scheme constant:Indicates National Specific encoding scheme for ITU.  */
    public static final int ES_NATIONAL_SPECIFIC = 3;
    /**
      * Returns the Global Title indicator. The Global Title Indicator attribute is
      * derived from the combination of Nature of Address, the Translation Type, and
      * Numbering Plan used.
      *
      * @return
      * either:<ul>
      * <li>GTINDICATOR_0000
      * <li>GTINDICATOR_0001
      * <li>GTINDICATOR_0010
      * <li>GTINDICATOR_0011
      * <li>GTINDICATOR_0100 </ul>
      */
    public Integer getGTIndicator();
    /**
      * Sets the Global Title indicator. The Global Title Indicator attribute is derived
      * from the combination of Nature of Address, the Translation Type, and Numbering
      * Plan used.
      *
      * @param indicator
      * one of the Global Title Indicator constants:<ul>
      * <li>GTINDICATOR_0000
      * <li>GTINDICATOR_0001
      * <li>GTINDICATOR_0010
      * <li>GTINDICATOR_0011
      * <li>GTINDICATOR_0100 </ul>
      *
      * @exception IllegalArgumentException
      * if the suppled indicator is not one of the defined Global Tilte Indicator
      * constants
      */
    public void setGTIndicator(Integer indicator)
        throws IllegalArgumentException;
    /**
      * Sets the Address Information of this Global Title Entry.
      *
      * @param addressInformation
      * the GlobalTitle Address Information subparameter is composed of digits in the form
      * of Binary Coded Decimal(BCD).
      *
      * @exception IllegalArgumentException
      * if the supplied address information is in an unnacceptable format
      */
    public void setAddressInformation(Byte[] addressInformation)
        throws IllegalArgumentException;
    /**
      * Gets the Address Information of this Global Title Entry.
      *
      * @return
      * the GlobalTitle Address Information subparameter is composed of digits in the form
      * of Binary Coded Decimal(BCD).
      */
    public Byte[] getAddressInformation();
    /**
      * Sets the Entity Set of this GlobalTitle Entry identified by the specified
      * ObjectName. The supplied entitiy set specifies the user address to be derived from
      * this global title translation.
      *
      * @param sccpEntitySet
      * the ObjectName of the SCCP Set entity (which determines the Subsystem number and
      * signalling point code).
      */
    public void setSccpEntitySet(ObjectName sccpEntitySet);
    /**
      * Returns the ObjectName of the Entity Set of this GlobalTitle Entry. The returned
      * entitiy set identifies the user address to be derived from this global title
      * translation.
      *
      * @return
      * the SCCP entity Set(which determines the Subsystem number and signalling point
      * code).
      */
    public ObjectName getSccpEntitySet();
    /**
      * Sets the Nature of Address Indicator of this Global Title Entry.
      *
      * @param natureOfAddrInd
      * one of the following: <ul>
      * <li>NA_UNKNOWN Indicates a unknown Nature of Address
      * <li>NA_SUBSCRIBERIndicates a subscriber number
      * <li>NA_RESERVED RESERVED for national use Nature of Address Indicator
      * <li>NA_NATIONAL_SIGNIFICANT Indicates a National Significant Number
      * <li>NA_INTERNATIONAL Indicates an international number </ul>
      *
      * @exception IllegalArgumentException
      * if the supplied indicator is not one of the defined constants
      */
    public void setNatureOfAddrInd(Integer natureOfAddrInd)
        throws IllegalArgumentException;
    /**
      * Gets the Nature of Address Indicator of this Global Title Entry.
      * 
      * @return
      * one of the following: <ul>
      * <li>NA_UNKNOWN Indicates a unknown Nature of Address
      * <li>NA_SUBSCRIBERIndicates a subscriber number
      * <li>NA_RESERVED Reserved for national use Nature of Address Indicator
      * <li>NA_NATIONAL_SIGNIFICANT Indicates a National Significant Number
      * <li>NA_INTERNATIONAL Indicates an international number </ul>
      */
    public Integer getNatureOfAddrInd();
    /**
      * Sets the Numbering Plan of this Global Title Entry.
      *
      * @return
      * one of the Numbering Plan constants: <ul>
      * <li>NP_UNKOWN Indicates an unkown Numbering Plan
      * <li>NP_ISDN_TELIndicates ISDN/Telephony Numbering Plan
      * <li>NP_RESERVED Indicates a resrved Numbering plan in ANSI.
      * <li>NP_DATA Indicates Data Numbering Plan
      * <li>NP_TELEX Indicates a Telex Numbering Plan
      * <li>NP_MARITIME_MOBILEIndicates a Maritime Mobile Numbering Plan
      * <li>NP_LAND_MOBILE Indicates a Land Mobile Numbering Plan
      * <li>NP_ISDN_MOBILE Indicates an ISDN Mobile Numbering Plan </ul>
      *
      * @exception IllegalArgumentException
      * if the supplied numbering plan is not one of the defined numbering plan constants
      */
    public void setNumberingPlan(Integer numberingPlan)
        throws IllegalArgumentException;
    /**
      * Gets the Numbering Plan of this Global Title Entry.
      *
      * @return
      * one of the Numbering Plan constants: <ul>
      * <li>NP_UNKOWN Indicates an unkown Numbering Plan
      * <li>NP_ISDN_TELIndicates ISDN/Telephony Numbering Plan
      * <li>NP_RESERVED Indicates a resrved Numbering plan in ANSI.
      * <li>NP_DATA Indicates Data Numbering Plan
      * <li>NP_TELEX Indicates a Telex Numbering Plan
      * <li>NP_MARITIME_MOBILEIndicates a Maritime Mobile Numbering Plan
      * <li>NP_LAND_MOBILE Indicates a Land Mobile Numbering Plan
      * <li>NP_ISDN_MOBILE Indicates an ISDN Mobile Numbering Plan </ul>
      */
    public Integer getNumberingPlan();
    /**
      * Gets the Translation Type of this Global Title Entry
      *
      * @return
      * the Translation Type.
      */
    public Byte getTranslationType();
    /**
      * Sets the Translation Type of this Global Title Entry
      *
      * @param translationType
      * the Translation Type.
      */
    public void setTranslationType(Byte translationType);
    /**
      * Sets the Encoding Scheme of this Global Title Entry.
      *
      * @param encodingScheme
      * one of the Encoding Scheme constants <ul>
      * <li>ES_UNKNOWN Indicates an unknown encoding scheme.
      * <li>ES_ODDIndicates BCD, odd number of digits
      * <li>ES_EVEN Indicates BCD, even number of digits
      * <li>ES_NATIONAL_SPECIFIC Indicates a national specific </ul>
      *
      * @exception IllegalArgumentException
      * if the supplied encoding scheme is not one of the defined Encoding Scheme
      * constants.
      */
    public void setEncodingScheme(Integer encodingScheme)
        throws IllegalArgumentException;
    /**
      * Gets the Encoding Scheme of this Global Title Entry.
      *
      * @return
      * one of the Encoding Scheme constants: <ul>
      * <li>ES_UNKNOWN Indicates an unknown encoding scheme.
      * <li>ES_ODDIndicates BCD, odd number of digits
      * <li>ES_EVEN Indicates BCD, even number of digits
      * <li>ES_NATIONAL_SPECIFIC Indicates a national specific </ul>
      */
    public Integer getEncodingScheme();
    /**
      * Sets the Administrative state of this Global Title Entry.
      *
      * @param locked
      * either <ul>
      * <li>True - locked
      * <li>False - Not locked </ul>
      */
    public void setLocked(Boolean locked);
    /**
      * Returns the Administrative state parameter of this Global Title Entry.
      *
      * @return
      * either <ul>
      * <li>True - locked
      * <li>False - Not locked </ul>
      */
    public Boolean isLocked();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
