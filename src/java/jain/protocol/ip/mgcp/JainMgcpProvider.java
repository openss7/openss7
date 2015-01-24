/*
 @(#) src/java/jain/protocol/ip/mgcp/JainMgcpProvider.java <p>
 
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

package jain.protocol.ip.mgcp;

/**
    This interface must be implemented by any class that represents the JAIN MGCP abstraction of an
    MGCP stack and that interacts directly with a proprietary (vendor-specific) implementation of
    that MGCP stack.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface JainMgcpProvider {
    /**
        Adds a JAIN MGCP Listener to the list of Event Listeners registered on an implementation of
        the JainMgcpProvider interface.
        @param jainMgcpListener A reference to an object that implements the JainMgcpListener
        interface and that is to be added to the list of Event Listeners.
        @exception java.util.TooManyListenersException Thrown if adding this Listener would exceed
        the number of JAIN MGCP Listeners permitted by the implementation.
      */
    public void addJainMgcpListener(JainMgcpListener jainMgcpListener)
        throws java.util.TooManyListenersException;
    /**
        Returns an object reference to the underlying JAIN MGCP Stack to which this JAIN MGCP
        Provider is bound.
        @return An object reference to the underlying JAIN MGCP Stack to which this JAIN MGCP
        Provider is bound.
      */
    public JainMgcpStack getJainMgcpStack();
    /**
        Removes a JAIN MGCP Listener from the list of Event Listeners registered on an
        implementation of the JainMgcpProvider interface. If the object reference to the
        JainMgcpListener does not correspond to a registered Listener, this method simply returns.
        @param jainMgcpListener A reference to an object that implements the JainMgcpListener
        interface and that is to be removed from the list of Event Listeners.
      */
    public void removeJainMgcpListener(JainMgcpListener jainMgcpListener);
    /**
        Sends a list of MGCP commands and responses into the MGCP stack. The use of a list is
        intended to facilitate "piggybacking" of messages, all of whose destinations are the same.
        Specific vendor implementations may impose restrictions on the number of command and
        response messages, the sequencing of those messages, and other such limitations on the usage
        of this method.
        @param jainMgcpEvents An array of JainMgcpEvent objects.
        @exception java.lang.IllegalArgumentException Thrown if any of the limitations imposed by a
        vendor's implementation are exceeded.
      */
    public void sendMgcpEvents(JainMgcpEvent[] jainMgcpEvents)
        throws java.lang.IllegalArgumentException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
