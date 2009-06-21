// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: EventParam.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:37 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:37 $ by $Author: brian $
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
Class EventParam

java.lang.Object
  |
  +--javax.megaco.message.descriptor.EventParam

   All Implemented Interfaces:
          java.io.Serializable

   See Also:
          Serialized Form
     _________________________________________________________________

   public class EventParam
   extends java.lang.Object

   The EventParam object is a class that shall be used to set the signal
   descriptor, digitMap descriptor, stream id, whether keep alive is
   active or not, package name, event id and the associated parameter
   names and values. The class optionally provides interface to specify
   the package name, item name and the associated parameters in the
   string format. This is an independent class derived from
   java.util.Object and shall not have any derived classes.
     _________________________________________________________________

   Constructor Summary
   EventParam()

              Constructs an object of type event parameter which shall be
   used within the event descriptor.

   Method Summary
    SignalDescriptor getSignalDescriptor()
             The method is used to get the embedded signal descriptor
   within event descriptor.
     java.lang.String getDigitMapName()
             This method returns a the embedded digit map name within the
   event descriptor.

                                                            DigitMapValue

   getDigitMapValue()
             This method returns a the embedded digit map value within
   the event descriptor

                                                         java.lang.String

   getDigitMapValueStr()
             This method returns a the embedded digit map value in the
   string format, within the event descriptor
    int  getStreamId()
           throws javax.megaco.pkg.MethodInvovationException

              This method returns the stream id if set.
     void  setSignalDescriptor (SignalDescriptor signalDesc)
           throws javax.megaco.InvalidArgumentException

             The method can be used to set the object reference of signal
   Descriptor.
     void  setDigitMapName (java.lang.String digitMap)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the embedded Digit Map name
   within the event descriptor.

                                                                     void

   setDigitMapValue (DigitMapValue digitMapValue)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the embedded Digit Map value
   within the event descriptor.

                                                                     void

   setDigitMapValueStr (java.lang.String digitMapValue)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the embedded Digit Map value
   in the string format, within the event descriptor.
     void  setStreamId(int streamId)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the stream id which shall be
   part of the event descriptor. This automatically sets the stream id
   present flag.
     boolean isStreamIdPresent()
              The method can be used to check whether stream id is
   present as a part of the event descriptor.

                                                                  boolean

   isKeepActivePresent()
              The method can be used to check whether Keep Active is
   present as a part of the event descriptor.

                                                                     void

   setKeepAvtive()
              The method can be used to set the Keep Active which shall
   be part of the event descriptor.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  EventParam

public EventParam()

          Constructs an object of type event parameter which shall be
          used within the event descriptor.
     _________________________________________________________________

   Method Detail

  getSignalDescriptor

public SignalDescriptor getSignalDescriptor()

          The method can be used to get the signal descriptor.

        Returns:
                signalDesc - The reference to the object corresponding to
                the signal descriptor. This shall be returned only if the
                signal descriptor is present in the event parameter of
                the event descriptor.
     _________________________________________________________________

  getDigitMapName

public java.lang.String getDigitMapName()

          The method can be used to get the embedded digit map name in
          the event descriptor. Only one of digit map name or digit map
          value shall be set.

        Returns:
                The reference to the embedded digitMap name. If this
                value is not set, then this method would return NULL.
     _________________________________________________________________

  getDigitMapValue

public DigitMapValue getDigitMapValue()

          The method can be used to get the embedded digit map value in
          the event descriptor. Only one of digit map name or digit map
          value shall be set.

        Returns:
                The reference to the embedded digitMap value. If this
                value is not set, then this method would return NULL.
     _________________________________________________________________

  getDigitMapValueStr

public java.lang.String getDigitMapValueStr()

          The method can be used to get the embedded digit map value in
          the event descriptor. Only one of digit map name or digit map
          value shall be set.

        Returns:
                The reference to the embedded digitMap value. If this
                value is not set, then this method would return NULL.
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

  setSignalDescriptor

public void setSignalDescriptor(SignalDescriptor signalDesc)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the signal descriptor within the
          event descriptor.

        Parameter:
                signalDesc - The reference to the object corresponding to
                the signal descriptor.

        Throws:
                InvalidArgumentException - Thrown if parameters set in
                the signal descriptor are such that the object cannot be
                set for the event descriptor.
     _________________________________________________________________

  setDigitMapName

public void setDigitMapName(java.lang.String digitMapName)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the digit map name. Only one of
          digit map name or digit map value shall be set. This
          automatically sets the value of digitMap descriptor present in
          the event parameter object to true. If digitmap value is
          already set, then this method would raise an exception.


        Parameter:
                digitMapName - The reference to the object corresponding
                to the digitMap name.

        Throws:
                InvalidArgumentException - Thrown if digit Map value has
                already been set. The protocol allows only either of them
                but not both.
     _________________________________________________________________

  setDigitMapValue

public void setDigitMapValue(DigitMapValue digitMapValue)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the digit map value. Only one of
          digit map name or digit map value shall be set. This
          automatically sets the value of digitMap descriptor present in
          the event parameter object to true. If digitmap name is already
          set, then this method would raise an exception.


        Parameter:
                digitMapValue - The reference to the object corresponding
                to the digitMap value.

        Throws:
                InvalidArgumentException - Thrown if digit Map name has
                already been set. The protocol allows only either of them
                but not both.
     _________________________________________________________________

  setDigitMapValueStr

public void setDigitMapValueStr(java.lang.String digitMapValue)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the digit map value. Only one of
          digit map name or digit map value shall be set. This
          automatically sets the value of digitMap descriptor present in
          the event parameter object to true. If digitmap name is already
          set, then this method would raise an exception.


        Parameter:
                digitMapValue - The reference to the object corresponding
                to the digitMap value.

        Throws:
                InvalidArgumentException - Thrown if digit Map name has
                already been set. The protocol allows only either of them
                but not both.
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

  setKeepActive

public void setKeepActive()

          The method can be used to set Keep Active flag in the event
          descriptor.
     _________________________________________________________________

  isKeepActivePresent

public  boolean isKeepActivePresent()

          The method can be used to find if the Keep Active is present in
          the current object.

        Return:
                 - Returns true if the Keep Active is present.
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
