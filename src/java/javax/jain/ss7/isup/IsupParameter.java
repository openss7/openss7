/*
 @(#) $RCSfile: IsupParameter.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-07-13 07:13:30 $ <p>
 
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
 
 Last Modified $Date: 2009-07-13 07:13:30 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** This class is used to represent arbitrary ISUP parameters using a byte encoding of
  * the parameter.  The first byte in the byte string must be the tag of the parameter.
  * The second byte in the byte string must be the proper length of the parameter.  The
  * remaining bytes in the byte string are the contents of the parameter.  This form
  * can only be used to represent optional parameters in ISUP messages.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class IsupParameter implements java.io.Serializable {
    /** An emptry constructor as required by serial and bean objects.  */
    public IsupParameter() {
    }
    /** Sets the parameter using the byte string representation (encoding) of the
      * parameter.
      * @param param The byte string representation (encoding) of the parameter.  */
    public void setParameter(byte[] param) {
        m_parameter = param;
    }
    /** Gets the parameter byte string representation (encoding) of the parameter.
      * @return The byte string representation (encoding) of the parameter.  */
    public byte[] getParameter() {
        return m_parameter;		
    }
    /** The toString method retrieves a string containing the values of the members of
      * the isupParameter class.  */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(512);
        buffer.append(super.toString());
        if(m_parameter != null){
            buffer.append("\nparameter = ");
            for(int i=0; i<m_parameter.length; i++)
                buffer.append(" " + Integer.toHexString(m_parameter[i] & 0xFF) );
        }
        return buffer.toString();
    }
    byte[] m_parameter;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
