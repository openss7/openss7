// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: MediaStreamParam.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:18 $ <p>
 
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
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class MediaStreamParam

java.lang.Object
  |
  +--javax.megaco.message.descriptor.MediaStreamParam

   All Implemented Interfaces:
          java.io.Serializable

   See Also:
          Serialized Form
     _________________________________________________________________

   public class MediaStreamParam
   extends java.lang.Object

   The MediaStreamParam object is a class that shall be used to set the
   local descriptor, local control descriptor and the remote descriptor.
   This is an independent class derived from java.util.Object and shall
   not have any derived classes.
     _________________________________________________________________

   Constructor Summary
   MediaStreamParam()
              Constructs an object of type media stream parameter which
   shall be used within the media descriptor.

   Method Summary
     LocalCtrlDescriptor getLclCtrlDescriptor()
              This method returns a local control descriptor if set.
     void  setLclCtrlDescriptor (LocalCtrlDescriptor localControlDesc)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the object reference of local
   control Descriptor.
    SDPInfo getLocalDescriptor()
              The method is used to get the SDP parameters within the
   local descriptor within media descriptor.
     void  setLocalDescriptor (SDPInfo sdpInfo)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the object reference of
   SDPInfo for local descriptor.
     SDPInfo getRemoteDescriptor()
              The method is used to get the SDP parameters within the
   remote descriptor within media descriptor.
     void  setRemoteDescriptor (SDPInfo sdpInfo)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the object reference of
   SDPInfo within remote Descriptor.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  MediaStreamParam

public MediaStreamParam()

          Constructs a Stream Parameter Object consisting of local,
          remote and local control descriptor.

   Method Detail

  getLclCtrlDescriptor

public LocalCtrlDescriptor getLclCtrlDescriptor()

          The method can be used to get the local control descriptor.

        Returns:
                localControlDesc - The reference to the object
                corresponding to the local control descriptor. This shall
                be returned only if the local control descriptor is
                present in the media stream parameter of the media
                descriptor else shall return a NULL value.
     _________________________________________________________________

  getLocalDescriptor

public SDPInfo getLocalDescriptor()

          The method can be used to get the SDP information for local
          descriptor.

        Returns:
                SDPinfo - The reference to the object corresponding to
                SDPInfo. This shall be returned only if the SDP
                information is present in the local descriptor of the
                media descriptor.
     _________________________________________________________________

  getRemoteDescriptor

public SDPInfo getRemoteDescriptor()

          The method can be used to get the SDP information for remote
          descriptor.

        Returns:
                SDPInfo - The reference to the object corresponding to
                SDPInfo. This shall be returned only if the SDP
                information is present in the remote descriptor of the
                media descriptor.
     _________________________________________________________________

  setLclCtrlDescriptor

public void setLclCtrlDescriptor(LocalCtrlDescriptor localControlDesc)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the local control descriptor
          within the media descriptor.

        Parameter:
                localControlDesc - The reference to the object
                corresponding to the local control descriptor.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if local
                control descriptor has incompatible parameters.
     _________________________________________________________________

  setLocalDescriptor

public void setLocalDescriptor(SDPInfo sdp)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the local descriptor within the
          media descriptor.

        Parameter:
                sdp - The reference to the object corresponding to the
                SDPInfo for setting the local descriptor.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if sdp
                information has incompatible parameters.
     _________________________________________________________________

  setRemoteDescriptor

public void setRemoteDescriptor(SDPInfo sdp)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the remote descriptor within the
          media descriptor.

        Parameter:
                sdp - The reference to the object corresponding to the
                SDPInfo for setting the remote descriptor.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if sdp
                information has incompatible parameters.
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
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
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
