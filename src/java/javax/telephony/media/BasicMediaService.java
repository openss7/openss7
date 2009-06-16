/*
 * BasicMediaService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media;

import javax.telephony.*;	// get core objects and Exceptions
import javax.telephony.media.async.*;
import javax.telephony.media.provider.*;
//import javax.telephony.media.provider.Request;
//import javax.telephony.media.provider.Base_MediaService;
import java.util.Dictionary;
import java.lang.reflect.Method;

// Generated: %?% %D% %?% Fri Oct 18 15:15:06 PDT 2002% %?%
/* %?gen% 
%(import javax.telephony.media 
  javax.telephony.media.provider
  javax.telephony.media.async
  javax.telephony
  java.util)%
%?gen% 
%?gen% 
*/
/**
 * Implements MediaService and the basic Resource interfaces.
 * Resource methods are delegated to the bound MediaGroup.
 * <p>
 * This class understands about binding and configuration,
 * and knows how to delegate resource methods to whatever
 * object or objects are bound to this one. And conversely
 * understand how to register Listeners for each resource,
 * and dispatch events from any of the resources 
 * to all of the Resource.Listeners.
 * <p>
 * The Resource methods in BasicMediaService are documented
 * in the associated Resource interface definition.
 * <P>
 * <b>Note:</b>
 * This class is documented as extending <tt>Base_MediaService</tt>.
 * However, actual implementations may extend some other vendor specific class.
 * In all cases, this class (or one of its super-classes)
 * implements MediaService and the defined Resource interfaces.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public class BasicMediaService extends Base_MediaService
    implements MediaService, 
	       TTSPlayer, 
	       Recorder, 
	       SignalDetector, 
	       SignalGenerator,
	       Async.MediaService,
	       Async_TTSPlayer,
	       Async_Recorder,
	       Async_SignalDetector,
	       Async_SignalGenerator
{
    /**
     * Get a MediaProvider from the given Strings.
     * <p>
     * If the given <tt>peerName</tt> is <tt>null</tt>: 
     * <ol><li>
     * Check <tt>System.getProperty("Jtapi.MediaPeer")</tt>.
     * <br>If that is non-null, instantiate that MediaPeer.
     * </li><li>
     * Check <tt>System.getProperty("Jtapi.Peer", "DefaultJtapiPeer")</tt>,
     * <br>See if that <tt>JtapiPeer.getProvider()</tt> produces a MediaPeer.
     * <br>[this step may be elided if Jtapi integration is not expected]
     * </li><li>
     * Check <tt>"javax.telephony.media.provider.NullMediaPeer"</tt>.
     * <br>That should produce <tt>NullMediaProvider</tt>.
     * </li></ol>
     * <p>
     * <b>Note:</b> This method can be used as a MediaProvider Factory.
     * For example: <br>
     * <tt>MediaProvider mp = BasicMediaService.findMediaProvider(...);</tt>
     * 
     * @param peerName name of the MediaPeer Class
     * @param providerString a String that identifies
     * the (MediaPeer-specific) MediaProvider and other information.
     * 
     * @throws ClassNotFoundException if MediaPeer class is not found
     * @throws IllegalAccessException if MediaPeer class is not accessible
     * @throws InstantiationException if MediaPeer class does not instantiate
     * @throws ProviderUnavailableException if Provider is not available
     * @throws ClassCastException if Provider instance is not a MediaProvider
     * @return a MediaProvider
     */ 
    public static 
	MediaProvider findMediaProvider(String peerName, String providerString)
	throws ClassNotFoundException, 
	       InstantiationException,
	       IllegalAccessException,
	       ProviderUnavailableException {
	return Base_MediaService.findMediaProvider(peerName, providerString);
    }

    /**
     * Create an unbound MediaService,
     * using the installation default MediaProvider.
     */
    public BasicMediaService() {super();}

    /**
     * Create an unbound MediaService, using the given MediaProvider object.
     * The MediaProvider object may be obtained from:
     * <ul><li>{@link #findMediaProvider(String,String) 
		       findMediaProvider(peerName, providerString)};</li>
     * <li>a JTAPI core Provider, by casting, 
     *     if that Provider implements MediaProvider</li>
     * <li>by other vendor-specific means.</li>
     * </ul>
     *
     * @param provider a MediaProvider instance
     */
    public BasicMediaService(MediaProvider provider) {
	super(provider);}
    
    /**
     * Create an unbound MediaService, using a MediaProvider
     * identified by the two String arguments.
     * <p>
     * <tt>peerName</tt> names a Class that implements <tt>MediaPeer</tt>.
     * If <tt>peerName</tt> is <tt>null</tt>, several 
     * alternatives are tried:
     * <ol><li>
     * Check <tt>System.getProperty("Jtapi.MediaPeer")</tt>.
     * If that is non-null, instantate that MediaPeer.
     * </li><li>
     * Check <tt>System.getProperty("Jtapi.Peer", "DefaultJtapiPeer")</tt>,
     * See if that <tt>JtapiPeer.getProvider()</tt> produces a MediaPeer.
     * <br><b>Note:</b>
     * This step may be elided if Jtapi integration is not expected.
     * </li><li>
     * Check <tt>"javax.telephony.media.provider.NullMediaPeer"</tt>.
     * That may produce <tt>NullMediaProvider</tt>.
     * </li></ol>
     * <p>
     * The format of a <code>providerString</code> 
     * as used to obtain a MediaProvider from the MediaPeer
     * is vendor specific.
     * <p>
     * The format of a <code>providerString</code> 
     * as used to obtain a Jtapi [core/Call] Provider 
     * is documented in 
     * {@link JtapiPeer#getProvider(String) JtapiPeer.getProvider(String)}.
     *
     * @param peerName the name of a Class that implements MediaPeer.
     * @param providerString a "login" string for that MediaPeer.
     *
     * @throws ClassNotFoundException if MediaPeer class is not found
     * @throws IllegalAccessException if MediaPeer class is not accessible
     * @throws InstantiationException if MediaPeer class does not instantiate
     * @throws ProviderUnavailableException if Provider is not available
     * @throws ClassCastException if Provider instance is not a MediaProvider
     */
    public BasicMediaService(String peerName, String providerString) 
	throws ClassNotFoundException, 
    	       InstantiationException, 
    	       IllegalAccessException, 
    	       ProviderUnavailableException {
	super(peerName, providerString);
     }

    /*
     * Javadoc should supply a pointer to actual documentation.
     * 
     * Following is template and the generated methods:
     * %?gen%
     *%/
%(implements TTSPlayer Recorder SignalDetector SignalGenerator)%
%(javagen "codegen4.scm")%
%-(big-time hack: should slice up the methName, but this works [for now])%
%(defun retfunc (m) 
 (if (String.equals m "void") "return"
 (if (String.endsWith m "Event") "return event"
 "return %"
 )))%
%(define return (maparray String retfunc retnType))%
%$methods<
    // @see %$declClass#%$methName(%, %$parmType)
    public %$retnType %$methName(%, %<%$parmType %$localvar%>)%<%Z<
	throws %>%,
	       %;%$excpType%> 
    {
	Async_%$retnType event = async_%$methName(%, %$localvar);
	event.throwIfMediaResourceException();
	%$return;
    }
%>%;// %?gen%
     */

    // @see Player#play(String, int, RTC[], Dictionary)
    public PlayerEvent play(String string0, int int1, RTC[] rtc2, Dictionary dictionary3)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_play(string0, int1, rtc2, dictionary3);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Player#play(String[], int, RTC[], Dictionary)
    public PlayerEvent play(String[] string0, int int1, RTC[] rtc2, Dictionary dictionary3)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_play(string0, int1, rtc2, dictionary3);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Player#adjustPlayerSpeed(Symbol)
    public PlayerEvent adjustPlayerSpeed(Symbol symbol0)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_adjustPlayerSpeed(symbol0);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Player#adjustPlayerVolume(Symbol)
    public PlayerEvent adjustPlayerVolume(Symbol symbol0)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_adjustPlayerVolume(symbol0);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Player#jumpPlayer(Symbol)
    public PlayerEvent jumpPlayer(Symbol symbol0)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_jumpPlayer(symbol0);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Player#pausePlayer()
    public PlayerEvent pausePlayer()
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_pausePlayer();
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Player#resumePlayer()
    public PlayerEvent resumePlayer()
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_resumePlayer();
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Player#stopPlayer()
    public PlayerEvent stopPlayer()
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_stopPlayer();
	event.throwIfMediaResourceException();
	return event;
    }

    // @see TTSPlayer#loadTTSDictionary(String[], Dictionary)
    public PlayerEvent loadTTSDictionary(String[] string0, Dictionary dictionary1)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_loadTTSDictionary(string0, dictionary1);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see TTSPlayer#activateTTSDictionary(String[], Dictionary)
    public PlayerEvent activateTTSDictionary(String[] string0, Dictionary dictionary1)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_activateTTSDictionary(string0, dictionary1);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see TTSPlayer#deactivateTTSDictionary(String[], Dictionary)
    public PlayerEvent deactivateTTSDictionary(String[] string0, Dictionary dictionary1)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_deactivateTTSDictionary(string0, dictionary1);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see TTSPlayer#unloadTTSDictionary(String[], Dictionary)
    public PlayerEvent unloadTTSDictionary(String[] string0, Dictionary dictionary1)
	throws MediaResourceException 
    {
	Async_PlayerEvent event = async_unloadTTSDictionary(string0, dictionary1);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Recorder#record(String, RTC[], Dictionary)
    public RecorderEvent record(String string0, RTC[] rtc1, Dictionary dictionary2)
	throws MediaResourceException 
    {
	Async_RecorderEvent event = async_record(string0, rtc1, dictionary2);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Recorder#pauseRecorder()
    public RecorderEvent pauseRecorder()
	throws MediaResourceException 
    {
	Async_RecorderEvent event = async_pauseRecorder();
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Recorder#resumeRecorder()
    public RecorderEvent resumeRecorder()
	throws MediaResourceException 
    {
	Async_RecorderEvent event = async_resumeRecorder();
	event.throwIfMediaResourceException();
	return event;
    }

    // @see Recorder#stopRecorder()
    public RecorderEvent stopRecorder()
	throws MediaResourceException 
    {
	Async_RecorderEvent event = async_stopRecorder();
	event.throwIfMediaResourceException();
	return event;
    }

    // @see SignalDetector#flushBuffer()
    public SignalDetectorEvent flushBuffer()
	throws MediaResourceException 
    {
	Async_SignalDetectorEvent event = async_flushBuffer();
	event.throwIfMediaResourceException();
	return event;
    }

    // @see SignalDetector#retrieveSignals(int, Symbol[], RTC[], Dictionary)
    public SignalDetectorEvent retrieveSignals(int int0, Symbol[] symbol1, RTC[] rtc2, Dictionary dictionary3)
	throws MediaResourceException 
    {
	Async_SignalDetectorEvent event = async_retrieveSignals(int0, symbol1, rtc2, dictionary3);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see SignalGenerator#sendSignals(Symbol[], RTC[], Dictionary)
    public SignalGeneratorEvent sendSignals(Symbol[] symbol0, RTC[] rtc1, Dictionary dictionary2)
	throws MediaResourceException 
    {
	Async_SignalGeneratorEvent event = async_sendSignals(symbol0, rtc1, dictionary2);
	event.throwIfMediaResourceException();
	return event;
    }

    // @see SignalGenerator#sendSignals(String, RTC[], Dictionary)
    public SignalGeneratorEvent sendSignals(String string0, RTC[] rtc1, Dictionary dictionary2)
	throws MediaResourceException 
    {
	Async_SignalGeneratorEvent event = async_sendSignals(string0, rtc1, dictionary2);
	event.throwIfMediaResourceException();
	return event;
    }
// %?gen%

/*
 * Now the Async methods. %?gen%
 *%/	 
%(implements 
  Async_TTSPlayer Async_Recorder
  Async_SignalDetector Async_SignalGenerator)%
%(javagen "codegen4.scm")%
%(define Base_retnType (changeName retnType "Async_" "Base_"))%
%(define baseName (changeName methName "async_" ""))%
%(define ev_Name (prefixName baseName "ev_" true))%
%$methods<
    static final Class[] argTypes%I = {%, %<%$parmType%.class%>};
    static final Method method%I = Request.theMethod((%$declClass%.class), "%$methName", argTypes%I);

    // @see %$declClass#%$methName(%, %$parmType)
    public %$retnType %$methName(%, %<%$parmType %$localvar%>)
    {
	Request request = new Request(method%I%,, %$localvar);
	return (%$retnType)invokeGroupMethod(request);
    }
%>%;// %?gen%
 */	 

    static final Class[] argTypes0 = {String.class, int.class, RTC[].class, Dictionary.class};
    static final Method method0 = Request.theMethod((Async_Player.class), "async_play", argTypes0);

    // @see Async_Player#async_play(String, int, RTC[], Dictionary)
    public Async_PlayerEvent async_play(String string0, int int1, RTC[] rtc2, Dictionary dictionary3)
    {
	Request request = new Request(method0, string0, int1, rtc2, dictionary3);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes1 = {String[].class, int.class, RTC[].class, Dictionary.class};
    static final Method method1 = Request.theMethod((Async_Player.class), "async_play", argTypes1);

    // @see Async_Player#async_play(String[], int, RTC[], Dictionary)
    public Async_PlayerEvent async_play(String[] string0, int int1, RTC[] rtc2, Dictionary dictionary3)
    {
	Request request = new Request(method1, string0, int1, rtc2, dictionary3);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes2 = {Symbol.class};
    static final Method method2 = Request.theMethod((Async_Player.class), "async_adjustPlayerSpeed", argTypes2);

    // @see Async_Player#async_adjustPlayerSpeed(Symbol)
    public Async_PlayerEvent async_adjustPlayerSpeed(Symbol symbol0)
    {
	Request request = new Request(method2, symbol0);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes3 = {Symbol.class};
    static final Method method3 = Request.theMethod((Async_Player.class), "async_adjustPlayerVolume", argTypes3);

    // @see Async_Player#async_adjustPlayerVolume(Symbol)
    public Async_PlayerEvent async_adjustPlayerVolume(Symbol symbol0)
    {
	Request request = new Request(method3, symbol0);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes4 = {Symbol.class};
    static final Method method4 = Request.theMethod((Async_Player.class), "async_jumpPlayer", argTypes4);

    // @see Async_Player#async_jumpPlayer(Symbol)
    public Async_PlayerEvent async_jumpPlayer(Symbol symbol0)
    {
	Request request = new Request(method4, symbol0);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes5 = {};
    static final Method method5 = Request.theMethod((Async_Player.class), "async_pausePlayer", argTypes5);

    // @see Async_Player#async_pausePlayer()
    public Async_PlayerEvent async_pausePlayer()
    {
	Request request = new Request(method5);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes6 = {};
    static final Method method6 = Request.theMethod((Async_Player.class), "async_resumePlayer", argTypes6);

    // @see Async_Player#async_resumePlayer()
    public Async_PlayerEvent async_resumePlayer()
    {
	Request request = new Request(method6);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes7 = {};
    static final Method method7 = Request.theMethod((Async_Player.class), "async_stopPlayer", argTypes7);

    // @see Async_Player#async_stopPlayer()
    public Async_PlayerEvent async_stopPlayer()
    {
	Request request = new Request(method7);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes8 = {String[].class, Dictionary.class};
    static final Method method8 = Request.theMethod((Async_TTSPlayer.class), "async_loadTTSDictionary", argTypes8);

    // @see Async_TTSPlayer#async_loadTTSDictionary(String[], Dictionary)
    public Async_PlayerEvent async_loadTTSDictionary(String[] string0, Dictionary dictionary1)
    {
	Request request = new Request(method8, string0, dictionary1);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes9 = {String[].class, Dictionary.class};
    static final Method method9 = Request.theMethod((Async_TTSPlayer.class), "async_activateTTSDictionary", argTypes9);

    // @see Async_TTSPlayer#async_activateTTSDictionary(String[], Dictionary)
    public Async_PlayerEvent async_activateTTSDictionary(String[] string0, Dictionary dictionary1)
    {
	Request request = new Request(method9, string0, dictionary1);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes10 = {String[].class, Dictionary.class};
    static final Method method10 = Request.theMethod((Async_TTSPlayer.class), "async_deactivateTTSDictionary", argTypes10);

    // @see Async_TTSPlayer#async_deactivateTTSDictionary(String[], Dictionary)
    public Async_PlayerEvent async_deactivateTTSDictionary(String[] string0, Dictionary dictionary1)
    {
	Request request = new Request(method10, string0, dictionary1);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes11 = {String[].class, Dictionary.class};
    static final Method method11 = Request.theMethod((Async_TTSPlayer.class), "async_unloadTTSDictionary", argTypes11);

    // @see Async_TTSPlayer#async_unloadTTSDictionary(String[], Dictionary)
    public Async_PlayerEvent async_unloadTTSDictionary(String[] string0, Dictionary dictionary1)
    {
	Request request = new Request(method11, string0, dictionary1);
	return (Async_PlayerEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes12 = {String.class, RTC[].class, Dictionary.class};
    static final Method method12 = Request.theMethod((Async_Recorder.class), "async_record", argTypes12);

    // @see Async_Recorder#async_record(String, RTC[], Dictionary)
    public Async_RecorderEvent async_record(String string0, RTC[] rtc1, Dictionary dictionary2)
    {
	Request request = new Request(method12, string0, rtc1, dictionary2);
	return (Async_RecorderEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes13 = {};
    static final Method method13 = Request.theMethod((Async_Recorder.class), "async_pauseRecorder", argTypes13);

    // @see Async_Recorder#async_pauseRecorder()
    public Async_RecorderEvent async_pauseRecorder()
    {
	Request request = new Request(method13);
	return (Async_RecorderEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes14 = {};
    static final Method method14 = Request.theMethod((Async_Recorder.class), "async_resumeRecorder", argTypes14);

    // @see Async_Recorder#async_resumeRecorder()
    public Async_RecorderEvent async_resumeRecorder()
    {
	Request request = new Request(method14);
	return (Async_RecorderEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes15 = {};
    static final Method method15 = Request.theMethod((Async_Recorder.class), "async_stopRecorder", argTypes15);

    // @see Async_Recorder#async_stopRecorder()
    public Async_RecorderEvent async_stopRecorder()
    {
	Request request = new Request(method15);
	return (Async_RecorderEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes16 = {int.class, Symbol[].class, RTC[].class, Dictionary.class};
    static final Method method16 = Request.theMethod((Async_SignalDetector.class), "async_retrieveSignals", argTypes16);

    // @see Async_SignalDetector#async_retrieveSignals(int, Symbol[], RTC[], Dictionary)
    public Async_SignalDetectorEvent async_retrieveSignals(int int0, Symbol[] symbol1, RTC[] rtc2, Dictionary dictionary3)
    {
	Request request = new Request(method16, int0, symbol1, rtc2, dictionary3);
	return (Async_SignalDetectorEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes17 = {};
    static final Method method17 = Request.theMethod((Async_SignalDetector.class), "async_flushBuffer", argTypes17);

    // @see Async_SignalDetector#async_flushBuffer()
    public Async_SignalDetectorEvent async_flushBuffer()
    {
	Request request = new Request(method17);
	return (Async_SignalDetectorEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes18 = {String.class, RTC[].class, Dictionary.class};
    static final Method method18 = Request.theMethod((Async_SignalGenerator.class), "async_sendSignals", argTypes18);

    // @see Async_SignalGenerator#async_sendSignals(String, RTC[], Dictionary)
    public Async_SignalGeneratorEvent async_sendSignals(String string0, RTC[] rtc1, Dictionary dictionary2)
    {
	Request request = new Request(method18, string0, rtc1, dictionary2);
	return (Async_SignalGeneratorEvent)invokeGroupMethod(request);
    }

    static final Class[] argTypes19 = {Symbol[].class, RTC[].class, Dictionary.class};
    static final Method method19 = Request.theMethod((Async_SignalGenerator.class), "async_sendSignals", argTypes19);

    // @see Async_SignalGenerator#async_sendSignals(Symbol[], RTC[], Dictionary)
    public Async_SignalGeneratorEvent async_sendSignals(Symbol[] symbol0, RTC[] rtc1, Dictionary dictionary2)
    {
	Request request = new Request(method19, symbol0, rtc1, dictionary2);
	return (Async_SignalGeneratorEvent)invokeGroupMethod(request);
    }
// %?gen%
}
// %?% Generator done: %(println (new Date))% %?% Generator done: % %?%
