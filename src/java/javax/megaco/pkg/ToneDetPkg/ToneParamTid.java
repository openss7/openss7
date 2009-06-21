// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: ToneParamTid.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:48 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:35:48 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.pkg
Class ToneParamTid

java.lang.Object
  |
  +--javax.megaco.pkg.PkgItemParam
         |
         +--javax.megaco.pkg.ToneDetPkg.ToneParamTid

   All Implemented Interfaces:
          None
     _________________________________________________________________

   public class ToneParamTid
   extends java.lang.Object

   The MEGACO parameter class for the ToneId Parameter is associated with
   Start Tone Detect, Long Tone Detect and End Tone Detect event of
   Network Package. This class defines all the static information for
   this parameter.
     _________________________________________________________________

   Constructor Summary
   ToneParamTid()
              Constructs a Tone package parameter class that specifies
   the parameter as tone id.

   Field Summary
    public static final int  TONE_DET_PARAM_TID
               Identifies ToneId parameter of the MEGACO Tone Detect
   Package.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Methods inherited from class javax.megaco.pkg.PkgItemParam
   getParamId, getParamName, getParamValueType, getParamsDescriptorIds,
   getParamsItemIds, getParamsPkgId

   Constructor Detail

  ToneParamTid

public ToneParamTid()

          Constructs a parameter class for tone detect package that
          specifies the parameter as tone id.

   Method Detail

  getParamId

public int getParamId()

          The method can be used to get the parameter identifier as
          defined in the MEGACO packages. The implementation of this
          method in this class returns Id of Tone Id parameter.


        Returns:
                paramId - Returns param id as TONE_DET_PARAM_TID.
     _________________________________________________________________

  getParamValueType

public int getParamValueType()

          The method can be used to get the type of the parameter as
          defined in the MEGACO packages. These could be one of string or
          enumerated value or integer or double value or boolean.


        Returns:
                It returns M_ITEM_PARAM_VALUE_STRING indicating that the
                parameter is of type strings.
     _________________________________________________________________

  getParamsDescriptorIds

public int[] getParamsDescriptorIds()

          The method can be used to get the descriptor ids corresponding
          to the parameters to which the parameter can be set.

        Returns:
                This parameter can be present in Event descriptor. It
                shall thus return a value M_OBSERVED_EVENT_DESC as a part
                of integer vector.
     _________________________________________________________________

  getParamsItemIds

public int[] getParamsItemIds()

          The method can be used to get the item ids corresponding to the
          parameters to which the parameter can be set. This method
          specifies the valid item (event/signal) ids to which the
          parameter can belong to.

        Returns:
                The integer values corresponding to start tone detected,
                end tone detected and long tone detected. Thus this shall
                return a vector containing the elements
                TONE_DET_STD_EVENT, TONE_DET_ETD_EVENT and
                TONE_DET_LTD_EVENT .
     _________________________________________________________________

  getParamsPkgId

public int getParamsPkgId()

          The method can be used to get the package id corresponding to
          the to which the parameter can be set. This method specifies
          the package for which the parameter is valid. Even though the
          parameter may be set for an item, but the parameter may not be
          valid for package to which the item belongs, but may be valid
          for a package which has extended this package.

        Returns:
                This shall return TONE_DET_PACKAGE as the integer value.
                The integer values are defined in PkgConsts.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().


        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Field Detail

  TONE_DET_PARAM_TID

public static final int TONE_DET_PARAM_TID

          Identifies Duration parameter of the MEGACO Tone Detect
          Package. Its value shall be set equal to 0x0003.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright © 2001 Hughes Software Systems
       HUGHES SOFTWARE SYSTEMS and JAIN JSPA SIGNATORIES PROPRIETARY
        This document contains proprietary information that shall be
     distributed, routed or made available only within Hughes Software
       Systems and JAIN JSPA Signatory Companies, except with written
                   permission of Hughes Software Systems
             _________________________________________________

   22 December 2003


    If you have any comments or queries, please mail them to
    Jmegaco_hss@hss.hns.com
*/
