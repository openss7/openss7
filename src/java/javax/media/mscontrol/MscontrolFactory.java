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

package javax.media.mscontrol;
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

Class MscontrolFactory

java.lang.Object
   extended by  javax.media.mscontrol.MscontrolFactory
     _________________________________________________________________

public class MscontrolFactory

   extends java.lang.Object

   This factory creates MediaSessionFactory for a particular
   implementation of the JSR 309.
     _________________________________________________________________

   Method Summary
    MediaSessionFactory
   createMediaSessionFactory(java.util.Properties aProperties)
             Creates a vendor specific implementation of the
   MediaSessionFactory The name of the instantiated class is:
   "getPathName() + .javax.media.mscontrol.MediaSessionFactoryImpl"
   static MscontrolFactory getInstance()
             Return a single instance of the JmscFactory
    java.lang.String getPathName()
             Get the pathname that identifies the location of a
   particular vendor's implementation of this specification.
    void setPathName(java.lang.String aPathName)
             Sets the pathname that identifies the location of a
   particular vendor's implementation of this specification.



   Methods inherited from class java.lang.Object
   equals, getClass, hashCode, notify, notifyAll, toString, wait, wait,
   wait



   Method Detail

  setPathName

public void setPathName(java.lang.String aPathName)

          Sets the pathname that identifies the location of a particular
          vendor's implementation of this specification.

        Parameters:
                aPathName -
     _________________________________________________________________

  getPathName

public java.lang.String getPathName()

          Get the pathname that identifies the location of a particular
          vendor's implementation of this specification.

        Returns:
                String
     _________________________________________________________________

  getInstance

public static MscontrolFactory getInstance()
                                    throws MscontrolException

          Return a single instance of the JmscFactory

        Returns:
                MscontrolFactory

        Throws:
                MscontrolException
     _________________________________________________________________

  createMediaSessionFactory

public MediaSessionFactory createMediaSessionFactory(java.util.Properties aProp
erties)
                                              throws MscontrolException

          Creates a vendor specific implementation of the
          MediaSessionFactory

          The name of the instantiated class is:

 "getPathName() + .javax.media.mscontrol.MediaSessionFactoryImpl"


        Parameters:
                aProperties - The instantiated factory class is passed a
                java.util.Properties as constructor parameter, containing
                any required factory options. The definition of these
                options is implementation-dependent.

        Returns:
                MediaSessionFactory

        Throws:
                MscontrolException

        See Also:
                to specifiy the selected implementation, e.g.
                "com.msvendor.jmsc"
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright © 2007-2008 Oracle and/or its affiliates. © Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
