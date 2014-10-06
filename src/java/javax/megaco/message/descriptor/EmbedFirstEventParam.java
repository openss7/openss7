// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: EmbedFirstEventParam.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:17 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:17 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES  NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class EmbedFirstEventParam

java.lang.Object
   |
   +--javax.megaco.message.descriptor.EmbedFirstEventParam

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class EmbedFirstEventParam

   The class constructs the embedded event descriptor within an event
   descriptor.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   EmbedFirstEventParam(int requestId)
           throws javax.megaco.InvalidArgumentException

              Constructs an Embedded Event Descriptor object. This is a
   parameter to the Requested Event param with the event descriptor. It
   describes the parameters for the emebeded event descriptor.
   EmbedFirstEventParam(int requestId, PkgEventItem eventItem)
       throws javax.megaco.InvalidArgumentException

             Constructs an object of type requested event params which is
   used as a parameter within event descriptor. An event item of the
   megaco package is attached to the current object.
   EmbedFirstEventParam(int requestId, PkgItemStr eventItemStr)
           throws javax.megaco.InvalidArgumentException

              Constructs an object of type requested event params which
   shall be used within the event descriptor. The eventItemStr passed in
   this constructor specifies the package parameters in the string
   format. Implementations may use this method for the classes which are
   not defined in the javax.megaco.pkg package.

   Method Summary

                                                             PkgEventItem

   getMegacoPkgEventItem()
              The method is used to get the event specified in the event
   request. PkgEventItem contains the associated package name, the event
   name and the parameters if any and their corresponding values.

                                                               PkgItemStr

   getMegacoPkgItemStr()
              The method is used to get the package name, event name and
   the associated parameters specified in the event request. This method
   returns package parameters in the string format.

                                                                     void

   setMegacoPkgEventItem(PkgEventItem eventItem)
       throws javax.megaco.InvalidArgumentException

              The method is used to get the event specified in the event
   request. PkgEventItem contains the associated package name, the event
   name and the parameters if any and their corresponding values.

                                                                     void

   setMegacoPkgItemStr(PkgItemStr eventItemStr)
       throws javax.megaco.InvalidArgumentException

              The method is used to get the package name, event name and
   the associated parameters specified in the event request. This method
   returns package parameters in the string format.

                                                                      int

   getRequestIdentifier()
              Returns an integer value that identifies the request
   identifier for the event. This is mandatorily set for the event.
    void setEventParam(EventParam eventParam)
           throws javax.megaco.InvalidArgumentException

              The method is used to set values for embedded signal
   descriptor, the event Digit Map, the "keep active token" and the
   stream id in the first level of the event descriptor. It also sets the
   package name, the event id and the associated parameters.
     EventParam getEventParam()

             The method is used to get values for embedded signal
   descriptor, the event Digit Map and the stream id in the first level
   of the event descriptor.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  EmbedFirstEventParam

public EmbedFirstEventParam(int requestId)
                     throws javax.megaco.InvalidArgumentException

          Constructs a Embedded Event Descriptor with specific request
          identifier.

        Parameters:
                requestId - An integer value specifying the request
                identifier, which uniquely identifies the event.

        Throws:
                InvalidArgumentException - Thrown if request id is set to
                an invalid value.
     _________________________________________________________________

  EmbedFirstEventParam

public EmbedFirstEventParam(int requestId, PkgEventItem eventItem)
                     throws javax.megaco.InvalidArgumentException

          Constructs a Embedded Event Descriptor with specific request
          identifier and the PkgEventItem object. The PkgEventItem object
          contains the package parameters for the event to be detected.

        Parameters:
                requestId - An integer value specifying the request
                identifier, which uniquely identifies the event.
                eventItem - An PkgEventItem object which identifies the
                event to be detected and corresponding package
                parameters.

        Throws:
                InvalidArgumentException - Thrown if request id is set to
                an invalid value or the reference of Package Event Item
                object is set to NULL.
     _________________________________________________________________

  EmbedFirstEventParam

public EmbedFirstEventParam(int requestId, PkgItemStr eventItemStr)
                     throws javax.megaco.InvalidArgumentException

          Constructs a Event Descriptor with specific request identifier
          and the PkgItemStr object. The PkgItemStr object contains the
          package parameters for the event to be detected in the string
          format. This constructor will be used for the MEGACO packages
          which are not part of the javax.megaco.pkg package.

        Parameters:
                requestId - An integer value specifying the request
                identifier, which uniquely identifies the event.
                eventItem - An PkgItemStr object which identifies the
                event to be detected and corresponding package parameters
                in the string format.

        Throws:
                InvalidArgumentException - Thrown if request id is set to
                an invalid value or the reference of Package Item java.lang.String
                object is set to NULL.
     _________________________________________________________________

   Method Detail

  getRequestIdentifier

public int getRequestIdentifier()

          This method returns an integer value corresponding to the
          unique requested event.


        Returns:
                Returns an integer value that identifies request
                identifier which uniquely identifies the event
                descriptor.
     _________________________________________________________________

  getEventParam

public EventParam getEventParam()

          The method can be used the to retrieve the parameters
          corresponding to whether the keep alive token is set, the digit
          map name or digit map value is set or the embeded signal is set
          or stream id is set.

        Returns:
                EventParam - object identifier corresponding to the event
                parameters corresponding to the non embedded event id.
                This object interface may optionally be there. If the
                event parameter is not set then this returns a null
                value.
     _________________________________________________________________

  setEventParam

public void setEventParam(EventParam EventParam)
                     throws javax.megaco.InvalidArgumentException

          The method can be used the to set the object reference to the
          event parameter which has reference to whether keepactive token
          is present, signal descriptor, digit map descriptor stream id.
          In case of an error, an exception is raised.

        Parameter:
                package - The objectIdentifier corresponding to the event
                paramater of first level event.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if an
                illegal event is set in the embedded event descriptor.
     _________________________________________________________________
     _________________________________________________________________

  getMegacoPkgEventItem

public PkgEventItem getMegacoPkgEventItem()

          The method can be used to get the package name in the Embedded
          Event descriptor. This method gives the package information,
          the attached event information and the parameter name and value
          for the event id.


        Returns:
                The object reference for the Event Item. This has the
                object reference of corresponding megaco package and has
                the reference of the parameter info associated with it.
                If the parameter has not been set, then this method shall
                return NULL.
     _________________________________________________________________

  getMegacoPkgItemStr

public PkgItemStr getMegacoPkgItemStr()

          The method can be used to get the pkdgName as set in the
          Embedded Event descriptor. This method gives the package
          information, the attached event information and the parameter
          name and value. Compared to the getMegacoPkgEventItem( )
          method, this method returnes the package parameters in the
          string format.


        Returns:
                The object reference for the megaco package item. This
                class holds information about package name, item name and
                the parameters in the string format. If the parameter has
                not been set, then this method shall return NULL.
     _________________________________________________________________

  setMegacoPkgEventItem

   public void setMegacoPkgEventItem(PkgEventItem eventItem)
       throws javax.megaco.InvalidArgumentException

          The method can be used to set the package name in the Embedded
          Event descriptor. The Package Event Item object contains
          package information, the attached event information and the
          parameter name and value for the event id.


        Parameter:
                eventItem - The object reference for the Event Item. This
                has the object reference of corresponding megaco package
                and has the reference of the parameter info associated
                with it.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Package Event Item passed to this method
                is NULL.
     _________________________________________________________________

  setMegacoPkgItemStr

public void setMegacoPkgItemStr(PkgItemStr eventItemStr)

          The method can be used to set the pkdgName as set in the
          Embedded Event descriptor. The Package Item java.lang.String contains
          package information, the attached event information and the
          parameter name and value. Compared to the
          setMegacoPkgEventItem( ) method, this method takes the package
          parameters in the string format.


        Parameter:
                eventItemStr - The object reference for the megaco
                package item. This class holds information about package
                name, item name and the parameters in the string format.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of package item string passed to this
                method is NULL.
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES  NO FRAMES
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
