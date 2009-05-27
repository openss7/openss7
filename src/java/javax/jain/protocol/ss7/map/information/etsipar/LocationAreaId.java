

package javax.jain.protocol.ss7.map.information.etsipar;

import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class LocationAreaId extends MapParameter {
	public LocationAreaId(String mcc, String mnc, byte[] lac)
		throws SS7InvalidParamException {
	}
	public LocationAreaId()
		throws SS7InvalidParamException {
	}
	public void setMobileCountryCode(String mcc)
		throws SS7InvalidParamException {
	}
	public String getMobileCountryCode() {
		return new String();
	}
	public void setMobileNetworkCode(String mnc)
		throws SS7InvalidParamException {
	}
	public String getMobileNetworkCode() {
		return new String();
	}
	public void setLocationAreaCode(byte[] lac)
		throws SS7InvalidParamException {
	}
	public byte[] getLocationAreaCode() {
		return null;
	}
	public String toString() {
		return new String();
	}
	public boolean equals(Object lai) {
		return false;
	}
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
