/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Module Name   : JAIN Common API
 * File Name     : SccpUserAddress.java
 * Originator    : Phelim O'Doherty
 * Approver      : Jain Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     28/09/2001  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7;

import java.io.Serializable;
import javax.jain.*;

/**
 * SccpUserAddress represents an address of a Sccp User application. <BR>
 * A Sccp User application may handle a number of Sccp User Addresses and will
 * register as an Event Listener with any JainProvider for each Sccp User Address.
 * <P>
 *
 * Any Events addressed to a User Address will be passed to the User
 * application to whom the User Address belongs. The User Address comprises of
 * one or both of the following:
 * <UL>
 *   <LI> <B>Sub-System Address</B>
 *   <LI> <B>Global Title</B> for use in Global Title translation.
 * </UL>
 * <BR>
 * It is permitted for more than one JainListener to register with the same
 * JainProvider with the same Sccp User Address and events sent to that Sccp User
 * Address will be sent to all JainListeners of that User Address.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public final class SccpUserAddress implements Serializable, SS7Address {

    /**
    *  Constructs a SccpUserAddress with the specified SubSystemAddress (Signalling Point Code and
    *  Sub-System Number). This constructor automatically sets the Routing
    *  indicator to ROUTING_SUBSYSTEM.
    *
    * @param  subSystemAddress the new SubSystem Address supplied to the constructor
    */
    public SccpUserAddress(SubSystemAddress subSystemAddress) {
        setSubSystemAddress(subSystemAddress);
        m_RoutingIndicator = RoutingIndicator.ROUTING_SUBSYSTEM;
    }

    /**
    * Constructs a SccpUserAddress with the specified Global Title. This
    * constructor automatically sets the Routing indicator to
    * ROUTING_GLOBALTITLE.
    *
    * @param  globalTitle the new GlobalTitle supplied to the method
    */
    public SccpUserAddress(GlobalTitle globalTitle) {
        setGlobalTitle(globalTitle);
        m_RoutingIndicator = RoutingIndicator.ROUTING_GLOBALTITLE;
    }

    /**
    * Sets the Routing indicator, which indicates if the global title or a
    * combination of the SubSystemAddress will be used by
    * SCCP to route a message.
    *
    * @param  routingIndicator  The new Routing Indicator value is one of the following:-
    *   <UL>
    *   <LI> ROUTING_SUBSYSTEM
    *   <LI> ROUTING_GLOBALTITLE
    *   </UL>
    *
    * @throws IllegalArgumentException if the supplied RoutingIndicator in invalid
    * @throws MandatoryParameterNotSetException if a RoutingIndicator is set
    * without first setting the relevent routing value
    *
    * @see RoutingIndicator
    */
    public void setRoutingIndicator(RoutingIndicator routingIndicator) throws MandatoryParameterNotSetException, IllegalArgumentException{
        if (RoutingIndicator.ROUTING_GLOBALTITLE != routingIndicator ||
            RoutingIndicator.ROUTING_SUBSYSTEM != routingIndicator) {
               throw new IllegalArgumentException("Invalid Routing Type");
        } else if (RoutingIndicator.ROUTING_GLOBALTITLE == routingIndicator &&
                  m_GlobalTitle == null){
                    throw new MandatoryParameterNotSetException("GlobalTitle not set");
               } else if (RoutingIndicator.ROUTING_SUBSYSTEM == routingIndicator &&
                  m_SubSystemAddress == null){
                    throw new MandatoryParameterNotSetException("Subsystem not set");
               } else {
                    m_RoutingIndicator = routingIndicator;
               }
    }

    /**
    * Sets the Global Title of this User Address. The Global Title is an object
    * which can be identified to be one of the following five different types:
    * <UL>
    *   <LI> GTIndicator0000
    *   <LI> GTIndicator0001
    *   <LI> GTIndicator0010
    *   <LI> GTIndicator0011
    *   <LI> GTIndicator0100
    * </UL>
    *
    * @param  globalTitle  the GlobalTitle of this SccpUserAddress
    * @see GlobalTitle
    */
    public void setGlobalTitle(GlobalTitle globalTitle){
        m_GlobalTitle = globalTitle;
    }

    /**
    * Sets the Subsystem address, containing the Signaling Point Code and Sub System Number of the
    * subsystem.
    *
    * @param  subSystemAddress              the new Subsystem Address
    * @see    SubSystemAddress
    */
    public void setSubSystemAddress(SubSystemAddress subSystemAddress) {
        m_SubSystemAddress = subSystemAddress;
    }

    /**
    * Sets the National Use of this Sccp User Address. The National Use
    * represents bit eight of the Address Indicator. It is reserved for national
    * use and is always set to zero on an international network. <b>Note to
    * developers</b> - The National Use field is defaulted to false (zero).
    *
    * @param  nationalUse  the new National Use value
    */
    public void setNationalUse(boolean nationalUse) {
        m_NationalUse = nationalUse;
    }


    /**
    * Sets the Network Indicator of this User Address, which identities the
    * network in which the signaling point code of this address is defined.
    * The Network Indicator can be identified to be one of the following
    * different types:
    * <UL>
    *   <LI> NI_INTERNATIONAL_00
    *   <LI> NI_INTERNATIONAL_01
    *   <LI> NI_NATIONAL_10
    *   <LI> NI_NATIONAL_11
    * </UL>
    *
    * @param  networkIndicator  the new NetworkIndicator value
    */
    public void setNetworkIndicator(NetworkIndicator networkIndicator) {
        m_NetworkIndicator = networkIndicator;
    }

    /**
    * Gets the Network Indicator of this User Address, which identities the
    * network in which the signaling point code of this address is defined.
    *
    * @return     The Network Indicator of this SccpUserAddress
    * @exception  ParameterNotSetException  this exception is thrown if the
    * Network Indicator parameter has not been set.
    *
    * @see NetworkIndicator
    */
    public NetworkIndicator getNetworkIndicator() throws ParameterNotSetException {
       if (m_NetworkIndicator == null) {
          throw new ParameterNotSetException("Network Indicator not set");
       }
       return m_NetworkIndicator;
    }



    /**
    * Gets the Routing indicator. The indicator indicates if global title or a
    * combination of SubSystemAddress will be used by SCCP
    * to route a message.
    *
    * @return    one of the following:-
    *      <UL>
    *        <LI> ROUTING_SUBSYSTEM
    *        <LI> ROUTING_GLOBALTITLE
    *      </UL>
    *
    */
    public RoutingIndicator getRoutingIndicator() {
        return m_RoutingIndicator;
    }

    /**
    * Gets the Global Title of the User Address. The GlobalTitle is an object, of
    * which the default value is null. If the get accessor method is used and the
    * GlobalTitle object has a null value, then a ParameterNotSetException will
    * be thrown.
    *
    * @return     The Global Title of this SccpUserAddress
    * @exception  ParameterNotSetException  this exception is thrown if the
    * GlobalTitle parameter has not been set, i.e Routing by Subsystem
    *
    * @see GlobalTitle
    */
    public GlobalTitle getGlobalTitle() throws ParameterNotSetException {
        if (null == m_GlobalTitle) {
            throw new ParameterNotSetException("Global Title has not been set");
        } else {
            return m_GlobalTitle;
        }
    }

    /**
    * Gets the Subsystem address, containing the Signaling Point Code and
    * Sub System Number of the subsystem.
    *
    * @return the Subsystem Address of this Sccp User Address
    * @exception  ParameterNotSetException  thrown if theSubSystemAddress
    * parameter has not been set, i.e Routing by GlobalTitle
    * @see        SubSystemAddress
    */
    public SubSystemAddress getSubSystemAddress() throws ParameterNotSetException {
        if (null == m_SubSystemAddress) {
            throw new ParameterNotSetException("SubSystem Address has not been set");
        } else {
            return m_SubSystemAddress;
        }
    }

    /**
    * Gets the National Use of this Sccp User Address. The National Use
    * represents bit eight of the Address Indicator. It is reserved for national
    * use and is always set to zero on an international network.
    *
    * @return    The National Use of this Sccp User Address one fo the following:-
    *        <UL>
    *            <LI> true  - Octet eight of the Address Indicator equals 1
    *            <LI> false - Octet eight of the Address Indicator equals 0
    *        </UL>
    */
    public boolean getNationalUse() {
        return m_NationalUse;
    }

    /**
    * String representation of class SccpUserAddress
    *
    * @return    String provides description of class SccpUserAddress
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(1000);
        buffer.append("\n\nroutingIndicator = ");
        buffer.append(this.getRoutingIndicator().toString());
        buffer.append("\n\nnationalUse = ");
        buffer.append(this.getNationalUse());
        buffer.append("\n\nnetworkIndicator = ");
        try {
              buffer.append(this.getNetworkIndicator().toString());
        } catch (ParameterNotSetException e){
            buffer.append("value is null");
        }
        buffer.append("\n\nsubSystemAddress: ");
        if (this.m_SubSystemAddress != null) {
          try {
            buffer.append(this.getSubSystemAddress().toString());
          } catch (ParameterNotSetException e){}
        } else {
            buffer.append("value is null");
        }
        buffer.append("\n\nglobalTitle: ");
        if (this.m_GlobalTitle != null) {
          try{
            buffer.append(this.getGlobalTitle().toString());
          } catch (ParameterNotSetException e){}
        } else {
            buffer.append("value is null");
        }
        return buffer.toString();
    }

    /**
    * Indicates the type of address routing used
    *
    * @serial    m_RoutingIndicator - a default serializable field
    */
    private RoutingIndicator m_RoutingIndicator = null;

    /**
    * Reserved field for national use
    *
    * @serial    m_NationalUse - a default serializable field
    */
    private boolean m_NationalUse = false;

    /**
    * The Network Indicator of the User Address
    *
    * @serial    m_NetworkIndicator - a default serializable field
    */
    private NetworkIndicator m_NetworkIndicator = null;

    /**
    * The Sub-System Number of the User Address
    *
    * @serial    m_SubSystemAddress - a default serializable field
    */
    private SubSystemAddress m_SubSystemAddress = null;

    /**
    * The Global Title of the User Address
    *
    * @serial    m_GlobalTitle - a default serializable field
    */
    private GlobalTitle m_GlobalTitle = null;
}
