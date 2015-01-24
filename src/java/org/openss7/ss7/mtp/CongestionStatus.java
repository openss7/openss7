/*
 @(#) src/java/org/openss7/ss7/mtp/CongestionStatus.java <p>
 
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
  * This class models the congestion status parmater of the MTP-STATUTS
  * message.
  * The congestion status can have one of the following values: <ul>
  * <li>{@link #CONG_STATUS}, congestion with no specific level;
  * <li>{@link #CONG_STATUS_0}, congestion at level 0;
  * <li>{@link #CONG_STATUS_1}, congestion at level 1;
  * <li>{@link #CONG_STATUS_2}, congestion at level 2; and
  * <li>{@link #CONG_STATUS_3}, congesiton at level 3. </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CongestionStatus extends MtpParameter {
    /** Internal integer value: congestion status.  */
    public static final int M_CONG_STATUS = -1;
    /** Internal integer value: congestion status 0.  */
    public static final int M_CONG_STATUS_0 = 0;
    /** Internal integer value: congestion status 1.  */
    public static final int M_CONG_STATUS_1 = 1;
    /** Internal integer value: congestion status 2.  */
    public static final int M_CONG_STATUS_2 = 2;
    /** Internal integer value: congestion status 3.  */
    public static final int M_CONG_STATUS_3 = 3;
    /** Internal integer field. */
    private int value;
    /** Constructs a new CongestionStatus. */
    private CongestionStatus(int value) {
        super();
        setValue(value);
    }
    /** Sets integer value.  */
    private void setValue(int value) {
        this.value = value;
    }
    /** Gets integer value for use in case statements.
      * @return
      * Integer value of the constant class. */
    public int getValue() {
        return value;
    }
    /** Congestion Status: congestion status.  */
    public static final CongestionStatus CONG_STATUS = new CongestionStatus(M_CONG_STATUS);
    /** Congestion Status: congestion status 0.  */
    public static final CongestionStatus CONG_STATUS_0 = new CongestionStatus(M_CONG_STATUS_0);
    /** Congestion Status: congestion status 1.  */
    public static final CongestionStatus CONG_STATUS_1 = new CongestionStatus(M_CONG_STATUS_1);
    /** Congestion Status: congestion status 2.  */
    public static final CongestionStatus CONG_STATUS_2 = new CongestionStatus(M_CONG_STATUS_2);
    /** Congestion Status: congestion status 3.  */
    public static final CongestionStatus CONG_STATUS_3 = new CongestionStatus(M_CONG_STATUS_3);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
