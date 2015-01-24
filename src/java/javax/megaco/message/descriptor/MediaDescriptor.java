// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/descriptor/MediaDescriptor.java <p>
 
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
     _________________________________________________________________

javax.megaco.message.descriptor
Class MediaDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.MediaDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class MediaDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class describes the
   media descriptor. The SDP information set within the local and remote
   descriptor for this class is outside the scope of JAIN MEGACO. It is
   defined in javax.sdp.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   MediaDescriptor()
              Constructs a Media Descriptor object. This extends the
   Descriptor class. It defines the Media descriptor of MEGACO protocol
   which contains the stream descriptor, stream param and the termination
   state descriptor.

   Method Summary
     MediaStreamParam getMediaStreamParam()
              Returns an object identifier for the media stream
   parameter. If stream parameter is not set, then this shall return a
   NULL value.
    void setMediaStreamParam (MediaStreamParam streamParam)
           throws javax.megaco.InvalidArgumentException

              Sets the object of type MediaStreamParam in a media
   descriptor. This shall specify the local SDP info, the remote SDP info
   and the local control descriptor for stream id = 1. If stream
   parameter is not to be sent, then this method would not be called.
   StreamDescriptor[] getStreamDescriptor()
              Returns the vector of the object identifier of type
   StreamDescriptor. If stream descriptor is not present for the media,
   then this shall return a NULL value.
   void setStreamDescriptor (StreamDescriptor[] streamDesc)
           throws javax.megaco.InvalidArgumentException

              Sets the vector of the StreamDescriptor in a media
   descriptor. If stream descriptor is not to be sent, then this method
   would not be called.
     TermStateDescriptor getTermStateDescriptor()
              Returns an object identifier of type TermStateDescriptor.
   If termination state descriptor is not sent for the media, then this
   shall return a NULL value.
   void setTermStateDescriptor (TermStateDescriptor termState)
           throws javax.megaco.InvalidArgumentException

              Sets the TermStateDescriptor in a media descriptor. If
   termination state descriptor is not to be sent, then this method would
   not be called.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  MediaDescriptor

public MediaDescriptor()

          Constructs a Media Descriptor object. It defines the Media
          descriptor of MEGACO protocol which contains the stream
          descriptor, stream param and the termination state descriptor.

   Method Detail

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type
          descriptor Media. This method overrides the corresponding
          method of the base class Descriptor.


        Returns:
                Returns an integer value that identifies this media
                object as the type of media descriptor. It returns that
                it is Media Descriptor i.e., M_MEDIA_DESC.
     _________________________________________________________________

  getMediaStreamParam

public final MediaStreamParam getMediaStreamParam()

          This method gets the Stream parameter for the media descriptor.
          This can be used to get the local descriptor, remote descriptor
          and the local control descriptor for stream id = 1. When stream
          param is not present, then this shall return a NULL value.


        Returns:
                Returns the object reference of type stream parameter. If
                the stream parameter is not set then this shall return a
                NULL value.
     _________________________________________________________________

  setMediaStreamParam

public final void setMediaStreamParam(MediaStreamParam streamParam)
            throws javax.megaco.InvalidArgumentException

          This method sets the stream parameter in the media descriptor.


        Parameter:
                streamParam - Sets the stream parameter object reference.
                The stream param object consists atleast one of local
                descriptor, local control and remote descriptor.

        Throws:
                InvalidArgumentException if the parameters set for the
                stream parameter are such that the Media Descriptor
                cannot be encoded.
     _________________________________________________________________

  getStreamDescriptor

public final StreamDescriptor[] getStreamDescriptor()

          This method gets the multiple stream descriptor as a vector
          format with multiple stream ids.


        Returns:
                Returns the vector of the multiple stream descriptor. If
                the stream descriptor is not set then this shall return a
                NULL value.
     _________________________________________________________________

  setStreamDescriptor

public final void setStreamDescriptor(StreamDescriptor[] streamDesc)
            throws javax.megaco.InvalidArgumentException

          This method sets the vector of Stream descriptor in the Media
          descriptor. Each vector position corresponds to the different
          stream in the Stream Descriptor.


        Parameter:
                streamDescriptor - Sets the stream descriptor object
                reference. There can be multiple stream descriptor set.

        Throws:
                InvalidArgumentException if the parameters set for the
                stream descriptor are such that the Media Descriptor
                cannot be encoded.
     _________________________________________________________________

  getTermStateDescriptor

public final TermStateDescriptor getTermStateDescriptor()

          This method gets the object reference for termination state
          descriptor for the media descriptor.


        Returns:
                Returns the object reference of type termination state
                descriptor. If the termination state descriptor is not
                set then this shall return a NULL value.
     _________________________________________________________________

  setTermStateDescriptor

public final void setTermStateDescriptor(TermStateDescriptor termStateDesc)
            throws javax.megaco.InvalidArgumentException

          This method sets the termination state descriptor.


        Parameter:
                termStateDesc - Sets the termination state descriptor
                object reference. This object consists atleast one of
                service state or event buffer control or parameter
                property.

        Throws:
                InvalidArgumentException if the parameters set for the
                termination state descriptor are such that the Media
                Descriptor cannot be encoded.
     _________________________________________________________________
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
