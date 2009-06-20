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

/**
  * This interface defines the methods required by all MTP user
  * applications to receive and process JAIN MAP Events that are emitted
  * by an object implementing the
  * {@link org.openss7.ss7.mtp.JainMtpProvider} interface.
  * It must be noted that any object than implements the JainMtpListener
  * interface is referred to as a MtpListenerImpl; the JainMtpProvider
  * interface, a JainMtpProviderImpl.  The events emitted by JainMtpProviderImpl
  * may be {@link org.openss7.ss7.mtp.MtpEvent} events.  These events
  * correspond to MTP indication primitives emitted as events by the
  * JainMtpProviderImpl.  MTP indication events represent MTP service
  * primtives that are sent from MTP to the MTP User. <p>
  *
  * A MTP User application will only receive indication events once it
  * has registered as an EventListener of a JainMtpProviderImpl.  The user
  * application will register with the JainMtpProviderImpl with a User
  * Address by invoking the ProviderImpl's {@link
  * org.openss7.ss7.mtp.JainMtpProvider#addMtpListener} method.  Any events
  * address to a User Address belonging to a User Application will be
  * passed to that User application. <p>
  *
  * The User application will use the JainMtpProviderImpl to send MTP
  * messages by send {@link org.openss7.ss7.mtp.MtpEvent} events to that
  * JainMtpProviderImpl's
  * {@link org.openss7.ss7.mtp.JainMtpProvider#sendMtpEvent} method.
  *
  * @author     Monavacon Limited
  * @version    1.2.2
  * @see        JainMtpProvider
  */
public interface JainMtpListener extends java.util.EventListener {
    /**
      * Process an MTP Event emitted by the JainMtpProvider.  Indication
      * and Confirmation events can only be passed from MTP (the
      * JainMtpProviderImpl) to the MTP User (MtpListenerImpl) and is
      * enforced by the JAIN MTP Event hierarchy and error checking
      * within the specification, to ensure MTP like processing.
      *
      * @param event  The new even supplied by the method.
      */
    public void processMtpEvent(MtpEvent event);
    /**
      * Adds a {@link org.openss7.ss7.mtp.MtpUserAddress} class to the
      * list of user addresses used by a MtpListenerImpl.
      * A MtpListenerImpl may choose to register as an Event Listener of
      * a JainMtpProvider for all of its user addreses.  Any MTP messages
      * addressed to any of this MtpListenerImpl's user addresses will
      * be passed to this MtpListenerImpl as events by the
      * JainMtpProviderImpl.
      *
      * @param userAddress  The feature to be added to the UserAddress
      * attribute.
      */
    public void addUserAddress(MtpUserAddress userAddress)
	throws jain.protocol.ss7.UserAddressLimitException;
    /**
      * Removes the {@link org.openss7.ss7.mtp.MtpUserAddress} class
      * form the list of user addresses held by a MtpListenerImpl.
      *
      * @param userAddress  The user address to be removed.
      */
    public void removeUserAddress(MtpUserAddress userAddress)
	throws jain.protocol.ss7.InvalidAddressException;
    /**
      * Returns the list of {@link org.openss7.ss7.mtp.MtpUserAddress}
      * used by this MtpListenerImpl.
      *
      * @return The MtpUserAddress[] of user addresses used by this
      * MtpListenerImpl.
      */
    public MtpUserAddress[] getUserAddressList()
	throws jain.protocol.ss7.UserAddressEmptyException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
