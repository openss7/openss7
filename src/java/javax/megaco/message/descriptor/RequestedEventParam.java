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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class RequestedEventParam

java.lang.Object
  |
  +--javax.megaco.message.descriptor.RequestedEventParam

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class RequestedEventParam
   extends java.lang.Object

   The RequestedEventParam object is a class that shall be used to set
   the event params within the event descriptor. The class optionally
   provides interface to specify the package name, item name and the
   associated parameters in the string format. This is an independent
   class derived from java.util.Object and shall not have any derived
   classes.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   RequestedEventParam(PkgEventItem eventItem)
       throws javax.megaco.InvalidArgumentException

             Constructs an object of type requested event params which is
   used as a parameter within event descriptor. An event item of the
   megaco package is attached to the current object.
   RequestedEventParam( PkgItemStr eventItemStr)
           throws javax.megaco.InvalidArgumentException

              Constructs an object of type requested event params which
   shall be used within the event descriptor. The eventItemStr passed in
   this constructor specifies the package parameters in the string
   format. Implementations may use this method for the classes which are
   not defined in the javax.megaco.pkg package.

   Method Summary
     EmbedFirstEventParam getEmbedFirstEventParam()
              The method gives the object identifier of the embeded
   first.
   final   void  setEmbedFirstEventParam( EmbedFirstEventParam
   embedFirst)
       throws javax.megaco.InvalidArgumentException

             The method can be used to set the embedding events, their
   request id and the related parameters to the embedded event.
     EventParam getEventParam()

             The method is used to get values for embedded signal
   descriptor, the event Digit Map and the stream id in the first level
   of the event descriptor. It also sets the package name, the event id
   and the associated parameters.
    void  setEventParam(EventParam eventParam)
           throws javax.megaco.InvalidArgumentException

              The method is used to set values for embedded signal
   descriptor, the event Digit Map, the "keep active token" and the
   stream id in the first level of the event descriptor. It also sets the
   package name, the event id and the associated parameters.

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
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  RequestedEventParam

public RequestedEventParam(PkgEventItem eventItem)
                     throws javax.megaco.InvalidArgumentException

          Constructs Requested event parameter object with the
          PkgEventItem object. This is used to set the first level event
          parameters.

   Throws:
          InvalidArgumentException - Thrown if an invalid eventItem
          object reference is set.
     _________________________________________________________________

  RequestedEventParam

public RequestedEventParam(PkgItemStr eventItemStr)
                     throws javax.megaco.InvalidArgumentException

          Constructs Requested event parameter object with the PkgItemStr
          object. This method would be used if the package parameters are
          to be conveyed in the string format. This is used to set the
          first level event parameters.

   Throws:
          InvalidArgumentException - Thrown if an invalid eventItemStr
          object reference is set.
     _________________________________________________________________

   Method Detail

  getEmbedFirstEventParam

public EmbedFirstEventParam getEmbedFirstEventParam()

          The method can be used the to retrieve the embedded event
          information.

        Returns:
                embedFirst - The object identifier corresponding to the
                embedded first. This object interface may optionally be
                there. If emberFirstEventParam is not set, then this
                method would return NULL.
     _________________________________________________________________

  setEmbedFirstEventParam

   public  void setEmbedFirstEventParam(EmbedFirstEventParam embedEvent)
       throws javax.megaco.InvalidArgumentException

          The method can be used the to set the object reference to the
          embeded event which has reference to the its event parameters,
          request id, and its package object. This method validates
          whether the package name, event id and the parameter can be set
          for the embedded event descriptor. In case of an error, an
          exception is raised.

        Parameter:
                embedEvent - The objectIdentifier corresponding to the
                embedded event descriptor.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of embed first event parameter passed to
                this method is NULL.
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
                event parameter is not set then this returns a NULL
                value.
     _________________________________________________________________

  setEventParam

public  void setEventParam(EventParam eventParam)
                     throws javax.megaco.InvalidArgumentException

          The method can be used the to set the object reference to the
          event parameter which has reference to whether keepactive token
          is present, signal descriptor, digit map descriptor stream id.
          In case of an error, an exception is raised.

        Parameter:
                eventParam - The objectIdentifier corresponding to the
                event paramater of first level event.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if an
                illegal event is set in the embedded event descriptor.
     _________________________________________________________________

  getMegacoPkgEventItem

public PkgEventItem getMegacoPkgEventItem()

          The method can be used to get the package name in the Event
          descriptor. This method gives the package information, the
          attached event information and the parameter name and value for
          the event id.

        Returns:
                package - The object reference for the Event Item. This
                has the object reference of corresponding megaco package
                and has the reference of the parameter info associated
                with it. If the parameter has not been set, then this
                method shall return NULL.
     _________________________________________________________________

  getMegacoPkgItemStr

public PkgItemStr getMegacoPkgItemStr()

          The method can be used to get the pkdgName as set in the Event
          descriptor. This method gives the package information, the
          attached event information and the parameter name and value.
          Compared to the  getMegacoPkgEventItem() method, this method
          returnes the package parameters in the string format.

        Returns:
                The object reference for the megaco package item. This
                class holds information about package name, item name and
                the parameters in the string format. If the parameter has
                not been set, then this method shall return NULL.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().


        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
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
