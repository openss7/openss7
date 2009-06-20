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

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * This class models the user status parameter of the MTP-STATUS
  * indication primitive of Q.704, ANSI T1.111.4, and other national
  * standards: it also models the parameter of the DUPU message of M3UA
  * (RFC 4666).
  * This class is a constant class with the following defined values:
  * <ul>
  * <li>{@link #UNEQUIPPED}, user inaccessible, reason: unequipped;
  * <li>{@link #UNAVAILABLE}, user inaccessible, reason: unavailable; and,
  * <li>{@link #INACCESSIBLE}, user inaccessible, reason: unknown. </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class UserStatus extends MtpParameter {
    /** Internal integer value: user inaccessible, reason: unequipped.  */
    public static final int M_UNEQUIPPED = 0;
    /** Internal integer value: user inaccessible, reason: unavailable.  */
    public static final int M_UNAVAILABLE = 1;
    /** Internal integer value: user inaccessible, reason: unknown.  */
    public static final int M_INACCESSIBLE = 2;
    /** Internal intever value of constant class. */
    private int value;
    /** Construct a UserStatus object.
      * @param value
      * The internal integer value.  This is one of: <ul>
      * <li>{@link #M_UNEQUIPPED},
      * <li>{@link #M_UNAVAILABLE} and
      * <li>{@link #M_INACCESSIBLE}. </ul>
      */
    private UserStatus(int value) {
        setValue(value);
    }
    /** Sets the internal integer value.
      * @param value
      * The internal integer value.  This is one of: <ul>
      * <li>{@link #M_UNEQUIPPED},
      * <li>{@link #M_UNAVAILABLE} and
      * <li>{@link #M_INACCESSIBLE}. </ul>
      */
    private void setValue(int value) {
        this.value = value;
    }
    /** Gets the integer value of the constant class.
      * @return
      * An integer value that can be used in case statements.  This
      * value is one of: <ul>
      * <li>{@link #M_UNEQUIPPED},
      * <li>{@link #M_UNAVAILABLE} and
      * <li>{@link #M_INACCESSIBLE}. </ul>
      */
    public int getValue() {
        return value;
    }
    /** MTP User Status: user inaccessible, reason: unequipped.  */
    public static final UserStatus UNEQUIPPED
        = new UserStatus(M_UNEQUIPPED);
    /** MTP User Status: user inaccessible, reason: unavailable.  */
    public static final UserStatus UNAVAILABLE
        = new UserStatus(M_UNAVAILABLE);
    /** MTP User Status: user inaccessible, reason: unknown.  */
    public static final UserStatus INACCESSIBLE
        = new UserStatus(M_INACCESSIBLE);
}


// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

