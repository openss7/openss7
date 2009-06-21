// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: SDPInfo.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:38 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:38 $ by $Author: brian $
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
Class SDPInfo

java.lang.Object
  |
  +--javax.megaco.message.descriptor.SDPInfo

   All Implemented Interfaces:
          java.io.Serializable

   See Also:
          Serialized Form
     _________________________________________________________________

   public class SDPInfo
   extends java.lang.Object

   The SDPInfo object is a class that shall be used to set the Media
   description, Session Description and Time description of the SDP.
   These would be defined in javax.sdp package. This is an independent
   class derived from java.util.Object and shall not have any derived
   classes.
     _________________________________________________________________

   Constructor Summary
   SDPInfo()
              Constructs an object of type megaco SDP info which shall be
   used within the media descriptor to set the local and remote
   descriptor.

   Method Summary
   SessionDescription getSessionDescription()
              The method is used to get the session description of the
   SDP which would be set in local or remote descriptor. If it is not set
   then this shall return a null value.
   void setSessionDescription (SessionDescription session)
           throws javax.megaco.InvalidArgumentException,
   javax.megaco.ParamNotSupportedException

              The method can be used to set the session description
   within an SDP for local/remote descriptor.

                                                         java.lang.String

   getSessionDescStr()
              The method is used to get the session description of the
   SDP in the string format, which would be set in local or remote
   descriptor. If it is not set then this shall return a null value.

                                                                     void

   setSessionDescStr (java.lang.String session)
       throws javax.megaco.InvalidArgumentException

              The method can be used to set the session description
   within an SDP for local/remote descriptor. The session description
   provided is in the string format.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  SDPInfo

public SDPInfo()

          Constructs an object of type megaco SDP info which shall be
          used within the media descriptor to set the local and remote
          descriptor.

   Method Detail

  getSessionDescription

public SessionDescription getSessionDescription()

          The method is used to get the session description of the SDP
          which would be set in local or remote descriptor. In an SDP
          this information necessarily needs to be set.

        Returns:
                session - The reference to the object corresponding to
                the session description. If the session description is
                not set then it shall return a null value.
     _________________________________________________________________

  setSessionDescription

public void setSessionDescription(SessionDescription session)
                     throws javax.megaco.InvalidArgumentException, javax.megaco
.ParamNotSupportedException

          The method can be used to set the session description within an
          SDP for local/remote descriptor.

        Parameter:
                session - The reference to the object corresponding to
                the session description.

        Throws:
                InvalidArgumentException - Thrown if parameters set in
                the session description are such that the object cannot
                be set for the local/remote descriptor.
                ParamNotSupportedException - Thrown if parameters set in
                the session description are such that they are not
                supported.
     _________________________________________________________________

  getSessionDescStr

public java.lang.String getSessionDescStr()

          The method is used to get the session description of the SDP
          which would be set in local or remote descriptor. The SDP info
          returned is in the string format. The applications may use this
          method if it is not concerned with the parameters contained in
          the SDP info. The SDP string obtained may be processed later or
          may be tunnelled to the other layer for processing.

        Returns:
                session - The reference to the object corresponding to
                the session description string. If the session
                description string is not set then it shall return a null
                value.
     _________________________________________________________________

  setSessionDescStr

public void setSessionDescStr(java.lang.String session)

              throws javax.megaco.InvalidArgumentException

          The method can be used to set the session description string
          within an SDP for local/remote descriptor. The applications may
          use this method in case it has pre-encoded SDP string with
          itself. The pre-encoded SDP string may be obtained either at
          the time of configuration of the application or may be obtained
          in the commands exchanged for the call. The SDP info string as
          set in this method must be in the format as defined by the RFC
          2327 for IN SDP and RFC 3018 for ATM SDP.

        Parameter:
                session - The reference to the object corresponding to
                the session description string.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of session description string passed to
                this method is NULL.
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
