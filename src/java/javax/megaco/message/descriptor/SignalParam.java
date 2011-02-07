// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: SignalParam.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:18 $ <p>
 
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
    PREV CLASS   NEXT CLASS FRAMES  NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class SignalParam

java.lang.Object
  |
  +--javax.megaco.message.descriptor.SignalParam

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class SignalParam
   extends java.lang.Object

   The SignalParam is a class that shall be used to set the signal params
   within the signal descriptor. This is an independent class derived
   from java.util.Object and shall not have any derived classes.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   SignalParam (SignalRequest signalRequest)
       throws javax.megaco.InvalidArgumentException

              Constructs an object of type SignalParam with parameters
   consisting of Signal Request. This class is used as a parameter within
   signal descriptor.
   SignalParam (short listId, SignalRequest[] signalRequest)
       throws javax.megaco.InvalidArgumentException

              Constructs an object of type SignalParam with parameters
   consisting of Signal List and Signal List Id. This class is used as a
   parameter within signal descriptor.

   Method Summary
    int  getSignalParamType()
              The method specifies whether the signal parameter was
   created for signal list or signal request.
    unsigned short  getSignalParamListId()
           throws javax.megaco.ParameterNotSetException,
   javax.megaco.MethodInvocationException

              The method is used to get the list id which identifies the
   signal list of signal descriptor.
     SignalRequest[]  getSignalRequest()

              The method gives the vector of object identifier of the
   signal request. If the signal param type represents that this signal
   param consists of only signal request and not signal list, then this
   would return a vector consisting of only one object identifier for
   signal request.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  SignalParam

   public SignalParam(SignalRequest signalRequest)
       throws javax.megaco.InvalidArgumentException

          Constructs a Jain Megaco Signal Parameter Object with the
          parameters consisting of SignalRequest. This method implicitly
          sets the signal param type to  M_SIGNAL_PARAM_REQUEST.



          Parameter:

          signalRequest - This contains a the object reference to the
          class signal request.

   Throws:
   InvalidArgumentException : This exception is raised if the reference
   of Signal request passed to this method is NULL.
         __________________________________________________________

public SignalParam(short listId, SignalRequest[] signalRequest)

                Constructs a Jain Megaco Signal Parameter Object with the
                parameters consisting of SignalList and the List Id. This
                method implicitly sets the signal param type to
                M_SIGNAL_PARAM_LIST.


              Parameter:
              signalRequest - This contains a vector of object reference
                      to the class signal request.listId               -
                      Identifies the list Id of the signal list.



                Throws:

                InvalidArgumentException : This exception is raised if
                the reference of Signal request passed to this method is
                NULL or the value of list Id is less than 0.

   Method Detail

  getSignalParamType

public SignalParamType getSignalParamType()

          The method can be used to get the type of the signal param that
          specifies whether the signal param is signal list or signal
          request in the signal descriptor. This value would be set in
          the constructor of this class.


        Returns:
                 The integer value specifying the type of signal param.
                If this value is not set, then this method would return
                value 0.
     _________________________________________________________________

  getSignalParamListId

public unsigned short getSignalParamListId()
                     throws javax.megaco.ParameterNotSetException, javax.megaco
.MethodInvocationException

          The method is used to get the list id which identifies the
          signal list of signal descriptor.

        Returns:
                 The list id for the signal list.

        Throws:
                javax.megaco.ParameterNotSetException - Thrown if signal
                param type specifies the type to be signal list, though
                this parameter has not been set.

          javax.megaco.MethodInvocationException - Thrown if signal param
          type specifies the type to be signal request.
     _________________________________________________________________

  getSignalRequest

public SignalRequest[] getSignalRequest()
                     throws javax.megaco.ParameterNotSetException

          The method gives the vector of object identifier of the signal
          request. The signal param type field of this class indicates if
          the signal parameter is signal request or signal list. If the
          signal param type is M_SIGNAL_PARAM_LIST, then this indicates
          that the signal request returned is part of the signal list.


        Returns:
                 The vector of object identifier for signal requests.

        Throws:
                javax.megaco.ParameterNotSetException - Thrown if signal
                request have not been set.
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
