

package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public abstract class Message extends MapParameter {
    public static final int ANSI_MESSAGE = 0;
    public static final int ETSI_MESSAGE = 1;
    public static final int ETSI_PDU = 2;
    public Message(int typeOfMessage)
        throws SS7InvalidParamException {
    }
    public Message()
        throws SS7InvalidParamException {
    }
    public int getTypeOfMessage() {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
