/*
 * ResourceSpec.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.util.Dictionary;

/**
 * Describes the attributes and other information for configuring Resources.
 * A media Resource is described by three items:<ul>
 * <li>a Symbol that denotes the Resource Class, </li>
 * <li>a Dictionary of selection <i>attributes</i>, and</li>
 * <li>a Dictionary of the initial <i>parameter</i> settings.</li></ul>
 * <p>
 * <h4>Allocation Operators:</h4>
 * Alternatively, a ResourceSpec may be a collection of nested
 * ResourceSpec objects related by an allocation operator.
 * The allocation operators are:
 * <tt>And</tt>, <tt>Alt</tt>, <tt>Or</tt>, <tt>Future</tt>, <tt>Add</tt>.
 * Each operator is represented by an inner subclass of ResourceSpec.
 * <p>
 * <b>Note:</b> operator ResourceSpecs do not have a name,
 * attributes or parameters.
 * <p>
 * <h4>Selection Attributes:</h4>
 * Each resource implementation is registered with a collection
 * of Attributes that describe the features or capabilities of 
 * that implementation. 
 * Selection attributes specify the features or capabilities
 * that an application expects or relies on.
 * During configuration, only Resource implementations that have the 
 * requested Attributes are allocated and connected into the group.
 * <p>
 * For example, a Player that is capable of speed changing and that
 * implements the speed changing API, RTCs, etc. would register the
 * attribute: <tt>(Player.a_Speed = TRUE)</tt>. 
 * An application that wants to use
 * the speed change features during <code>play()</code> should request
 * a Player with Attribute: <tt>(Player.a_Speed = TRUE)</tt>
 * by including it in the <tt>selectionAttributes</tt> Dictionary
 * when constructing the ResourceSpec.
 * That is:<pre>
 *	Dictionary attrs = new Hashtable(17);
 *	attrs.put(Player.a_Speed, Boolean.TRUE);
 * 	new ResourceSpec( "SpeedPlayer", Player.v_Class, attrs, null);</pre>
 * <p>
 * The Attributes of a given resouce implementation are fixed 
 * ('final' as one would say in Java). If a resource is configured
 * with <tt>(a_Speed=TRUE)</tt>,
 * then <tt>a_Speed</tt> will always report <tt>TRUE</tt>.
 * <p>
 * <h4>Attributes versus Parameters</H4>
 * In contrast, the amount of speed change to be invoked when the
 * speed is changed is a <i>variable</i> controlled by the application.
 * Application controllable settings are called <b>parameters</b>.
 * Applications can set the parameters of a resource using 
 * {@link MediaService#setParameters(Dictionary) MediaService.setParameters()}
 * or pre-set them during configuration by including them in the
 * <tt>initialParameters</tt> of a ResourceSpec.
 * <p>
 * <b>Note:</b>
 * This specification does not constrain how or whether the structured
 * sub-components of a ResourceSpec are shared or copied between uses.
 * Application developers are advised that there may be side-effects
 * if an Array or Dictionary used in a ResourceSpec is subsequently 
 * modified.  For example, it would be poor practice to modify the
 * <tt>ResourceSpec[]</tt> that was used in the constructor to <tt>Alt()</tt>.
 * Modifications should be made only to <i>copies</i> of such Arrays 
 * or Dictionaries.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public class ResourceSpec {
    /*
     * <b>Implementors Note:</b>
     * When externalizing a ResourceSpec into S.200, each ResourceSpec
     * becomes an entry in the <tt>Group_ConfigSpec</tt> KVSet.
     * <br>The <b>key</b> for a ResourceSpec is the <tt>resourceSymbol</tt>
     * <br>The <b>value</b> is a KVSet containing KV pairs
     * for the resSpecName, Attributes, Parameters, and RTCs.
     */

    /**
     * String to identify this particular ResourceSpec.
     * Useful when processing alternative choices.
     * May help app recognize which choice was taken.
     */
    private String resSpecName = "";

    /** Private field to store the ResourceClass Symbol. */
    private Symbol resourceSymbol;
    
    /** Private field to store the selection attributes */
    private Dictionary selectionAttributes;

    /** Private field to store the initial parameters. */
    private Dictionary initialParameters; 

    /**
     * Elide package name, include getName(). 
     * @return a String that identifies this ResourceSpec instance.
     */
    public String toString() {
	String fullName = this.getClass().getName();
	String shortName = fullName.substring( fullName.lastIndexOf('.') + 1 );
	String cname = getName();
	return shortName+"@"+hashCode()+"("+((cname==null)?"null":cname)+")";
    }
   
    /**
     * Constructor with a name String, the resource Symbol, 
     * selection attributes and initial parameters.
     * <p>
     * The <tt>resSpecName</tt> is useful when processing alternative choices;
     * the name helps the application recognize which choice was taken.
     * <p>
     * A <tt>resourceSymbol</tt> is defined for each standard 
     * Resource interface, typically with the field name <tt>v_Class</tt>.
     * For example, <tt>Player.v_Class</tt> or <tt>ESymbol.Player_ResourceClass</tt>.
     * <p>
     * If the other arguments are <code>null</code>, 
     * they are treated the same as empty collections.
     *
     * @param resSpecName a String to identify this ResourceSpec.
     * @param resourceSymbol the Symbol for the requested Resource interface. 
     * @param selectionAttributes a Dictionary containing required values
     * for Attributes of the Resource implementation.
     * @param initialParameters a Dictionary of initial parameter settings
     * for this resource.
     */
    public ResourceSpec(String resSpecName,
			Symbol resourceSymbol,
			Dictionary selectionAttributes, 
			Dictionary initialParameters) {
	this.resSpecName = resSpecName;
	this.resourceSymbol = resourceSymbol;
	this.selectionAttributes = selectionAttributes; 
	this.initialParameters = initialParameters;
    }
    
    /**
     * Constructor using the resource class, selection attributes
     * and initial parameters.
     * <p>
     * The resouceClass must be a valid Resource <code>Class</code> object. 
     * If the other arguments are <code>null</code>, 
     * they are treated the same as empty collections.
     * <p>
     * <i>Deprecated because it is inadaquate/ambiguous whether
     * to specify the simple/sync interface or the async interface.
     * For example: <tt>Player.class</tt> or <tt>Async_Player.class</tt>
     * </i>
     * <p>
     * @param resourceClass the Class object for the requested Resource interface. 
     * @param selectionAttributes a Dictionary containing required values
     * for Attributes of the Resource implementation.
     * @param initialParameters a Dictionary of initial parameter settings
     * for this resource.
     * 
     * @deprecated as of 1.4 use the
     * {@link #ResourceSpec(String,Symbol,Dictionary,Dictionary) <tt>resourceSymbol</tt> }
     * constructor
     */
    public ResourceSpec(Class resourceClass, // deprecated
			Dictionary selectionAttributes, 
			Dictionary initialParameters) {
	if(!Resource.class.isAssignableFrom(resourceClass))
	    throw new ClassCastException(resourceClass+" is not a Resource Class");
	this.resourceClass = resourceClass;
	// half-hearted attempt to find the resourceSymbol:
	try {
	    this.resourceSymbol = (Symbol)resourceClass
		.getField("v_Class").get(resourceClass);
	} catch (Exception ex) {
	    throw new IllegalArgumentException("Bad resourceClass "+resourceClass);
	}
	this.selectionAttributes = selectionAttributes; 
	this.initialParameters = initialParameters;
    }
    
    /**
     * Constructor using the resource name, selection attributes
     * and initial parameters.
     * This form of the constructor defers .class lookup until runtime.
     * <p>
     * The <tt>resourceClassName</tt> must identify a valid Resource class. 
     * The <tt>selectionAttributes</tt> and <tt>initialParameters</tt>
     * may be <code>null</code>, which is treated as an empty Dictionary.
     *
     * @param resourceClassName the FQCN of the requested Resource interface. 
     * @param selectionAttributes a Dictionary containing required values
     * for Attributes of the Resource implementation.
     * @param initialParameters a Dictionary of initial parameter settings
     * for this resource.
     *
     * @throws ClassNotFoundException if class is not found
     * @deprecated as of 1.4 use the
     * {@link #ResourceSpec(String,Symbol,Dictionary,Dictionary) <tt>resourceSymbol</tt> }
     * constructor
     */
    public ResourceSpec(String resourceClassName, // deprecated
			Dictionary selectionAttributes, 
			Dictionary initialParameters)
	throws ClassNotFoundException {
	    this(Class.forName(resourceClassName),
		 selectionAttributes, 
		 initialParameters);
	}
    
    /**
     * Returns the String name assigned to this ResourceSpec.
     * @return the String name assigned to this ResourceSpec.
     */
    public String getName() {
	return resSpecName;
    }
    
    /**
     * Returns the Symbol for the ResourceClass of this ResourceSpec.
     * <p>
     * @return The Symbol representing the requested Resource interface.
     */
    public Symbol getResourceSymbol() {
	return resourceSymbol;
    }
    
    /** deprecated, obsolete. */
    private Class resourceClass = null;
    /**
     * Returns the Class of the media resource associated with this object.
     * <p>
     * @return The Class object representing the requested Resource interface.
     * @deprecated use getResourceSymbol()
     */
    public Class getResourceClass() { // deprecated
	return resourceClass;
    }
    
    /**
     * Get the Dictionary of additional selection attributes.
     * @return a Dictionary containing required values
     * for Attributes of the Resource implementation.
     */
    public Dictionary getSelectionAttributes() {
	return selectionAttributes;
    }

    /**
     * Get the Dictionary of initial parameters.
     * @return a Dictionary of initial parameter settings
     * for this resource.
     */
    public Dictionary getInitialParameters() {
	return initialParameters;
    }
    
    /** 
     * Represents the ResourceSpec operator nodes: And, Alt, Or, Add, Future.
     * Non-Operator ResourceSpecs are the "leaf-nodes".
     * <p>
     * Note that these ResourceSpec nodes have <code>null</code>
     * for Attributes and Parameters.
     * <p>
     * Operator implements Resource, so that the first argument
     * to the ResourceSpec constructor is a Resource Class...
     */
    public static abstract class Operator extends ResourceSpec
	implements Resource {
	/* Implements Resource so we can use super constructor.
	 * If this causes trouble, take it out: change constructor 
	 * to check for resourceClass isa Resource or Operator.
	 */

	/** a Resource must have a v_Class field.
	 * <p>
	 * <i>obsolete/deprecated, use <tt>resourceSymbol</tt> ctor</i>
	 */
	public static final Symbol v_Class = ESymbol.KVS_Operator;

	/** The array of ResourceSpec objects. */
	protected ResourceSpec[] resSpecs;

	/** 
	 * Unused/obsolete public contstructor for abstract class.
	 * 
	 * @param resSpecs  an array of ResourceSpec objects
	 * @deprecated use protected constructor with Symbol
	 */
	public Operator(ResourceSpec[] resSpecs) {
	    super(javax.telephony.media.ResourceSpec.Operator.class, null, null);
	    //super(Operator.class, null, null);
	    this.resSpecs = resSpecs;
	}

	/**
	 * Constructs a ResourceSpec$Operator object.
	 *
	 * @param oper      a Symbol, one of: op_And, op_Or, op_Alt, op_Add, op_Future
	 * @param resSpecs  an array of ResourceSpec objects
	 */
	protected Operator( Symbol oper, ResourceSpec[] resSpecs) {
	    super(null, oper, null, null);
	    //super(Operator.class, null, null);
	    this.resSpecs = resSpecs;
	}

	/** get the array of ResourceSpecs.
	 *
	 * @return an array of ResourceSpec objects
	 */
	public ResourceSpec[] getResourceSpecs() {
	    return resSpecs;
	}
    }

    /**
     * All ResourceSpecs must be satisfied, simultaneously.
     */
    public static class And extends Operator {
	// public static final Symbol v_Class = ESymbol.KVS_OperatorAnd;
	/** Constructor for the And operator.
	 * @param resSpecs an Array of ResourceSpec.
	 */
	public And(ResourceSpec[] resSpecs) {super(op_And, resSpecs);}
    }

    /**
     * All ResourceSpecs must be satisfied, 
     * but only needs to statisfy one at a time.
     * That is, the implementation is allowed to switch between 
     * each Resource dynamically; for example, a Recorder may
     * be disabled while a Player is playing.
     */
    public static
    class Alt extends Operator {
	/** Constructor for the Alt operator.
	 * @param resSpecs an Array of ResourceSpec.
	 */
	public Alt(ResourceSpec[] resSpecs) {super(op_Alt, resSpecs);}
    }

    /**
     * The given ResourceSpec[] is searched in order for a ResourceSpec
     * that is satisfied.  This construct allows an application to
     * specify a series of fallback options if the best alternative
     * is not available. If none of the ResourceSpecs are available
     * allocation of this ResourceSpec fails.
     */
    public static
    class Or extends Operator {
	/** Constructor for the Or operator.
	 * @param resSpecs an Array of ResourceSpec.
	 */
	public Or(ResourceSpec[] resSpecs) {super(op_Or, resSpecs);}
    }

    /**
     * Add each Resource, unless a compatible Resource is already configured.
     * In common usage only one ResourceSpec is supplied;
     * if multiple ResourceSpecs are supplied,
     * they are treated as for <code>And</code>.
     */
    public static
    class Add extends Operator {
	/** Constructor for the Add operator.
	 * @param resSpecs an Array of ResourceSpec.
	 */
	public Add(ResourceSpec[] resSpecs) {super(op_Add, resSpecs);}
    }

    /**
     * The given ResourceSpec[] specifies Resources that will
     * be added in the future.  Operationally, the configuration
     * implementation must determine that Resources for each 
     * ResourceSpec are installed and connectable to this group.
     * However, the Resource are not actually allocated and connected
     * at this time.
     * <p>
     * This acts as a hint to the allocation process about
     * how and where to implement the other Resources.
     * <p>
     * Typically, one or more of these <i>future</i> Resources may
     * be allocated and configured into the group by a later configure()
     * request, possibly using a <tt>ResourceSpec.Add</tt> object.
     */
    public static
    class Future extends Operator {
	/** Constructor for the Future operator.
	 * @param resSpecs an Array of ResourceSpec.
	 */
	public Future(ResourceSpec[] resSpecs) {super(op_Future, resSpecs);}
    }

    /* data section */

    /**
     * A Player with default parameters and attributes.
     */
    public static final ResourceSpec basicPlayer =  
	new ResourceSpec( "basicPlayer", Player.v_Class, null, null);

    /**
     * A Recorder with default parameters and attributes.
     */
    public static final ResourceSpec basicRecorder =
	new ResourceSpec( "basicRecorder", Recorder.v_Class, null, null);

    /**
     * A SignalDetector with default parameters and attributes.
     */
    public static final ResourceSpec basicSignalDetector =
	new ResourceSpec( "basicSigDet", SignalDetector.v_Class, null, null);

    /**
     * A SignalGenerator with default parameters and attributes.
     */
    public static final ResourceSpec basicSignalGenerator =
	new ResourceSpec( "basicSigGen", SignalGenerator.v_Class, null, null);

    /**
     * An ASR Resource with default parameters and attributes.
     */
    public static final ResourceSpec basicASR      =
	new ResourceSpec( "basicASR", ASR.v_Class, null, null);


    /** ResourceSpec[] used to define Alt({Player, Recorder}). */
    private static final 
	ResourceSpec[] aryPlayerRecorder = {basicPlayer, basicRecorder};

    /**
     * A ResourceSpec that provides alternating access to Player and Recorder.
     */
    public static final 
	ResourceSpec basicAltPlayerRecorder = new Alt(aryPlayerRecorder);

    /**
     * This ResourceSpec requests no actual resource.
     * As such, it is always satisfiable, by any server.
     * This may be used to ensure that a {@link ResourceSpec.Or} always succeeds.
     */
    public static final 
	ResourceSpec none = new And(null);

    /** Indicates that ResourceSpecs should be added to the current configuration. */
    public static final Symbol op_Add		= ESymbol.KVS_OperatorAdd;
    /** Indicates that ResourceSpecs are all required, but at different times.*/
    public static final Symbol op_Alt		= ESymbol.KVS_OperatorAlt;
    /** Indicates the <b>all</b> the given ResourceSpecs should be used. */
    public static final Symbol op_And		= ESymbol.KVS_OperatorAnd;
    /** Indicates the the given ResourcesSpecs maybe be added at some future time. */
    public static final Symbol op_Future	= ESymbol.KVS_OperatorFuture;
    /** Indicates that any one given alternative ResourceSpecs may be used.*/
    public static final Symbol op_Or		= ESymbol.KVS_OperatorOr;
}
