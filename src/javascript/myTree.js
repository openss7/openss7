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
// All of the images are stored as inline data so as to not need an external images directory or
// files as well as not requiring many lookups to the server from the client.  Let the client do
// all of the processing.  All of these objects are a 16x16 pixel matrix including blank.
//
Array images;
images["blank"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAAAAAAAAACH5BAEAAAAALAAAAAAQABAAAAIOhI+py+0Po5y02ouzPgUAOw==";
images["obook"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI4nI+pB7DZhHAvSkmRvTcPEATcmDXhyFHAdKLYh7VpGYvYGmi1FXrr9Ot5DCtcoPH4gJBJ4rD5KAAAOw==";
images["obook-red"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook-red"] = "data:image/gif;base64,R0lGODlhEAAQAPIEAAAAAIsAAP8AAICAgP///wAAAAAAAAAAACH5BAEAAAUALAAAAAAQABAAAAM8WLrcvgC8FoWQs1qLmd5bVwBBAJ5dVJ4gBlwry41c3Ka1yQHE4OURQm8gel14w4jjxRsQRRTAc+KBUqkJADs=";
images["obook-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPIEAAAAAACAAACLAICAgP///wAAAAAAAAAAACH5BAEAAAUALAAAAAAQABAAAAM8WLrcvgC8FkOQs1qLmd5bVwCCAJ5dVJ4gBlwry41c3Ka1yQHE4OURQm8gel14w4jjxRsQRRTAc+KBUqkJADs=";
images["obook-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAEBAQL+/vwAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPIEAAAAAEBAQICAgL+/v////wAAAAAAAAAAACH5BAEAAAUALAAAAAAQABAAAAM8WLrcvgC8FseQs1qLmd5bVwBBAJ5dVJ4gBlwry41c3Ka1yQGE4OURQk8gel14w4jjxRMQRRTAc+KBUqkJADs=";
images["ocols"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI8nA0Jxw0iQGhMWXreiwkPHQRJhGmQtThR+LBTFZ3QqkpiSMPblmfrHeghYkThB2V5MYC2kLKieHqc0kEBADs=";
images["ccols"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI6nA0Jxw0iQGhMWUXfizDO6gVS4DncWYLjmB4atLVIyErmFttqIPLy8ELpXBfLh9FL8o4VBZNiWD4HBQA7";
images["ocols-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI8nA0Jxw0iQGhMWXreiwkPHQRJhGmQtThR+LBTFZ3QqkpiSMPblmfrHeghYkThB2V5MYC2kLKieHqc0kEBADs=";
images["ccols-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nA0Jxw0iQGhMWUXfizDO6gVS4DncWYLjmB4atLVIyErmFttqIPLy8ELpXBfLh9FL8o4VBZNiWD4HBQA7";
images["ocols-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI8nA0Jxw0RgGhMWXreiwkPLQhJhGmQtThR+LBTFZ3QqkpiSMPblmfrLeghYkThB2V5MYC2kLKieHqc0kEBADs=";
images["ccols-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nA0Jxw0RgGhMWUXfizDO6gmS4DncWYLjmB4atLVIyErmFtuqIPLy8ELpXBfLh9FL8o4VBZNiWD4HBQA7";
images["odocs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI2nI+ZwA16gJiTwSGDFvIpSXHNJ2ibBgRLGKaraXIqArLyCwc3bVdzVMrtgD0XzYH8HXK5i7MAADs=";
images["cdocs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+ZwO0KhJwShCWC3sJiOnlItHHiEYHVNWZl0LGoumIvfBqpmg/kG/vwZDqHkWi4lRRMRAEAOw==";
images["odocs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI2nI+ZwA16gJiTwSGDFvIpSXHNJ2ibBgRLGKaraXIqArLyCwc3bVdzVMrtgD0XzYH8HXK5i7MAADs=";
images["cdocs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+ZwO0KhJwShCWC3sJiOnlItHHiEYHVNWZl0LGoumIvfBqpmg/kG/vwZDqHkWi4lRRMRAEAOw==";
images["orows"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwA16gJiTwSGDFvIpSXHNJ2ibBgQJGFaq4cQxzJl2kKYY11K5KgvqaLdATRPpuVbCIaJou0gHBQA7";
images["crows"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+JwO0KhJwShBSD3sJiSnmLsHHiEYHVZTiu23ZlaXlRqtbXy5/ZLNOgVCtM73SYzRTMRAEAOw==";
images["ohost"] = "data:image/gif;base64,R0lGODlhEAAQAPIAAAAAAICAgL+/v/X19QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAQABAAAAM+SLrcCzBKwIC4OAuqrFvbIwxkqQxhNzIlyhHWwJ4pPLZt7X2Erv0v2GTSoAQIxySsgmwqjxyjcwoVDq+8bAIAOw==";
images["chost"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAICAgAAAAAAAACH5BAEAAAIALAAAAAAQABAAAAI0lI95wO2AQJi0Bmjkc3fdzXTZB4qCBmpeqo6sKVkWlrF0JgT5rt+QDuSdcMHiDmJ7KJaIAgA7";
images["ohost-red"] = "data:image/gif;base64,R0lGODlhEAAQAPIAAAAAAIsAAP8AAPX19QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAQABAAAANCSLrcGzDKwIK4OAuqrFvbIwxkqQxhNzIlyhHWwJ4pPLZt7X2Erv0v2GTSAGyOFguAYUwij0tC8+lUSgHYrFbL6yYAADs=";
images["chost-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAIsAAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI4nI95wO2AgJi0Cmhk2JzfJXTil4Wihw3auZGqybprnEqWlarPk2jhH3C9gkQgyVdMfnZMhuKJKAAAOw==";
images["ohost-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPIAAAAAAACAAACLAPX19QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAQABAAAANCSLrcKzBKwUS4OAeqrFvbEwxkqQxhNzIlyhHWwJ4pPLZt7X2Erv0v2GTSAGyOFguAYUwij0tC8+lUSgHYrFbL6yYAADs=";
images["chost-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI4nI95wO2AQJi0Bmik2JzfFXTil4Wihw3auZGqybprnEqWlarPk2jhL3C9gkQgyVdMfnZMhuKJKAAAOw==";
images["oinds"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI7nI+JwA16gJiTwSGDFvIpWTneImhMGQBBAlKUypbNBpPhu5KmGnB59GrVgDuVL+bCsUSO30EDjTovigIAOw==";
images["cinds"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwO0KhJwShBSD3sLiyYDXImiM1o1HRIlf0KAeWbHpC1vBTXf2vCrlLLxgq6IyOJbAw+a5UUgTBQA7";
images["oinds-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI7nI+JwA16gJiTwSGDFvIpWTneImhMGQBBAlKUypbNBpPhu5KmGnB59GrVgDuVL+bCsUSO30EDjTovigIAOw==";
images["cinds-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwO0KhJwShBSD3sLiyYDXImiM1o1HRIlf0KAeWbHpC1vBTXf2vCrlLLxgq6IyOJbAw+a5UUgTBQA7";
images["oinds-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI7nI+JwA16QJiTwSGFDvIpWTneEmhMKQBCAlKUypbNBpPhu5KmKnB59GrVgDuVL+bCsUSO30EDjTovigIAOw==";
images["cinds-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwO0KgpwSiBSF3sHiyYDXEmiM1o1HRImf0KAeWbHpC1vCTXf2vCrlLLxgq6IyOJbAw+a5UUgTBQA7";
images["onote"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+py40Ao4wHiIulAMFYfH0W54FhtHXDZ4LkKgSyHAehatH1nJati5sIX4OZ8ahyKJfMQQEAOw==";
images["cnote"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+py40Ao4wHiIulAMFYfH0W54FhtHXDZ4LkKgSyHAehatH1nJati5sIX4OZ8ahyKJfMQQEAOw==";
images["oobjs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["cobjs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["oobjs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["cobjs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["oobjs-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHAgUrklzAArrzCHetwnHSG5iWgmmw5YJ2r6i97iM8ACuvdNFhkREAQA7";
images["cobjs-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHAgUrklzAArrzCHetwnHSG5iWgmmw5YJ2r6i97iM8ACuvdNFhkREAQA7";
images["oobjs-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAEBAQL+/vwAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["cobjs-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAEBAQL+/vwAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["oparm"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI4nI+pCLDZhHAvykkZuOHmYQndhR2bSI6TeUqq1Jhu6rE0DASBjOL6bvPpFpvG7wHKDZGGJfNpKAAAOw==";
images["cparm"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI4nI+pCLDZhHAvykkZuOHmYQndhR2bSI6TeUqq1Jhu6rE0DASBjOL6bvPpFpvG7wHKDZGGJfNpKAAAOw==";

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

//
// An octet string is a type of value.
//
function OctetString(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// An object identity is a type of value.
//
function ObjectIdentity(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// A bits is a type of value.
//
function Bits(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// An Integer is a type of value.
//
function Integer(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// An Counter64 is a type of value.
//
function Counter64(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// An Unsigned32 is a type of value.
//
function Unsigned32(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// An IPaddr is a type of value.
//
function IPaddr(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// An Netaddr is a type of value.
//
function Null(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}
//
// An Netaddr is a type of value.
//
function Null(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidDescrip = oidDescrip;
	this.oidName = oidName;
}

//
// A Table is a node in the oid tree that has rows associated with it.  A table
// is annotated with a dog-earred or flat document.
//
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

function MyObject(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
function MyConceptualRow(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.imageOpen = "mini_doc5.xpm";
	this.imageClosed = "mini_doc6.xpm";
	this.imageColor = "";
}
function MyConceptualIndex(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.imageOpen = "mini_ind1.xpm";
	this.imageClosed = "mini_ind2.xpm";
	this.imageColor = "";
}
function MyConceptualColumn(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
function MyConceptualTable(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
function MyObjectIdentifier(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.image = "book";
	this.color = "";
	this.state = "c";
}
function MyModuleIdentity(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
function MyNotification(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.image = "note";
	this.color = "";
	this.state = "c";
}
function MyNotifyParameter(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.image = "parm";
	this.color = "";
	this.state = "c";
}

function MyObjectType(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.image = "objs";
	this.color = "";
	this.state = "c";
}
function MyObjectIdentity(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.image = "objs";
	this.color = "-blu";
	this.state = "c";
}
function myLeaf(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
	this.image = "objs";
	this.color = "";
	this.state = "c";
}
function MyObject(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
function MyObject(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
function MyObject(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
function MyObject(oidNumber, oidName, oidDescrip) // constructor
{
	this.oidNumber = oidNumber;
	this.oidName = oidName;
	this.oidDescrip = oidDescrip;
}
