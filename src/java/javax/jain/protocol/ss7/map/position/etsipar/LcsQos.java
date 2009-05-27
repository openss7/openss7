

package javax.jain.protocol.ss7.map.position.etsipar;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class LcsQos extends MapParameter {
    public static final int LOW_DELAY_RESPONSE = 0;
    public static final int DELAYTOLERANT = 1;
    public LcsQos()
        throws SS7InvalidParamException {
    }
    public void setHorizontalAccuracy(float horizontalAccuracy)
        throws SS7InvalidParamException {
    }
    public float getHorizontalAccuracy() {
    }
    public boolean isHorizontalAccuracyPresent() {
    }
    public void setVerticalCoordinateRequest(boolean verticalCoordinateRequest)
        throws SS7InvalidParamException {
    }
    public boolean getVerticalCoordinateRequest() {
    }
    public boolean isVerticalCoordinateRequestPresent() {
    }
    public void setVerticalAccuracy(float verticalAccuracy)
        throws SS7InvalidParamException {
    }
    public float getVerticalAccuracy() {
    }
    public boolean isVerticalAccuracyPresent() {
    }
    public void setResponseTime(int responseTime)
        throws SS7InvalidParamException {
    }
    public int getResponseTime() {
    }
    public boolean isResponseTimePresent() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
