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

package javax.jain.ss7;

import javax.jain.*;

/**
  * SccpUserAddress represents an address of a Sccp User application.
  * A Sccp User application may handle a number of Sccp User Addresses
  * and will register as an Event Listener with any JainProvider for
  * each Sccp User Address. <p>
  *
  * Any Events addressed to a User Address will be passed to the User
  * application to whom the User Address belongs. The User Address
  * comprises of one or both of the following: <ul>
  *
  * <li>Sub-System Address
  *
  * <li>Global Title for use in Global Title translation. </ul>
  *
  * It is permitted for more than one JainListener to register with the
  * same JainProvider with the same Sccp User Address and events sent to
  * that Sccp User Address will be sent to all JainListeners of that
  * User Address.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public final class SccpUserAddress implements java.io.Serializable, SS7Address {
    /**
      * Constructs a SccpUserAddress with the specified SubSystemAddress
      * (Signalling Point Code and Sub-System Number).
      * This constructor automatically sets the Routing indicator to
      * {@linke RoutingIndicator#ROUTING_SUBSYSTEM ROUTING_SUBSYSTEM}.
      * @param subSystemAddress
      * The new SubSystem Address supplied to the constructor.
      */
    public SccpUserAddress(SubSystemAddress subSystemAddress) {
        setSubSystemAddress(subSystemAddress);
    }
    /**
      * Constructs a SccpUserAddress with the specified Global Title.
      * This constructor automatically sets the Routing indicator to
      * {@linke RoutingIndicator#ROUTING_GLOBALTITLE ROUTING_GLOBALTITLE}.
      * @param globalTitle
      * The new GlobalTitle supplied to the method.
      */
    public SccpUserAddress(GlobalTitle globalTitle) {
        setGlobalTitle(globalTitle);
    }
    /**
      * Sets the Routing indicator, which indicates if the global title
      * or a combination of the SubSystemAddress will be used by SCCP to
      * route a message.
      * @param
      * routingIndicator
      * The new Routing Indicator value is one of the following:- <ul>
      * <li>{@linke RoutingIndicator#ROUTING_SUBSYSTEM ROUTING_SUBSYSTEM}
      * <li>{@linke RoutingIndicator#ROUTING_GLOBALTITLE ROUTING_GLOBALTITLE} </ul>
      * @exception java.lang.IllegalArgumentException
      * Thrown when the supplied RoutingIndicator in invalid.
      * @exception MandatoryParameterNotSetException
      * Thrown when a RoutingIndicator is set without first setting the
      * relevent routing value.
      * @see RoutingIndicator
      */
    public void setRoutingIndicator(RoutingIndicator routingIndicator)
        throws MandatoryParameterNotSetException,
                          java.lang.IllegalArgumentException {
        m_routingIndicator = routingIndicator;
        m_routingIndicatorIsSet = (routingIndicator != null);
    }
    /**
      * Sets the Global Title of this User Address.
      * The Global Title is an object which can be identified to be one
      * of the following five different types: <ul>
      * <li>{@linke GlobalTitle GTIndicator0000}
      * <li>{@linke GTIndicator0001}
      * <li>{@linke GTIndicator0010}
      * <li>{@linke GTIndicator0011}
      * <li>{@linke GTIndicator0100} </ul>
      * @param globalTitle
      * The GlobalTitle of this SccpUserAddress.
      * @see GlobalTitle
      */
    public void setGlobalTitle(GlobalTitle globalTitle) {
        m_globalTitle = globalTitle;
        m_globalTitleIsSet = (globalTitle != null);
    }
    /**
      * Sets the Subsystem address, containing the Signaling Point Code
      * and Sub System Number of the subsystem.
      * @param subSystemAddress
      * The new Subsystem Address.
      * @see SubSystemAddress
      */
    public void setSubSystemAddress(SubSystemAddress subSystemAddress) {
        m_subSystemAddress = subSystemAddress;
        m_subSystemAddressIsSet = (subSystemAddress != null);
    }
    /**
      * Sets the National Use of this Sccp User Address.
      * The National Use represents bit eight of the Address Indicator.
      * It is reserved for national use and is always set to zero on an
      * international network. Note to developers - The National Use
      * field is defaulted to false (zero).
      * @param nationalUse
      * The new National Use value.
      */
    public void setNationalUse(boolean nationalUse) {
        m_nationalUse = nationalUse;
        m_nationalUseIsSet = true;
    }
    /**
      * Sets the Network Indicator of this User Address, which
      * identities the network in which the signaling point code of this
      * address is defined.
      * The Network Indicator can be identified to be one of the
      * following different types: <ul>
      * <li>{@link NetworkIndicator#NI_INTERNATIONAL_00 NI_INTERNATIONAL_00}
      * <li>{@link NetworkIndicator#NI_INTERNATIONAL_01 NI_INTERNATIONAL_01}
      * <li>{@link NetworkIndicator#NI_NATIONAL_10 NI_NATIONAL_10}
      * <li>{@link NetworkIndicator#NI_NATIONAL_11 NI_NATIONAL_11} </ul>
      * @param networkIndicator
      * The new NetworkIndicator value.
      * @see NetworkIndicator
      */
    public void setNetworkIndicator(NetworkIndicator networkIndicator) {
        m_networkIndicator = networkIndicator;
        m_networkIndicatorIsSet = (networkIndicator != null);
    }
    /**
      * Gets the Network Indicator of this User Address, which
      * identities the network in which the signaling point code of this
      * address is defined.
      * @return
      * The Network Indicator of this SccpUserAddress.
      * @exception ParameterNotSetException
      * This exception is thrown if the Network Indicator parameter has
      * not been set.
      * @see NetworkIndicator
      */
    public NetworkIndicator getNetworkIndicator()
        throws ParameterNotSetException {
        if (m_networkIndicatorIsSet)
            return m_networkIndicator;
        throw new ParameterNotSetException("Network Indicator: not set.");
    }
    /**
      * Gets the Routing indicator.
      * The indicator indicates if global title or a combination of
      * SubSystemAddress will be used by SCCP to route a message.
      * @return
      * One of the following:- <ul>
      * <li>{@link RoutingIndicator#ROUTING_SUBSYSTEM ROUTING_SUBSYSTEM}
      * <li>{@link RoutingIndicator#ROUTING_GLOBALTITLE ROUTING_GLOBALTITLE} </ul>
      * @see RoutingIndicator
      */
    public RoutingIndicator getRoutingIndicator() {
        return m_routingIndicator;
    }
    /**
      * Gets the Global Title of the User Address.
      * The GlobalTitle is an object, of which the default value is
      * null. If the get accessor method is used and the GlobalTitle
      * object has a null value, then a ParameterNotSetException will be
      * thrown.
      * @return
      * The Global Title of this SccpUserAddress.
      * @exception ParameterNotSetException
      * This exception is thrown if the GlobalTitle parameter has not
      * been set, i.e Routing by Subsystem.
      * @see GlobalTitle
      */
    public GlobalTitle getGlobalTitle()
        throws ParameterNotSetException {
        if (m_globalTitleIsSet)
            return m_globalTitle;
        throw new ParameterNotSetException("Global Title: not set.");
    }
    /**
      * Gets the Subsystem address, containing the Signaling Point Code
      * and Sub System Number of the subsystem.
      * @return
      * The Subsystem Address of this Sccp User Address.
      * @exception ParameterNotSetException
      * Thrown if theSubSystemAddress parameter has not been set, i.e
      * Routing by GlobalTitle.
      * @see SubSystemAddress
      */
    public SubSystemAddress getSubSystemAddress()
        throws ParameterNotSetException {
        if (m_subSystemAddressIsSet)
            return m_subSystemAddress;
        throw new ParameterNotSetException("SubSystem Address: not set.");
    }
    /**
      * Gets the National Use of this Sccp User Address.
      * The National Use represents bit eight of the Address Indicator.
      * It is reserved for national use and is always set to zero on an
      * international network.
      * @return
      * The National Use of this Sccp User Address one fo the following:- <ul>
      * <li>true - Octet eight of the Address Indicator equals 1.
      * <li>false - Octet eight of the Address Indicator equals 0. </ul>
      */
    public boolean getNationalUse() {
        return m_nationalUse;
    }
    /**
      * String representation of class SccpUserAddress.
      * @return
      * String provides description of class SccpUserAddress.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.SccpUserAddress");
        b.append("\n\tm_networkIndicator = ");
        if (m_networkIndicator != null) b.append(m_networkIndicator.toString());
        b.append("\n\tm_networkIndicatorIsSet = " + m_networkIndicatorIsSet);
        b.append("\n\tm_routingIndicator = ");
        if (m_routingIndicator != null) b.append(m_routingIndicator.toString());
        b.append("\n\tm_routingIndicatorIsSet = " + m_routingIndicatorIsSet);
        b.append("\n\tm_globalTitle = ");
        if (m_globalTitle != null) b.append(m_globalTitle.toString());
        b.append("\n\tm_globalTitleIsSet = " + m_globalTitleIsSet);
        b.append("\n\tm_subSystemAddress = ");
        if (m_subSystemAddress != null) b.append(m_subSystemAddress.toString());
        b.append("\n\tm_subSystemAddressIsSet = " + m_subSystemAddressIsSet);
        b.append("\n\tm_nationalUse = " + m_nationalUse);
        b.append("\n\tm_nationalUseIsSet = " + m_nationalUseIsSet);
        return b.toString();
    }
    protected NetworkIndicator m_networkIndicator = null;
    protected boolean m_networkIndicatorIsSet = false;
    protected RoutingIndicator m_routingIndicator = null;
    protected boolean m_routingIndicatorIsSet = false;
    protected GlobalTitle m_globalTitle = null;
    protected boolean m_globalTitleIsSet = false;
    protected SubSystemAddress m_subSystemAddress = null;
    protected boolean m_subSystemAddressIsSet = false;
    protected boolean m_nationalUse = false;
    protected boolean m_nationalUseIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
