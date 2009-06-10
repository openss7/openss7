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
  * This abstract class modles the MTP management primitives of Q.704,
  * ANSI T1.111.4, and other national standards.
  * These primitives are issued to inform the Jain MTP User that
  * signalling traffic from a concerned point code has changed.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class MtpManagementEvent extends MtpEvent {
    /** Private concerned point code field. */
    private javax.jain.ss7.SignalingPointCode concernedPointCode = null;
    /** Private concerned point code present field. */
    private boolean concernedPointCodeIsSet = false;
    /** Constructs a new MtpManagementEvent with specified object source
      * and primitive type.
      * @param source
      * The source object generating the event.
      * @param primitiveType
      * The type of primitive.
      */
    public MtpManagementEvent(java.lang.Object source,
            MtpPrimitiveType primitiveType) {
        super(source, primitiveType);
    }
    /** Constructs a new MtpManagementEvent with specified source,
      * primitive type and concerned point code.
      * @param source
      * The source object generating the event.
      * @param primitiveType
      * The type of primitive.
      * @param concernedPointCode
      * The concerned point code. The management status is only affected
      * from  the point of view of the concerned point code.  Whereas
      * the management status is for a remote MTP user or signalling
      * route, the concerned point code is the originating signalling
      * point for which the management status has changed. <p>
      *
      * The concerned point code is the signalling point code of the
      * JainMtpStackImpl of the JainMtpProvider from which the event was
      * issued.
      */
    public MtpManagementEvent(java.lang.Object source,
            MtpPrimitiveType primitiveType,
            javax.jain.ss7.SignalingPointCode concernedPointCode) {
        super(source, primitiveType);
        setConcernedPointCode(concernedPointCode);
    }
    /** Sets the concerned point code.
      * @param concernedPointCode
      * The concerned point code. The management status is only affected
      * from  the point of view of the concerned point code.  Whereas
      * the management status is for a remote MTP user or signalling
      * route, the concerned point code is the originating signalling
      * point for which the management status has changed. <p>
      *
      * The concerned point code is the signalling point code of the
      * JainMtpStackImpl of the JainMtpProvider from which the event was
      * issued.
      */
    public void setConcernedPointCode(javax.jain.ss7.SignalingPointCode concernedPointCode) {
        this.concernedPointCode = concernedPointCode;
        this.concernedPointCodeIsSet = true;
    }
    /** Gets the concerned point code.
      * @return
      * The concerned point code. The management status is only affected
      * from  the point of view of the concerned point code.  Whereas
      * the management status is for a remote MTP user or signalling
      * route, the concerned point code is the originating signalling
      * point for which the management status has changed. <p>
      *
      * The concerned point code is the signalling point code of the
      * JainMtpStackImpl of the JainMtpProvider from which the event was
      * issued.
      */
    public javax.jain.ss7.SignalingPointCode getConcernedPointCode() {
        return concernedPointCode;
    }
    /** Whether the concerned point code is present in the event.  The
      * concerned point code might be implied by the source of the
      * event.  In the case that the primitive originates from a
      * JainMtpProvider object, the concerned point code is implied by
      * the signalling point code of the JainMtpStackImpl object
      * associated with the JainMtpProvider.
      * @return
      * True or false.  True when the concerned point code is present in
      * the event; false, otherwise.
      */
    public boolean isConcernedPointCodePresent() {
        return concernedPointCodeIsSet;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

