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
    Async_ContainerEvent async_setInfo(String fullName, 
				       Dictionary dict);

    public abstract
    Async_ContainerEvent async_getInfo(String fullName, Symbol[] keys, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_setParameters(String fullName, 
					     Dictionary params);

    public abstract
    Async_ContainerEvent async_getParameters(String fullName, Symbol[] params, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_changeMode(String fullName, Symbol accessMode, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_copy(String fullName, String target,
				    Dictionary optargs);

    public abstract
    Async_ContainerEvent async_rename(String fullName, String newName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_destroy(String fullName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_listObjects(String fullName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_createContainer(String fullName, Dictionary optargs);

    public abstract
    Async_ContainerEvent async_openDataObject(String fullName, 
					      Symbol accessMode, 
					      boolean ifNoExist,
					      boolean truncate,
					      Dictionary optargs);

    public abstract
    Async_ContainerEvent async_createDataObject(String fullName, 
						Symbol accessMode, 
						Dictionary optargs);

    /******************** implement ContainerService *****/
    /* could be generated from ContainerService interface */


    public
    void setInfo(String fullName, Dictionary dict)
	throws ContainerException {
	Async_ContainerEvent event = async_setInfo( fullName, dict );
	event.throwIfContainerException();
    }

    public
    Dictionary getInfo(String fullName, Symbol[] keys, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_getInfo( fullName, keys, optargs );
	event.throwIfContainerException();
	return event.getInfoDict();
    }

    public
    void setParameters(String fullName, Dictionary params)
	throws ContainerException {
	Async_ContainerEvent event = async_setParameters( fullName, params);
	event.throwIfContainerException();
    }

    public
    Dictionary getParameters(String fullName, Symbol[] params, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_getParameters( fullName, params, optargs);
	event.throwIfContainerException();
	return event.getParamDict();
    }

    public
    void changeMode(String fullName, Symbol accessMode, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_changeMode(fullName, accessMode, optargs);
	event.throwIfContainerException();
    }

    public
    void copy(String fullName, String target, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_copy( fullName, target, optargs);
	event.throwIfContainerException();
    }

    public
    void rename(String fullName, String newName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_rename( fullName, newName, optargs );
	event.throwIfContainerException();
    }

    public
    void destroy(String fullName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_destroy( fullName, optargs );
	event.throwIfContainerException();
    }

    public
    String[] listObjects(String fullName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_listObjects( fullName, optargs );
	event.throwIfContainerException();
	return event.getObjectList();
    }


    public
    void createContainer(String fullName, Dictionary optargs)
	throws ContainerException {
	Async_ContainerEvent event = async_createContainer( fullName, optargs );
	event.throwIfContainerException();
    }

    public
    DataObject createDataObject( String fullName, 
				 Symbol accessMode, 
				 Dictionary optargs )
	throws ContainerException {
	Async_ContainerEvent event = 
	    async_createDataObject(fullName, accessMode, optargs);
	event.throwIfContainerException();
	return event.getDataObject();
    }

    public
    DataObject openDataObject( String fullName, 
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
