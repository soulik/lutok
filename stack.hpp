#ifndef LUTOK2_STACK_H
#define LUTOK2_STACK_H

namespace lutok2 {
	class State;
	class StackDebugger;

	class Stack {
	private:
		lua_State * state;
		/*
			C++ function wrapper
		*/

	public:
		Stack(){
			this->state = nullptr;
		}
		explicit Stack(lua_State * state){
			this->state = state;
		}

		/*
			Basic stack operations		
		*/

		inline int getTop(){
			return lua_gettop(state);
		}

		inline void setTop(int index){
			lua_settop(state, index);
		}

		inline int upvalueIndex(const int index){
			return lua_upvalueindex(index);
		}

		inline void pop(int n = 1){
			lua_pop(state, n);
		}

		inline void insert(int index){
			lua_replace(state, index);
		}

		inline void replace(int index){
			lua_replace(state, index);
		}

		inline void remove(int index){
			lua_remove(state, index);
		}

		inline void getGlobal(const std::string & name){
			lua_getglobal(state, name.c_str());
		}

		inline void setGlobal(const std::string & name){
			lua_setglobal(state, name.c_str());
		}

		/*
			Tables
		*/

		inline void newTable(){
			lua_newtable(state);
		}

		inline void newTable(const int acount, const int nacount){
			lua_createtable(state, acount, nacount);
		}

		inline void getTable(const int index = -3){
			lua_gettable(state, index);
		}

		inline void setTable(const int index = -3){
			lua_settable(state, index);
		}

		inline void concat(const int count){
			lua_concat(state, count);
		}

		inline void rawGet(const int index = -3){
			lua_rawget(state, index);
		}

		inline void rawGet(const int n, const int index = -3){
			lua_rawgeti(state, index, n);
		}

		inline void rawSet(const int index = -3){
			lua_rawset(state, index);
		}

		inline void rawSet(const int n, const int index = -3){
			lua_rawseti(state, index, n);
		}

		inline void getField(const std::string & key, const int index){
			lua_getfield (state, index, key.c_str());
		}

		inline void setField(const std::string & key, const int index = -2){
			lua_setfield (state, index, key.c_str());
		}

		/*
			Value passing
		*/

		template<typename T> inline void push(T value);
		template<typename T> inline T to(const int index = -1);
		template<typename T> inline void setField(const std::string & name, T value, const int index = -2);
		
		template<> inline void push(int value){
			lua_pushinteger(state, value);
		}
		
		template<> inline void push(LUA_NUMBER value){
			lua_pushnumber(state, value);
		}

		template<> inline void push(bool value){
			lua_pushboolean(state, value);
		}

		template<> inline void push(const char * value){
			lua_pushstring(state, value);
		}

		template<> inline void push(const std::string value){
			lua_pushstring(state, value.c_str());
		}

		template<> inline void push(lua_CFunction value){
			lua_pushcfunction(state, value);
		}

		template<> inline void push(Function value){
			Function ** wrappedFunction = static_cast<Function **>(newUserData(sizeof(Function*)));
			*wrappedFunction = new Function(value);
			pushClosure(cxx_function_wrapper, 1);
		}

		inline void push(Function value, int n){
			Function ** wrappedFunction = static_cast<Function **>(newUserData(sizeof(Function*)));
			*wrappedFunction = new Function(value);
			pushClosure(cxx_function_wrapper, n + 1);
		}
		template<> inline void push(cxx_function value){
			Function ** wrappedFunction = static_cast<Function **>(newUserData(sizeof(Function*)));
			*wrappedFunction = new Function(value);
			pushClosure(cxx_function_wrapper, 1);
		}

		inline void push(cxx_function value, int n){
			Function ** wrappedFunction = static_cast<Function **>(newUserData(sizeof(Function*)));
			*wrappedFunction = new Function(value);
			pushClosure(cxx_function_wrapper, n + 1);
		}

		inline void pushClosure(lua_CFunction fn, int n){
			lua_pushcclosure(state, fn, n);
		}

		template<> inline void push(void * value){
			lua_pushlightuserdata(state, value);
		}

		inline void pushLString(const std::string value, size_t len){
			lua_pushlstring(state, value.c_str(), len);
		}

		inline void pushVFString(const char * fmt, ...){
			char buffer[1024];
			va_list args;
			va_start (args, fmt);
			vsprintf (buffer, fmt, args);
			lua_pushstring(state, buffer);
			va_end (args);
		}

		inline void pushLiteral(const std::string value){
			lua_pushlstring(state, value.c_str(), value.size());
		}

		inline void pushNil(){
			lua_pushnil(state);
		}

		inline void pushValue(const int index){
			lua_pushvalue(state, index);
		}

		template<> inline bool to(const int index){
			return lua_toboolean(state, index);
		}

		template<> inline int to(const int index){
			return lua_tointeger(state, index);
		}

		template<> inline LUA_NUMBER to(const int index){
			return lua_tonumber(state, index);
		}

		template<> inline std::string to(const int index){
			const char * tmpString = lua_tostring(state, index);
			return std::string(tmpString);
		}

		template<> inline void * to(const int index){
			return lua_touserdata(state, index);
		}

		inline std::string toLString(const int index = -1){
			size_t len = 0;
			const char * tmpString = lua_tolstring(state, index, &len);
			return std::string(tmpString, len);
		}

		template<> inline void setField(const std::string & name, bool value, const int index){
			push<bool>(value);
			lua_setfield(state, index, name.c_str());
		}

		template<> inline void setField(const std::string & name, int value, const int index){
			push<int>(value);
			lua_setfield(state, index, name.c_str());
		}

		template<> inline void setField(const std::string & name, LUA_NUMBER value, const int index){
			push<LUA_NUMBER>(value);
			lua_setfield(state, index, name.c_str());
		}

		template<> inline void setField(const std::string & name, const char * value, const int index){
			push<const char *>(value);
			lua_setfield(state, index, name.c_str());
		}

		template<> inline void setField(const std::string & name, const std::string & value, const int index){
			push<const std::string>(value);
			lua_setfield(state, index, name.c_str());
		}

		template<> inline void setField(const std::string & name, Function value, const int index){
			push<Function>(value);
			lua_setfield(state, index, name.c_str());
		}

		template<> inline void setField(const std::string & name, cxx_function value, const int index){
			push<cxx_function>(value);
			lua_setfield(state, index, name.c_str());
		}

		template<> inline void setField(const std::string & name, void * value, const int index){
			push<void *>(value);
			lua_setfield(state, index, name.c_str());
		}

		inline void setFieldLString(const std::string & name, const std::string & value, size_t len, const int index=-1){
			pushLString(value, len);
			lua_setfield(state, index, name.c_str());
		}

		inline int ref(const int index = LUA_REGISTRYINDEX){
			return luaL_ref(state, index);
		}
		inline void  unref(const int ref, const int index = LUA_REGISTRYINDEX){
			luaL_unref(state, index, ref);
		}

		/*
			Values
		*/

		inline const int objLen(const int index = -1){
			return lua_objlen(state, index);
		}

		inline const int type(const int index = -1){
			return lua_type(state, index);
		}

		template<int TYPE> inline const bool is(const int index = -1){
			return lua_type(state, index) == TYPE;
		}

		inline const std::string typeName(const int index = -1){
			return std::string(lua_typename(state, index));
		}

		inline void * newUserData(size_t size){
			return lua_newuserdata(state, size);
		}

		inline void * checkUserData(const int narg, const std::string& name){
			return luaL_checkudata(state, narg, name.c_str());
		}

		/*
			Metatables
		*/

		inline void getMetatable(const int index = -1){
			lua_getmetatable(state, index);
		}

		inline void getMetatable(const std::string & name){
			luaL_getmetatable(state, name.c_str());
		}

		inline void setMetatable(const int index = -2){
			lua_setmetatable(state, index);
		}

		inline bool newMetatable(const std::string & name){
			return luaL_newmetatable(state, name.c_str()) == 1;
		}

		inline bool getMetaField(const std::string & name, const int index = -1){
			return luaL_getmetafield(state, index, name.c_str()) != 0;
		}

		/*
			Functions
		*/

		void call(const int nargs, const int nresults){
			lua_call(state, nargs, nresults);
		}

		void pcall(const int nargs, const int nresults, const int errFunction = 0){
			int result = lua_pcall(state, nargs, nresults, errFunction);
			if (result != 0){
				std::string errMessage = to<std::string>();
				if (result == LUA_ERRRUN){
					throw std::runtime_error("Runtime error: " + errMessage);
				}else if(result == LUA_ERRMEM){
					throw std::runtime_error("Allocation error: " + errMessage);
				}else if(result == LUA_ERRERR){
					throw std::runtime_error("Error handler error: " + errMessage);
				}else{
					throw std::runtime_error("Unknown error: " + errMessage);
				}
			}
		}
		/*
		StackDebugger debug(){
			return StackDebugger(this);
		}
		*/

	};
};

#endif
