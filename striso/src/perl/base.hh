//  ==========================================================================
//  
//  @(#) $Id$
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
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

#ident "@(#) $Id$"

//
//  Base class definitions for GDMO objects.
//

#ifndef __HEAD_HH__
#define __HEAD_HH__

#pragma interface

class MangedObjectFactory;
class MangedObject;
class Package;
class Parameter;
class NameBinding;
class Attribute;
class AttributeGroup;
class Behaviour;
class Action;
class Notification;
class ConstraintRule;
class SubordinationRule;

class GdmoObject {
    private:
	const char *const name;
    public:
	GdmoObject(const char *n);
};

class MangedObjectFactory {
	// Each factory interface shall define a single operation for clients to use to create
	// objects.  The name of this operation shall be "create" and it shall return a reference to
	// the type of object created by the factory.  The first four paraemters to every create
	// operation are alwasy the same.  After these come parameters for each writable or
	// set-by-create attribute defined for the managed object.  (A set-by-create attribute is one
	// for which the object has no "set" operation, but for which a value is specified on the
	// create operation.)  The names of these paramters are the same as the name of the attribute.
	// (This is that name of an attribute accessor operation minus the ending "Get" or "Set".)
	// Each create operation also has to access paraemters to set the values of any writable or
	// set-by-create attributes ofa ll super-classes of the object created by the factory.
	static ManagedObject& create(NameBinding& nameBinding,
			ManagedObject& superiorObject,
			basic_string& name,
			list <ObjectClass>& packages);
	// nameBinding:
	//
	// The nameBinding parameter conveys the name of a module containing managed object name
	// binding information, as described in Section 9.8.  An example value might be
	// "itu_m3120::NameBindings::Equipment_Equipment".  Given this, the factory can check to see
	// if the value is a valid name binding identifier.  (A factory might either be "hard-coded"
	// with the name binding information available when the system is compiled, or it might access
	// the information in the CORBA Interface Repository at run-time.)  If the name binding
	// information can not be found, the factory shall rails an invalidParameter ApplicationError
	// exception, returning "nameBinding" as an argument.  (This is an ApplicationError exception
	// with the error code set to invalidParameter and the details string set to "nameBinding".)
	// If the name binding informaiton can be found but is incomplete, the factory shall raise an
	// InvalidNameBinding CreateError exception.
	//
	// The factory must also check to see if the subordinate calss type specified in the name
	// binding module matches the type of objects it creates.  If it is doesn't, the factory can
	// then check to see if the type of objects it creates is a a subclass of the subordinate
	// calss constant value.  If it is, and if the subordinateSubclassesAllowed constant is true,
	// it can proceed to create the object.  If not, it would reject the request by raingin an
	// invalidNameBinding CreateError exception.
	//
	// Finally, if the managesMayCreate constant in the name binding module is false, the factory
	// would also reject the request by raising an invalidNameBinding CreateError exception.
	// (Factorie may have a second create operation for internal use by the managed system that
	// does not check this value and that is not exposed across the management interface.)  The
	// inclusion of name binding modules with managersMayCreate values set to false enables
	// capturing all of the containment information in IDL, as is possible with GDMO, even if the
	// objects are created only by the managed system iself.
	//
	// The other information in the name binding module will be used by the factory when it
	// creates the object and its CORBA naming service name binding.  The deletePolicy constant
	// will be assigned to the new managed object's attribute of the same name.  The 'kind'
	// constant value will be used when the factory creates the managed object's name binding in
	// the CORBA naming service.
	//
	// superiorObject:
	//
	// The second parameter in the create operation is a reference to the superior object, under
	// which the new object is to be created.  Using standard CORBA capabilities, the factory
	// shall examine the class of the superior object to determine if it matches the type
	// specified in the superiorCLass constant defined in the name binding module.  If it doesn't,
	// the factory must check to see if the supplied reference is as subclass of the type
	// specified in the superiorClass constant.  If it is, and if the superiorSubclassesAllowed
	// constant in the name binding is true, the factory may proceed to create the object.  If
	// not, the factory must reject the request by raising an invalidNameBinding CreateError
	// exception, returning "superiorObject" int he details.
	//
	// If the superiorClass constant in the name binding module is an emptyr string, then objects
	// of the subordinate class may be created with no superior object (parent), and their name is
	// bound directly to a local root naming context.  Usually, these objects will be created by
	// the managed system, but in these cases the superio object reference would be null.
	//
	// name:
	//
	// The third parameter is the name to be assigned to the new object.  This string will become
	// the ID field of the CORBA Name Binding created in the CORBA naming service for the new
	// object.  This will be relative to the superior object's name.  If the parameter is inout,
	// it indicates that the factory must support auto-naming.  In this case, a client may submit
	// a null string for the new name, and the factory will choose a suitable string and return
	// the chosen value.  If instead the client submits a string, the factory shall use this value
	// instead (and return it as the out value).  If the parameter is in only, auto-naming is not
	// supported and the client must supply a name.  If it doesn't, the factory shall raise a
	// badName CreateError exception.  The factory raises a duplicateName CreateError exception if
	// the supplied name is a duplicate.  (This means both the ID and kind fields match an
	// existing object contained by the superior object.)
	//
	// packages:
	//
	// The packages attribute is important.  It tells the factory not only which packages an
	// instance must support, but which parameter values on the create operation it must ignore.
	// Because they are strongly typed, create methods include a paraemter for each writable or
	// set-by-create attriabute of an object, even if an attribute is part of a conditional
	// package.  The factory must ignore the values for any attribute in packages that are not
	// requested by the client, even if the factory instantiates the object with the package
	// anyway.  (If the factory instantiates an object with a package not requested by the client,
	// the factory must choose the initial values.)  This frees the client form having to supply
	// values for attribtues in packages it does not want.  Instead, the client can submit any
	// value.  For efficiency, the values submitted for attributes in packages not requested by
	// teh client should be short.
	//
}; // class ManagedObjectFactory

class MangedObject : public GdmoObject {
    public:
	ManagedObject(const char *n);
}; // class ManagedObject

class Package : public GdmoObject {
    public:
	Package(const char *n);
}; // class Package

class Parameter : public GdmoObject {
    public:
	Parameter(const char *n);
}; // class Parameter

class NameBinding : public GdmoObject {
    public:
	NameBinding(const char *n);
}; // class NameBinding

class Attribute : public GdmoObject {
    protected:
	virtual Attribute& Get() { return this; };
	virtual Replace(Attibute& that) { this = that; };
	virtual Set(Attribute& that) { this  = that; };
	virtual ReplaceWithDefault();
	virtual SetDefault();
	virtual Add(Attribute& that) { this += that; };
	virtual Remove(Attribute& that) { this -= that; };
	virtual SetByCreate(Attribute& that) { this = that; };
	virtual Attribute& operator+(Attribute& that) { this->Include(that); return this; }
	virtual Attribute& operator-(Attribute& that) { this->Exclude(that); return this; }
    public:
	virtual Include(Attribute &that);
	virtual Exclude(Attribute &that);
	Attribute(const char *n);
}; // class Attribute

class AttributeGroup : public GdmoObject {
    public:
	AttributeGroup(const char *n);
}; // class AttributeGroup

class Behaviour : public GdmoObject {
    private:
	const char *const definedas;
    public:
	Behaviour(const char *n, const char *d);
}; // class Behaviour

class Action : public GdmoObject {
    public:
	Action(const char *n);
}; // class Action

class Notification : public GdmoObject {
    public:
	Notification(const char *n);
}; // class Notification

class ConstraintRule : public GdmoObject {
    public:
	ConstraintRule(const char *n);
}; // class ConstraintRule

class SubordinationRule : public GdmoObject {
    public:
	SubordinationRule(const char *n);
}; // class SubordinationRule


#endif				/* __HEAD_HH__ */
