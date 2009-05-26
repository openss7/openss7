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
* DATE         : $Date: 2008/05/16 12:23:59 $
* 
* MODULE NAME  : $RCSfile: UserTeleserviceInfoItu.java,v $
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


package javax.jain.ss7.isup.itu;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class UserTeleserviceInfoItu extends java.lang.Object implements java.io.Serializable{

	public UserTeleserviceInfoItu(){

	}

	public UserTeleserviceInfoItu(byte in_presentation,
                              byte in_interpretation,
                              byte in_codingStandard,
                              boolean in_highLayerCharacteristicsIdPresent,
                              byte in_highLayerCharacteristicsId,
                              byte in_extendedHighLayerCharacteristicsId)
                       throws ParameterRangeInvalidException{

			codingStandard                      = in_codingStandard;
			extendedHighLayerCharacteristicsId  = in_extendedHighLayerCharacteristicsId; 
			highLayerCharacteristicsId          = in_highLayerCharacteristicsId;
			highLayerCharacteristicsIdPresent   = in_highLayerCharacteristicsIdPresent;
			interpretation                      = in_interpretation;
			presentation                        = in_presentation;


	}

	public byte getExtendedHighLayerCharacteristicsId(){
		return extendedHighLayerCharacteristicsId;
	}

	public void setExtendedHighLayerCharacteristicsId(byte in_extendedHighLayerCharacteristicsId)
                                           throws ParameterRangeInvalidException{
		extendedHighLayerCharacteristicsId = in_extendedHighLayerCharacteristicsId;
	}
		
	public byte getHighLayerCharacteristicsId(){
		return highLayerCharacteristicsId;
	}

	public void setHighLayerCharacteristicsId(byte in_highLayerCharacteristicsId)
                                   throws ParameterRangeInvalidException{
		highLayerCharacteristicsId = in_highLayerCharacteristicsId;
	}

	public boolean getHighLayerCharacteristicsIdPresent(){
		return highLayerCharacteristicsIdPresent;
	}

	public void setHighLayerCharacteristicsIdPresent(boolean in_highLayerCharacteristicsIdPresent){
		highLayerCharacteristicsIdPresent = in_highLayerCharacteristicsIdPresent;
	}

	public byte getInterpretation(){
		return interpretation;
	}

	public void setInterpretation(byte in_interpretation)
                       throws ParameterRangeInvalidException{
		interpretation = in_interpretation;
	}

	public byte getCodingStandard(){
		return codingStandard;
	}

	public void setCodingStandard(byte in_codingStandard)
                       throws ParameterRangeInvalidException{
		codingStandard = in_codingStandard;
	}
	
	/**
    * String representation of class UserTeleserviceInfoItu
    *
    * @return    String provides description of class UserTeleserviceInfoItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncodingStandard = ");
				buffer.append(codingStandard);	
				buffer.append("\nextendedHighLayerCharacteristicsId = ");
				buffer.append(extendedHighLayerCharacteristicsId);	
				buffer.append("\nhighLayerCharacteristicsId = ");
				buffer.append(highLayerCharacteristicsId);	
				buffer.append("\nhighLayerCharacteristicsIdPresent = ");
				buffer.append(highLayerCharacteristicsIdPresent);	
				buffer.append("\ninterpretation = ");
				buffer.append(interpretation);	
				buffer.append("\npresentation = ");
				buffer.append(presentation);	
				return buffer.toString();
		
		}

		byte    codingStandard;
		byte    extendedHighLayerCharacteristicsId;
		byte    highLayerCharacteristicsId;
		boolean highLayerCharacteristicsIdPresent;
		byte    interpretation;
		byte    presentation;

	

		public static final byte CS_ITU_STANDARD = 0x00;
		public static final byte CS_ISO_IEC_STANDARD = 0x01; 
		public static final byte CS_NATIONAL_STANDARD = 0x02;
		public static final byte CS_STANDARD_FOR_NETWORK = 0; /* Value not found in the specs */
		public static final byte INTERPRETATION_FIRST_HIGH_LAYER_CHARACTERISTICS_IDENTIFICATION = 0; /* Value not found in the specs */
		public static final byte PRESENTATION_HIGH_LAYER_PROTOCOL_PROFILE = 0; /* Value not found in the specs */
		public static final boolean EI_NEXT_OCTET_EXIST = false;
		public static final boolean EI_LAST_OCTET = true;
		public static final byte HLCI_TELEPHONY = 0; /* Value not found in the specs */
		public static final byte HLCI_FASCIMILE_GROUP_2_3 = 0; /* Value not found in the specs */
		public static final byte HLCI_FACSIMILE_GROUP_4 = 0; /* Value not found in the specs */
		public static final byte HLCI_FACSIMILE_SERVICE_GROUP_4 = 0; /* Value not found in the specs */
		public static final byte HLCI_SYNTAX_BASED_VIDEOTEX = 0; /* Value not found in the specs */
		public static final byte HLCI_INTERNATIONLA_VIDEOTEX_INTERWORKING = 0; /* Value not found in the specs */
		public static final byte HLCI_TELEX_SERVICE = 0; /* Value not found in the specs */
		public static final byte HLCI_MESSAGE_HANDLING_SYSTEM = 0; /* Value not found in the specs */
		public static final byte HLCI_OSI_APPLICATION = 0; /* Value not found in the specs */
		public static final byte HLCI_FTAM_APPLICATION = 0; /* Value not found in the specs */
		public static final byte HLCI_MAINTENANCE = 0; /* Value not found in the specs */
		public static final byte HLCI_MANAGEMENT = 0; /* Value not found in the specs */
		public static final byte HLCI_VIDEOTELEPHONY = 0; /* Value not found in the specs */
		public static final byte HLCI_VIDEOCONFERENCING = 0; /* Value not found in the specs */
		public static final byte HLCI_AUDIOGRAPHIC_CONFERENCING = 0; /* Value not found in the specs */
		public static final byte HLCI_MULTIMEDIA_SERVICE = 0; /* Value not found in the specs */
		public static final byte EHLCI_TELEPHONY = 0; /* Value not found in the specs */
		public static final byte EHLCI_CAPABILITY_SET_OF_INITIAL_CHANEL_H221 = 0; /* Value not found in the specs */
		public static final byte EHLCI_CAPABILITY_SET_OF_INITIAL_SUBSEQUENT_CHANEL_H221 = 0; /* Value not found in the specs */
		public static final byte EHLCI_CAPABILITY_SET_OF_INITIAL_CHANEL_H221_with_3DOT1_KHZ_AUDIO_SPEECH = 0; /* Value not found in the specs */
		public static final byte EHLCI_FASCIMILE_GROUP_2_3 = 0; /* Value not found in the specs */
		public static final byte EHLCI_FACSIMILE_GROUP_4 = 0; /* Value not found in the specs */
		public static final byte EHLCI_FACSIMILE_SERVICE_GROUP_4 = 0; /* Value not found in the specs */
		public static final byte EHLCI_SYNTAX_BASED_VIDEOTEX = 0; /* Value not found in the specs */
		public static final byte EHLCI_INTERNATIONLA_VIDEOTEX_INTERWORKING = 0; /* Value not found in the specs */
		public static final byte EHLCI_TELEX_SERVICE = 0; /* Value not found in the specs */
		public static final byte EHLCI_MESSAGE_HANDLING_SYSTEM = 0; /* Value not found in the specs */
		public static final byte EHLCI_OSI_APPLICATION = 0; /* Value not found in the specs */
		public static final byte EHLCI_FTAM_APPLICATION = 0; /* Value not found in the specs */
		public static final byte EHLCI_VIDEOTELEPHONY = 0; /* Value not found in the specs */
		public static final byte EHLCI_VIDEOCONFERENCING = 0; /* Value not found in the specs */
		public static final byte EHLCI_AUDIOGRAPHIC_CONFERENCING = 0; /* Value not found in the specs */
		public static final byte EHLCI_MULTIMEDIA_SERVICE = 0; /* Value not found in the specs */


}




