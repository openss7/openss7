// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: DeleteAssocReq.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:15 $ <p>
 
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
Class DeleteAssocReq

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                 |
                 +--javax.megaco.association.DeleteAssocReq

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class DeleteAssocReq
   extends AssociationEvent

   The class extends JAIN MEGACO Association Events. This represents the
   delete association request event. The event causes the stack initiate
   assoiation teardown procedures with the peer. This would case the
   stack to send a ServiceChange command with specified
   ServiceChangeMethod (e.g. Forced or Graceful) and ServiceChangeReason.
   After completion of the protocol procedure, the association is deleted
   with the peer and can no more be used for exchaging commands with the
   peer.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   DeleteAssocReq (java.lang.Object source, int assocHandle)

             Constructs a Delete Association Event object.

   Method Summary
    LocalAddr getSrvChngAddress()
              Returns the service change address transport address. If
   the service change address is not set, then this shall return a NULL
   value.
    void  setSrvChngAddress(LocalAddr srvChngAddress)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change address.
    LocalAddr getHandOffMGCId()
              Returns the new MGC id to try transport address. If the
   service change MGC Id  is not set, then this shall return a NULL
   value.
    void  setHandOffMGCId(LocalAddr handedOffMGCId)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the new MGC id to try.
    int getSrvChngReason()
              Returns the object reference corresponding to the value of
   the service change reason. If Service Change Reason is not set then
   this shall return value 0.
    void  setSrvChngReason(SrvChngReason reason)
       throws javax.megaco.InvalidArgumentException

              Sets one of the valid values of service change reason as
   defined in the class SrvChngReason.
    int getSrvChngMethod()
             Returns the object reference corresponding to the value of
   the service change method. If Service Change Method is not set then
   this shall return value 0.
    void  setSrvChngMethod(SrvChngMethod method)
       throws javax.megaco.InvalidArgumentException

              Sets one of the valid values of service change method as
   defined in the class SrvChngMethod.
    java.lang.String  getSrvChngMethodExtension()
           throws javax.megaco.association.MethodExtensionException

              Returns the string corresponding to the extension
   parameter.
    void  setSrvChngMethodExtension(java.lang.String extMethod)
           throws javax.megaco.association.MethodExtensionException,
          javax.megaco.InvalidArgumentException

              Sets the string value of the extended service change
   method.
   int  getSrvChngDelay()
       throws javax.megaco.ParameterNotSetException

              Returns the service change delay timing value. This
   represents the time in milliseconds.
    void  setSrvChngDelay(int delay)
       throws javax.megaco.InvalidArgumentException

              Sets the service change delay parameters. This
   automatically sets the service change delay parameter present flag.
   This represents the time in milliseconds.
    boolean isSrvChngDelayPresent()
              Returns true if the service change delay is configured.

   Methods inherited from class javax.megaco. AssociationEvent
   getAssocHandle, getAssocOperIdentifiegetAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  DeleteAssocReq

public DeleteAssocReq(java.lang.Object source, int assocHandle)
    Constructs a Delete Association Request Event object.

        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.

   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as create association
          request event. This method overrides the corresponding method
          of the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of create association request event.
                This is defined as M_DELETE_ASSOC_REQ in AssocEventType.
     _________________________________________________________________

  getSrvChngReason

public int getSrvChngReason()

          Gets the integer value which identifies the service change
          reason. The stack would use the same ServiceChange Reason in
          the ServiceChange command request sent to peer.


        Returns:
                Returns the integer value corresponding to the service
                change reason. If the ServiceChangeReason is not set,
                then this method would return value 0. The possible
                values are field constants defined for the class
                SrvChngReason.
     _________________________________________________________________

  setSrvChngReason

public void setSrvChngReason(SrvChngReason reasonCode)
        throws javax.megaco.InvalidArgumentException

          This method sets the ServiceChangeReason field. The stack would
          use the same ServiceChange Reason in the ServiceChange command
          request sent to peer.


        Parameter:
                The object reference to service change reason.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change Reason passed to this
                method is NULL.
     _________________________________________________________________

  getSrvChngAddress

public LocalAddr getSrvChngAddress()

          Gets the object reference corresponding to the service change
          address. The stack would use the same ServiceChange Address in
          the ServiceChange command request sent to peer.


        Returns:
                Returns the service change address. If the service change
                address has not been specified for this class, then this
                method returns NULL.
     _________________________________________________________________

  setSrvChngAddress

   public void setSrvChngAddress(LocalAddr srvChngAddress)
       throws javax.megaco.InvalidArgumentException

          This method sets the service change address. The stack would
          use the same ServiceChange Address in the ServiceChange command
          request sent to peer.


        Parameter:
                srvChngAddress - The service change address.

        Throws:
                InvalidArgumentException : This exception is raised if
                the argument passed to this method is invalid.
     _________________________________________________________________

  getHandOffMGCId

public LocalAddr getHandOffMGCId()

          Gets the identity of the MGC to which the association is to be
          handoffed. The stack would use the same ServiceChangeMGCId in
          the ServiceChange command request sent to peer.


        Returns:
                Returns the identity of the MGC to which the association
                is to be handoffed. If HandedOffMGCId is not specified
                for this class, then this method returns NULL.
     _________________________________________________________________

  setHandOffMGCId

   public void setHandOffMGCId(LocalAddr handedOffMGCId)
       throws javax.megaco.InvalidArgumentException

          This method sets the identity of the MGC to which the
          association is to be handoffed. This method is valid only if
          the application is MGC. The stack would use the same
          ServiceChangeMGCId in the ServiceChange command request sent to
          peer.


        Parameter:
                handedOffMGCId - The identity of the MGC to which the
                association is to be handoffed.

        Throws:
                InvalidArgumentException : This exception is raised if
                the argument passed to this method is invalid.
     _________________________________________________________________

  getSrvChngMethod

public int getSrvChngMethod()

       Gets the integer value which identifies the service change method.
   The stack would use the same ServiceChange Method in the ServiceChange
   command request sent to peer.

        Returns:
                Returns the integer value corresponding to the service
                change method. If the ServiceChangeMethod is not set,
                then this method would return value 0. The possible
                values are field constants defined for the class
                SrvChngMethod.
     _________________________________________________________________

  setSrvChngMethod

public void setSrvChngMethod(SrvChngMethod method)
        throws javax.megaco.InvalidArgumentException

          This method sets the service change method. The stack would use
          the same ServiceChange Method in the ServiceChange command
          request sent to peer.


        Parameter:
                method - The object reference to service change method.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change Method passed to this
                method is NULL.
     _________________________________________________________________

  getSrvChngMethodExtension

public java.lang.String getSrvChngMethodExtension()
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

  setSrvChngMethodExtension

public void setSrvChngMethod(java.lang.String extMethod)
      throws javax.megaco.association.MethodExtensionException,
             javax.megaco.InvalidArgumentException

          This method sets the extended service change method. This needs
          to be set if and only if the service change method is
          M_SVC_CHNG_METHOD_EXTENSION.


        Parameter:
                extMethod - The string value of the extended service
                change method.

        Throws:
                javax.megaco.association.MethodExtensionException -
                Thrown if service change method has not been set to
                M_SVC_CHNG_METHOD_EXTENSION.
                javax.megaco.InvalidArgumentException - Thrown if
                extension string does not follow the rules of the
                extension parameter, e.g, should start with X+ or X- etc.
     _________________________________________________________________

  getSrvChngDelay

   public LocalAddr getSrvChngDelay()
       throws javax.megaco.ParameterNotSetException

          Gets the integer value of the delay parameter for the service
          change. This is in milliseconds. This method must be invoked
          after invoking the isSrvChngdelayPresent() method. The stack
          would use the same as the ServiceChangeDelay in the
          ServiceChange command request sent to peer.


        Returns:
                Returns the integer value of the delay value in
                milliseconds.

        Throws:
                ParameterNotSetException: This exception is raised if the
                service change delay parameter has not been set.
     _________________________________________________________________

  setSrvChngDelay

public void setSrvChngDelay(int delay)
        throws javax.megaco.InvalidArgumentException

          Sets the integer value of the delay parameter for the service
          change. This is in milliseconds. This automatically sets the
          service change delay value to be present. The stack would use
          the same as the ServiceChangeDelay in the ServiceChange command
          request sent to peer.


        Parameter:
                delay - The integer value of the delay value in
                milliseconds.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of service change delay passed to this method
                is less than 0.
     _________________________________________________________________

  isSrvChngDelayPresent

public boolean isSrvChngDelayPresent()

          Returns TRUE if the service change delay parameter is present.


        Returns:
                TRUE if service change delay parameter has been set, else
                returns FALSE.
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
