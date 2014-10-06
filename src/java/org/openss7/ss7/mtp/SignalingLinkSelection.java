/*
 @(#) $RCSfile: SignalingLinkSelection.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:36:46 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:36:46 $ by $Author: brian $
 */

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * This class proivdes the realization of the signalling link selection
  * parameter of the MTP. <p>
  *
  * The signalling link selection field of the MTP transfer primitive is
  * used to specify a signalling link in a signalling link set that will
  * be used for normal traffic bearing the specified signalling link
  * selection value.  Application that require assurance of in-order
  * delivery of a set of messages must specify the same signaling link
  * selection value for transfer on all messages within such a set. <p>
  *
  * The signalling link selection value used by the underlying MTP
  * protocol can be of 4 significant bits (0 &le; sls &le; 15), 5
  * significant bits (0 &le; sls &le; 31), or 8 significant bits (0 &le;
  * sls &le; 255).  The user simply supplies a signalling link selection
  * parameter that is significant to as many bits as possible and the
  * underlying MTP implementation will select from the least significant
  * bits of the value to generate the field used by the specific MTP
  * protocol.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SignalingLinkSelection extends MtpParameter {
    private byte signallingLinkSelection;
    private static byte current = 0;
    /** Constructs a SignallingLinkSelection object with the default
      * value.  The default is to increment the value last used: this
      * ensures an equal distribution of values over signaling links in
      * a signalling link set. */
    public SignalingLinkSelection() {
        super();
        setSignallingLinkSelection(current++);
    }
    /** Constructs a SignalingLinkSelection object with the specified
      * value.
      * @param signalingLinkSelection
      * The signalling link selection to set.  The value may need to be
      * specified should the user wish to use a specific value of
      * signalling link selection.  This should be avoided to ensure
      * that the values are distrbuted as much as possible. */
    SignalingLinkSelection(byte singalingLinkSelection) {
        super();
        setSignalingLinkSelection(signalingLinkSelection);
    }
    /** Set the signalling link selection.
      * @param signalingLinkSelection
      * The signalling link selection to set.  Any one byte value (0
      * &le; sls &le; 255) is valid.  When distributing message traffic
      * over a number of signalling link selections, the user should
      * ensure that the less significant bits of this value change more
      * rapidly that the more significant bits.  This can be
      * accomplished easily simply by incrementing the previous value.
      */
    public void setSignalingLinkSelection(byte signalingLinkSelection) {
        this.signalingLinkSelection = signalingLinkSelection;
    }
    /** Get the signalling link selection.
      * @return
      * The value returned is 0 &le; sls &le; 255. */
    public byte getSignalingLinkSelection() {
        return signalingLinkSelection;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

