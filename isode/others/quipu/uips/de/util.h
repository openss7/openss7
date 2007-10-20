/* template.c - your comments here */

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/util.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: util.h,v $
 * Revision 9.0  1992/06/16  12:45:59  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef _xdua_util_h_
#define _xdua_util_h_

char *copy_string();
char *lastComponent();
char * removeLastRDN();
char * lastRDN();
char *dn2pstr();

void addToList();
void addToAttList();
void addToCoList();
void addToPhoneList();
void printLastComponent();
void printListCos();
void printListOrgs();
void printListOUs();
void printListPRRs();
void freeCos();
void freeOrgs();
void freeOUs();
void freePRRs();
void initVideo();
void clearProblemFlags();
void setProblemFlags();
void logSearchSuccess();
void logListSuccess();
void initAlarm();
void alarmCleanUp();
void startUnbindTimer();
void stopUnbindTimer();

#define LEADSUBSTR 1
#define TRAILSUBSTR 2
#define ANYSUBSTR 3
#define LEADANDTRAIL 4

#endif _xdua_util_h_

