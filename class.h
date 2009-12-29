/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "asobjects.h"
#include "swf.h"

#ifndef CLASS_H
#define CLASS_H

namespace lightspark
{

template<typename T>
class ClassName
{
public:
	static const char* name;
};

#define REGISTER_CLASS_NAME(X) template<> \
	const char* ClassName<X>::name = #X;

#define REGISTER_CLASS_NAME2(X,NAME) template<> \
	const char* ClassName<X>::name = NAME;

class Class_inherit:public Class_base
{
public:
	Class_inherit(const tiny_string& name):Class_base(name){}
	IInterface* getInstance(bool construct=false);
};

template< class T>
class Class: public Class_base
{
private:
	Class(const tiny_string& name):Class_base(name){}
	T* getInstance(bool construct)
	{
		ASObject* obj=new ASObject;
		obj->max_level=max_level;
		obj->prototype=this;
		obj->actualPrototype=this;
		//TODO: Add interface T to ret
		T* ret=new T;
		ret->obj=obj;
		obj->implementation=ret;
		//As we are the prototype we should incRef ourself
		incRef();
		if(construct && constructor)
			constructor->call(obj,NULL,obj->max_level);
		return ret;
	}
public:
	static T* getInstanceS(bool construct=false)
	{
		Class<T>* c=Class<T>::getClass();
		return c->getInstance(construct);
	}
	template <typename ARG1>
	static T* getInstanceS(ARG1 a1)
	{
		Class<T>* c=Class<T>::getClass();
		ASObject* obj=new ASObject;
		//TODO: Add interface T to ret
		obj->max_level=c->max_level;
		obj->prototype=c;
		obj->actualPrototype=c;
		T* ret=new T(a1);
		ret->obj=obj;
		obj->implementation=ret;
		//As we are the prototype we should incRef ourself
		c->incRef();
		return ret;
	}
	static Class<T>* getClass(const tiny_string& name)
	{
		std::map<tiny_string, Class_base*>::iterator it=sys->classes.find(name);
		if(it==sys->classes.end()) //This class is not yet in the map, create it
		{
			Class<T>* ret=new Class<T>(name);
			T::sinit(ret);
			sys->classes.insert(std::make_pair(name,ret));
			return ret;
		}
		else
			return static_cast<Class<T>*>(it->second);
	}
	static Class<T>* getClass()
	{
		return getClass(ClassName<T>::name);
	}
};

};
#endif
