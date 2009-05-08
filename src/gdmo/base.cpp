//  ==========================================================================
//  
//  @(#) $Id: base.cpp,v 0.9.2.1 2007/09/06 10:59:09 brian Exp $
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by the Free
//  Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//  
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <http://www.gnu.org/licenses/>, or write
//  to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
//  USA.
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
//  Last Modified $Date: 2007/09/06 10:59:09 $ by $Author: brian $
//  
//  --------------------------------------------------------------------------
//  
//  $Log: base.cpp,v $
//  Revision 0.9.2.1  2007/09/06 10:59:09  brian
//  - added new gdmo files
//
//  ==========================================================================

#ident "@(#) $Id: base.cpp,v 0.9.2.1 2007/09/06 10:59:09 brian Exp $"

static char const ident[] = "$Id: base.cpp,v 0.9.2.1 2007/09/06 10:59:09 brian Exp $";

GdmoObject::GdmoObject(const char *n) : name(n) { };

//

ManagedObject::ManagedObject(const char *n)
: GdmoObject(n) {
};

Package::Package(const char *n)
: GdmoObject(n) {
};

Parameter::Parameter(const char *n)
: GdmoObject(n) {
};

NameBinding::NameBinding(const char *n)
: GdmoObject(n) {
};

Attribute::Attribute(const char *n)
: GdmoObject(n) {
};

AttributeGroup::AttributeGroup(const char *n)
: GdmoObject(n) {
};

Behaviour::Behaviour(const char *n, const char *d)
: GdmoObject(n), definedas(d) {
};

Action::Action(const char *n)
: GdmoObject(n) {
};

Notification::Notification(const char *n)
: GdmoObject(n) {
};

ConstraintRule::ConstraintRule(const char *n)
: GdmoObject(n) {
};

SubordinationRule::SubordinationRule(const char *n)
: GdmoObject(n) {
};
