/*
 * Request.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.provider;

import javax.telephony.media.*;
import javax.telephony.media.async.*;
import javax.telephony.*;		// get core Objects and Exceptions
import java.lang.reflect.*;
import java.util.Dictionary;
import java.util.EventListener;

/** 
 * Represents an API method request on a MediaGroup. 
 * <p>
 * A Request is a Method plus some args. 
 * When the request is invoked, the indicated Method is invoked 
 * (on the given MPI.MediaGroup) with the previously supplied args.
 * <p>
 * Requests are created in BasicMediaService,
 * and invoked in Base_MediaService.
 * This technique allows Base_MediaService to invoke methods 
 * that are beyond the pre-defined MPI methods; 
 * including all the <i>Resource</i> methods.
 * <p>
 * This class includes a static wrapper around <tt>reflect.getMethod(...)</tt>
 * which catches any exceptions, so that method can be used in
 * static initializers to find the required Method. 
 * <P>
 * The constructor comes in various flavors with different numbers
 * of Object arguments, and a couple with strategically placed <tt>int</tt>
 * args. These are sufficient to handle the S.410 media resource methods.
 * <p>
 * This bit of <i>reflection</i> allows the MediaGroup object and
 * its public methods to be hidden from the application. 
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */ 
public
class Request {
    private Method method;
    private Object[] arglist;

    /** return the internal, cached Method, 
     * so one can look up the declaring class. 
     * @return the Method to be invoked.
     */
    public Method getMethod() { return method; }

    public Request(Method method, Object[] arglist) {
	this.method = method;
	this.arglist = arglist;
    }

    public Request(Method method) {
	this(method, new Object[0]);
    }

    public Request(Method method, Object arg0) {
	this(method, new Object[1]);
	arglist[0] = arg0;
    }

    public Request(Method method, Object arg0, Object arg1) {
	this(method, new Object[2]);
	arglist[0] = arg0;
	arglist[1] = arg1;
    }
    public Request(Method method, Object arg0, Object arg1, Object arg2) {
	this(method, new Object[3]);
	arglist[0] = arg0;
	arglist[1] = arg1;
	arglist[2] = arg2;
    }
    public Request(Method method, Object arg0, Object arg1, Object arg2, 
		   Object arg3) {
	this(method, new Object[4]);
	arglist[0] = arg0;
	arglist[1] = arg1;
	arglist[2] = arg2;
	arglist[3] = arg3;
    }
    public Request(Method method, Object arg0, Object arg1, Object arg2, 
		   Object arg3, 
		   Object arg4) {
	this(method, new Object[5]);
	arglist[0] = arg0;
	arglist[1] = arg1;
	arglist[2] = arg2;
	arglist[3] = arg3;
	arglist[4] = arg4;
    }
    public Request(Method method, int arg0, Object arg1, 
		   Object arg2,
		   Object arg3) {
	this(method, new Integer(arg0), arg1, arg2, arg3);
    }

    public Request(Method method, Object arg0, int arg1, 
		   Object arg2, 
		   Object arg3) {
	this(method, arg0, new Integer(arg1), arg2, arg3);
    }
    

    /** Invoke this Request on the target object.
     * <p>
     * The Request.method is not invoked directly on the
     * given MPI.MediaGroup.
     * The MPI.MediaGroup is allowed to delegate the method
     * to any implementation object. 
     * The MPI.MediaGroup implementation should return the
     * proper object for each method as the value of
     * MPI.MediaGroup.getTargetForMethod(java.lang.reflect.Method).
     * <p>
     * If an Exception is generated, either by Method.invoke()
     * or by the actual method implementation, it is wrapped
     * as a RuntimeException and thrown.
     * <p>
     * <b>Note:</b>
     * for media resource methods, most exceptions are
     * bundled into the return value.
     * 
     * @param group the MPI.MediaGroup on which this request is invoked.
     * @return the Object that is the return value of the method.
     * @throws RuntimeException if any Throwable/Exception is generated.
     */
    public Object invoke(MPI.MediaGroup group) {
	Object target = group.getTargetForMethod(method);
	// System.err.println("invoke: method = "+method);
	// System.err.println("invoke: target = "+target);
	// System.err.println("invoke: arglist = "+arglist+" "+arglist.length);
	try {
	    return method.invoke(target, arglist);
	} catch (InvocationTargetException e) {
	    // e.printStackTrace(System.err);
	    // could be anything!
	    Throwable targetException = e.getTargetException(); 
	    if (targetException instanceof RuntimeException)
		throw (RuntimeException)targetException;
	    else
		// wrap it in a RuntimeException:
		throw new RuntimeException(targetException.toString());
	} catch (IllegalAccessException e) {
	    // e.printStackTrace(System.err);
	    throw new RuntimeException(e.toString());
	}			
    }
    /** Find the indicated Method.
     * Catch any Exceptions so this can be used in static initializers 
     */
    static public Method theMethod(Class clas, String name, Class[] argtypes) {
	try {return clas.getMethod(name, argtypes);}
	catch (Exception ex) {return null;}
    }
}
