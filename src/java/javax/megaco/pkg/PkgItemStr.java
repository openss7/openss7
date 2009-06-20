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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.pkg
Class PkgItemStr

java.lang.Object
  |
  +--javax.megaco.pkg.PkgItemStr

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          None
     _________________________________________________________________

   public class PkgItemStr

   The MEGACO Package Item java.lang.String class is used to define the Package,
   Items and Parameters in the string format. If the implementations uses
   a MEGACO package which is not defined in the javax.megaco.pkg package,
   then the values of Package, Item and Parameter, are exchanged using
   this class. This class has been provided to take care of the
   extensibility within the protocol using protocol packages. The
   descriptor classes have necessary method to pass reference to the
   object of the PkgItemStr class in addition to the existing interface
   for PkgEventItem or PkgSignalItem etc.

   Note:
   Since there is no configuration of the package parameters passed in
   this class, the parameters passed are not validated.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   PkgItemStr()
              Constructs a MEGACO Package Item class to specify the
   package parameters in the string format.

   Method Summary
   final void setPkgName(java.lang.String pkgName)
       throws javax.megaco.InvalidArgumentException
              This method is used to specify the Package name. The
   Package Name specified is in string format.

                                                                    final
                                                         java.lang.String

   getAssociatedPkgName()
              This method is used to get the Package name. The Package
   Name returned is in string format.

                                                               final void

   setItemName(java.lang.String itemName)
       throws javax.megaco.InvalidArgumentException
              This method is used to specify the Item name. The Item Name
   specified is in string format.

                                                                    final
                                                         java.lang.String

   getItemName()
              This method is used to get the Item name. The Item Name
   returned is in string format.

                                                               final void

   setParameter(java.lang.String parameter)
       throws javax.megaco.InvalidArgumentException
              This method is used to specify the parameter. The parameter
   here contains the parameter name followed by the relational operator
   and then the parameter values. The parameter specified must be in the
   syntax defined by the MEGACO protocol.

                                                                    final
                                                         java.lang.String

   getParameter()
              This method is used to get the paramter. The parameter
   returned here contains the parameter name followed by the relational
   operator and then the parameter values. The parameter returned is in
   the syntax defined by the MEGACO protocol.

                                                               final void

   setItemType(PktItemType itemType)
       throws javax.megaco.InvalidArgumentException

              This method is used to specify the Item type.

                                                                final int

   getItemType()
              This method is used to get the Item type. This method
   returns one of the constants defined in the class PktItemType.
    java.lang.String toString()
              Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  PkgItemStr

public PkgItemStr()

          Constructs a MEGACO Package Item class to specify the package
          parameters in the string format. This class would be used to
          specify the package parameters for the package which is not
          defined in the javax.megaco.pkg package.

   Method Detail

  setPkgName

public final void setPkgName(java.lang.String pkgName)
    throws javax.megaco.InvalidArgumentException

          This method is used to set the package name. The package name
          specified must be in the syntax defined by the MEGACO
          protocol.Since the pkgName specified is not configured in the
          class, therefore its value cannot be validated.

        Parameters:
        pkgName - The string corresponding to the package name.
             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Package name string passed to this
                method is NULL.
     _________________________________________________________________

  getAssociatedPkgName

public final java.lang.String getAssociatedPkgName()

          This method is used to get the package name. The package name
          returned is in the string format.

        Returns:
                java.lang.String corresponding to the package name. In case no
                value for package name is present, then this method shall
                return NULL.
     _________________________________________________________________

  setItemName

public final void setItemName(java.lang.String itemName)
    throws javax.megaco.InvalidArgumentException

          This method is used to set the item name. The item name
          specified must be in the syntax defined by the MEGACO
          protocol.Since the item name specified is not configured in the
          class, therefore its value cannot be validated.

        Parameters:
        itemName - The string corresponding to the item name.
             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Item name string passed to this method
                is NULL.
     _________________________________________________________________

  getItemName

public final java.lang.String getItemName()

          This method is used to get the item name. The item name
          returned is in the string format.

        Returns:
                java.lang.String corresponding to the item name. In case no value
                for item name is present, then this method shall return
                NULL.
     _________________________________________________________________

  setParameter

public final void setParameter(java.lang.String parameter)
    throws javax.megaco.InvalidArgumentException

          This method is used to set the package parameter. The parameter
          specified here has parameter name followed by the relational
          operator and then followed by the parameter values. The
          parameter specified must be in the syntax defined by the MEGACO
          protocol.Since the parameter is not configured in the class,
          therefore its value cannot be validated.

        Parameters:
        parameter - The string corresponding to the package parameters.
             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Package Item object passed to this
                method is NULL.
     _________________________________________________________________

  getParameter

public final java.lang.String getParameter()

          This method is used to get the package parameter. The parameter
          returnes here has parameter name followed by the relational
          operator and then followed by the parameter values.

        Returns:
                java.lang.String corresponding to the package parameter. In case no
                value for parameter is specified, then this methos shall
                return NULL.
     _________________________________________________________________

  setItemType

   public final void setItemType(PkgItemType itemType)
       throws javax.megaco.InvalidArgumentException

          This method is used to set the Item type for which the
          parameters are specified. The item_type is set to M_ITEM_ALL if
          the item_name is '*'. The item_type is set to M_ITEM_EVENT, if
          the descriptor for which the parameters are specified is one of
          Event Descriptor, Observed Event Descriptor or Event Buffer
          Descriptor. The item_type is set to M_ITEM_STATISTICS, if the
          descriptor for which the parameters are specified is Statistics
          Descriptor. The item_type is set to M_ITEM_SIGNAL, if the
          descriptor for which the parameters are specified is Signal
          Descriptor. The item_type is set to M_ITEM_PROPERTY, if the
          descriptor for which the parameters are specified is Media
          Descriptor.

        Parameters:
                item_type : The object reference of the class PkgItemType
                which identifies the item type to which the parameter
                belongs.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Package Item object passed to this
                method is NULL.
     _________________________________________________________________

  getItemType

public final int getItemType()

          This method returns the item_type to which the package
          parameters belong. This method returns integer value which is
          one of field constants defined for the class PkgItemType .

        Returns:
                The integer value which identifies the item type to which
                the parameter belongs.
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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
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
