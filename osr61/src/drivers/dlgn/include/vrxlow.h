/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : vrxlow.h
 * Description                  : VR host to board commands/replies
 *
 *
 **********************************************************************/

#ifndef __VRXLOW_H__
#define __VRXLOW_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Miscellaneous VR defines
 */
#define VR_DEVNUM        	0x31     /* Device number for VR/xxx */
#define VR_MAXDBD160       4


/*
 * Messages to board.
 */
#define VR_RDFWVER         0x01  /* Read FW version */
#define VR_RDCOPY          0x02  /* Read Copyright */
#define VR_UCHNPRM         0x03  /* Update channel parameter */
#define VR_RDCHNPRM        0x04  /* Read channel parameter */
#define VR_UBDPRM          0x05  /* Update board parameter */
#define VR_RDBDPRM         0x06  /* Read board parameter */
#define VR_DNLDRESVOC      0x07  /* Download resident vocabulary */
#define VR_RDTEMPS         0x08  /* Read generated template */
#define VR_STREC           0x0C  /* Start recognition */
#define VR_STOPREC         0x0D  /* Stop recognition */
#define VR_STTERMCND       0x0E  /* Set terminating conditions */
#define VR_RECCHNCT        0x0F  /* Record channel characteristics */
#define VR_RDSCORES        0x10  /* Read scores */


/*
 * Messages to host.
 */
#define VR_RDFWVERCMP      0x01  /* Read  FW version complete */
#define VR_RDCOPYCMP       0x02  /* Read copyright complete */
#define VR_UCHNPRMCMP      0x03  /* Update channel parameter complete */
#define VR_RDCHNPRMCMP     0x04  /* Read channel parameter complete */
#define VR_UBDPRMCMP       0x05  /* Update board parameter complete */
#define VR_RDBDPRMCMP      0x06  /* Read board parameter complete */
#define VR_ERROR           0x07  /* VR f/w error code */
#define VR_DNLDRESVOCCMP   0x08  /* Download resident vocab complete */
#define VR_RDTEMPSCMPI     0x09  /* Read generated template comp. intermed */
#define VR_RDTEMPSCMPF     0x0A  /* Read generated template comp. final */
#define VR_STRECCMPI       0x0E  /* Start recognition complete intermed */
#define VR_STRECCMPF       0x0F  /* Start recognition complete final */
#define VR_RECRPT          0x10  /* Recognition report */
#define VR_RECCHNCTCMP     0x11  /* Record chan characteristics complete */
#define VR_STTERMCNDCMP    0x12  /* Set termination conditions complete */
#define VR_RDSCORESCMPI    0x13  /* Read scores complete immediate */
#define VR_RDSCORESCMPF    0x14  /* Read scores complete final */

#define VR_VOCDNLD_START   0x00  /* First vocab download message */
#define VR_VOCDNLD_INT     0x01  /* Intermediate vocab download message */
#define VR_VOCDNLD_LAST    0x02  /* Last buffer of vocab file download */

#define VR_TSCORES         0x01  /* Return scores for each template ID */


#endif


