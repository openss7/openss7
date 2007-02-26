/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
//========================================================================
// PURPOSE:
//    Version file
//
//    Version number looks like this:
//    "KEYWORD TMmmBBAA-<build> exename <optional string>"
//
// Where:
//       KEYWORD = DLcid
//       T  = type (0-Prod, 1-Beta, 2-Alpha, 3-Expr, 4-Special)
//       M  = major number (0-9)
//       mm = minor number (00-99)
//       BB = beta number  (00-99)
//       AA = alpha/experimental number (00-99)
//       <build> - build number (reset to 0 when major/minor/alpha/beta change)
//       <exename> - name of file this is a version of
//       <optional string> - optional comment string
//
// Some examples:
//
//  "DLcid 31000002-00 d40drv.exe"
//       1.00 Exp 0.02 of the file d40drv.exe
//
//  "DLcid 11000100-02 d40drv.exe"
//        1.00 Beta 1 build 2 of d40drv.exe
//
//========================================================================

#ifndef _PMACDVERSION_
#define _PMACDVERSION_

char VersionString[41]="DLcid 04000000-04 pmac.o";

#endif // _PMACDVERSION_
