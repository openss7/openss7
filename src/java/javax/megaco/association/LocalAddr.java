// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Use Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASSNEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class LocalAddr

java.lang.Object
  |
  +--javax.megaco.association.LocalAddr

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class LocalAddr
   extends java.lang.Object

   This class represents the local transport address. This transport
   address is used by the stack to send messages to peer. It can be used
   to store and retrieve the local transport type and the corresponding
   address. Only one of the transport addresses can be set for the local
   entity. If the transport is SCTP, then multiple local IP addresses can
   be set. If transport is TCP or UDP, then only one IPv4/IPv6 address or
   domain name can be set. In this case optional port id can also be
   specified.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   LocalAddr ()
             Constructs a new Local address object identifier.
   LocalAddr (java.lang.String[] ipaddr, TransportType tpt_type)
           throws javax.megaco.InvalidArgumentException

             Constructs a new Local address with IPv4/ IPv6 addresses.
   The list of address may be specified in case the transport type is
   M_SCTP_TPT. In case of IPv4 or IPv6 address, the transport type must
   be set to M_UDP_TPT or M_TCP_TPT.
   LocalAddr (java.lang.String addrString, TransportType tpt_type)
           throws javax.megaco.InvalidArgumentException

             Constructs a new Local entity with domain name/address. The
   possible values for transport type could be M_SCTP_TPT, M_UDP_TPT,
   M_TCP_TPT, M_MTP3B_TPT and M_ATM_TPT.
   LocalAddr (java.lang.String aal5Addr)
           throws javax.megaco.InvalidArgumentException

             Constructs a new Local entity with ATM AAL type 5 Address,
   if the signalling is over ATM layer. The transport type in this case
   is set to M_ATM_TPT.

   Method Summary
    void setDomainName(java.lang.String domainName, TransportType
   tpt_type)
           throws javax.megaco.InvalidArgumentException
                     Sets the domain name for the local address. The
   transport type in this case can be set to M_SCTP_TPT, M_UDP_TPT,
   M_TCP_TPT, and M_ATM_TPT. The underlying transport in this case would
   resolve the domain name to actual transport address.
   void setIpAddr(java.lang.String[] ipAddr, TransportType tpt_type)
           throws javax.megaco.InvalidArgumentException

             Sets the IPv4/ IPv6 addresses for the local address. In case
   the transport type is M_SCTP_TPT, the user can specify multiples of IP
   addresses. The other valid values for transport type are M_UDP_TPT and
   M_TCP_TPT.
   void setPortId(int portId)
           throws javax.megaco.InvalidArgumentException

             Sets the local port identity.

                                                                     void

   setMtp3Addr(java.lang.String mtpAddr)
           throws javax.megaco.InvalidArgumentException

              Sets the MTP-3 Address for the local address. This is used
   if the underlying link is over SS7.

                                                                     void

   setAAL5Addr(java.lang.String aal5Addr)
           throws javax.megaco.InvalidArgumentException

        Sets the AAL of type 5 address for the local address when the
   local transport is over ATM cells.
    java.lang.String getDomainName()

             Gets the domain name for the local address.
    java.lang.String[] getIpAddr()

             Gets the vector of IPv4/ IPv6 addresses for the local
   address.
    int  getPortId()
             throwsjavax.megaco.ParameterNotSetException

             Gets the local port identity.
    boolean  isPortIdPresent()

             Specifies if the local port identity is present.
    java.lang.String getMtp3Addr()

              Gets the MTP-3 Address for the local address. This is used
   if the underlying link is over SS7.
    java.lang.String getAAL5Addr()

        Gets the AAL of type 5 address for the local address when the
   local transport is over ATM cells.
    int getTransportType()

             Gets the Transport Type of this Local address. This
   identifies the transport used for MG-MGC communication.
    java.lang.String toString()

             Returns a description of this class

   Methods inherited from class java.lang.Object
   equals, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  LocalAddr

public LocalAddr()

          Constructs a empty Local Address Identifier reference object.
          The parameters would be set to this object using get methods
          defined for this class.
     _________________________________________________________________

  LocalAddr

public LocalAddr(java.lang.String[] ipAddr, TransportType tpt_type)
                     throws javax.megaco.InvalidArgumentException

          Constructs a new Local Address Identifier with IP4/IP6
          addresses. Using this, constructor will automaticaly set the
          IP4/IP6 address in address type. The address specified must be
          in the format
                      IP Token:IP address
                          where
                          IP Token = "IPv4"|"IPv6"
                          IP address = string

             Parameters:

                ipAddr - the IP4/IP6 address. If the transport is over
                IP, then the local transport address for IP can be set
                using this. If the transport is M_SCTP_TPT, then multiple
                local IP addresses could be used for creating the local
                address. If the transport is not SCTP, then depending
                upon port type i.e., UDP or TCP, the transport type will
                be M_UDP_TPT or M_TCP_TPT respectively.
                tpt_type - The transport type identifies the underlying
                transport. This can be set to M_SCTP_TPT, M_UDP_TPTor
                M_TCP_TPT.

        Throws:
                InvalidArgumentException - If the ipAddr string passed in
                the constructor is not as per the format of IPv4 or IPv6
                address, then this exception is raised. This exception is
                also raised if the reference to IP Address or the
                Transport Type passed to this method is NULL.
     _________________________________________________________________

  LocalAddr

public LocalAddr(java.lang.String addrString, TransportType tpt_type)
                     throws javax.megaco.InvalidArgumentException

          Constructs a new Local Address Identifier with domain
          Name/Address. Using this, constructor will automaticaly set the
          domain name in the local address.

        Parameters:
                addrString - the domain name/address. If the transport is
                over IP, then the local transport address for IP can be
                set using this.
                tpt_type - The transport type identifies the underlying
                transport. This can be set to M_SCTP_TPT, M_UDP_TPT,
                M_TCP_TPT, M_MTP3B_TPT or M_ATM_TPT.

        Throws:
                InvalidArgumentException - This exception is raised if
                the reference to address string or the Transport Type
                passed to this method is NULL.
     _________________________________________________________________

  LocalAddr

public LocalAddr(java.lang.String aal5Addr)
                     throws javax.megaco.InvalidArgumentException

          Constructs a new Local entity with ATM AAL type 5 Address, if
          the signalling is over ATM layer. The transport type in this
          case is set to M_ATM_TPT. The string passed for the aal5addr
          must be of the format

                   atm_addr_type : atm_addr
                            where
                                atm_addr_type = "NSAP"|"GWID"|"E164"
                                atm_addr = string

        Parameters:
                aal5Addr - the ATM address. If the transport is over ATM
                cells, then the local transport address can be set using
                this.

        Throws:
                InvalidArgumentException - If the aal5Addr specified is
                not as per the format defined above, then the exception
                is raised. This exception is also raised if the reference
                of aal5Addr string passed to the constructor is NULL.
     _________________________________________________________________

   Method Detail

  setDomainName

public void setDomainName(java.lang.String domainName, TransportType tpt_type)
                     throws javax.megaco.InvalidArgumentException

          Set the domain name of the local address. The transport type
          identifies the underlying transport. This can be set to
          M_SCTP_TPT, M_UDP_TPT, M_TCP_TPT or M_ATM_TPT. The stack in
          this case will resolve the domain name to transport address.


        Parameters:
                domainName - the string indicating the domain name of the
                local address.
                tpt_type - This is the reference to the object of class
                TransportType. This identifies the underlying transport.
                This can be set to M_SCTP_TPT, M_UDP_TPT, M_TCP_TPT or
                M_ATM_TPT.

        Throws:
                InvalidArgumentException - This exception is raised if
                the reference to domain name or the Transport Type passed
                to this method is NULL.
     _________________________________________________________________

  setIpAddr

public void setIpAddr(java.lang.String[] ipAddr, TransportType tpt_type)
            throws InvalidArgumentException

                Sets the IPv4/IPv6 addresses for the local address. The
                traport type identifies whether the underlying transport
                is M_SCTP_TPT, M_UDP_TPT or M_TCP_TPT. The IP address
                specified must be in the format
                            IP Token:IP address
                                where
                                IP Token = "IPv4"|"IPv6"
                                IP address = string


              Parameters:
                      ipAddr - the group of ipAddresses. In case
                      transport is not SCTP, then only one local IP
                      address needs to be specified.
                      tpt_type - This is the reference to the object of
                      class TransportType. This identifies the underlying
                      transport. This can be set to M_SCTP_TPT,
                      M_UDP_TPT, or  M_TCP_TPT.

               Throws:
                      InvalidArgumentException - If the IPv4 address
                      specified is not as per the valid format, then the
                      exception is raised. This exception is also raised
                      if the reference to IP Address string or the
                      Transport Type passed to this method is NULL.
     _________________________________________________________________

  setPortId

public void setPortId(int portId)

               throws InvalidArgumentException     Sets the local port
   number. If the application is Media Gateway (MG) then the port number
   is a mandatory field, and if the application is Media Gateway
   Controller (MGC), then in the absence of this field, the stack would
   use the default value of the port number number as defined by the
   protocol.

   Parameters:
          portId - the local port number.

    Throws:
          InvalidArgumentException - This exception is raised if the port
          number passed to this method is less than or equal to 0.
     _________________________________________________________________

  setMtp3Addr

public void setMtp3Addr(java.lang.String mtpAddr)

                       throws InvalidArgumentException

          Sets the MTP-3 Address for the local entity. This is used if
          the underlying link is over SS7. This method would set the
          transport type to M_MTP3B_TPT. The string passed should be in
          the format
                  pointcode:nw_indicator
                  where
                  pointcode = integer
                  nw_indicator = integer


        Parameters:
                mtpAddr - The string identifying the MTP-3 address.

        Throws:
                InvalidArgumentException - If the mtp3Addr specified is
                not as per the format defined above, then the exception
                is raised.
     _________________________________________________________________

  setAAL5Addr

public void setAAL5Addr(java.lang.String aal5Addr)

                throws InvalidArgumentException

          Sets the AAL of type 5 address for the local entity when the
          local transport is over ATM cells. This method would set the
          transport type to M_ATM_TPT. The string passed should be in the
          format

                   atm_addr_type : atm_addr
                            where
                                atm_addr_type =
                "NSAP"|"GWID"|"E164"|"ALIAS"
                                atm_addr = string

        Parameters:
                The string identifying the AAL5 address.

        Throws:
                InvalidArgumentException - If the aal5Addr specified is
                not as per the format defined above, then the exception
                is raised.
     _________________________________________________________________

  getDomainName

public java.lang.String getDomainName()

          Gets the domain name that has been set for the local entity.

   Returns:
          The domain name as a string value. If no value for domain name
          is present, then this method would return NULL reference.
     _________________________________________________________________

  getIpAddr

public java.lang.String[] getIpAddr()

          Gets the IPv4/IPv6 addresses for the local entity. This method
          would return multiple IP addresses in case the transport type
          is M_SCTP_TPT.

   Returns:
          The vector of string identifying IPv4 address. If no value for
          IP Address is present, then this method would return NULL
          reference.
     _________________________________________________________________

  getPortId

public int getPortId()
            throws ParameterNotSetException

          Gets the local port identity. This function must be invoked
          after verifying that the port number is present using method
          isPortIdPresent().

   Returns:
          The integer value of the local port id.

   Throws:
           ParameterNotSetException - This exception is thrown if port
          number parameter has not yet been set
     _________________________________________________________________

  isPortIdPresent

public boolean isPortIdPresent()

          Method used for checking if local port identity is set.

   Returns:
          True if the local port identity is present.
     _________________________________________________________________

  getMtp3Addr

public java.lang.String getMtp3Addr()

          Gets the MTP-3 Address for the local entity. This is used if
          the underlying link is over SS7. The string returned is in the
          format of the MTP address as discussed earlier. This method
          must be invoked if the transport type specified is M_MTP3B_TPT.


        Returns:
                The string identifying the MTP-3 address. If no value for
                the Mtp3b address is specified, then this method would
                return NULL.
     _________________________________________________________________

  getAAL5Addr

public java.lang.String getAAL5Addr()

          Gets the AAL of type 5 address for the local entity when the
          local transport is over ATM cells. The string returned is in
          the format of the AAL5 address as discussed earlier. This
          method must be invoked if the transport type specified is
          M_ATM_TPT.

        Returns:
                The string identifying the AAL5 address. If no value for
                the AAL5 address is specified, then this method would
                return NULL.
     _________________________________________________________________

  getTransportType

public int getTransportType()

          Gets the Transport type for the local identity. This identifies
          the transport used for MG-MGC communication.


        Returns:
                Reference of the object of the class TransportType. This
                is used to identify the underlying transport type. This
                method would return value 0, if no value for the
                transport type is specified.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns a description of this class


        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Overview Package  Class Use Tree Deprecated Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASSNEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright © 2001 Hughes Software Systems
       HUGHES SOFTWARE SYSTEMS and JAIN JSPA SIGNATORIES PROPRIETARY
        This document contains proprietary information that shall be
     distributed, routed or made available only within Hughes Software
       Systems and JAIN JSPA Signatory Companies, except with written
                   permission of Hughes Software Systems
             _________________________________________________

   22 December 2003


    If you have any comments or queries, please mail them to
    Jmegaco_hss@hss.hns.com
*/
