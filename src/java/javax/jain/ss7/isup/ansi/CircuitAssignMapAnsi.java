/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI ISUP Circuit Assignment Map parameter.  This class
  * provides the access methods for getting and setting the different sub-fields of the
  * parameter.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CircuitAssignMapAnsi implements java.io.Serializable {
    public static final byte MP_DS1 = 0;
    /** Constructs a new Ansi CircuitAssignMap class, parameters with default values.  */
    public CircuitAssignMapAnsi() {
    }
    /** Constructs an ANSI CircuitAssignMap class from the input parameters specified.
      * @param in_mapType  The map type, range 0 - 0x3f; <ul> <li>MP_DS1.  </ul>
      * @param in_mapFormat  The map format, range 0 - 0xffffff.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CircuitAssignMapAnsi(byte in_mapType, int in_mapFormat) throws ParameterRangeInvalidException {
        this();
        this.setMapType(in_mapType);
        this.setMapFormat(in_mapFormat);
    }
    /** Gets the Map Type field of the parameter.
      * @return The map type value, range 0 - 0x3f, see CircuitAssignMapAnsi().  */
    public byte getMapType() {
        return m_mapType;
    }
    /** Sets the Map Type field of the parameter.
      * @param aMapType  Map type, range 0 - 0x3f, see CircuitAssignMapAnsi().
      * @exception ParameterRangeInvalidException  Thrown if the sub-field value is out
      * of the specified range.  */
    public void setMapType(byte aMapType) throws ParameterRangeInvalidException {
        if (0 > aMapType || aMapType > 0x3f)
            throw new ParameterRangeInvalidException("Map Type " + aMapType + " out of range.");
        m_mapType = aMapType;
    }
    /** Gets the Map Format field of the parameter. The map format range is 0 -
      * 0xffffff, where each bit indicates whether the 64Kbps circuit is used in the
      * NxDS0 connection or not. A value of one in a bit position specified that that
      * circuit is used in the connection otherwise the bit is coded as zero.
      * @return The map format, the range being 0 - 0xffffff.  */
    public int getMapFormat() {
        return m_mapFormat;
    }
    /** Sets the Map Format field of the parameter. Refer to getMapFormat for more
      * details.
      * @param aMapFormat  Map Format, range 0 - 0xffffff.  */
    public void setMapFormat(int aMapFormat) throws ParameterRangeInvalidException {
        if (0 > aMapFormat || aMapFormat > 0xffffff)
            throw new ParameterRangeInvalidException("Map Format " + aMapFormat + " out of range.");
        m_mapFormat = aMapFormat;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CircuitAssignMapAnsi class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\nCircuitAssignMap:");
        b.append("\n\tMap Type: " + m_mapType);
        b.append("\n\tMap Format: " + m_mapFormat);
        return b.toString();
    }
    private byte m_mapType;
    private int m_mapFormat;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
