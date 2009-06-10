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

package javax.jain.protocol.ss7.map.information.etsipar;

import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This parameter class represents a Localised Service Area Id (LSA ID) value as defined
  * in the specification 3G TS 23.003. <p>
  *
  * The following rules applies for the setting of parameter components:- <ul>
  * <li>Mandatory component with no default value must be provided to the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional / conditional component is by default not present. Such a component
  * becomes present by setting it. </ul> <p>
  *
  * <h4>Parameter components:-</h4> <ul>
  *
  * <li>lsaId - Localised Service Area Id, mandatory component with no default </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class LsaId extends MapParameter {
    /** The constructor.
      * @param lsaId  Localised Service Area Id (3 bytes)
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of range.
      */
    public LsaId(byte[] lsaId)
        throws SS7InvalidParamException {
        setLsaId(lsaId);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public LsaId()
        throws SS7InvalidParamException {
    }
    /** Change the value of the Localised Service Area Id (LSA ID).
      * @param lsaId  Byte array with 3 byte LSA ID.
      */
    public void setLsaId(byte[] lsaId) {
        m_lsaId = lsaId;
    }
    /** Get the value of the Localised Service Area Id (LSA ID).
      * @return Byte array with 3 byte LSA ID.
      */
    public byte[] getLsaId() {
        return m_lsaId;
    }
    /** Check if the Localised Service Area Id (LSA ID) is universal or PLMN significant.
      * This is indicated by least significant bit in the LSA ID value: 1 = universal, 0 =
      * PLMN significant
      * @return True if LSA ID is universal, otherwise false.
      */
    public boolean isLsaIdUniversal() {
        if (m_lsaId != null) {
            return ((m_lsaId[0] & 0x01) != 0);
        }
        return false;
    }
    protected byte[] m_lsaId = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
