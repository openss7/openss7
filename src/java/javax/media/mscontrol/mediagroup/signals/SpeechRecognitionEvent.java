/*
 @(#) src/java/javax/media/mscontrol/mediagroup/signals/SpeechRecognitionEvent.java <p>
 
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

javax.media.mscontrol.mediagroup.signals
Interface SpeechRecognitionEvent

   All Superinterfaces:
          MediaEvent<SignalDetector>, ResourceEvent<SignalDetector>,
          SignalDetectorEvent
     _________________________________________________________________

public interface SpeechRecognitionEvent

   extends SignalDetectorEvent

   Describes the result of a speech recognition.

   Basic speech-enabled applications can use getTag() and/or
   getUserInput() , as shown below:

   Assuming that the following grammar was set as a pattern of the
   SignalDetector:
 <grammar version="1.0" xmlns="http://www.w3.org/2001/06/grammar"
         xml:lang="en-US" tag-format="semantics/1.0-literals" root="answer">
    <rule id="answer" scope="public">

       <one-of>
          <item><ruleref uri="#yes"/></item>
          <item><ruleref uri="#no"/></item>
       </one-of>

    </rule>

    <rule id="yes">
       <one-of>
          <item>yes</item>
          <item>yeah<tag>yes</tag></item>

          <item><token>you bet</token><tag>yes</tag></item>
          <item xml:lang="fr-CA">oui<tag>yes</tag></item>

       </one-of>
    </rule>
    <rule id="no">
       <one-of>
          <item>no</item>

          <item>nope</item>
          <item>no way</item>
       </one-of>
       <tag>no</tag>

    </rule>
 </grammar>


   In the SignalDetector listener:
  class AppSignalDetectorListener implements MediaEventListener<SignalDetectorE
vent> {
      public void onEvent(SignalDetectorEvent anEvent) {
          if (anEvent instanceof SpeechRecognitionEvent) {
             java.lang.String tag = ((SpeechRecognitionEvent)anEvent).getTag();
             if (tag != null) {
                 if (tag.equals("yes")) {
                    log.debug("customer agrees");
                    ...
                 } else {
                    log.debug("customer disagrees");
                    ...
                 }
             } else {
                 log.debug("Please say yes or no");
                 ...
             }
          }
      }
  }


   More complex applications will use the xml document returned by
   getSemanticResult().

   The parameter SignalDetectorConstants.p_InitialTimeout applies also
   for speech recognition. If no input is detected, getQualifier() will
   return SignalDetectorConstants.q_InitialTimeout.

   If the recognized was unable to match the input with any grammar,
   getQualifier() returns q_NoMatch.
     _________________________________________________________________

   Field Summary
   static Symbol q_NoMatch
             This qualifier indicates that the recognizer could not match
   the input with any grammar.
   static Symbol v_EndOfSpeech
             A predefined pattern that matches the end of a speech
   sequence.
   static Symbol v_StartOfSpeech
             A predefined pattern that matches the start of a speech
   sequence.



   Method Summary
    java.net.URL getSemanticResult()
             Return the URL of a semantic interpretation document, either
   NSLML (http://www.w3.org/TR/nl-spec/) or EMMA
   (http://www.w3.org/TR/emma/).
    java.lang.String getTag()
             Return the tag associated to the user input, as defined by
   SISR.
    java.lang.String getUserInput()
             Return the recognized utterance, as a java.lang.String.



   Methods inherited from interface
   javax.media.mscontrol.mediagroup.signals.SignalDetectorEvent
   getPatternIndex, getSignalBuffer, getSignalString



   Methods inherited from interface
   javax.media.mscontrol.resource.ResourceEvent
   getQualifier, getRTCTrigger



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEvent
   getError, getErrorText, getEventID, getSource



   Field Detail

  v_StartOfSpeech

static final Symbol v_StartOfSpeech

          A predefined pattern that matches the start of a speech
          sequence.
          This Symbol is used as a special pattern value. The pattern
          will be matched by the start of a speech sequence.

          Example:

params.put(p_Pattern[5], v_StartOfSpeech);

          This can be used to generate a SpeechRecognitionEvent, either
          by adding p_Pattern[5] in the patterns argument of
          receiveSignals, or by adding ev_Pattern[5] in the list of
          events subscribed by p_EnabledEvents.
          Additionally, rtcc_Pattern[5] can be used as an RTC trigger.

          See also How to define a Pattern
     _________________________________________________________________

  v_EndOfSpeech

static final Symbol v_EndOfSpeech

          A predefined pattern that matches the end of a speech sequence.
          See v_StartOfSpeech.
     _________________________________________________________________

  q_NoMatch

static final Symbol q_NoMatch

          This qualifier indicates that the recognizer could not match
          the input with any grammar.

   Method Detail

  getTag

java.lang.String getTag()

          Return the tag associated to the user input, as defined by
          SISR. (See http://www.w3.org/TR/semantic-interpretation/)
          Return null if no tag was specified, or if multiple tags were
          matched with various confidence levels. In those cases, the
          semantic result returned by getSemanticResult() must be used.
     _________________________________________________________________

  getUserInput

java.lang.String getUserInput()

          Return the recognized utterance, as a java.lang.String.
          Return null if no utterance was recognized, or if multiple
          utterances were recognized with various confidence levels. In
          those cases, the semantic result returned by
          getSemanticResult() must be used.
     _________________________________________________________________

  getSemanticResult

java.net.URL getSemanticResult()

          Return the URL of a semantic interpretation document, either
          NSLML (http://www.w3.org/TR/nl-spec/) or EMMA
          (http://www.w3.org/TR/emma/). This document describes in detail
          what words were recognized in the utterance, with confidence
          levels, and what is their meaning.
          Return null if no document is provided.
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
package javax.jain.media.mscontrol.mediagroup.signals;
