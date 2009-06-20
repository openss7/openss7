/*
 * ConfigSpec.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.Provider;
import java.util.Dictionary;
import java.util.Hashtable;
import java.io.Serializable;

/**
 * <b>Implementation Note:</b>
 * S410 do not segegate the primary resource.
 * The primary will be one of CCR, SPR, CPR.
 * If a ResourceSpec is supplied for the appropriate
 * primary resource, those attrs/params should be applied.
 * <p>
 * <ul><tt>bindToTerminalName(cs, null);</tt></ul>
 * Where <tt>cs</tt> defines normal 'secondary' resources
 * will create a SPR-headed group suitable for MediaConnection API.
 * <ul><tt>bindToTerminalName(cs, null);</tt></ul> 
 * where <tt>cs</tt> defines a CPR resource
 * will create a CPR-headed group, suitable for MediaConnection API.
 * <ul><tt>bindToTerminaName(cs, "real CCR device")</tt></ul> 
 * or 
 * <ul><tt>bindAndConnect(cs, ...)</tt></ul> 
 * will create the usual CCR-headed group.
 *
 * <ul><tt>bindToServiceName(cs, ...)</tt></ul>
 * binds whatever group is sent to the named service.
 * Presumably the server will detect (and not handoff)
 * if the primary is inappropriate. 
 * (For example, if a CPR is required)
 */

/**
 * Stores the information necessary to configure
 * (or re-configure) a collection of resources.
 * A ConfigSpec is essentially a <i>read-only</i> structure consisting of <ul>
 * <li>an Array of ResourceSpecs,</li>
 * <li>a configuration timeout,</li>
 * <li>a Dictionary of selection attributes and values,</li>
 * <li>a Dictionary of initial parameter settings, and</li>
 * <li>an Array of initial RTC settings.</li></ul>
 * <p>
 * A ConfigSpec object is an <i>argument</i> to 
 * <a href="MediaService.html#bindMethods">the bind methods</a>
 * or 
 * {@link MediaService#configure(ConfigSpec) configure()}
 * where it describes the requested configuration.
 * A ConfigSpec is also the <i>return value</i> from 
 * {@link MediaService#getConfiguration() getConfiguration()}
 * where it describes the current configuration.
 * <p>
 * A ConfigSpec used in configure() or the bind methods
 * is obtained using the constructor, and is not modified thereafter.  
 * A ConfigSpec returned from getConfiguration() is inspected using
 * one of the ConfigSpec field accessor methods.
 * <p>
 * <h4>Timeout:</h4>
 * Timeout indicates how long the application is willing to wait for
 * the configuration process to complete.  
 * The implementation is allowed to wait up to <code>timeout</code>
 * milliseconds for resources to become available.  If the requested 
 * resources are not available at that time, configuration will fail,
 * and the application can continue to interact with the caller using
 * the previously configured resources.
 * <p><b>Note:</b>
 * Timeout may be specified as 
 * <code>{@link ResourceConstants#FOREVER ResourceConstants.FOREVER}</code> 
 * (or -1).
 * Other negative values for timeout may generate an IllegalArgumentException.
 * <p>
 * <h4>Attributes:</h4>
 * When configuring a MediaGroup, the media provider 
 * may have choices regarding how and where the Resources
 * are implemented.  If the application is sensitive
 * to the possible alternatives, it can specify the particular
 * type of implemention required by supplying a Dictionary
 * that defines a value for the attributes of interest.
 * <p>
 * ConfigSpec attributes are also used to specify processing options 
 * for <code>bindAndConnect</code> and related methods.
 * <p>
 * The defined attributes for a ConfigSpec are described
 * in {@link ConfigSpecConstants}.
 * <p>
 * <h4>Pre-defined ConfigSpecs:</h4>
 * Two simple ConfigSpec objects are pre-defined:
 * <table border="1" cellpadding="3">
 * <tr><td><code>{@link #basicConfig}	</code></td>
 * <td>requests the default SignalDetector, SignalGenerator, and
 * alternating use of the default Player and Recorder</td></tr>
 * <tr><td><code>{@link #anyConfig} 	</code></td>
 * <td>requests that no reconfiguration be attempted.</td></tr>
 * </table>
 * <p>
 * <b>Note:</b>
 * <code>ConfigSpec.anyConfig</code> is identified by 
 * <code>(ResourceSpec[] == null).</code><br>
 * An empty ResourceSpec[], that is: 
 * <code>(ResourceSpec[] = new ResourceSpec[])</code> 
 * indicates a configuration with no Resources.
 * <p>
 * <b>Note:</b>
 * This specification does not constrain how or whether the structured
 * sub-components of a ConfigSpec are shared or copied between uses.
 * Application developers are advised that there may be side-effects
 * if an Array or Dictionary used in a ConfigSpec is subsequently 
 * modified.  For example, it would be poor practice to modify
 * the ResourceSpec[] returned from 
 * <code>basicConfig.getResourceSpecs()</code>.
 * Modifications should be made only to <i>copies</i> of such Arrays 
 * or Dictionaries.
 * <p>
 * <b>FAQ:</b>
 * The ConfigSpec returned from <code>getConfiguration</code> is 
 * <i>not</i> the same ConfigSpec used in a previous <code>configure</code>.
 * For example, the ResourceSpec[] in the returned ConfigSpec
 * will contain descriptions of the Resources actually configured. 
 * Also, the Attributes of the ConfigSpec may contain additional 
 * attributes that were not mentioned in the ConfigSpec passed to configure.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
class ConfigSpec implements ConfigSpecConstants, java.io.Serializable {

    /** A name for this ConfigSpec. 
     * This may be used in some ECTF-based implementations.
     * <p>
     * <b>Note:</b> the value for key <tt>Group_ECTF_Config</tt>.
     */
    protected java.lang.String configName = "S410"; // 

    /**
     * This protected variable stores the array of resource specs.
     */
    protected ResourceSpec[] resSpecs;	// array of ResourceSpecs
    /** timeout for this ConfigSpec. */
    protected int csTimeout = 2000; 	// total time to wait, if config
					// is not complete, abort the attempt.
    /** array of persistant RTCs, included in every media operation. */
    protected RTC[] rtcs;		// array of persistant RTCs.
    // persistant RTCs are set on every media operation.

    /**
     * Parameter setting to be applied during <code>configure</code>.
     */
    protected Dictionary parameters = null; // parameter settings to be applied.

    /**
     * Attributes that control binding, resource selection 
     * and MediaGroup processing. 
     * Attributes are used to constrain the choice of Resource
     * implementation and control the overall MediaGroup processing.
     */
    protected Dictionary attributes = null;	// attributes to be used.


    /**
     * Elide package name, include getConfigName(). 
     * @return a java.lang.String that identifies this ConfigSpec instance.
     */
    public java.lang.String toString() {
	java.lang.String fullName = this.getClass().getName();
	java.lang.String shortName = fullName.substring( fullName.lastIndexOf('.') + 1 );
	java.lang.String cname = getConfigName();
	return shortName+"@"+hashCode()+"{"+((cname==null)?"null":cname)+"}";
    }
    
    /**
     * Construct a ConfigSpec and set the configName.
     * <p>
     * The <tt>configName</tt> is a java.lang.String name that identifies
     * this configuration.  
     *
     * @param configName a java.lang.String to name this configSpec
     * @param timeout int number of milliseconds to wait for resources.
     * <code>configure()</code> should complete or abort in this time.
     * @param specs an array of ResourceSpec
     * @param attributes Dictionary of binding and selection attributes.
     * @param parameters Dictionary of initial configuration parameters.
     * @param rtcs Array of persistant RTCs.
     */
    public ConfigSpec(java.lang.String configName,
		      int timeout, 		// 
		      ResourceSpec[] specs, 	// resource specs
		      Dictionary attributes,	// selection attributes
		      Dictionary parameters,	// configuration parameters
		      RTC[] rtcs 		// persistant RTC settings
		      ) {
	this(specs, timeout, attributes, parameters, rtcs);
	this.configName = configName;
    }

    /**
     * Copy a ConfigSpec and set the configName and timeout.
     * <p>
     * The <tt>configName</tt> is a java.lang.String name that identifies
     * this configuration.  
     *
     * @param configName a java.lang.String to name this configSpec
     * @param timeout int number of milliseconds to wait for resources.
     * <code>configure()</code> should complete or abort in this time.
     * @param configSpec use RessourceSpec[], Attributes, Parameters and RTC[] from here.
     */
    public ConfigSpec(java.lang.String configName,
		      int timeout,
		      ConfigSpec configSpec // template
		      ) {
	this(configName, timeout, configSpec.resSpecs, configSpec.attributes,
	     configSpec.parameters, configSpec.rtcs);
    }


    /**
     * Constructor with full range of arguments.
     * <p>
     * When <code>specs</code> is <code>null</code>, the ConfigSpec specifies
     * that {@link #anyConfig} configuration of Resources is acceptable.
     * <p>
     * If the other arguments are <code>null</code>, 
     * they are treated the same as empty collections.
     *
     * @param specs an array of ResourceSpec
     * @param timeout int number of milliseconds to wait for resources.
     * <code>configure()</code> should complete or abort in this time.
     * @param attributes Dictionary of binding and selection attributes.
     * @param parameters Dictionary of initial configuration parameters.
     * @param rtcs Array of persistant RTCs.
     */
    public ConfigSpec(ResourceSpec[] specs, 
		      int timeout, 		// 
		      Dictionary attributes,	// selection attributes
		      Dictionary parameters,	// configuration parameters
		      RTC[] rtcs 		// persistant RTC settings
		      ) {
	this.resSpecs = specs;
	this.csTimeout = timeout;
	this.attributes = attributes;
	this.parameters = parameters; // run group.setParameters(paramters);
	this.rtcs = rtcs;
    }

    /** 
     * Return the java.lang.String name of this ConfigSpec.
     * May return <tt>null</tt> if no name is assigned.
     * 
     * @return the java.lang.String name of this ConfigSpec.
     */
    public java.lang.String getConfigName() {
	return configName;
    }

    /**
     * Returns the internal timeout value.
     * @return the int number of milliseconds to wait for resources.
     */
    public int getTimeout() {
	return csTimeout;
    }

    /**
     * Retrieves the ResourceSpec[] from this ConfigSpec.
     * <p>
     * @return the array of ResourceSpec objects. 
     */
    public ResourceSpec[] getResourceSpecs() {
	return resSpecs;
    }

    /**
     * Gets the Dictionary of attributes from this ConfigSpec.
     *
     * @return the Dictionary of selection/configuration attributes.
     */
    public Dictionary getAttributes() {
	return attributes;
    }
    
    /**
     * Gets the Dictionary of parameters from this ConfigSpec.
     *
     * @return the Dictionary of initial or current parameters
     */
    public Dictionary getParameters() {
	return parameters;
    }

    /**
     * Gets the persistant RTC[] for this ConfigSpec.
     *
     * @return the RTC[] of persistant RTCs.
     */
    public RTC[] getRTC() {
	return rtcs;
    }

    private static final ResourceSpec[] emptyResourceSpec = {};

    // This object can be modified! 
    // Should offer a constructor to create a clean version?
    // No: let the developers protect themselves.
    /** ConfigSpec for a MediaGroup with no Resources. */
    public static final 
	ConfigSpec emptyConfig = new ConfigSpec("EmptyConfig", 100, emptyResourceSpec, null, null, null);


    /** used for ConfigSpec.basic() */
    private static final ResourceSpec[] basicResourceSpecs = {
	ResourceSpec.basicAltPlayerRecorder, 
	ResourceSpec.basicSignalDetector,
	ResourceSpec.basicSignalGenerator
    };
    
    // should this be public, to replace the "final" static field?
    /**
     * Returns a clean copy of a basic ConfigSpec.
     * This basic ConfigSpec waits 12 seconds to allocate
     * {@link ResourceSpec#basicAltPlayerRecorder basicAltPlayerRecorder}, 
     * {@link ResourceSpec#basicSignalDetector basicSignalDetector}, and
     * {@link ResourceSpec#SignalGenerator SignalGenerator}.
     * @return a clean copy of a basic ConfigSpec.
     */
    private static ConfigSpec getBasicConfig() {
	return new ConfigSpec("BasicConfig", 12000, basicResourceSpecs, null, null, null);
    }

    /**
     * A basic ConfigSpec. 
     * This basic ConfigSpec waits 2 seconds to allocate
     * {@link ResourceSpec#basicAltPlayerRecorder}, 
     * {@link ResourceSpec#basicSignalDetector}, and
     * {@link ResourceSpec#basicSignalGenerator}.
     */
    public static final ConfigSpec basicConfig = getBasicConfig();

    /**
     * A ConfigSpec instance that specifes that any configuration 
     * of Resources is acceptable.
     * Informs the server that it should not do any reconfiguration.
     * <p>
     * <code>ConfigSpec.anyConfig</code> is identified 
     * by <code>(ResourceSpec[] == null)</code>.
     */
    public static final ConfigSpec anyConfig = new ConfigSpec("AnyConfig", 1, null, null, null, null);

}
