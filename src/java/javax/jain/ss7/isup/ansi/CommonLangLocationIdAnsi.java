/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ansi ISUP CommonLangLocationId parameter.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CommonLangLocationIdAnsi extends java.lang.Object implements java.io.Serializable {
    /** Constructs a new ANSI CommonLangLocationId class, parameters with default values.
      */
    public CommonLangLocationIdAnsi() {
    }
    /** Constructs an ANSI class from the input parameters specified.
        @param in_town - The town, range 1-4 IA5 coded characters.
        @param in_state - The state, range 1-2 IA5 coded characters.
        @param in_building - The building, range 1-2 IA5 coded characters.
        @param in_buildingSubDivision - The building subDivision, range 1-3 IA5 coded
        characters.
        @exception ParameterRangeInvalidException - Thrown when value is out of range.
      */
    public CommonLangLocationIdAnsi(java.lang.String in_town, java.lang.String in_state,
            java.lang.String in_building, java.lang.String in_buildingSubDivision)
        throws ParameterRangeInvalidException {
        this.setTown(in_town);
        this.setState(in_state);
        this.setBuilding(in_building);
        this.setBuildingSubDivision(in_buildingSubDivision);
    }
    /** Gets the Town field of CommonLangLocationId parameter.
        @return Town, range 1-4 IA5 coded characters.
      */
    public java.lang.String getTown() {
        return m_town;
    }
    /** Sets the Town field of CommonLangLocationId parameter.
        @param in_town - The Town value, range 1-4 IA5 coded characters.
      */
    public void setTown(java.lang.String in_town)
        throws ParameterRangeInvalidException {
        if (1 <= in_town.length() && in_town.length() <= 4) {
            m_town = in_town;
            return;
        }
        throw new ParameterRangeInvalidException("Town value " + in_town + " out of range.");
    }
    /** Gets the State field of CommonLangLocationId parameter.
        @return State, range 1-2 IA5 coded characters.
      */
    public java.lang.String getState() {
        return m_state;
    }
    /** Sets the State field of CommonLangLocationId parameter.
        @param in_state - The State value, range 1-2 IA5 coded characters.
      */
    public void setState(java.lang.String in_state)
        throws ParameterRangeInvalidException {
        if (1 <= in_state.length() && in_state.length() <= 2) {
            m_state = in_state;
            return;
        }
        throw new ParameterRangeInvalidException("State value " + in_state + " out of range.");
    }
    /** Gets the Building field of CommonLangLocationId parameter.
        @return Building, range 1-2 IA5 coded characters
      */
    public java.lang.String getBuilding() {
        return m_building;
    }
    /** Sets the Building field of CommonLangLocationId parameter.
        @param in_building - The Building value, range 1-2 IA5 coded characters.
      */
    public void setBuilding(java.lang.String in_building)
        throws ParameterRangeInvalidException {
        if (1 <= in_building.length() && in_building.length() <= 2) {
            m_building = in_building;
            return;
        }
        throw new ParameterRangeInvalidException("Building value " + in_building + " out of range.");
    }
    /** Gets the BuildingSubDivision field of CommonLangLocationId parameter.
        @return BuildingSubDivision, range 1-3 IA5 coded characters.
      */
    public java.lang.String getBuildingSubDivision() {
        return m_buildingSubDivision;
    }
    /** Sets the BuildingSubDivision field of CommonLangLocationId parameter.
        @param in_buildingSubDivision - The BuildingSubDivision value, range 1-3 IA5 coded
        characters.
      */
    public void setBuildingSubDivision(java.lang.String in_buildingSubDivision)
        throws ParameterRangeInvalidException {
        if (1 <= in_buildingSubDivision.length() && in_buildingSubDivision.length() <= 3) {
            m_buildingSubDivision = in_buildingSubDivision;
            return;
        }
        throw new ParameterRangeInvalidException("BuildingSubDivision value " + in_buildingSubDivision + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        CommonLangLocationIdAnsi class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.CommonLangLocationIdAnsi");
        b.append("\n\tm_town: " + m_town);
        b.append("\n\tm_state: " + m_state);
        b.append("\n\tm_building: " + m_building);
        b.append("\n\tm_buildingSubDivision: " + m_buildingSubDivision);
        return b.toString();
    }
    protected String  m_town;
    protected String  m_state;
    protected String  m_building;
    protected String  m_buildingSubDivision;
}


// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
