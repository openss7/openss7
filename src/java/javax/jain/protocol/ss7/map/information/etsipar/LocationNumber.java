
package javax.jain.protocol.ss7.map.information.etsipar;

import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class LocationNumber extends E164Number {
    public LocationNumber(String cc, String lsp)
        throws SS7InvalidParamException {
    }
    public LocationNumber()
        throws SS7InvalidParamException {
    }
    public void setLocallySignificantPart(String lsp)
        throws SS7InvalidParamException {
    }
    public String getLocallySignificantPart() {
        return new String();
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
