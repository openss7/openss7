
package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MtMessageReqEvent extends TransactionEvent {
	public MtMessageReqEvent(Object source, MsNumber msId, Message message, boolean availabilityNotif, SS7Number originatingAddress)
		throws SS7InvalidParamException {
	}
	public MtMessageReqEvent()
		throws SS7InvalidParamException {
	}
	public void setMsId(MsNumber msId)
		throws SS7InvalidParamException {
	}
	public MsNumber getMsId() {
	}
	public void setMessage(Message message)
		throws SS7InvalidParamException {
	}
	public Message getMessage() {
	}
	public void setAvailabilityNotif(boolean availabilityNotif)
		throws SS7InvalidParamException {
	}
	public boolean getAvailabilityNotif() {
	}
	public void setOriginatingAddress(SS7Number originatingAddress)
		throws SS7InvalidParamException {
	}
	public SS7Number getOriginatingAddress() {
	}
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
