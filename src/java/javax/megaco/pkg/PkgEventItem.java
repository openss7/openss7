// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/pkg/PkgEventItem.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
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
Class PkgEventItem

java.lang.Object
  |
  +--javax.megaco.pkg.PkgItem
        |
        +--javax.megaco.pkg.PkgEventItem

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          AnalogLineSFlashEvt , AnalogLineSOffEvt , AnalogLineSOnEvt ,
          ContComplEvt, CPToneDetBtEvt, CPToneDetCrEvt , CPToneDetCtEvt ,
          CPToneDetCwtEvt , CPToneDetDtEvt , CPToneDetPtEvt ,
          CPToneDetRtEvt , CPToneDetSitEvt , CPToneDetWtEvt, DTMFDetCeEvt
          , GenCauseEvent , GenSigComplEvent , NetworkNetfailEvt ,
          NetworkQualertEvt , RTPPltransEvt , MegacoToneEtdEventl ,
          ToneLtdEvent , ToneStdEvent

   See Also:
          Serialized Form
     _________________________________________________________________

   public abstract class PkgEventItem
   extends PkgItem

   The MEGACO event item class is an abstract class defined as the base
   class for all the event items in the MEGACO Package. This class shall
   be used for setting the events, list of parameters names and their
   values. This extends the PkgItem class. This is an abstract class and
   hence cannot be created as a separate object.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   PkgEventItem()
              Constructs a MEGACO Event item from an abstract class.

   Method Summary
    abstract int  getEventId()
              The method can be used to get the event identifier.
   final  PkgItemParam[]  getMegacoPkgItemParam()
              The method can be used to get the attached Parameter name
   and the corresponding values.
   final   void  setMegacoPkgItemParam
              (PkgItemParam[] paramInfo)
           throws javax.megaco.InvalidArgumentException,
   javax.megaco.MethodInvocationException,

              The method can be used to set the vector of object
   identifier to get the Parameter name and their corresponding values.
    java.lang.String toString()
              Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Methods inherited from class javax.megaco.pkg.PkgItem
   getItemName, getItemId, getItemType, getItemsPkgId, setAssociatedPkgId

   Constructor Detail

  PkgEventItem

public PkgEventItem()

          Constructs a Jain MEGACO Package Event Item Object. This is an
          abstract class and can be called only by the derived classes.

   Method Detail

  getEventId

public abstract int getEventId()

          The method can be used to get the event identifier. This method
          gives the event id of the event item. This is an abstract
          method and is defined by the methods of the derived class which
          shall return an hard coded value for the event id.

        Returns:
                eventId - The integer corresponding to the event id.
     _________________________________________________________________

  getItemId

public abstract int getItemId()

          The method can be used to get the event identifier. This method
          gives the item id of the event item. This is an abstract method
          and is defined by the methods of the derived class which shall
          return an hard coded value for the event id.

        Returns:
                eventId - The integer corresponding to the event id.
     _________________________________________________________________

  getItemType

public final int getItemType()

          This method overrides the corresponding method in PkgItem. This
          shall return an hard coded value to indicate the item type is
          event.

        Returns:
                itemType - An integer value for the item type
                corresponding to the event. This shall return
                M_ITEM_EVENT.
     _________________________________________________________________

  setAssociatedPkgId

public void setAssociatedPkgId(MegacoPkg packageId)
      throws javax.megaco.InvalidArgumentException

          This method overrides the corresponding method in PkgItem. This
          method would set a flag to indicate that the dynamic package
          has been associated and would not allow the application to
          overwrite the package association once the parameter has been
          set for the item. If the package name can be linked, then this
          method inturn calls the corresponding method of the PkgItem
          class.

        Parameter:
                packageId - The object reference of the package object to
                which the item is dynamically associated.

        Throws:
                InvalidArgumentException This exception will be raised in
                the following cases
                    1. If the method is called to modify the associated
                package after the parameter has been set.
                    2. If the package that is dynamically being linked
                cannot be set due to the fact that the item does not
                belong to the package or the package is not one of the
                ancestor package of the package, to which the item
                belongs.
                    3. If the reference of package Id passed to this
                method is NULL.
     _________________________________________________________________

  getMegacoPkgItemParam

public final PkgItemParam[] getMegacoPkgItemParam()

          The method can be used to get the vector object identifier that
          specifies the attached Parameter name and the corresponding
          values for the event.

        Returns:
                paramInfo - The vector of objectIdentifier corresponding
                to the param information set for the package. If the
                parameter is not set then this shall return a NULL value.
     _________________________________________________________________

  setMegacoPkgItemParam

public final void setMegacoPkgItemParam
                           (PkgItemParam[] paramInfo)
      throws javax.megaco.InvalidArgumentException, javax.megaco.MethodInvocati
onException

          The method can be used to set the Parameter information. This
          method sets the attached parameters for the event. This method
          should be called only after a package has been associated with
          the item. Once the parameter has been set, it shall not allow
          the associated item to be overwritten. This method shall verify
          the following:

    1. The parameter can be set for the relevant item.
    2. The parameter too belongs to the same package for which the item
       belongs.
    3. If the parameter does not belong to the same package, then does it
       belong to a package that extends the package to which the item
       belongs.
    4. If the parameter and the item belong to different packages, but if
       the package to which the parameter belongs extends the package to
       which the item belongs, then it should validate that the current
       association of the item is to the package to which the parameter
       belongs or should be such that it extends the package to which the
       parameter belongs.

   Parameter:
          paramInfo - The vector of objectIdentifier corresponding to the
          parameter information.

   Throws:
          InvalidArgumentException If the parameter cannot be linked to
          the current event.
          MethodInvocationException If the item has not been associated
          with a package.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().


        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright (C) 2001 Hughes Software Systems
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
