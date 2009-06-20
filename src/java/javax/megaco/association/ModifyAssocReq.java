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
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class ModifyAssocReq

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                    |
                    +--javax.megaco.association.ModifyAssocReq

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class ModifyAssocReq
   extends AssociationEvent

   The class extends JAIN MEGACO Association Events. This represents the
   modify association request event. This is used by the application in
   requesting the stack to modify the association.



   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   ModifyAssocReq (java.lang.Object source, int assocHandle)
           throws javax.megaco.InvalidArgumentException

             Constructs a Modify Association Event object.

   Method Summary
   LocalAddr getSrvChngAddress()
              Returns the service change address transport address which
   may or may not be different from the local entity configured in the
   user address or during create association. This would be the parameter
   sent in service change address.
    void setSrvChngAddress(LocalAddr srvChngAddress)
       throws javax.megaco.InvalidArgumentException

              Sets the valid value of the service change address.
    int getSrvChngReason()
              Returns the object reference corresponding to the value of
   the service change reason. If Service Change Reason is not set then
   this shall return value 0.
   void setSrvChngReason(SrvChngReason reason)
       throws javax.megaco.InvalidArgumentException

              Sets one of the valid values of service change reason as
   defined in the class SrvChngReason.
    int getSrvChngMethod()
               Returns the object reference corresponding to the value of
   the service change method. If Service Change Method is not set then
   this shall return value 0.
    void setSrvChngMethod(SrvChngMethod method)
       throws javax.megaco.InvalidArgumentException

              Sets one of the valid values of service change method as
   defined in the class SrvChngMethod.
    java.lang.String  getSrvChngMethodExtension()
           throws javax.megaco.association.MethodExtensionException

              Returns the string corresponding to the extension
   parameter.
   void setSrvChngMethodExtension(java.lang.String extMethod)
           throws javax.megaco.association.MethodExtensionException,
          javax.megaco.InvalidArgumentException

              Sets the string value of the extended service change
   method.

   Methods inherited from class javax.megaco. AssociationEvent
   getAssocHandle, getAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  ModifyAssocReq

public ModifyAssocReq(java.lang.Object source, int assocHandle)
                     throws javax.megaco.InvalidArgumentException

          Constructs a Modify Association Request Event object.


        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.

        Throws:
                InvalidArgumentException if the parameters send in the
                constructor are invalid.

   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as create association
          request event. This method overrides the corresponding method
          of the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of create association request event.
                This is defined as M_MODIFY_ASSOC_REQ in AssocEventType.
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

          This method sets the service change reason. The stack would use
          the same ServiceChange Reason in the ServiceChange command
          request sent to peer.


        Parameter:
                reasonCode - The object reference to service change
                reason.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change Reason passed to this
                method is NULL.
     _________________________________________________________________

  getSrvChngAddress

public LocalAddr getSrvChngAddress()

          Gets the service change address. If the service change address
          field is missing, then this method would return NULL.


        Returns:
                Returns the service change address.
     _________________________________________________________________

  setSrvChngAddress

   public void setSrvChngAddress(LocalAddr srvChngAddress)
       throws javax.megaco.InvalidArgumentException

          This method sets the service change address. The stack would
          use the same ServiceChange Address in ServiceChange command
          request sent to peer.


        Parameter:
                srvChngAddress - The service change address.

        Throws:
                InvalidArgumentException : This exception is raised if
                the specified service change address is invalid.
     _________________________________________________________________

  getSrvChngMethod

public inr getSrvChngMethod()

       Gets the integer value which identifies the service change method.
   The stack would use the same ServiceChange Method in ServiceChange
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
          the same ServiceChange Method in ServiceChange command request
          sent to peer.


        Parameter:
                method - The object reference to service change method.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service Change method passed to this
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

public void setSrvChngMethodExtension(java.lang.String extMethod)
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
