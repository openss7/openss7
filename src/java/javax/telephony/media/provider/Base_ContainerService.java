/*
 * Base_ContainerService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.io.*;
import java.util.*;
import javax.telephony.media.*;
import javax.telephony.media.async.*;
import javax.telephony.media.container.*;
import javax.telephony.media.container.async.*;

/**
 * Generic code that is independent of the protocol/implementation layer.
 * provider implementer must supply the Async_ContainerService methods.
 * <p>
 * Depends on various items from provider package.
 *
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
abstract
public class Base_ContainerService extends Base_Owner
    implements ContainerService, Async_ContainerService {    

    /************ implement Async_ContainerService *****/

    public abstract
    Async_ContainerEvent async_setInfo(java.lang.String fullName, 
				       Dictionary dict);

    public abstract
    Async_ContainerEvent async_getInfo(java.lang.String fullName, Symbol[] keys, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_setParameters(java.lang.String fullName, 
					     Dictionary params);

    public abstract
    Async_ContainerEvent async_getParameters(java.lang.String fullName, Symbol[] params, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_changeMode(java.lang.String fullName, Symbol accessMode, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_copy(java.lang.String fullName, java.lang.String target,
				    Dictionary optargs);

    public abstract
    Async_ContainerEvent async_rename(java.lang.String fullName, java.lang.String newName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_destroy(java.lang.String fullName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_listObjects(java.lang.String fullName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_createContainer(java.lang.String fullName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_openDataObject(java.lang.String fullName, 
					      Symbol accessMode, 
					      boolean ifNoExist,
					      boolean truncate,
					      Dictionary optargs);

    public abstract
    Async_ContainerEvent async_createDataObject(java.lang.String fullName, 
						Symbol accessMode, 
						Dictionary optargs);

    /******************** implement ContainerService *****/
    /* could be generated from ContainerService interface */


    public
    void setInfo(java.lang.String fullName, Dictionary dict)
	throws ContainerException {
	Async_ContainerEvent event = async_setInfo( fullName, dict );
	event.throwIfContainerException();
    }

    public
    Dictionary getInfo(java.lang.String fullName, Symbol[] keys, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_getInfo( fullName, keys, optargs );
	event.throwIfContainerException();
	return event.getInfoDict();
    }

    public
    void setParameters(java.lang.String fullName, Dictionary params)
	throws ContainerException {
	Async_ContainerEvent event = async_setParameters( fullName, params);
	event.throwIfContainerException();
    }

    public
    Dictionary getParameters(java.lang.String fullName, Symbol[] params, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_getParameters( fullName, params, optargs);
	event.throwIfContainerException();
	return event.getParamDict();
    }

    public
    void changeMode(java.lang.String fullName, Symbol accessMode, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_changeMode(fullName, accessMode, optargs);
	event.throwIfContainerException();
    }

    public
    void copy(java.lang.String fullName, java.lang.String target, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_copy( fullName, target, optargs);
	event.throwIfContainerException();
    }

    public
    void rename(java.lang.String fullName, java.lang.String newName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_rename( fullName, newName, optargs );
	event.throwIfContainerException();
    }

    public
    void destroy(java.lang.String fullName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_destroy( fullName, optargs );
	event.throwIfContainerException();
    }

    public
    java.lang.String[] listObjects(java.lang.String fullName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_listObjects( fullName, optargs );
	event.throwIfContainerException();
	return event.getObjectList();
    }


    public
    void createContainer(java.lang.String fullName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_createContainer( fullName, optargs );
	event.throwIfContainerException();
    }

    public
    DataObject createDataObject( java.lang.String fullName, 
				 Symbol accessMode, 
				 Dictionary optargs )
	throws ContainerException {
	Async_ContainerEvent event = 
	    async_createDataObject(fullName, accessMode, optargs);
	event.throwIfContainerException();
	return event.getDataObject();
    }

    public
    DataObject openDataObject( java.lang.String fullName, 
			       Symbol accessMode, 
			       boolean ifNoExist,
			       boolean truncate,
			       Dictionary optargs )
	throws ContainerException {
	Async_ContainerEvent event = 
	    async_openDataObject(fullName, accessMode, ifNoExist, 
				 truncate, optargs);
	event.throwIfContainerException();
	return event.getDataObject();
    }
}
