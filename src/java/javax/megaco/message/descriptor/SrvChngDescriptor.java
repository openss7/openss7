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

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class SrvChngDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.SrvChngDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class SrvChngDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class represents the
   service change descriptor defined in the MEGACO protocol. This
   descriptor describes the service change method, reason, delay,
   address, profile, mgcid, version, the timestamp and any extension to
   it. Among these the service change method is mandatory.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   SrvChngDescriptor()
              Constructs a SrvChng Descriptor object. This extends the
   Descriptor class.

   Method Summary
    int getSrvChngMethod()

              Returns the object reference of the service change method.
   The values of the service change method are defined in SrvChngMethod.
   The SrvChngMethod clas is defined in the association package.
    void  setSrvChngMethod (SrvChngMethod method)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change method. The
   values of the service change method are defined in SrvChngMethod. The
   SrvChngMethod clas is defined in the association package.
    java.lang.String  getSvcChngMethodExtension()
        throws javax.megaco.association.MethodExtensionException

              Returns the string corresponding to the extension
   parameter.
    void  setSvcChngMethodExtension(java.lang.String extMethod)
           throws javax.megaco.association.MethodExtensionException,
          javax.megaco.InvalidArgumentException

              Sets the string value of the extended service change
   method.
    int getSrvChngReason()
              Returns the object reference of the service change reason.
    void  setSrvChngReason (SrvChngReason reasonCode)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change reason.
    int getSrvChngDelay()
           throws javax.megaco.ParameterNotSetException,

              Returns the service change delay set in association
   indication from peer.
    void  setSrvChngDelay(int delay)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change delay. This
   method automatically sets the service change delay Present to true.
    boolean isSrvChngDelayPresent()
             Returns true if the service change delay is set.
    LocalAddr getSrvChngAddress()
              Returns the value of the service change address.
    void  setSrvChngAddress (LocalAddr LocalAddr)
           throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change address.
    java.lang.String  getSrvChngProfile()
              Returns the service change profile set in association
   indication from peer. If service change profile is not present then
   this returns a NULL value.
    void  setSrvChngProfile(java.lang.String profile)
           throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change profile.
    LocalAddr getHandOffMGCId()
              Returns the value of the identity of MGC to which the
   association is to be handed off.
    void  setHandOffMGCId(LocalAddr LocalAddr)
           throws javax.megaco.InvalidArgumentException

              Sets the valid value of the identity of MGC to which the
   association is to be handed off.
    int  getProtocolVersion()
           throws javax.megaco.ParameterNotSetException,

              Returns the protocol version set in association indication
   from peer.
    void  setProtocolVersion(int version)
           throws javax.megaco.InvalidArgumentException

              Sets the valid value of the protocol version. This method
   automatically sets the protocol version Present to true.
    boolean isProtocolVersionPresent()
             Returns true if the protocol version is set.
    java.lang.String getDateValue()
              Returns the string corresponding to the date in timestamp.
   If the date in timestamp is not set then this shall return a NULL
   value.
    void setDateValue(java.lang.String dateValue)
           throws javax.megaco.InvalidArgumentException

              Sets the string value of the date in timestamp for the
   service change.
    java.lang.String getTimeValue()
              Returns the string corresponding to the time in timestamp.
   If the time in timestamp is not set then this shall return a NULL
   value.
    void setTimeValue(java.lang.String timeValue)
           throws javax.megaco.InvalidArgumentException

              Sets the string value of the time in timestamp for the
   service change.
    java.lang.String getParameterExtension()
              Returns the string corresponding to the extension
   parameter. If the extension parameter is not set then this shall
   return a NULL value.
    void setParameterExtension(java.lang.String extParam)
           throws javax.megaco.InvalidArgumentException

              Sets the string value of the extended parameter for the
   service change.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId,

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  SrvChngDescriptor

public SrvChngDescriptor()

          Constructs a SrvChng Descriptor. The service change descriptor
          class contain service change method and atleast one of service
          change method, reason, delay, address, profile, mgcid, version,
          the timestamp and any extension parameter.

   Method Detail

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type
          descriptor SrvChng. This method overrides the corresponding
          method of the base class Descriptor.


        Returns:
                Returns an integer value that identifies this service
                change object as the type of service change descriptor.
                It returns the the value M_SERVICE_CHANGE_DESC of a
                Service Change Descriptor.
     _________________________________________________________________

  getSrvChngMethod

public int getSrvChngMethod()

          Gets the integer value which identifies the service change
          method.


        Returns:
                Returns the integer value of service change method. The
                values of the service change method are defined in
                SrvChngMethod. The service change method is to be
                mandatorily set. If Service Change method is not set then
                this method would return value 0.
     _________________________________________________________________

  setSrvChngMethod

   public void setSrvChngMethod(SrvChngMethod method)
       throws javax.megaco.InvalidArgumentException

          This method sets the service change method. The values of the
          service change method are defined in SrvChngMethod. The service
          change method is to be mandatorily set.


        Parameter:
                The object reference of service change method.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change Method passed to this
                method is NULL.
     _________________________________________________________________

  getSvcChngMethodExtension

public java.lang.String getSvcChngMethodExtension()
      throws javax.megaco.association.MethodExtensionException

          Gets the string value of the extended service change method.


        Returns:
                Returns string value of the extended service change
                method. This is to be set only if the service change
                method is set to M_SVC_CHNG_METHOD_EXTENSION.

        Throws:
                javax.megaco.association.MethodExtensionException -
                Thrown if service change method has not been set to
                M_SVC_CHNG_METHOD_EXTENSION.
     _________________________________________________________________

  setSvcChngMethodExtension

public void setSvcChngMethodExtension(java.lang.String extMethod)
      throws javax.megaco.association.MethodExtensionException,
             javax.megaco.InvalidArgumentException

          This method sets the extended service change method. This needs
          to be set if and only if the service change method is
          M_SVC_CHNG_METHOD_EXTENSION.


        Parameter:
                The string value of the extended service change method.

        Throws:
                javax.megaco.association.MethodExtensionException -
                Thrown if service change method has not been set to
                M_SVC_CHNG_METHOD_EXTENSION.
                javax.megaco.InvalidArgumentException - Thrown if
                extension string does not follow the rules of the
                extension parameter, e.g, should start with X+ or X- etc.
     _________________________________________________________________

  getSrvChngReason

public int getSrvChngReason()

          Gets the integer value which identifies the service change
          reason.


        Returns:
                Returns the object reference corresponding to the service
                change reason. The values of the service change reason
                are defined in SrvChngReason. If the ServiceChangeReason
                is not set, then this method would return value 0.
     _________________________________________________________________

  setSrvChngReason

   public void setSrvChngReason(SrvChngReason reasonCode)
       throws javax.megaco.InvalidArgumentException

          This method sets the service change reason. The values of the
          service change reason are defined in SrvChngReason.


        Parameter:
                The object reference to the corresponding service change
                reason.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if the
                reference of ServiceChange reason passed to this method
                is NULL.
     _________________________________________________________________

  getSrvChngDelay

public int getSrvChngDelay()
      throws javax.megaco.ParameterNotSetException

          Gets the service change delay value received from peer in the
          service change.


        Returns:
                Returns the service change delay parameter as an integer
                value.

        Throws:
                javax.megaco.ParameterNotSetException - Thrown if service
                change delay parameter has not been set.
     _________________________________________________________________

  setSrvChngDelay

   public void setSrvChngDelay(int delay)
       throws javax.megaco.InvalidArgumentException

          This method sets the service change delay value.


        Parameter:
                The service change delay as an integer value.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of service change delay passed to this method
                is less than 0.
     _________________________________________________________________

  isSrvChngDelayPresent

public boolean isSrvChngDelayPresent()

          Identifies whether the service change delay is present.


        Returns:
                Returns true if the service change delay is present.
     _________________________________________________________________

  getSrvChngAddress

public LocalAddr getSrvChngAddress()

          Gets the service change address. If the service change command
          has been received from peer, then it refers to the the new
          transport address the peer intends to use subsequently.


        Returns:
                Returns the service change address. If the service change
                address is missing, then the method would return NULL.
     _________________________________________________________________

  setSrvChngAddress

public void setSrvChngAddress(LocalAddr LocalAddr)
         throws javax.megaco.InvalidArgumentException

          This method sets the service change address. This is not a
          mandatory parameter.


        Parameter:
                The object reference of service change address.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if a
                parameter in service change address is set such that it
                is invalid.
     _________________________________________________________________

  getSrvChngProfile

public java.lang.String getSrvChngProfile()

          Gets the service change profile value received from peer in the
          service change.


        Returns:
                Returns the service change profile parameter as a string
                value. If the service change profile is not set then it
                returns a NULL value.
     _________________________________________________________________

  setSrvChngProfile

public void setSrvChngProfile(java.lang.String profile)
      throws javax.megaco.InvalidArgumentException

          This method sets the service change profile value.


        Parameter:
                The service change profile as a string value.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if service
                change profile parameter has invalid format.
     _________________________________________________________________

  getHandOffMGCId

public LocalAddr getHandOffMGCId()

          Gets the identity of the MGC to which the association is to be
          handoffed.
          As specified in the protocol, in case of failure of MGC, it
          would handoff the control of the MG to the new MGC. This is
          conveyed using service change command on ROOT termination, with
          service change method set to M_SVC_CHNG_METHOD_HANDOFF and
          transport parameters of the new MGC specified in the mgcidToTry
          field of the service change descriptor. This mgcidToTry field
          of the service change descriptor is represented using
          HandedOffMGCId field on this class.


        Returns:
                Returns the identity of the MGC to which the association
                is to be handoffed.
     _________________________________________________________________

  setHandOffMGCId

public void setHandOffMGCId(LocalAddr mgcidToTry)
         throws javax.megaco.InvalidArgumentException

          This method sets the identity of the MGC to which the
          association is to be handoffed.

          As specified in the protocol, in case of failure of MGC, it
          would handoff the control of the MG to the new MGC. This is
          conveyed using service change command on ROOT termination, with
          service change method set to M_SVC_CHNG_METHOD_HANDOFF and
          transport parameters of the new MGC specified in the mgcidToTry
          field of the service change descriptor. This mgcidToTry field
          of the service change descriptor is represented using
          HandedOffMGCId field on this class.


        Parameter:
                The identity of the MGC to which the association is to be
                handoffed.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if a
                parameter in mgc Id is set such that it is invalid.
     _________________________________________________________________

  getProtocolVersion

public int getProtocolVersion()
      throws javax.megaco.ParameterNotSetException

          Gets the protocol version value received from peer in the
          service change. This is ther protocol version after
          negotiation.


        Returns:
                Returns the protocol version parameter as an integer
                value.

        Throws:
                javax.megaco.ParameterNotSetException - Thrown if service
                change version parameter has not been set.
     _________________________________________________________________

  setProtocolVersion

public void setProtocolVersion(int version)
      throws javax.megaco.InvalidArgumentException

          This method sets the protocol version value.


        Parameter:
                The protocol version as an integer value.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if service
                change version parameter is less than or equal to 0.
     _________________________________________________________________

  isProtocolVersionPresent

public boolean isProtocolVersionPresent()

          Identifies whether the protocol version is present.


        Returns:
                Returns true if the protocol version is present.
     _________________________________________________________________

  getDateValue

public java.lang.String getDateValue()

          Gets the string value of the date value of timestamp in service
          change descriptor.


        Returns:
                Returns string value of the date value of timestamp in
                service change. If the date value of timestamp in service
                change is not set then a NULL value is returned.
     _________________________________________________________________

  setDateValue

public void setDateValue(java.lang.String dateValue)
      throws javax.megaco.InvalidArgumentException

          This method sets the date value of timestamp in service change
          descriptor.


        Parameter:
                The string value of the date value of timestamp in
                service change descriptor.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if date
                value of timestamp in service change descriptor is not of
                8 digit length.
     _________________________________________________________________

  getTimeValue

public java.lang.String getTimeValue()

          Gets the string value of the time value of timestamp in service
          change descriptor.


        Returns:
                Returns string value of the time value of timestamp in
                service change descriptor. If the time value of timestamp
                in service change descriptor is not set then a NULL value
                is returned.
     _________________________________________________________________

  setTimeValue

public void setTimeValue(java.lang.String timeValue)
      throws javax.megaco.InvalidArgumentException

          This method sets the time value of timestamp in service change
          descriptor.


        Parameter:
                The string value of the time value of timestamp in
                service change descriptor.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if time
                value of timestamp in service change descriptor is not of
                8 digit length.
     _________________________________________________________________

  getParameterExtension

public java.lang.String getParameterExtension()

          Gets the string value of the extended service change parameter.


        Returns:
                Returns string value of the extended service change
                parameter. If the service change parameter is not set
                then this a NULL value is returned.
     _________________________________________________________________

  setParameterExtension

public void setParameterExtention(java.lang.String extMethod)
      throws javax.megaco.InvalidArgumentException

          This method sets the extended service change parameter.


        Parameter:
                The string value of the extended service change
                parameter.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if
                extension string does not follow the rules of the
                extension parameter, e.g, should start with X+ or X- etc.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
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
