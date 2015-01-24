/*
 @(#) src/java/org/openss7/ss7/mtp/MessagePriority.java <p>
 
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

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * This class provides the realization of the message priority
  * parameter of the MTP.  The values of the message priority can be:
  * <ul>
  *
  * <li>{@link #PRIORITY_NONE}, the message priority is none, or not
  * specified;
  *
  * <li>{@link #PRIORITY_00}, the mesage priority is zero (0);
  *
  * <li>{@link #PRIORITY_01}, the message priority is one (1);
  *
  * <li>{@link #PRIORITY_10}, the message priority is two (2); and,
  *
  * <li>{@link #PRIORITY_11}, the messagae priority is three (3). </ul>
  *
  * {@link #PRIORITY_NONE} is used when the message priority is not
  * specified or is not applicable (because, for example, the underlying
  * MTP protocol variant does not support message priorities). <p>
  *
  * {@link #PRIORITY_11} is not permitted for MTP User transfer
  * primitives.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class MessagePriority extends MtpParameter {
    /** Internal integer value: no priority.  */
    public static final int M_PRIORITY_NONE = -1;
    /** Internal integer value: message priority 0.  */
    public static final int M_PRIORITY_00 = 0;
    /** Internal integer value: message priority 1.  */
    public static final int M_PRIORITY_01 = 1;
    /** Internal integer value: message priority 2.  */
    public static final int M_PRIORITY_10 = 2;
    /** Internal integer value: message priority 3.  */
    public static final int M_PRIORITY_11 = 3;
    private int value;
    private MessagePriority(int value) {
        setValue(value);
    }
    /** Sets the integer value of the constant class.
      * @param value
      * The integer value associated with the constant class.  This is
      * one of: <ul>
      * <li>{@link #M_PRIORITY_NONE},
      * <li>{@link #M_PRIORITY_00},
      * <li>{@link #M_PRIORITY_01},
      * <li>{@link #M_PRIORITY_10} and
      * <li>{@link #M_PRIORITY_11}. </ul>
      */
    private void setValue(int value) {
        this.value = value;
    }
    /** Gets the integer value of the constant class.
      * @return
      * The integer value associated with the constant class.  This is
      * one of: <ul>
      * <li>{@link #M_PRIORITY_NONE},
      * <li>{@link #M_PRIORITY_00},
      * <li>{@link #M_PRIORITY_01},
      * <li>{@link #M_PRIORITY_10} and
      * <li>{@link #M_PRIORITY_11}. </ul>
      */
    public int getValue() {
        return value;
    }
    /** Message Priority: no message priority.  */
    public static final MessagePriority PRIORITY_NONE
        = new MessagePriority(M_PRIORITY_NONE);
    /** Message Priority: message priority 0.  */
    public static final MessagePriority PRIORITY_00
        = new MessagePriority(M_PRIORITY_00);
    /** Message Priority: message priority 1.  */
    public static final MessagePriority PRIORITY_01
        = new MessagePriority(M_PRIORITY_01);
    /** Message Priority: message priority 2.  */
    public static final MessagePriority PRIORITY_10
        = new MessagePriority(M_PRIORITY_10);
    /** Message Priority: message priority 3.  */
    public static final MessagePriority PRIORITY_11
        = new MessagePriority(M_PRIORITY_11);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

