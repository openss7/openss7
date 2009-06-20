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

package javax.jain.protocol.ss7;

import javax.jain.*;

/**
  * This class contains the following utilities: <ul>
  * <li>bytesToHex - Convert an array of bytes to java.lang.String with hex values;
  * <li>isArraysEqual - Check if two byte arrays are equal.
  * </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class JainSS7Utility {
    /** Convert an array of bytes to java.lang.String with hex values.
      * @param data
      * A byte array.
      * @param start
      * The start in data from where to convert.
      * @param end
      * The end in data.
      * @return
      * A byte sequence ready to convert to a java.lang.String, i.e: <br>
      * <code>new java.lang.String(bytesToHex(data, 0, len))</code> */
    public static byte[] bytesToHex(byte[] data, int start, int end) {
        byte[] result;
        byte nibble;
        if (end <= start)
            return new byte[0];
        result = new byte[3*(end-start)];
        for (int p=0, i=start; i<data.length && i<end; i++, p += 3) {
            nibble = (byte) ((data[i] & 0xF0) >> 4);
            result[p+0] = (byte) (nibble > 9 ? nibble + 55 : nibble + 48);
            nibble = (byte) ((data[i] & 0x0F) >> 0);
            result[p+1] = (byte) (nibble > 9 ? nibble + 55 : nibble + 48);
            result[p+2] = 32;
        }
        return result;
    }
    /** Check if two byte arrays are equal.
      * @param data1
      * The first array.
      * @param data2
      * The second array.
      * @return
      * True if data1 and data2 are equal */
    public static boolean isArraysEqual(byte[] data1, byte[] data2) {
        if (data1 == null && data2 != null)
            return false;
        if (data1 != null && data2 == null)
            return false;
        if (data1 == null && data2 == null)
            return true;
        if (data1.length != data2.length)
            return false;
        for (int i=0; i < data1.length; i++) {
            if (data1[i] != data2[i])
                return false;
        }
        return true;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
