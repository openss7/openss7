

package javax.jain.protocol.ss7.map.position.etsipar;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class LcsClientId extends MapParameter {
    public static final int UNDEFINED_SERVICES = 0;
    public static final int EMERGENCY_SERVICES = 1;
    public static final int VALUE_ADDED_SERVICES = 2;
    public static final int PLMN_OPERATOR_SERVICES = 3;
    public static final int LAWFUL_INTERCEPT_SERVICES = 4;
    public static final int CLIENT_BROADCAST_SERVICE = 5;
    public static final int CLIENT_OAM_HPLMN = 6;
    public static final int CLIENT_OAM_VPLMN = 7;
    public static final int CLIENT_ANONYMOUS_LOCATION = 8;
    public static final int CLIENT_MS_SUBSCRIBED_SERVICE = 9;
    public LcsClientId(int lcsClientType)
        throws SS7InvalidParamException {
    }
    public LcsClientId()
        throws SS7InvalidParamException {
    }
    public void setLcsClientType(int lcsClientType)
        throws SS7InvalidParamException {
    }
    public int getLcsClientType() {
        return null;
    }
    public void setLcsClientExternalId(SS7Number lcsClientExternalId)
        throws SS7InvalidParamException {
    }
    public SS7Number getLcsClientExternalId() {
        return null;
    }
    public boolean isLcsClientExternalIdPresent() {
        return null;
    }
    public void setLcsClientDialedByMS(SS7Number lcsClientDialedByMS)
        throws SS7InvalidParamException {
    }
    public SS7Number getLcsClientDialedByMS() {
        return null;
    }
    public boolean isLcsClientDialedByMSPresent() {
        return null;
    }
    public void setLcsClientInternalId(int lcsClientInternalId)
        throws SS7InvalidParamException {
    }
    public int getLcsClientInternalId() {
        return null;
    }
    public boolean isLcsClientInternalIdPresent() {
        return null;
    }
    public void setLcsClientName(String lcsClientName)
        throws SS7InvalidParamException {
    }
    public String getLcsClientName() {
        return null;
    }
    public boolean isLcsClientNamePresent() {
        return null;
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
