/*
 ****************************************************************
 *                                                              *
 *  HULA project						*
 *                                             			*
 *  program:  	uderrors.h    unit data error messages          *
 *  author:   	kurt dobbins                                    *
 *  date:     	2/89                                            *
 *                                                              *
 *  This file contains the error messages for the unit data     *
 *  transport service over.					*
 *                                  				*
 ****************************************************************
 */


#ifndef _UDERRORS_
#define _UDERRORS_
#endif


#define UD_BASE 	20	

#define UDERR_NO_NSAP_ADDRS 	 	(UD_BASE + 0)
#define UDERR_ILLEGAL_UD_SIZE 	 	(UD_BASE + 1)
#define UDERR_NO_MEMORY 	 	(UD_BASE + 2)
#define UDERR_NSAP_NOT_SUPPORTED 	(UD_BASE + 3)
#define UDERR_SERVICE_NOT_SUPPORTED	(UD_BASE + 4)
#define UDERR_ZERO_LENGTH_DATA		(UD_BASE + 5)
#define UDERR_INVALID_XPORT_DESC	(UD_BASE + 6)
#define UDERR_CLNS_NOT_SUPPORTED	(UD_BASE + 7)
#define UDERR_MISSING_PARAMETER 	(UD_BASE + 8)
#define UDERR_TSAP_SERVICE_UNKNOWN 	(UD_BASE + 9)
#define UDERR_RHOST_UNKNOWN 		(UD_BASE + 10)
#define UDERR_LHOST_UNKNOWN 		(UD_BASE + 11)
#define UDERR_ADDR_FAMILY_MISMATCH      (UD_BASE + 12)
#define UDERR_START_CLIENT_FAILED       (UD_BASE + 13)
#define UDERR_START_SRVR_FAILED       	(UD_BASE + 14)
#define UDERR_JOIN_SRVR_FAILED       	(UD_BASE + 15)
#define UDERR_UNBIND_FAILED       	(UD_BASE + 16)
#define UDERR_UNKNOWN       		(UD_BASE + 17)
#define UDERR_INVALID_SESSION_DESC	(UD_BASE + 18)
#define UDERR_TOO_MANY_VECTORS		(UD_BASE + 19)
#define UDERR_TUNITWRITE_FAILED		(UD_BASE + 20)
#define UDERR_TUNITREAD_FAILED		(UD_BASE + 21)
#define UDERR_DECODE_UDSPDU_FAILED	(UD_BASE + 22)
#define UDERR_UNEXPECTED_SPDU_TYPE	(UD_BASE + 23)
#define UDERR_ENCODE_UDSPDU_FAILED	(UD_BASE + 24)
#define UDERR_NO_REMOTE_ADDR		(UD_BASE + 25)
#define UDERR_BAD_INIT_VECTOR		(UD_BASE + 26)


int uderror();





char *uderror_text[] 

#if UDERR_ALLOCATE
= 

  {
  "No NSAP addresses in called parameter.",
  "Illegal unit data size in request.",
  "No more memory left for buffer allocation.",
  "NSAP address not supported.",
  "Transport service not supported.",

  "Zero-length on unit data not allowed.",
  "Invalid transport descriptor.",
  "Transport flag not set for connectionless service.",
  "Mandatory parameter missing.",
  "Unable to find the port for the TSAP daemon",

  "Remote Host is not defined",
  "Local Host is not defined",
  "Address family mismatch in local and remote addr",
  "Unable to create local socket for client",
  "Unable to create local socket for server",

  "Unable to join server",
  "Unable to BIND the socket",
  "Unknown error code",
  "Invalid session descriptor.",
  "Too Many vectors in the user udvec.",

  "Request to TUnitDataWrite failed.",
  "Request to TUnitDataRead failed.",
  "Decode the unitdata spdu from the tsdu failed.",
  "Unexpected SPDU received.",
  "Encode the unitdata spdu failed.",
  "No remnote address specified for re-bind.",
  "Bad initialization vector."
  };
#else
  ;


#define UDERR_MAX	( (sizeof(uderror_text)) / (sizeof(uderror_text[0])) ) 
 

#endif






