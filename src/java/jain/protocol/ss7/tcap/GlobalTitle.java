/*
 @(#) $RCSfile: GlobalTitle.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:29 $ <p>
 
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
  * When instantiated this class represents a Global Title format which
  * informs the user that the GlobalTitle is not included, its Global
  * Title Indicator is a value '0000'.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class GlobalTitle implements java.io.Serializable {
    /** @deprecated
      * Constructs a new Global Title of format GTINDICATOR_0000, which
      * indicates that the GlobalTitle is not included.  */
    public GlobalTitle() {
    }
    /** @deprecated
      * Gets the format of the Global Title. The Global Title format of
      * this class is GTINDICATOR_0000. The other Global Title formats
      * that extend this class include: <ul>
      * <li>GTINDICATOR_0001
      * <li>GTINDICATOR_0010
      * <li>GTINDICATOR_0011
      * <li>GTINDICATOR_0100 </ul> */
    public int getGTIndicator() {
        return jain.protocol.ss7.tcap.TcapConstants.GTINDICATOR_0000;
    }
    /** @deprecated
      * Sets the Address Information of this Global Title.
      * @param addressInformation
      * The GlobalTitle Address Information subparameter is composed of
      * digits in the form of Binary Coded Decimal(BCD).  */
    public void setAddressInformation(byte[] addressInformation) {
        this.addressInformation = addressInformation;
        this.addressInformationPresent = true;
    }
    /** @deprecated
      * Indicates if the Address Information is present.  
      * @return
      * True if the Address Information has been set.  */
    public boolean isAddressInformationPresent() {
        return addressInformationPresent;
    }
    /** @deprecated
      * Gets the Address Information of this Global Title.
      * @return
      * The GlobalTitle Address Information subparameter is composed of
      * digits in the form of Binary Coded Decimal(BCD).
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public byte[] getAddressInformation()
        throws ParameterNotSetException {
        if (addressInformationPresent)
            return addressInformation;
        throw new ParameterNotSetException("Address Information: not set.");
    }
    /** @deprecated
      * Clears all previously set parameters.  */
    public void clearAllParameters() {
        addressInformationPresent = false;
    }
    /** The Address Information of this Global Title Indicator.
      * @serial addressInformation
      * - a default serializable field.  */
    private byte[] addressInformation = null;
    /** Indicates if the address Information parameter has been set.
      * @serial addressInformationPresent
      * - a default serializable field.  */
    private boolean addressInformationPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
