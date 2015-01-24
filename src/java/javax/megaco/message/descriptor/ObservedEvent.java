// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/descriptor/ObservedEvent.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES  NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class ObservedEvent

java.lang.Object
  |
  +--javax.megaco.message.descriptor.ObservedEvent

   All Implemented Interfaces:
          java.io.Serializable

   See Also:
          Serialized Form
     _________________________________________________________________

   public class ObservedEvent
   extends java.lang.Object

   The ObservedEvent object is a class that shall be used to set the
   observed events and the stream id in which it was observed along with
   optionally the time at which it was detected. The class optionally
   provides interface to specify the package name, item name and the
   associated parameters in the string format. This is an independent
   class derived from java.util.Object and shall not have any derived
   classes.
     _________________________________________________________________

   Constructor Summary
   ObservedEvent (PkgEventItem eventItem)
           throws javax.megaco.InvalidArgumentException

              Constructs an object of type event parameter which shall be
   used within the observed event descriptor.
   ObservedEvent (PkgItemStr eventItemStr)
           throws javax.megaco.InvalidArgumentException

              Constructs an object of type event parameter which shall be
   used within the observed event descriptor. The eventItemStr passed in
   this constructor specifies the package parameters in the string
   format. implementations may use this method for the classes which are
   not defined in the javax.megaco.pkg package.

   Method Summary
    PkgEventItem getMegacoPkgEvtItem()
              The method is used to get the pkgdName and eventOther in
   the observed event descriptor.

                                                               PkgItemStr

   getMegacoPkgItemStr()
              The method is used to get the pkgdName and eventOther in
   the event buffer descriptor in the string format. This method returns
   package parameters in the string format.
    int  getStreamId()
           throws javax.megaco.pkg.MethodInvovationException

              This method returns the stream id if set.
     void  setStreamId(int streamId)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the stream id which shall be
   part of the event descriptor. This automatically sets the stream id
   present flag.
     boolean  isStreamIdPresent()
              The method can be used to check whether stream id is
   present as a part of the event descriptor.
    TimeStamp getTimeStamp()
              The method is used to get the time stamp, if set, in the
   observed event descriptor. If the timestamp is not set, then this
   returns a NULL value.
    void  setTimeStamp (TimeStamp timeStamp)
           throws javax.megaco.InvalidArgumentException

              The method is used to set the time stamp for the observed
   vent descriptor.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  ObservedEvent

public ObservedEvent(PkgEventItem eventItem)
                     throws javax.megaco.InvalidArgumentException

          Constructs Observed Event parameter Object. This is to be set
          within an observed event descriptor.

   Throws:
          InvalidArgumentException - Thrown if an invalid event id is
          set.
     _________________________________________________________________

  ObservedEvent

public ObservedEvent(PkgItemStr eventItemStr)
                     throws javax.megaco.InvalidArgumentException

          Constructs Observed Event parameter object with the PkgItemStr
          object. This method would be used if the package parameters are
          to be conveyed in the string format. This is to be set within
          an event buffer descriptor.

   Throws:
          InvalidArgumentException - Thrown if an invalid eventItemStr
          object reference is set.
     _________________________________________________________________

   Method Detail

  getMegacoPkgEvtItem

public PkgEventItem getMegacoPkgEvtItem()

          The method can be used to get the pkdgName as set in the
          observed event descriptor. This method gives the package
          information, the attached event information and the parameter
          name and value for the event id.

        Returns:
                The object reference for the megaco package, which has
                the object reference for megaco package event and which
                in turn has the reference for the parameter info. If the
                parameter has not been set, then this method shall return
                NULL.
     _________________________________________________________________

  getMegacoPkgItemStr

public PkgItemStr getMegacoPkgItemStr()

          The method can be used to get the pkdgName as set in the
          observed event descriptor. This method gives the package
          information, the attached event information and the parameter
          name and value. As compares to the getMegacoPkgEvtItem()
          method, this method returns the package parameters in the
          string format.

        Returns:
                The object reference for the megaco package item. This
                class holds information about package name, item name and
                the parameters in the string format. If the parameter has
                not been set, then this method shall return NULL.
     _________________________________________________________________

  getStreamId

public int getStreamId()
                     throws javax.megaco.MethodInvocationException

          The method can be used to get stream id in the event
          descriptor.

        Returns:
                streamId - The integer value of the stream id shall be
                returned. This shall be returned only if the streamId is
                present in the event parameter of the event descriptor.

        Throws:
                MethodInvocationException- Thrown if streamId has not
                been set. Thus this method should be called only after
                verifying that the streamId is set using
                isStreamIdPresent()
     _________________________________________________________________

  setStreamId

public void setStreamId(int streamId)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set stream id in the event
          descriptor. This automatically sets the value of stream id
          present in the event parameter object to true.


        Parameter:
                streamId - The integer value of the stream id shall be
                set.

        Throws:
                InvalidArgumentException - Thrown if streamId is set with
                an invalid value.
     _________________________________________________________________

  isStreamIdPresent

public  boolean isStreamIdPresent()

          The method can be used to find if the stream id is present in
          the current object.

        Return:
                 - Returns true if the stream id is present. This needs
                to be checked before getting the stream id from this
                object.
     _________________________________________________________________

  getTimeStamp

public TimeStamp getTimeStamp()

          The method is used to retrieve the time stamp at which the
          event was detected.

        Returns:
                timeStamp - The object reference for the timestamp. It
                returns a NULL value when not set.
     _________________________________________________________________

  setTimeStamp

public void setTimeStamp(TimeStamp timeStamp)
                     throws javax.megaco.InvalidArgumentException

          The method is used to set the time stamp at which the event was
          detected.

        Parameters:
                timeStamp - The object reference for the timestamp.

        Throws:
                InvalidArgumentException - Thrown if timestamp is set
                with an invalid value.
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
    PREV CLASS   NEXT CLASS FRAMES  NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
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
