/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlver.h
 * Description                  : version number specific definitions
 *
 *
 **********************************************************************/


/**********************************************************************
 *
 *    Version number looks like this:
 *    "KEYWORD TMmmBBAA-<build> exename <optional string>"
 *
 * Where:
 *       KEYWORD = DLcid
 *       T  = type (0-Prod, 1-Beta, 2-Alpha, 3-Expr, 4-Special)
 *       M  = major number (0-9)
 *       mm = minor number (00-99)
 *       BB = beta number  (00-99)
 *       AA = alpha/experimental number (00-99)
 *       <build> - build number (reset to 0 when major/minor/alpha/beta change)
 *       <exename> - name of file this is a version of
 *       <optional string> - optional comment string
 *
 * Some examples:
 *
 *  "DLcid 31000002-00 d40drv.exe"
 *       1.00 Exp 0.02 of the file d40drv.exe
 *
 *  "DLcid 11000100-02 d40drv.exe"
 *        1.00 Beta 1 build 2 of d40drv.exe
 *
 ***************************************************************************/

#define  KEYWORD  "DLcid"

/* Where in string these parts of the version number reside */
#define  FIELD_TYPE        (sizeof(KEYWORD))
#define  FIELD_MAJOR       (FIELD_TYPE+1)
#define  FIELD_MINOR_DIG1  (FIELD_MAJOR+1)
#define  FIELD_MINOR_DIG2  (FIELD_MINOR_DIG1+1)
#define  FIELD_BETA_DIG1   (FIELD_MINOR_DIG2+1)
#define  FIELD_BETA_DIG2   (FIELD_BETA_DIG1+1)
#define  FIELD_ALPHA_DIG1  (FIELD_BETA_DIG2+1)
#define  FIELD_ALPHA_DIG2  (FIELD_ALPHA_DIG1+1)
#define  FIELD_BUILD_START (FIELD_ALPHA_DIG2+2)


/* Possible types */
#define  TYPE_PRODUCTION   '0'
#define  TYPE_BETA         '1'
#define  TYPE_ALPHA        '2'
#define  TYPE_EXP          '3'
#define  TYPE_SPEC         '4'


/* Options for String2DiaVersion function */
#define  VER_VERSION    0     /* Standard version number */
#define  VER_WITHBUILD  1     /* include the build number */
#define  VER_BUILDONLY  2     /* Only report back the build number */
#define  VER_EXENAME    3     /* Return the executable name */
#define  VER_COMMENT    4     /* Return only the comment */

/* Prototype */
#ifdef _cplusplus
extern "C"{
#endif
int String2DiaVersion(char *,int, char *);
int DiaVersion2String(char *,int, char *);
#ifdef _cplusplus
}
#endif














