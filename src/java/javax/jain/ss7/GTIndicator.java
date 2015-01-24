/*
 @(#) src/java/javax/jain/ss7/GTIndicator.java <p>
 
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
  * Constants used by the JAIN SS7 GlobalTitle class..
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class GTIndicator {
    /** Hidden default constructor. */
    private GTIndicator() { }
    /** Protected default constructure with value. */
    protected GTIndicator(java.lang.String value) {
        m_value = value;
        m_valueIsSet = true;
    }
    /** java.lang.String representation of the Constant class.
      * @return java.lang.String provides description of Constant.  */
    public java.lang.String toString() {
        return m_value;
    }
    /** Value of constant class.
      * @serial m_value
      * A default serialized field.  */
    protected java.lang.String m_value = "(error)";
    /** Whether the value is set.
      * @serial m_valueIsSet
      * A default serialized field.  */
    protected boolean m_valueIsSet = false;
    /** Indicates a that the GlobalTitle is not included.  */
    public static final GTIndicator GTINDICATOR_0000 =
        new GTIndicator("GTINDICATOR_0000");
    /** Indicates a GTIndicator0001 primitive.  */
    public static final GTIndicator GTINDICATOR_0001 =
        new GTIndicator("GTINDICATOR_0001");
    /** Indicates a GTIndicator0010 primitive.  */
    public static final GTIndicator GTINDICATOR_0010 =
        new GTIndicator("GTINDICATOR_0010");
    /** Indicates a GTIndicator0011 primitive.  */
    public static final GTIndicator GTINDICATOR_0011 =
        new GTIndicator("GTINDICATOR_0011");
    /** Indicates a GTIndicator0100 primitive.  */
    public static final GTIndicator GTINDICATOR_0100 =
        new GTIndicator("GTINDICATOR_0100");
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
