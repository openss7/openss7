/*
 @(#) src/java/org/openss7/ss7/mtp/MtpUserPart.java <p>
 
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
  * This class models an MTP user part.  An MTP User part consists of a
  * constant between 0 and 15 inclusive.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class MtpUserPart extends MtpParameter {
    /** Internal integer value: any user part. */
    public static final int M_ANY_USER      = -1;
    //public static final int M_SNMM_USER   = 0;
    //public static final int M_SNTM_USER   = 1;
    //public static final int M_SSTM_USER   = 2;
    /** Internal integer value: SCCP user.  */
    public static final int M_SCCP_USER     = 3;
    /** Internal integer value: TUP user.  */
    public static final int M_TUP_USER      = 4;
    /** Internal integer value: ISUP user.  */
    public static final int M_ISUP_USER     = 5;
    /** Internal integer value: DUP call user.  */
    public static final int M_DUP1_USER     = 6;
    /** Internal integer value: DUP facility user.  */
    public static final int M_DUP2_USER     = 7;
    /** Internal integer value: MTUP user.  */
    public static final int M_MTUP_USER     = 8;
    /** Internal integer value: Broadband ISUP user.  */
    public static final int M_BISUP_USER    = 9;
    /** Internal integer value: Satellite ISUP user.  */
    public static final int M_SISUP_USER    = 10;
    /** Internal integer value: user #11.  */
    public static final int M_USER_11       = 11;
    /** Internal integer value: AAL2 signalling.  */
    public static final int M_AAL2_USER     = 12;
    /** Internal integer value: BICC user.  */
    public static final int M_BICC_USER     = 13;
    /** Internal integer value: GCP user.  */
    public static final int M_GCP_USER      = 14;
    /** Internal integer value: reserved user.  */
    public static final int M_RESERVED_USER = 15;
    /** Private internal value of the constant class. */
    private int userPart;
    /** Create an MtpUserPart instance.
      * @param userPart
      * The internal integer value of the user part.  */
    protected MtpUserPart(int userPart) {
        setUserPart(userPart);
    }
    /** Set the internal integer user part.
      * @param userPart
      * The internal integer value of the user part.  */
    protected void setUserPart(int userPart) {
        this.userPart = userPart;
    }
    /** Get the internal integer user part value.
      * @return
      * The internal integer value of the user part.  */
    public int getUserPart() {
        return userPart;
    }
    /** MTP User Part: any user.  */
    public static final MtpUserPart ANY_USER      = new MtpUserPart(M_ANY_USER);
    /** MTP User Part: SCCP user.  */
    public static final MtpUserPart SCCP_USER     = new MtpUserPart(M_SCCP_USER);
    /** MTP User Part: TUP user.  */
    public static final MtpUserPart TUP_USER      = new MtpUserPart(M_TUP_USER);
    /** MTP User Part: ISUP user.  */
    public static final MtpUserPart ISUP_USER     = new MtpUserPart(M_ISUP_USER);
    /** MTP User Part: DUP1 user.  */
    public static final MtpUserPart DUP1_USER     = new MtpUserPart(M_DUP1_USER);
    /** MTP User Part: DUP2 user.  */
    public static final MtpUserPart DUP2_USER     = new MtpUserPart(M_DUP2_USER);
    /** MTP User Part: MTUP user.  */
    public static final MtpUserPart MTUP_USER     = new MtpUserPart(M_MTUP_USER);
    /** MTP User Part: BISUP user.  */
    public static final MtpUserPart BISUP_USER    = new MtpUserPart(M_BISUP_USER);
    /** MTP User Part: SISUP user.  */
    public static final MtpUserPart SISUP_USER    = new MtpUserPart(M_SISUP_USER);
    /** MTP User Part: user #11.  */
    public static final MtpUserPart USER_11       = new MtpUserPart(M_USER_11);
    /** MTP User Part: AAL2 user.  */
    public static final MtpUserPart AAL2_USER     = new MtpUserPart(M_AAL2_USER);
    /** MTP User Part: BICC user.  */
    public static final MtpUserPart BICC_USER     = new MtpUserPart(M_BICC_USER);
    /** MTP User Part: GCP user.  */
    public static final MtpUserPart GCP_USER      = new MtpUserPart(M_GCP_USER);
    /** MTP User Part: RESERVED user.  */
    public static final MtpUserPart RESERVED_USER = new MtpUserPart(M_RESERVED_USER);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
