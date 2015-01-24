/*
 @(#) src/java/javax/jain/ss7/GlobalTitle.java <p>
 
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

package javax.jain.ss7;

import javax.jain.*;

/**
  * When instantiated this class represents a Global Title format which
  * informs the user that the GlobalTitle is not included, its Global
  * Title Indicator is of value '0000'.
  *
  * The various Global Title Indicators that extend this class are: <ul>
  * <li>{@link GTIndicator#GTINDICATOR_0000 GTINDICATOR_0000}
  * <li>{@link GTIndicator#GTINDICATOR_0001 GTINDICATOR_0001}
  * <li>{@link GTIndicator#GTINDICATOR_0010 GTINDICATOR_0010}
  * <li>{@link GTIndicator#GTINDICATOR_0011 GTINDICATOR_0011}
  * <li>{@link GTIndicator#GTINDICATOR_0100 GTINDICATOR_0100} </ul>
  *
  * The complete combination of GlobalTitles included support the
  * following standards: <ul>
  * <li>ANSI SCCP T1.112 (1992)
  * <li>ANSI SCCP T1.112 (1996)
  * <li>ITU SCCP Q711-716 (03/1993)
  * <li>ITU SCCP Q711-716 (07/1996) </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class GlobalTitle implements java.io.Serializable {
    /**
      * Constructs a new Global Title of format
      * {@link GTIndicator#GTINDICATOR_0000 GTINDICATOR_0000}, which indicates that
      * the GlobalTitle is not included.
      * That is no Address Information included.
      */
    public GlobalTitle() {
    }
    /**
      * Construct a new Global Title supplying the address information.
      * @param addressInformation
      * Address information with which to create the Global Title.
      */
    protected GlobalTitle(byte[] addressInformation) {
        super();
        setAddressInformation(addressInformation);
    }
    /**
      * Sets the Address Information of this Global Title of the
      * subclassed GTIndicators. <p>
      * <em>Note to developers:-</em> An Address Information is
      * forbidden an instantiation of this class, that is
      * GTindicator-0000.
      * @param addressInformation
      * The new Address Information.
      */
    public void setAddressInformation(byte[] addressInformation) {
        m_addressInformation = addressInformation;
        m_addressInformationIsSet = (m_addressInformation != null);
    }
    /**
      * Gets the format of the Global Title.
      * The Global Title format of this class is
      * {@link GTIndicator#GTINDICATOR_0000 GTINDICATOR_0000}.
      * @return
      * The Global Title Indicator value.
      */
    public GTIndicator getGTIndicator() {
        return GTIndicator.GTINDICATOR_0000;
    }
    /**
      * Gets the Address Information of the sub-classed Global Title
      * Indicators. <p>
      * <em>Note to developers:-</em> An Address Information is
      * forbidden an instantiation of this class, that is
      * GTindicator-0000.
      * @return
      * The GlobalTitle Address Information subparameter is composed of
      * digits in the form of Binary Coded Decimal(BCD).
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this parameter has not been set.
      * @exception ParameterNotSetException
      * This exception is thrown if this method is invoked when the
      * GlobalTitle is of type GTIndicator0000, that is an instantiation
      * of the GlobalTitle class.
      */
    public byte[] getAddressInformation()
        throws ParameterNotSetException, MandatoryParameterNotSetException {
        if (m_addressInformationIsSet)
            return m_addressInformation;
        throw new ParameterNotSetException("Address Information: not set.");
    }
    /**
      * java.lang.String representation of class GlobalTitle
      * @return
      * java.lang.String provides description of class GlobalTitle.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njain.protocol.ss7.GlobalTitle:");
        b.append("\n\tGlobal Title Indicator: = GTINDICATOR_0000");
        b.append("\n\tm_addressInformation = ");
        if (m_addressInformation != null)
            b.append(m_addressInformation.toString());
        else
            b.append("(null)");
        b.append("\n\tm_addressInformationIsSet = " + m_addressInformationIsSet);
        return b.toString();
    }
    protected byte[] m_addressInformation = null;
    protected boolean m_addressInformationIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
