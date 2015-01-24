/*
 @(#) src/java/javax/jain/ss7/isup/ansi/GenericNameAnsi.java <p>
 
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

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI ISUP Generic Name parameter sent in the forward
    direction containing specific name information.

    @author Monavacon Limited
    @version 1.2.2
  */
public class GenericNameAnsi implements java.io.Serializable {
    public static final byte PRESENTATION_ALLOWED = 0;
    public static final byte PRESENTATION_RESTRICTED = 1;
    public static final byte PRESENTATION_BLOCKING_TOGGLE = 2;
    public static final byte PRESENTATION_NO_INDICATION = 3;
    public static final boolean AVAILABILITY_NAME_AVAILABLE_UNKNOWN = false;
    public static final boolean AVAILABILITY_NAME_UNAVAILABLE = true;
    public static final byte TN_CALLING_NAME = 1;
    public static final byte TN_ORIGINAL_CALLED_NAME = 2;
    public static final byte TN_REDIRECTING_NAME = 3;
    public static final byte TN_CONNECTED_NAME = 4;
    /** Constructs a new ANSI GenericName class, parameters with default values.  */
    public GenericNameAnsi() {
    }
    /** Constructs an ANSI class from the input parameters specified.
        @param in_pres - The presentation value, range 0 to 3. <ul>
        <li>PRESENTATION_ALLOWED, <li>PRESENTATION_RESTRICTED,
        <li>PRESENTATION_BLOCKING_TOGGLE and <li>PRESENTATION_NO_INDICATION. </ul>
        @param in_isAvail - The availability. <ul> <li>AVAILABILITY_NAME_AVAILABLE_UNKNOWN
        and <li>AVAILABILITY_NAME_UNAVAILABLE. </ul>
        @param in_typeOfName - The type of name, range 0 to 7. <ul> <li>TN_CALLING_NAME,
        <li>TN_ORIGINAL_CALLED_NAME, <li>TN_REDIRECTING_NAME and <li>TN_CONNECTED_NAME.
        </ul>
        @param in_genName - The name of the party, the number of characters in the name
        can be up to 15.
      */
    public GenericNameAnsi(byte in_pres, boolean in_isAvail, byte in_typeOfName, java.lang.String in_genName)
        throws ParameterRangeInvalidException {
        this.setPresentation(in_pres);
        this.setAvailability(in_isAvail);
        this.setTypeOfName(in_typeOfName);
        this.setName(in_genName);
    }
    /** Gets the Presentation field of the parameter, range 0 to 3.
        @return The Presentation sub-field, range 0 to 3, see GenericNameAnsi().
      */
    public byte getPresentation() {
        return m_presentation;
    }
    /** Sets the Presentation field of the parameter, range 0 to 3.
        @param aPresentation - The Presentation value, range 0 to 3, see
        GenericNameAnsi().
        @exception ParameterRangeInvalidException - Thrown when the sub-field is out of
        the parameter range specified.
      */
    public void setPresentation(byte aPresentation)
        throws ParameterRangeInvalidException {
        if (0 <= aPresentation && aPresentation <= 3) {
            m_presentation = aPresentation;
            return;
        }
        throw new ParameterRangeInvalidException("Presentation value " + aPresentation + " out of range.");
    }
    /** Gets the Availability field of the parameter.
        @return The Availability, see GenericNameAnsi().
      */
    public boolean getAvailability() {
        return m_availability;
    }
    /** Sets the Availability field of the parameter.
        @param aAvailability - The Availability value, see GenericNameAnsi().
      */
    public void setAvailability(boolean aAvailability) {
        m_availability = aAvailability;
    }
    /** Gets the Type of Name field of the parameter.
        @return The TypeOfName value, range 0 to 7, see GenericNameAnsi().
      */
    public byte getTypeOfName() {
        return m_typeOfName;
    }
    /** Sets the Type of Name field of the parameter.
        @param aTypeOfName - The TypeOfName value, range 0 to 7, see GenericNameAnsi().
        @exception ParameterRangeInvalidException - Thrown when the sub-field is out of
        the parameter range specified.
      */
    public void setTypeOfName(byte aTypeOfName)
        throws ParameterRangeInvalidException {
        if (0 <= aTypeOfName && aTypeOfName <= 7) {
            m_typeOfName = aTypeOfName;
            return;
        }
        throw new ParameterRangeInvalidException("TypeOfName value " + aTypeOfName + " out of range.");
    }
    /** Gets the Name field of the corresponding to a party.
        The number of characters in the name can be upto 15.
        @return The Name value.
      */
    public java.lang.String getName() {
        return m_name;
    }
    /** Sets the Name field of the corresponding to a party.
        @exception ParameterRangeInvalidException - Thrown when the sub-field is out of
        the parameter range specified.
      */
    public void setName(java.lang.String aName)
        throws ParameterRangeInvalidException {
        if (0 <= aName.length() && aName.length() <= 15) {
            m_name = aName;
            return;
        }
        throw new ParameterRangeInvalidException("TypeOfName value " + aName + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        GenericNameAnsi class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.ansi.GenericNameAnsi");
        b.append("\n\tm_presentation: " + m_presentation);
        b.append("\n\tm_availability: " + m_availability);
        b.append("\n\tm_typeOfName: " + m_typeOfName);
        b.append("\n\tm_name: " + m_name);
        return b.toString();
    }
    protected byte m_presentation;
    protected boolean m_availability;
    protected byte m_typeOfName;
    protected java.lang.String m_name;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
