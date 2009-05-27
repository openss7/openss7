
package org.openss7.javax.jcat;

import org.openss7.javax.csapi.cc.jcc.*;
import javax.csapi.cc.jcc.*;
import javax.jcat.*;

public class JcatProviderImpl extends JccProviderImpl implements JcatProvider {
	public native java.util.Set getCalls(JcatAddress address);
	public native java.util.Set getTerminals(java.lang.String nameRegex);
	public native EventFilter createEventFilterRegistration(java.lang.String terminalNameRegex, int matchDisposition, int nomatchDisposition)
		throws ResourceUnavailableException, InvalidArgumentException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

