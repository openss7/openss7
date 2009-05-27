
package javax.jain.protocol.ss7.map.information.etsipar;

import java.util.Date;
import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class Location extends MapParameter {
    public Location()
        throws SS7InvalidParamException {
    }
    public void setLocationNumber(LocationNumber locationNumber) {
    }
    public LocationNumber getLocationNumber() {
        return new LocationNumber();
    }
    public boolean isLocationNumberPresent() {
        return false;
    }
    public void setLocationAreaId(LocationAreaId locationAreaId) {
    }
    public LocationAreaId getLocationAreaId() {
        return new LocationAreaId();
    }
    public boolean isLocationAreaIdPresent() {
        return false;
    }
    public void setPosition(Position position) {
    }
    public Position getPosition() {
        return new Position();
    }
    public boolean isPositionPresent() {
        return false;
    }
    public void setVlrNumber(VlrNumber vlrNumber) {
    }
    public VlrNumber getVlrNumber() {
        return new VlrNumber();
    }
    public boolean isVlrNumberPresent() {
        return false;
    }
    public void setLsaId(LsaId lsaId) {
    }
    public LsaId getLsaId() {
        return new LsaId();
    }
    public boolean isLsaIdPresent() {
        return false;
    }
    public void setAgeOfLocationInfo(Date ageOfLocationInfo) {
    }
    public Date getAgeOfLocationInfo() {
        return false;
    }
    public boolean isAgeOfLocationInfoPresent() {
        return false;
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
