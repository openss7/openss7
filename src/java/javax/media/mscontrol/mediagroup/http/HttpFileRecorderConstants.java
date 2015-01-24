/*
 @(#) src/java/javax/media/mscontrol/mediagroup/http/HttpFileRecorderConstants.java <p>
 
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

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.mediagroup.http
Interface HttpFileRecorderConstants
     _________________________________________________________________

public interface HttpFileRecorderConstants

   This interface defines constants that can be used to configure the
   http file recorder
   These symbols must be used with an associated URL.
   The pseudo code below records a file to an http server, with the
   mime-type "video/3gpp":
        Parameters myParameters = mySession.createParameters();
        myParameters.put(HttpRecorderConstants.p_FileFormat, HttpRecorderConsta
nts.v_3GPFormat);
        myMG.record(new URI("http://www.server.com/record.jsp"), 0, null, myPar
ameters)

     _________________________________________________________________

   Field Summary
   static Symbol e_HttpRecordFail
             Generic error thrown when the http record fail
   static Symbol p_AudioCoder
             An alias for p_Coder
   static Symbol p_Coder
             Symbol indicating the audio coder to use for a new recording
   (if it includes an audio track).
   static Symbol p_FileFormat
             A Symbol identifying the File Format used during recording.
   static Symbol p_LocalFile
             Indicate a local file where the stream must be stored before
   being sent to the http server.
   static Symbol p_Method
             Indicates witch http method to use for the record.
   static Symbol p_SubmitTimeout
             The interval in ms to wait for the content to be transfered
   to the http server before throwing an Error.e_Timeout event.
   static Symbol p_VideoCoder
             Symbol indicating the video coder to use for a new recording
   (if it includes a video track).
   static Symbol v_Post
             http POST method
   static Symbol v_Put
             http PUT method



   Field Detail

  p_Coder

static final Symbol p_Coder

          Symbol indicating the audio coder to use for a new recording
          (if it includes an audio track).
          Default: v_Inferred
          For a list of Coder type Symbols,

        See Also:
                CoderConstants
     _________________________________________________________________

  p_AudioCoder

static final Symbol p_AudioCoder

          An alias for p_Coder
     _________________________________________________________________

  p_VideoCoder

static final Symbol p_VideoCoder

          Symbol indicating the video coder to use for a new recording
          (if it includes a video track).
          Default: v_Inferred
          For a list of Coder type Symbols,

        See Also:
                CoderConstants
     _________________________________________________________________

  p_FileFormat

static final Symbol p_FileFormat

          A Symbol identifying the File Format used during recording.
          Default value: v_Inferred (file extension is used)
          Other values: v_WavFormat, v_RawFormat, v_GSMFormat,
          v_3GPFormat

        See Also:
                CoderConstants
     _________________________________________________________________

  p_Method

static final Symbol p_Method

          Indicates witch http method to use for the record. Can be
          either v_Put or v_Post
     _________________________________________________________________

  p_LocalFile

static final Symbol p_LocalFile

          Indicate a local file where the stream must be stored before
          being sent to the http server. The created file is under the
          control of the application, including deletion.
     _________________________________________________________________

  v_Put

static final Symbol v_Put

          http PUT method

        See Also:
                p_Method
     _________________________________________________________________

  p_SubmitTimeout

static final Symbol p_SubmitTimeout

          The interval in ms to wait for the content to be transfered to
          the http server before throwing an Error.e_Timeout event.
     _________________________________________________________________

  v_Post

static final Symbol v_Post

          http POST method

        See Also:
                p_Method
     _________________________________________________________________

  e_HttpRecordFail

static final Symbol e_HttpRecordFail

          Generic error thrown when the http record fail
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright (C) 2007-2008 Oracle and/or its affiliates. (C) Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
package javax.jain.media.mscontrol.mediagroup.http;
