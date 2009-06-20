// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.pkg
Class PkgItem

java.lang.Object
  |
  +--javax.megaco.pkg.PkgItem

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          PkgEventItem, PkgSignalItem, PkgStatsItem, PkgPrptyItem,

   See Also:
          Serialized Form
     _________________________________________________________________

   public class PkgItem
   extends java.lang.Object

   The MEGACO item class is defined as the base class for all the items
   in MEGACO packages. This class shall be used for setting the events,
   signals, statistics, properties, list of parameters names and their
   values.
     _________________________________________________________________

   Constructor Summary
   PkgItem()
             Constructs a MEGACO Event Item or Signal Item or Statistics
   Item or Property Item from the base class.

   Method Summary
    int  getItemId()
              The method returns the value of the item id as defined in
   the MEGACO Package. This method may be overriden by the derived class.
   When called from within this class shall return a value 0xFFFF to
   indicate all values.
    int  getItemType()
              This method specifies the item type as event or signal or
   stats or property. This method is overriden by the derived class. When
   the item is to represent all events or signal or statistics or
   property, then the object would be formed of the current class and
   would specify the item type as M_ITEM_ALL.
    MegacoPkg getItemsPkgId()
              This method gets the reference to the package id to which
   this item belongs. When this method from the current class is called,
   it shall return a NULL value.
     final MegacoPkg getAssociatedPkgId()
           throws javax.megaco.ParameterNotSetException

              This method gets the object reference of the package id
   which is associated when the item is created as part of a megaco
   message. Though the item might belong to one package according to the
   protocol, but while sending it across, the item might be sent for the
   same package that the protocol specfies or a package that extends this
   package.
     void  setAssociatedPkgId(MegacoPkg package)
           throws javax.megaco.InvalidArgumentException

              This method sets the object reference of the package id
   which is associated when the item is created as part of a megaco
   parameter. This may be overriden by the derived class.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  PkgItem

public PkgItem()

          Constructs a Jain MEGACO Item Object. This class may be derived
          by the different MEGACO event, signal, property and statistics
          item classes. For item to represent ALL items, object of this
          class is created.

   Method Detail

  getItemId

public int getItemId()

          The method returns the value of the item id as defined in the
          MEGACO Package. This method may be overriden by the derived
          class. When called from within this class shall return a value
          0xFFFF to indicate all values.

        Returns:
                itemId - An integer value representing the item as
                defined in the megaco package..
     _________________________________________________________________

  getItemType

public int getItemType()

          This method may be overriden by the derived class. This shall
          return an integer value defined in PkgItemType.

        Returns:
                itemType - An integer value for the item type
                corresponding to the derived object. This shall return an
                integer value M_ITEM_ALL when called from this class.
     _________________________________________________________________

  getItemsPkgId

public MegacoPkg getItemsPkgId()

          This method gets the object reference for the package id to
          which the item belongs.

        Returns:
                packageId - The object reference of the package object to
                which the item is associated. It shall return a NULL
                value when the method from this class is called,
                indicating that there is no package specified for item as
                "*".
     _________________________________________________________________

  getAssociatedPkgId

public final MegacoPkg getAssociatedPkgId()
      throws javax.megaco.ParameterNotSetException

          This method gets the object reference for the package id for
          which this item has been set in the megaco parameters. If the
          package that is to be associated is ALL, i.e. "*", then a
          package id need not be associated explicitly. If the package is
          not associated it would imply that the package id used for
          dynamic linking is "*". If the package associated is ALL, then
          the item too definitely needs to be ALL and hence in that case
          item object should be formed from the current class and not
          from any derived class. Though the item might belong to one
          package according to the protocol, but while sending it across,
          the item might be sent for the same package that the protocol
          specfies or a package that extends this package.

        Returns:
                packageId - The object reference of the package object to
                which the item is dynamically associated.

        Throws:
                ParameterNotSetException If the Package had not been
                associated and the item type too denotes anything other
                than M_ITEM_ALL.
     _________________________________________________________________

  setAssociatedPkgId

public void setAssociatedPkgId(MegacoPkg packageId)
      throws javax.megaco.InvalidArgumentException

          This method sets the object reference for the package id for
          which this item has been set in the megaco parameters. If the
          package that is to be associated is ALL, i.e. "*", then a
          package id need not be associated explicitly. If the package is
          not associated it would imply that the package id used for
          dynamic linking is "*". If the package associated is ALL, then
          the item too definitely needs to be ALL and hence in that case
          item object should be formed from the current class and not
          from any derived class. Though the item might belong to one
          package according to the protocol, but while sending it across,
          the item might be sent for the same package that the protocol
          specfies or a package that extends this package. This method
          may be overriden by the derived class.

        Parameter:
                packageId - The object reference of the package object to
                which the item is dynamically associated.

        Throws:
                InvalidArgumentException If the package that is
                dynamically being linked cannot be set due to the fact
                that the item does not belong to the package or the
                package is not one of the ancestor package of the
                package, to which the item belongs.
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
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
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
