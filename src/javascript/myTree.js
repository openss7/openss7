//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================


// This is an implementation of a JavaScript tree menu structure much the same
// as that used in the OpenSS7 PerlTk implementation of the MIB browser.  The
// only difference is that JavaScript and the browser are used as the front-end
// instead of Tk.  The API is designed in particular for SNMP and is not general
// purpose at all.  There are several kinds of objects in SNMP, that do not map
// directly onto the "Folders & Documents" approach taken by general purpose
// JavaScript tree implementations.
//
// A node:  just a node in the OID tree with no entity associated with it.
// A MIB:   the root of a rooted tree of entities belonging to the same
//	    management information base.
// An OBJECT IDENTITY entity.
// An OBJECT outside of a table (scalar).
// A TABLE.
// An INDEX (ROW).
// An ENTRY (COLUMN).
//
// It is desired that the semantic interface be as close as possible to the tree
// widget in Tk, so that the existing mibbrowser and stream-manager code can be
// reused to the extent possible.
//


//
// An Oid is just a node in the oid tree.  It does not have any special
// attachment associated with it; however, it may be named (e.g. iso).  The icon
// for an Oid is simply an open or closed circle.
//
function Oid(oidNumber, oidName, oidDescrip) //constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}

//
// A Mib is a node in the oid tree that represents the root of a MIB.  A Mib is
// represented as a open or closed book.
//
function Mib(mibNumber, mibName, mibDescrip) // constructor
{
	this.oidNumber = mibNumber;
	this.oidDescrip = mibDescrip;
	this.oidName = mibName;
}

//
// An ObjectIdentity is a node in the oid tree that represents some globally
// unique value (such as the identifier for an algorithm).  An object identity
// is annotated with a open or closed circle.
//
function ObjectIdentity(oiNumber, oiName, oiDescrip) // constructor
{
	this.oidNumber = oiNumber;
	this.oidDescrip = oiDescrip;
	this.oidName = oiName;
}

//
// A Scalar is a node in the oid tree that has a single instance value
// associated with it.  A scalar is annotated with a dog-eared or flat document
// symbol.
//
function Scalar(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}

function Table(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}

function Row(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}

function Index(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}

function Column(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}

//
// A Notification represents a NOTIFICATION-TYPE in the oid tree.  It is
// identified by a dog-eared or flat document with an `i' in the middle of the
// page.
//
function Notification(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
