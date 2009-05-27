

package javax.jain.protocol.ss7.map.position.etsipar;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class PosRequestSpecifier extends MapParameter {
    public static final int CURRENT_LOCATION = 0;
    public static final int CURRENT_OR_LAST_LOCATION = 1;
    public static final int INITIAL_LOCATION = 2;
    public static final int NORMAL_PRIORITY = 3;
    public static final int HIGHEST_PRIORITY = 4;
    public PosRequestSpecifier(LcsClientId lcsClientId)
        throws SS7InvalidParamException {
    }
    public PosRequestSpecifier()
        throws SS7InvalidParamException {
    }
    public void setLocationType(int locationType)
        throws SS7InvalidParamException {
    }
    public int getLocationType() {
        return null;
    }
    public void setLcsClientId(LcsClientId lcsClientId)
        throws SS7InvalidParamException {
    }
    public LcsClientId getLcsClientId() {
        return null;
    }
    public void setPrivacyOverride(boolean privacyOverride)
        throws SS7InvalidParamException {
    }
    public boolean getPrivacyOverride() {
        return null;
    }
    public boolean isPrivacyOverridePresent() {
        return null;
    }
    public void setLcsPriority(int lcsPriority)
        throws SS7InvalidParamException {
    }
    public int getLcsPriority() {
        return null;
    }
    public boolean isLcsPriorityPresent() {
        return null;
    }
    public void setLcsQos(LcsQos lcsQos)
        throws SS7InvalidParamException {
    }
    public LcsQos getLcsQos() {
        return null;
    }
    public boolean isLcsQosPresent() {
        return null;
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
