/*
 @(#) $RCSfile: ServiceAreaId.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:40 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:40 $ by $Author: brian $
 */

package javax.jain.protocol.ss7.map.information.etsipar;

import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This parameter class represents a Service Area Id (SAI) value as
  * defined in the specification 3G TS 23.003.
  * The following relationsship exists between SAI and Location Area Id
  * (LAI). <ul>
  * <li>SAI = LAI + SAC </ul>
  *
  * Therefore this class extends LocationAreaId class adding SAC
  * (Service Area Code). <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory component with default value need not be set.
  *
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h5>Parameter components:</h5><ul>
  *
  * <li>sac - Service Area Code, mandatory component with no default
  * </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class ServiceAreaId extends LocationAreaId {
    /**
      * The constructor.
      *
      * @param mcc
      * Mobile Country Code (3 digits)
      *
      * @param mnc
      * Mobile Network Code (2-3 digits)
      *
      * @param lac
      * Location Area Code (2 bytes)
      *
      * @param sac
      * Service Area Code (2 bytes)
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public ServiceAreaId(java.lang.String mcc, java.lang.String mnc, byte[] lac, byte[] sac)
        throws SS7InvalidParamException {
        super(mcc, mnc, lac);
        setServiceAreaCode(sac);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public ServiceAreaId()
        throws SS7InvalidParamException {
    }
    /**
      * Change the value of the Service Area Code (SAC).
      *
      * @param sac
      * Byte array with 2 byte SAC.
      */
    public void setServiceAreaCode(byte[] sac) {
        m_sac = sac;
        m_sacIsSet = (sac != null);
    }
    /**
      * Get the value of the Service Area Code (SAC).
      *
      * @return
      * Byte array with 2 byte SAC.
      */
    public byte[] getServiceAreaCode() {
        return m_sac;
    }
    /**
      * Get the whole Service Area Code (MCC+MNC+LAC+SAC) as a
      * java.lang.String.
      *
      * @return
      * java.lang.String containing SAC.
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
    protected byte[] m_sac = null;
    protected boolean m_sacIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
