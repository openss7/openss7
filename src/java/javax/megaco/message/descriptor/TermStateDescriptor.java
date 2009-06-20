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
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class TermStateDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.TermStateDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class TermStateDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class describes the
   termination state descriptor. It specifies the service state, event
   buffer control and the package property values. Atleaset one of the
   above needs to be specified for the termination state descriptor.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   TermStateDescriptor()
              Constructs a TermState Descriptor object. This extends the
   Descriptor class. It defines the TermState descriptor of the Megaco
   which contains the service state, event buffer control and the package
   property values. Atleast one of the above defined needs to be set in
   the object.

   Method Summary
     int getServiceState()
              Returns a object identifier for the service state. If
   service state is not set, then this shall return a NULL value.
    void setServiceState (ServiceState serviceState)
       throws javax.megaco.InvalidArgumentException

              Sets the object of type ServiceState in a termination state
   descriptor. This shall specify whether the service state is one of out
   of service or in service or test. If service state is not to be sent,
   then this method would not be called.
     int getEvtBufferControl()
              Returns a object identifier for the event buffer control.
   If event buffer control is not set, then this shall return a NULL
   value.
    void setEvtBufferControl (EventBufferCtrl eventBufferControl)
       throws javax.megaco.InvalidArgumentException

              Sets the object of type EventBufferCtrl in a termination
   state descriptor. This shall specify whether the event buffer control
   is one of off or lock step. If event buffer control is not to be sent,
   then this method would not be called.
    PkgPrptyItem[] getMegacoPkgPrptyItem()
              The method is used to get the vector of property param
   within the termination state descriptor. If this is not set then this
   method shall return a NULL value.

                                                             PkgItemStr[]

   getMegacoPkgItemStr()
              The method is used to get the vector of property param
   within the termination state descriptor. If this is not set then this
   method shall return a NULL value. The property param returned have
   package name, item name and associated parameters specified in the
   string format.
    void setMegacoPkgPrptyItem (PkgPrptyItem[] prptyParam)
           throws javax.megaco.InvalidArgumentException

              Sets the vector of type PkgPrptyItem in a termination state
   descriptor. If Megaco package property is not to be sent, then this
   method would not be called.

                                                                     void

   setMegacoPkgItemStr (PkgItemStr[] prptyParam)
           throws javax.megaco.InvalidArgumentException

              Sets the vector of type PkgPrptyItem in a termination state
   descriptor. If Megaco package property is not to be sent, then this
   method would not be called. The property param specified here have
   package name, item name and associated parameters specified in the
   string format.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  TermStateDescriptor

public TermStateDescriptor()

          Constructs a TermState Descriptor which may contain atleast one
          of service state, reserve value, reserve group and property
          param.

   Method Detail

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type
          descriptor TermState. This method overrides the corresponding
          method of the base class Descriptor.


        Returns:
                Returns an integer value that identifies this termination
                state object as the type of termination state descriptor.
                It returns that it is TermState Descriptor i.e.,
                M_TERMINATION_STATE_DESC.
     _________________________________________________________________

  getServiceState

public final int getServiceState()

          This method gets the service state for the termination state
          descriptor. This shall specify one of out of service or in
          service or test. When service state is not present, then this
          shall return value 0.


        Returns:
                Returns the object reference of type service state. If
                the service state is not set then this shall return value
                0.
     _________________________________________________________________

  setServiceState

public final void setServiceState(ServiceState serviceState)
        throws javax.megaco.InvalidArgumentException

          This method sets the service state with one of out of service
          or in service or test. When service state is not to be sent
          then this method would not be invoked.


        Parameter:
                serviceState - Sets the object reference of the derived
                object of ServiceState to specify one of out of service
                or in service or test.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Service State passed to this method is
                NULL.
     _________________________________________________________________

  getEvtBufferControl

public final int getEvtBufferControl()

          This method gets the event buffer control for the termination
          state descriptor. This shall specify one of off or lock step.
          When event buffer control is not present, then this shall
          return value 0.


        Returns:
                Returns the object reference of type event buffer
                control. If the event buffer control is not set then this
                shall return value 0.
     _________________________________________________________________

  setEvtBufferControl

   public final void setEvtBufferControl(EventBufferCtrl
   eventBufferControl)
       throws javax.megaco.InvalidArgumentException

          This method sets the event buffer control with one of off or
          lock step. When event buffer control is not to be sent then
          this method would not be invoked.


        Parameter:
                eventBufferControl - Sets the object reference of the
                derived object of EventBufferCtrl to specify one of off
                or lock step.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Event Buffer Control passed to this
                method is NULL.
     _________________________________________________________________

  getMegacoPkgPrptyItem

public final PkgPrptyItem[] getMegacoPkgPrptyItem()

          The method is used to get the vector of property param within
          the termination state descriptor. If this is not set then this
          method shall return a NULL value.


        Returns:
                Returns the vector of object reference of type
                PkgPrptyItem. If the package property item is not set
                then this shall return a NULL value.
     _________________________________________________________________

  setMegacoPkgPrptyItem

public final void setMegacoPkgPrptyItem(PkgPrptyItem[] prptyParam)
            throws javax.megaco.InvalidArgumentException

          Sets the vector of type PkgPrptyItem in a termination state
          descriptor. If Megaco package property is not to be sent, then
          this method would not be called.


        Parameter:
                prptyParam - The Megaco Property parameter specifying the
                property for the termination in the command.

        Throws:
                InvalidArgumentException if the parameters set for the
                property parameter are such that the TermState Descriptor
                cannot be encoded.
     _________________________________________________________________

  getMegacoPkgItemStr

public final PkgItemStr[] getMegacoPkgItemStr()

          The method is used to get the vector of property param within
          the termination state descriptor. The property param returned
          in this case have package name, item name and associated
          parameters specified in the string format. If this is not set
          then this method shall return a NULL value.


        Returns:
                Returns the vector of object reference of type
                PkgPrptyItem. If the package property item is not set
                then this shall return a NULL value.
     _________________________________________________________________

  setMegacoPkgItemStr

   public final void setMegacoPkgItemStr(PkgItemStr[] prptyParamStr)
       throws javax.megaco.InvalidArgumentException

          Sets the vector of type PkgItemStr in a termination state
          descriptor. If Megaco package property is not to be sent, then
          this method would not be called. This method would invoked for
          the packages which have not been defined in javax.megaco.pkg
          package.


        Parameter:
                prptyParam - The Megaco Property parameter specifying the
                property for the termination in the command.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Package Item string passed to this
                method is NULL.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
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
