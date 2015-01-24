// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/descriptor/SignalRequest.java <p>
 
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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class SignalRequest

java.lang.Object
  |
  +--javax.megaco.message.descriptor.SignalRequest

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class SignalRequest
   extends java.lang.Object

   The SignalRequest object is a class that shall be used to set the
   fields of signal request within the signal descriptor. This is an
   independent class derived from java.util.Object and shall not have any
   derived classes. If the signal param type of the SignalParam class is
   M_SIGNAL_PARAM_LIST, then it indicates that the SignalRequest is part
   of the signal list of the signal descriptor. The class optionally
   provides interface to specify the package name, item name and the
   associated parameters in the string format.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   SignalRequest (PkgSignalItem signalItem)
           throws javax.megaco.InvalidArgumentException

              Constructs an object of type signal request which is used
   as a parameter within signal descriptor. A signal item of the megaco
   package is attached to the current object.
   SignalRequest ( PkgItemStr signalItemStr)
           throws javax.megaco.InvalidArgumentException

              Constructs an object of type signal request which shall be
   used within the signal buffer descriptor. The signalItemStr passed in
   this constructor specifies the package parameters in the string
   format. implementations may use this method for the classes which are
   not defined in the javax.megaco.pkg package.

   Method Summary
     PkgSignalItem getMegacoPkgSignalItem()
              The method is used to get the signal specified in the
   signal request. PkgSignalItem contains the associated package name,
   the signal name and the parameters if any and their corresponding
   values.

                                                               PkgItemStr

   getMegacoPkgItemStr()
              The method is used to get the package name, signal name and
   the associated parameters specified in the signal request. This method
   returns package parameters in the string format.
     boolean  isStreamIdPresent()
              This method specifies whether the stream id is present or
   not.
     int  getStreamId()
           throws javax.megaco.ParameterNotSetException

              The method gets the stream id.
     void  setStreamId(int streamId)
           throws javax.megaco.InvalidArgumentException

              The method sets the stream id.
     int getSignalType()
              The method is used to get the type of signal which could be
   one of brief or on-off or other.
   final void  setSignalType( SignalType signalType)
       throws javax.megaco.InvalidArgumentException

             The method is used to set the type of signal.
     int getSignalNotifyReason()
              The method is used to get the notification reason which
   could be one of timeout or interrupted due to an event received or
   interrupted due to another signal overwriting the current signal or
   other.
   final void setSignalNotifyReason( SignalNotifyReason notifyReason)
       throws javax.megaco.InvalidArgumentException

             The method is used to set the signal notification reason.
     boolean isKeepAliveSet()
              This method specifies whether the keep alive token is to be
   set or not.

                                                               final void

   setKeepAlive()
              This method is used to enable the keep alive flag.

                                                                  boolean

   isDurationPresent()
              This method specifies whether the signal duration is set or
   not.

                                                               final void

   setSignalDuration(int signal_duration)
         throws javax.megaco.InvalidArgumentException

              This method is used to set the signal duration.

                                                                      int

   getSignalDuration()
        throws javax.megaco.ParameterNotSetException

              This method is used to get the signal duration. This method
   must be invoked after checking for the presence of the signal duation
   in the parameters.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  SignalRequest

public SignalRequest(PkgSignalItem signalItem)
          throws javax.megaco.InvalidArgumentException

          Constructs Signal request parameter object with the
          PkgSignalItem object.

   Throws:
          InvalidArgumentException - Thrown if an invalid signalItem
          object reference is set.
     _________________________________________________________________

  SignalRequest

public SignalRequest(PkgItemStr signalItemStr)
                     throws javax.megaco.InvalidArgumentException

          Constructs Signal request object with the PkgItemStr object.
          This method would be used if the package parameters are to be
          conveyed in the string format.

   Throws:
          InvalidArgumentException - Thrown if an invalid signalItemStr
          object reference is set.
     _________________________________________________________________

   Method Detail

  getMegacoPkgSignalItem

public PkgSignalItem getMegacoPkgSignalItem()

          The method can be used to get the package name in the signal
          descriptor. This method gives the package information, the
          attached signal information and the parameter name and value
          for the signal id.

        Returns:
                package - The object reference for the Signal Item. This
                has the object reference of corresponding megaco package
                and has the reference of the parameter info associated
                with it.
     _________________________________________________________________

  getMegacoPkgItemStr

public PkgItemStr getMegacoPkgItemStr()

          The method can be used to get the pkdgName as set in the signal
          descriptor. This method gives the package information, the
          attached signal information and the parameter name and value.
          As comapres to the  getMegacoPkgSignalItem() method, this
          method returnes the package parameters in the string format.

        Returns:
                The object reference for the megaco package item. This
                class holds information about package name, item name and
                the parameters in the string format. If the parameter has
                not been set, then this method shall return NULL.
     _________________________________________________________________

  isStreamIdPresent

public boolean isStreamIdPresent()

          This method specifies whether the stream id is present or not.


        Returns:
                 The TRUE if the stream id is present. Else it shall
                return FALSE.
     _________________________________________________________________

  getStreamId

public int getStreamId()
                throws javax.megaco.ParameterNotSetException

          The method is used to retrieve the stream id only if the
          isStreamIdPresent() returns TRUE.

        Returns:
                 The stream id set for this signal.

        Throws:
                javax.megaco.ParameterNotSetException if
                isStreamIdPresent() returns FALSE.
     _________________________________________________________________

  setStreamId

public int setStreamId(int streamId)
                        throws javax.megaco.InvalidArgumentException

          The method is sets the stream id and automatically sets a flag
          such that isStreamIdPresent() returns TRUE.

        Parameter:
                streamId - Stream id that is to be set for this signal.

        Throws:
                InvalidArgumentException - If the stream Id specified is
                invalid, then this exception is raised.
     _________________________________________________________________

  getSignalType

public int getSignalType()

          The method is used to get the type of signal which could be one
          of brief or on-off or other.

        Returns:
                 The integer value which identifies the SignalType. If
                SignalType is not specified, then this method would
                return value 0.
     _________________________________________________________________

  setSignalType

   public void setSignalType(SignalType signalType)
       throws javax.megaco.InvalidArgumentException

          The method is used to set the type of signal.

        Parameter:
                signalType - The object reference to a derived class of
                SignalType.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Signal Type passed to this method is
                NULL.
     _________________________________________________________________

  getSignalNotifyReason

public int getSignalNotifyReason()

          The method is used to get the notification reason which could
          be one of timeout or interrupted due to an event received or
          interrupted due to another signal overriding the current signal
          or other.

        Returns:
                The integer value which identifies the
                SignalNotifyReason. If the signal notify reason is not
                specified, then this method would return value 0. The
                possible values are defined in the class
                SignalNotifyReason.
     _________________________________________________________________

  setSignalNotifyReason

public void setSignalNotifyReason(SignalNotifyReason notifyReason)

              throws javax.megaco.InvalidArgumentException

          The method is used to set the signal notification reason.

        Parameter:
                notifyReason - The object reference to a derived class of
                SignalNotifyReason.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Signal Notify Reason passed to this
                method is NULL.
     _________________________________________________________________

  isKeepAliveSet

public boolean isKeepAliveSet()

          This method specifies whether the keep alive token is set or
          not.

        Return:
                 Returns TRUE if the keep alive token has been set.
     _________________________________________________________________

  setKeepAlive

public void setKeepAlive()

          This method is used to set the keep alive flag. Following this
          the method isKeepAliveSet() method would return value TRUE,
          indicating that the keep alive flag is set.
     _________________________________________________________________

  isDurationPresent

public boolean isDurationPresent()

          This method specifies whether the duration field is present or
          not.

        Return:
                 Returns TRUE if the duration field is present.
     _________________________________________________________________

  setSignalDuration

public int setSignalDuration(int signal_duration)
                        throws javax.megaco.InvalidArgumentException

          This method is used to set the signal duration in the signal
          parameters.

        Parameter:
                signal_duration - specifies the signal duration parameter
                in the signal descriptor.

        Throws:
                InvalidArgumentException - If the signal duration
                specified is invalid, then this exception is raised.
     _________________________________________________________________

  getSignalDuration

   public int getSignalDuration()
             throws javax.megaco.ParameterNotSetException

          This method returns the signal duration if it is specified in
          the signal parameters. This method must be invoked after
          checking for the presence of the signal duration field in the
          signal parameters.

        Return:
                 Returns the value of the signal duration that has been
                set.

        Throws:
                javax.megaco.ParameterNotSetException : This is raised if
                no value of signal duration is set. This method must be
                invoked after invoking isDurationPresent() method.
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
