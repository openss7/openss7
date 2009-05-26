/******************************************************************************
*                                                                             *
*                                                                             *
* Copyright (c) SS8 Networks, Inc.                                            *
* All rights reserved.                                                        *
*                                                                             *
* This document contains confidential and proprietary information in which    *
* any reproduction, disclosure, or use in whole or in part is expressly       *
* prohibited, except as may be specifically authorized by prior written       *
* agreement or permission of SS8 Networks, Inc.                               *
*                                                                             *
*******************************************************************************
* VERSION      : $Revision: 1.1 $
* DATE         : $Date: 2008/05/16 12:24:10 $
* 
* MODULE NAME  : $RCSfile: CommonLangLocationIdAnsi.java,v $
* AUTHOR       : Nilgun Baykal [SS8]
* DESCRIPTION  : 
* DATE 1st REL : 
* REV.HIST.    : 
* 
* Date      Owner  Description
* ========  =====  ===========================================================
* 
* 
*******************************************************************************
*                                                                             *
*                     RESTRICTED RIGHTS LEGEND                                *
* Use, duplication, or disclosure by Government Is Subject to restrictions as *
* set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and    *
* Computer Software clause at DFARS 252.227-7013                              *
*                                                                             *
******************************************************************************/


package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class CommonLangLocationIdAnsi extends java.lang.Object implements java.io.Serializable{

	public CommonLangLocationIdAnsi(){

	}

	public CommonLangLocationIdAnsi(java.lang.String in_town,
                                java.lang.String in_state,
                                java.lang.String in_building,
                                java.lang.String in_buildingSubDivision)
                         throws ParameterRangeInvalidException{
		
		building            = in_building;
		buildingSubDivision = in_buildingSubDivision;
		state               = in_state;
		town                = in_town;
	}

	public java.lang.String getBuilding(){
		return building;
	}
	
	public void setBuilding(java.lang.String in_building)
         	        throws ParameterRangeInvalidException{
		building            = in_building;
	}

	public java.lang.String getBuildingSubDivision(){
		return buildingSubDivision;
	}

	public void setBuildingSubDivision(java.lang.String in_buildingSubDivision)
         	                   throws ParameterRangeInvalidException{
		buildingSubDivision = in_buildingSubDivision;
	}
	
	public java.lang.String getState(){
		return state;
	}

	public void setState(java.lang.String in_state)
              throws ParameterRangeInvalidException{
		state               = in_state;
	}	

	public java.lang.String getTown(){
		return town;
	}

	public void setTown(java.lang.String in_town)
             throws ParameterRangeInvalidException{
		town                = in_town;
	}

	public void putCommonLangLocationIdAnsi(byte[] arr,int index,byte par_len){

		town = new String(arr,index,4);
		
		state = new String(arr,index+4,2);

		building = new String(arr,index+6,2);
		
		buildingSubDivision = new String(arr,index+8,3);

	}

	public byte[] flatCommonLangLocationIdAnsi(){

		byte[] rc = ByteArray.getByteArray(11);	

		town.getBytes(0,4, rc,0);
		state.getBytes(0,2,rc,4);
		building.getBytes(0,2,rc,6);
		buildingSubDivision.getBytes(0,3,rc,8);
						
		return rc;
	}

	
	/**
    * String representation of class CommonLangLocationIdAnsi
    *
    * @return    String provides description of class CommonLangLocationIdAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nbuilding = ");
				buffer.append(building);
				buffer.append("\nbuildingSubDivision = ");
				buffer.append(buildingSubDivision);
				buffer.append("\nstate = ");
				buffer.append(state);
				buffer.append("\ntown = ");
				buffer.append(town);
				return buffer.toString();
		
		}


	java.lang.String building;
	java.lang.String buildingSubDivision;
	java.lang.String state;
	java.lang.String town;

	

}	


