// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: AssociationInd.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:15 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:15 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class AssociationInd

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                 |
                 +--javax.megaco.AssociationInd

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class AssociationInd
   extends AssociationEvent

   The class extends JAIN MEGACO Asociation Events. This class represents
   the association indication class that is sent asynchronously by the
   stack to indicate the change in state of the association. The service
   change parameters associated with this class are the parameters which
   caused the change in the state of the association. If the change in
   the association state is beacuse of the message received from peer,
   then service change parameters as same as received in the
   ServiceChange command. Similarly, if the change in the association
   state is due to the Graceful timeout, the service change parameters
   should indicate indicate the parameters when Graceful request was sent
   i.e., ServiceChange method set to GRACEFUL, ServiceChange delay set to
   Graceful timeout.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   AssociationInd (java.lang.Object source, int assocHandle,
   AssocIndReason ind_reason)

       throwsjavax.megaco.InvalidArgumentException

              Constructs an Association Indication Event object.

   Method Summary
    LocalAddr getLocalAddr()
              Returns the local entity transport address. This is the
   current local transport address configured at the stack.
    void setLocalAddr(LocalAddr LocalAddr)
       throws javax.megaco.InvalidArgumentException

              Sets the current value of the local entity id specifying
   the local ip and port number for an IP transport.
    SrvChngReason getSrvChngReason()
              Returns the object reference of the service change reason.
    void setSrvChngReason(SrvChngReason reasonCode)         throws
   javax.megaco.InvalidArgumentException

               Sets the valid value of the service change reason.
    LocalAddr getSrvChngAddress()
              Returns the value of the service change address.
    void setSrvChngAddress(LocalAddr srvChngAddress)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change address.
    LocalAddr getHandOffMGCId()
              Returns the value of the identity of MGC to which the
   association is to be handed off.
    void setHandOffMGCId(LocalAddr handedOffMGCId)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the identity of MGC to which the
   association is to be handed off. This method automatically sets the
   Hand off mgcId present to true.
    AssocState getAssociationState()
              Returns the object reference of the association state. The
   values of the association state are defined in AssocState. The
   association state is to be set mandatorily.
    void setAssociationState(AssocState assocState)
              Sets the valid value of the association state. The
   association state is to be set mandatorily.
    RemoteAddr getRemoteAddr()
              Returns the remote entity transport address which may or
   may not be different from the remote entity configured in the user
   address.
    void setRemoteAddr(RemoteAddr RemoteAddr)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the remote entity id.
    SrvChngMethod getSrvChngMethod()
              Returns the object reference of the service change method.
   The values of the service change method are defined in SrvChngMethod.
    void setSrvChngMethod(SrvChngMethod method)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change method. The
   values of the service change method are defined in SrvChngMethod.
    java.lang.String getSvcChngMethodExtension()
           throws javax.megaco.association.MethodExtensionException

              Returns the string corresponding to the extension
   parameter.
    void setSvcChngMethodExtension(java.lang.String extMethod)
           throws javax.megaco.association.MethodExtensionException,
          javax.megaco.InvalidArgumentException

              Sets the string value of the extended service change
   method.
    java.lang.String getParameterExtension()
              Returns the string corresponding to the extension
   parameter. If the extension parameter is not set then this shall
   return a NULL value.
    void setParameterExtension(java.lang.String extParam)
           throws javax.megaco.InvalidArgumentException

              Sets the string value of the extended parameter for the
   service change received from peer.
    int getSrvChngDelay()
       throws javax.megaco.ParameterNotSetException

              Returns the service change delay.
    void
   setSrvChngDelay(int delay)

       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change delay. This
   method automatically sets the service change delay Present to true.
    boolean isSrvChngDelayPresent()
             Returns true if the service change delay is set.
   int getProtocolVersion()
       throws javax.megaco.ParameterNotSetException

              Returns the protocol version as received in the indication
   from peer.
    void  setProtocolVersion(int version)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the protocol version. This method
   automatically sets the protocol version Present to true.
    boolean isProtocolVersionPresent()
             Returns true if the protocol version is set.
    java.lang.String  getSrvChngProfile()
              Returns the service change profile as received in
   indication from peer. If service change profile is not present then
   this returns a NULL value.
    void  setSrvChngProfile(java.lang.String profile)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change profile.

                                                           AssocIndReason

   getAssocIndReason()
              Get the reference of the association state change
   indication reason.

   Methods inherited from class javax.megaco. AssociationEvent
   getAssocHandle, getAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  AssociationInd

public AssociationInd(java.lang.Object source, int assocHandle, AssocIndReason
ind_reason)
    throws javax.megaco.InvalidArgumentException
     Constructs an Association Indication Event object.

        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.
                ind_reason - This indicates the reason for the change in
                the state of the association.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Association Indication Reason passed to
                this method is NULL.

   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as create association
          request event. This method overrides the corresponding method
          of the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of association indication event. This
                is defined as M_ASSOC_STATE_IND in AssocEventType.
     _________________________________________________________________

  getLocalAddr

public LocalAddr getLocalAddr()

          Gets the local entity transport address. The local address is
          the current value of the local address at the stack.


        Returns:
                Returns the local entity transport address which may or
                may not be different from the local entity configured in
                the user address. This method would return NULL, if no
                value for the local address is configured.
     _________________________________________________________________

  setLocalAddr

   public void setLocalAddr(LocalAddr LocalAddr)
       throws javax.megaco.InvalidArgumentException

          This method sets the local entity transport address. The local
          address is the current value of the local address at the stack.


        Parameter:
                LocalAddr - The local entity transport address which may
                or may not be different from the local entity configured
                in the user address.

        Throws:
                InvalidArgumentException : This exception is raised if
                the parameters passed as arguments is invalid.
     _________________________________________________________________

  getSrvChngReason

public SrvChngReason getSrvChngReason()

          Gets the object reference of service change reason.
          If change in the association state is due to the ServiceChange
          command from peer, the service change reason is same as what is
          received in ServiceChangeDescriptor.


        Returns:
                Returns the object reference corresponding to the service
                change reason. The values of the service change reason
                are defined in SrvChngReason. This method would return
                NULL, if no value for service change reason field is set.
     _________________________________________________________________

  setSrvChngReason

public void setSrvChngReason(SrvChngReason reasonCode)

              throwsjavax.megaco.InvalidArgumentException

          This method sets the service change reason. The values of the
          service change reason are defined in SrvChngReason.


                If change in the association state is due to the
                ServiceChange command from peer, the service change
                reason is same as what is received in
                ServiceChangeDescriptor.


        Parameter:
                The object reference to the corresponding service change
                reason.

        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change Reason passed to this
                method is NULL.
     _________________________________________________________________

  getSrvChngAddress

public LocalAddr getSrvChngAddress()

          Gets the service change address.


                If change in the association state is due to the
                ServiceChange command from peer, the service change
                address is same as what is received in
                ServiceChangeDescriptor.


        Returns:
                Returns the service change address. If the service change
                address field is missing, then this method would return
                NULL.
     _________________________________________________________________

  setSrvChngAddress

   public void setSrvChngAddress(LocalAddr LocalAddr)
       throws javax.megaco.InvalidArgumentException

          This method sets the service change address.


        Parameter:
                The object reference of service change address.

        Throws:
        InvalidArgumentException : This exception is raised if the
                parameters passed as arguments is invalid.
     _________________________________________________________________

  getHandOffMGCId

public LocalAddr getHandOffMGCId()

          Gets the identity of the MGC to which the association is to be
          handoffed.


        Returns:
                Returns the identity of the MGC to which the association
                is to be handoffed. If the handed off MGC Id field is
                missing, then this method would return NULL.
     _________________________________________________________________

  setHandOffMGCId

   public void setHandOffMGCId(LocalAddr LocalAddr)

       throws javax.megaco.InvalidArgumentException

          This method sets the identity of the MGC to which the
          association is to be handoffed.


        Parameter:
                The identity of the MGC to which the association is to be
                handoffed.

        Throws:
        InvalidArgumentException : This exception is raised if the
                parameters passed as arguments is invalid.
     _________________________________________________________________

  getAssociationState

public AssocState getAssociationState()

          Gets the object reference of association state.


        Returns:
                Returns the integer value of association state. The
                values of the association state are defined in
                AssocState. The association state is to be set
                mandatorily. If the assoc state field is missing, then
                this method would return NULL.
     _________________________________________________________________

  setAssociationState

   public void setAssociationState(AssocState associationState)

              throwsjavax.megaco.InvalidArgumentException

          This method sets the association state. The values of the
          association state are defined in AssocState. The association
          state is to be set mandatorily.


        Parameter:
                The object reference of association state.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Association State passed to this method
                is NULL.
     _________________________________________________________________

  getSrvChngMethod

public SrvChngMethod getSrvChngMethod()

          Gets the object reference of service change method.


        Returns:
                Returns the integer value of service change method. The
                values of the service change method are defined in
                SrvChngMethod. The service change method is to be set
                mandatorily. If the service change method field is
                missing, then this method would return NULL.
     _________________________________________________________________

  setSrvChngMethod

public void setSrvChngMethod(SrvChngMethod method)

          throwsjavax.megaco.InvalidArgumentException

          This method sets the service change method. The values of the
          service change method are defined in SrvChngMethod. The service
          change method is to be set mandatorily.


        Parameter:
                The object reference of service change method.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of service change method passed to this
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
                extMethod - The string value of the extended service
                change parameter.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if
                extension string does not follow the rules of the
                extension parameter, e.g, should start with X+ or X- etc.
     _________________________________________________________________

  getRemoteAddr

public RemoteAddr getRemoteAddr()

          Gets the remote entity transport address which may or may not
          be different from the remote entity configured in the user
          address.


        Returns:
                Returns the remote entity transport address which may or
                may not be different from the remote entity configured in
                the user address. If the remote entity Id field is
                missing, then this method would return NULL.
     _________________________________________________________________

  setRemoteAddr

   public void setRemoteAddr(RemoteAddr RemoteAddr)
       throws javax.megaco.InvalidArgumentException

          This method sets the remote entity identity.


        Parameter:
                RemoteAddr - The remote entity transport address which
                may or may not be different from the remote entity
                configured in the user address.

        Throws:
        InvalidArgumentException : This exception is raised if the
                parameters passed as arguments is invalid.
     _________________________________________________________________

  getSrvChngDelay

   public int getSrvChngDelay()
       throws javax.megaco.ParameterNotSetException

          Gets the service change delay value received from peer in the
          service change. This method must be invoked after verifying the
          presence of the service change delay using method
          isSrvChngDelayPresent().


        Returns:
                Returns the service change delay parameter as an integer
                value.

        Throws:
                ParameterNotSetException : This exception is raised if
                the service change delay has not been specified.
     _________________________________________________________________

  setSrvChngDelay

public void setSrvChngDelay(int delay)
        throws javax.megaco.InvalidArgumentException

          This method sets the service change delay value. Shall be used
          by stack.


        Parameter:
                delay - The service change delay as an integer value.

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

  getProtocolVersion

   public int getProtocolVersion()
       throws javax.megaco.ParameterNotSetException

          Gets the protocol version value received from peer in the
          service change. This is ther protocol version after
          negotiation. This method must be invoked after verifying the
          presence of the protocol version field using method
          isProtocolVersionPresent().


        Returns:
                Returns the protocol version parameter as an integer
                value.

        Throws:
        ParameterNotSetException : This exception is raised if the
                protocol version has not been specified.
     _________________________________________________________________

  setProtocolVersion

public void setProtocolVersion(int version)
        throws javax.megaco.InvalidArgumentException

          This method sets the protocol version value. Shall be used by
          stack.


        Parameter:
                version - The protocol version as an integer value.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of protocol version passed to this method is
                less than or equal to 0.
     _________________________________________________________________

  isProtocolVersionPresent

public boolean isProtocolVersionPresent()

          Identifies whether the protocol version is present.


        Returns:
                Returns true if the protocol version is present.
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
                profile - The service change profile as a string value.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of service change profile passed to this
                method is NULL.
     _________________________________________________________________

  getAssocIndReason

public AssocIndReason getAssocIndReason()

          This method returns the reference of the AssocIndReason object
          as set for this class in the constructor.


        Returns:
                Reference of AssocIndReason object.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright (C) 2001 Hughes Software Systems
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
