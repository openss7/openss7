// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: LocalCtrlDescriptor.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:18 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:18 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class LocalCtrlDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.LocalCtrlDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class LocalCtrlDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class describes the
   local control descriptor. It specifies the stream mode, value of
   reserve group/ reserve value and the package property values.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   LocalCtrlDescriptor()
              Constructs a LclCtrl Descriptor object. This extends the
   Descriptor class. It defines the LclCtrl descriptor of the Megaco
   which contains the stream mode the reserve group, reserve value and
   the package property. Atleast one of the above defined needs to be set
   in the object.

   Method Summary
     int getStreamMode()
              Returns a integer value which identifies the stream mode.
   If stream mode is not set, then this shall return value 0.
    void  setStreamMode (StreamMode streamMode)
       throws javax.megaco.InvalidArgumentException

              Sets the object of type StreamMode in a local control
   descriptor. This shall specify whether the stream mode is one of send
   only or receive only or send receive or inactive or loopback.
    boolean  isReserveValuePresent()
              Specifies if reserve value is set or not.
    boolean  getReserveValue()
              Specifies if reserve value is True or False. If the reserve
   value was not set then this shall return the default value - False.
    void  setReserveValue(boolean reserveValue)
              Sets the reserve value to be True or False.
    boolean  isReserveGroupPresent()
              Specifies if reserve group is set or not.
    boolean  getReserveGroup()
              Specifies if reserve group is True or False. If the reserve
   group was not set then this shall return the default value - False.
    void  setReserveGroup(boolean reserveGroup)
              Sets the reserve group to be True or False.
    PkgPrptyItem[] getMegacoPkgPrptyItem()
              The method is used to get the vector of property param
   within the local control descriptor. If this is not set then this
   method shall return a NULL value.
    void  setMegacoPkgPrptyItem (PkgPrptyItem[] prptyParam)
           throws javax.megaco.InvalidArgumentException

              Sets the vector of type PkgPrptyItem in a local control
   descriptor. If Megaco package property is not to be sent, then this
   method would not be called.

                                                             PkgItemStr[]

   getMegacoPkgItemStr()
              The method is used to get the vector of property param
   within the local control descriptor. The property param returned have
   package name, item name and associated parameters specified in the
   string format. If this is not set then this method shall return a NULL
   value.

                                                                     void

   setMegacoPkgItemStr (PkgItemStr[] prptyParamStr)    throws
   javax.megaco.InvalidArgumentException

              Sets the vector of type PkgItemStr in a local control
   descriptor. If Megaco package property is not to be sent, then this
   method would not be called. The property param specified here have
   package name, item name and associated parameters specified in the
   string format.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  LocalCtrlDescriptor

public LocalCtrlDescriptor()

          Constructs a LclCtrl Descriptor object. The object may contain
          atleast one of stream mode, reserve value, reserve group and
          property param.

   Method Detail

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type
          descriptor LclCtrl. This method overrides the corresponding
          method of the base class Descriptor.


        Returns:
                Returns an integer value that identifies this local
                control object as the type of local control descriptor.
                It returns that it is LclCtrl Descriptor i.e.,
                M_LOCAL_CONTROL_DESC.
     _________________________________________________________________

  getStreamMode

public final int getStreamMode()

          This method gets the Stream mode for the local control
          descriptor. This shall specify one of send only or receive only
          or send receive or inactive or loopback. When stream mode is
          not present, then this shall return value 0.


        Returns:
                Returns the integer value which identifies the stream
                mode. If the stream mode is not set then this shall
                return value 0. The possible values are field constants
                defined in the class StreamMode.
     _________________________________________________________________

  setStreamMode

public final void setStreamMode(StreamMode streamMode)
        throws javax.megaco.InvalidArgumentException

          This method sets the stream mode with one of send ronly or
          receive only or send receive or inactive or loopback. When
          stream mode is not to be sent then this method would not be
          invoked.


        Parameter:
                streamMode - Sets the object reference of the derived
                object of StreamMode to specify one of send only or
                receive only or send receive or inactive or loopback.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Stream Mode passed to this method is
                NULL.
     _________________________________________________________________

  isReserveValuePresent

public final boolean isReserveValuePresent()

          Specifies if reserve value is set or not.


        Returns:
                Returns TRUE if the reserve value was set.
     _________________________________________________________________

  getReserveValue

public final boolean getReserveValue()

          Specifies if reserve value is True or False. If the reserve
          value was not set then this shall return the default value -
          False. Thus application can call this method without checking
          for whether the reserve value was set or not, but the stack
          should always call this method only after verifying that the
          reserve value was set.


        Returns:
                Returns TRUE if the reserve value was set to ON else
                shall return False.
     _________________________________________________________________

  setReserveValue

public final void setReserveValue(boolean reserveValue)

          Sets the reserve value to be True or False. If application does
          not call this method then the corresponding parameter would not
          be sent to the peer.


        Parameters:
                reserveValue - Takes a value TRUE if the reserve value is
                to be set to ON and FALSE if reserve value is to be set
                to False.
     _________________________________________________________________

  isReserveGroupPresent

public final boolean isReserveGroupPresent()

          Specifies if reserve group is set or not.


        Returns:
                Returns TRUE if the reserve group was set.
     _________________________________________________________________

  getReserveGroup

public final boolean getReserveGroup()

          Specifies if reserve group is True or False. If the reserve
          group was not set then this shall return the default value -
          False. Thus application can call this method without checking
          for whether the reserve group was set or not, but the stack
          should always call this method only after verifying that the
          reserve group was set.


        Returns:
                Returns TRUE if the reserve group was set to ON else
                shall return False.
     _________________________________________________________________

  setReserveGroup

public final void setReserveGroup(boolean reserveGroup)

          Sets the reserve group to be True or False. If application does
          not call this method then the corresponding parameter would not
          be sent to the peer.


        Parameters:
                reserveGroup - Takes a value TRUE if the reserve group is
                to be set to ON and FALSE if reserve group is to be set
                to False.
     _________________________________________________________________

  getMegacoPkgPrptyItem

public final PkgPrptyItem[] getMegacoPkgPrptyItem()

          The method is used to get the vector of property param within
          the local control descriptor. If this is not set then this
          method shall return a NULL value.


        Returns:
                Returns the vector of object reference of type
                PkgPrptyItem. If the package property item is not set
                then this shall return a NULL value.
     _________________________________________________________________

  setMegacoPkgPrptyItem

public final void setMegacoPkgPrptyItem(PkgPrptyItem[] prptyParam)
            throws javax.megaco.InvalidArgumentException

          Sets the vector of type PkgPrptyItem in a local control
          descriptor. If Megaco package property is not to be sent, then
          this method would not be called.


        Parameter:
                prptyParam - The Megaco Property parameter specifying the
                property for the termination in the command.

        Throws:
                InvalidArgumentException if the parameters set for the
                property parameter are such that the LclCtrl Descriptor
                cannot be encoded.
     _________________________________________________________________

  getMegacoPkgItemStr

public final PkgItemStr[] getMegacoPkgItemStr()

          The method is used to get the vector of property param within
          the local control descriptor. The property param returned in
          this case have package name, item name and associated
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

          Sets the vector of type PkgItemStr in a local control
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
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
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
