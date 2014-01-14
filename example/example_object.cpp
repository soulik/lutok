#include <lutok/lutok.hpp>

#include "example_object.hpp"

namespace Example {
	static int Lua_ExampleObject_New(lutok::state& state){
		try{
			ExampleObject * object = new ExampleObject();

			LuaExampleObject & wrapper = LOBJECT_INSTANCE(LuaExampleObject);
			wrapper.push(object);
			return 1;
		}catch(std::bad_alloc e){
			state.error("Cannot allocate memory");
		}
		return 0;
	}

	int LuaExampleObject::LOBJECT_METHOD(doSomething2, ExampleObject * object){
		void * userData = (void*)123456789;
		state.push_boolean(true);
		state.push_integer(12345);
		state.push_number(12345.6789);
		state.push_string("Something has to be done here too!");
		state.push_lightuserdata(userData);
		state.new_table();
			state.set_field("number", (lua_Number)12345.6789);
			state.set_field("integer", (int)12345);
			state.set_field("string", "A text");
			state.set_field("boolean", false);
			state.push_lightuserdata(userData);
			state.set_field(-2, "userData");
		state.to_integer();
		return 6;
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

extern "C" LUA_API int luaopen_example(void * current_state){

	lutok::state state(current_state);
	Example::moduleDef module;

	Example::init_exampleObject(state, module);
	
	//optionally we can force "example" namespace in Lua
#ifdef EXAMPLE_SET_NAMESPACE
	lutok::registerLib(state, "example", module);
#else
	state.new_table();
	lutok::registerLib(state, module);
#endif
	return 1;
}
