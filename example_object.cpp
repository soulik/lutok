#include <Lua/lua.hpp>

#include <lutok/exceptions.hpp>
#include <lutok/operations.hpp>
#include <lutok/state.ipp>
#include <lutok/lobject.hpp>
#include <lutok/luna.hpp>

#include "example_object.hpp"

namespace Example {
	static int Lua_ExampleObject_New(lutok::state& state){
		try{
			ExampleObject * object = new ExampleObject();

			LuaExampleObject * wrapper = LOBJECT_INSTANCE(LuaExampleObject);
			wrapper->push(object);
			return 1;
		}catch(bad_alloc e){
			state.error("Cannot allocate memory");
		}
		return 0;
	}

	int LuaExampleObject::LOBJECT_METHOD(doSomething2, ExampleObject * object){
		state.push_string("Something has to be done here too!");
		return 1;
	}

	void init_exampleObject(lutok::state & state, moduleDef & module){
		//you've got to initialize wrapper singleton first
		LOBJECT_INSTANCE(LuaExampleObject);

		module["ExampleObject"] = Lua_ExampleObject_New;

		/*
			After this you call create a new object inside Lua script
		*/
	}

}