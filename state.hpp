#include "common.hpp"
#include "exceptions.hpp"
#include "lobjects.hpp"

namespace lutok2 {
	typedef std::function<int(State &)> cxx_function;
	typedef std::function<int(void *)> cxx_function_ex;
	typedef std::map<std::string, cxx_function> module;

	class StackDebugger {
	private:
		State * state;
		int stackItems;
	public:
		StackDebugger(State * state){
			this->state = state;
			stackItems = state->stack.getTop();
		}
		~StackDebugger(){
			if (stackItems != state->stack.getTop()){
				throw std::exception("Stack corruption detected!");
			}
		}
		void setReturnValues(int count){
			stackItems += count;
		}
	};

	class Stack {
	private:
		State * state;
		/*
			C++ function wrapper
		*/

		static int cxx_function_wrapper(lua_State * L){
			State state = State(L);
			cxx_function originalFunction = static_cast<cxx_function>(state.stack.to<void*>(state.stack.upvalueIndex(1)));
			try{
				return originalFunction(state);
			}catch(const std::exception & e){
				state.error("Unhandled exception: %s", e.what());
				return 1;
			}
		}

	public:
		Stack(){
			this->state = nullptr;
		}
		explicit Stack(State * state){
			this->state = state;
		}

		/*
			Basic stack operations		
		*/

		inline int getTop(){
			return lua_gettop(state->state);
		}

		inline void setTop(int index){
			lua_settop(state->state, index);
		}

		inline int upvalueIndex(const int index){
			return lua_upvalueindex(index);
		}

		inline void pop(int n = 1){
			lua_pop(state->state, n);
		}

		inline void insert(int index){
			lua_replace(state->state, index);
		}

		inline void replace(int index){
			lua_replace(state->state, index);
		}

		inline void remove(int index){
			lua_remove(state->state, index);
		}

		inline void getGlobal(const std::string & name){
			lua_getglobal(state->state, name.c_str());
		}

		inline void setGlobal(const std::string & name){
			lua_setglobal(state->state, name.c_str());
		}

		/*
			Tables
		*/

		inline void newTable(){
			lua_newtable(state->state);
		}

		inline void newTable(const int acount, const int nacount){
			lua_createtable(state->state, acount, nacount);
		}

		inline void getTable(const int index = -3){
			lua_gettable(state->state, index);
		}

		inline void setTable(const int index = -3){
			lua_settable(state->state, index);
		}

		inline void concat(const int count){
			lua_concat(state->state, count);
		}

		inline void rawGet(const int index = -3){
			lua_rawget(state->state, index);
		}

		inline void rawGet(const int n, const int index = -3){
			lua_rawgeti(state->state, index, n);
		}

		inline void rawSet(const int index = -3){
			lua_rawset(state->state, index);
		}

		inline void rawSet(const int n, const int index = -3){
			lua_rawseti(state->state, index, n);
		}

		inline void getField(const std::string & key, const int index){
			lua_getfield (state->state, index, key.c_str());
		}

		inline void setField(const std::string & key, const int index){
			lua_setfield (state->state, index, key.c_str());
		}

		/*
			Value passing
		*/

		template<typename T> inline void push(T value);
		template<typename T> inline T to(const int index = -1);
		template<typename T> inline void setField(const std::string & name, T value, const int index = -1);
		
		template<> inline void push(int value){
			lua_pushinteger(state->state, value);
		}
		
		template<> inline void push(LUA_NUMBER value){
			lua_pushnumber(state->state, value);
		}

		template<> inline void push(bool value){
			lua_pushboolean(state->state, value);
		}

		template<> inline void push(const std::string value){
			lua_pushstring(state->state, value.c_str());
		}

		template<> inline void push(void * value){
			lua_pushlightuserdata(state->state, value);
		}

		template<> inline void push(lua_CFunction value){
			lua_pushcfunction(state->state, value);
		}

		template<> inline void push(std::function<cxx_function> value){
			std::function<cxx_function> wrappedFunction = static_cast<std::function<cxx_function>>(newUserData(sizeof(cxx_function)));
			wrappedFunction = value;
			pushClosure(cxx_function_wrapper, 1);
		}

		inline void push(std::function<cxx_function> value, int n){
			std::function<cxx_function> wrappedFunction = static_cast<std::function<cxx_function>>(newUserData(sizeof(cxx_function)));
			wrappedFunction = value;
			pushClosure(cxx_function_wrapper, n + 1);
		}

		inline void pushClosure(lua_CFunction fn, int n){
			lua_pushcclosure(state->state, fn, n);
		}

		inline void pushLString(const std::string value, size_t len){
			lua_pushlstring(state->state, value.c_str(), len);
		}

		inline void pushVFString(const char * fmt, ...){
			char buffer[1024];
			va_list args;
			va_start (args, fmt);
			vsprintf (buffer, fmt, args);
			lua_pushstring(state->state, buffer);
			va_end (args);
		}

		inline void pushLiteral(const std::string value){
			lua_pushlstring(state->state, value.c_str(), value.size());
		}

		inline void pushNil(){
			lua_pushnil(state->state);
		}

		inline void pushValue(const int index){
			lua_pushvalue(state->state, index);
		}

		template<> inline bool to(const int index){
			return lua_toboolean(state->state, index);
		}

		template<> inline int to(const int index){
			return lua_tointeger(state->state, index);
		}

		template<> inline LUA_NUMBER to(const int index){
			return lua_tonumber(state->state, index);
		}

		template<> inline std::string to(const int index){
			const char * tmpString = lua_tostring(state->state, index);
			return std::string(tmpString);
		}

		template<> inline void * to(const int index){
			return lua_touserdata(state->state, index);
		}

		inline std::string toLString(const int index = -1){
			size_t len = 0;
			const char * tmpString = lua_tolstring(state->state, index, &len);
			return std::string(tmpString, len);
		}

		template<> inline void setField(const std::string & name, bool value, const int index){
			pushLiteral(name);
			push<bool>(value);
			setTable(index);
		}

		template<> inline void setField(const std::string & name, int value, const int index){
			pushLiteral(name);
			push<int>(value);
			setTable(index);
		}

		template<> inline void setField(const std::string & name, LUA_NUMBER value, const int index){
			pushLiteral(name);
			push<LUA_NUMBER>(value);
			setTable(index);
		}

		template<> inline void setField(const std::string & name, const std::string & value, const int index){
			pushLiteral(name);
			push<const std::string>(value);
			setTable(index);
		}

		template<> inline void setField(const std::string & name, void * value, const int index){
			pushLiteral(name);
			push<void *>(value);
			setTable(index);
		}

		inline void setFieldLString(const std::string & name, const std::string & value, size_t len, const int index=-1){
			pushLiteral(name);
			pushLString(value, len);
			setTable(index);
		}

		inline int ref(const int index = LUA_REGISTRYINDEX){
			return luaL_ref(state->state, index);
		}
		inline void  unref(const int ref, const int index = LUA_REGISTRYINDEX){
			luaL_unref(state->state, index, ref);
		}

		/*
			Values
		*/

		inline const int objLen(const int index = -1){
			return lua_objlen(state->state, index);
		}

		inline const int type(const int index = -1){
			return lua_type(state->state, index);
		}

		template<int TYPE> inline const bool is(const int index = -1){
			return lua_type(state->state, index) == TYPE;
		}

		inline const std::string typeName(const int index = -1){
			return std::string(lua_typename(state->state, index));
		}

		inline void * newUserData(size_t size){
			return lua_newuserdata(state->state, size);
		}

		inline void * checkUserData(const int narg, const std::string& name){
			return luaL_checkudata(state->state, narg, name.c_str());
		}

		/*
			Metatables
		*/

		inline void getMetatable(const int index = -1){
			lua_getmetatable(state->state, index);
		}

		inline void getMetatable(const std::string & name){
			luaL_getmetatable(state->state, name.c_str());
		}

		inline void setMetatable(const int index = -1){
			lua_setmetatable(state->state, index);
		}

		inline bool newMetatable(const std::string & name){
			return luaL_newmetatable(state->state, name.c_str()) == 1;
		}

		inline bool getMetaField(const std::string & name, const int index = -1){
			return luaL_getmetafield(state->state, index, name.c_str()) != 0;
		}

		/*
			Functions
		*/

		void call(const int nargs, const int nresults){
			lua_call(state->state, nargs, nresults);
		}

		void pcall(const int nargs, const int nresults, const int errFunction = 0){
			int result = lua_pcall(state->state, nargs, nresults, errFunction);
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
		
		StackDebugger debug(){
			return StackDebugger(state);
		}
	};

	class State {
	friend class Stack;
	private:
		lua_State * state;
		bool owned;

		inline const char * findTable(const int index, const std::string & name, int szHint){
			return luaL_findtable(state, index, name.c_str(), szHint);
		}
		
		void findLib(const std::string & name, const int size, const int nup){
			findTable(LUA_REGISTRYINDEX, "_LOADED", 1);
			stack.getField(name, -1);  /* get _LOADED[name] */
			if (!stack.is<LUA_TTABLE>()) {  /* not found? */
				stack.pop();  /* remove previous result */
				/* try global variable (and create one if it does not exist) */
				if (findTable(LUA_GLOBALSINDEX, name, size ) != NULL)
					error("Name conflict for module " LUA_QS, name);
				stack.pushValue(-1);
				stack.setField(name, -3);  /* _LOADED[name] = new table */
			}
			stack.remove(-2);  /* remove _LOADED table */
			stack.insert(-(nup+1));  /* move library table to below upvalues */
		}
	public:
		Stack stack;

		State(){
			newState();
			initState();
		}

		explicit State(lua_State * state){
			this->state = state;
			owned = false;
			initState();
		}

		~State(){
			if (owned){
				closeState();
			}
		}

		State & operator= (State & arg){
			state = arg.state;
			owned = arg.owned;
		}

		bool operator== (State & arg){
			return (arg.state == state);
		}

		void initState(){
			stack = Stack(this);
			if (stack.newMetatable("_lutok2")){
				stack.setField("State", static_cast<void*>(this));
				stack.pop();
			}
		}

		void newState(){
			state = luaL_newstate();
			owned = true;
		}

		void closeState(){
			if (state != nullptr){
				lua_close(state);
				state = nullptr;
			}
		}

		void openLibs(){
			luaL_openlibs(state);
		}

		/*
			Loaders
		*/

		void loadFile(const std::string & fileName){
			if (luaL_loadfile(state, fileName.c_str()) != 0){
				throw std::runtime_error("Can't open file: "+ fileName);
			}
		}

		void loadString(const std::string & chunk){
			if (luaL_loadstring(state, chunk.c_str()) != 0){
				throw std::runtime_error("Can't load a chunk");
			}
		}

		/*
			Libraries
		*/

		void registerLib(const module & members){
			assert(stack.is<LUA_TTABLE>());

			for (std::map< std::string, cxx_function >::const_iterator
				iter = members.begin(); iter != members.end(); iter++) {
					stack.push<std::string>((*iter).first);
					stack.push<cxx_function>((*iter).second);
					stack.setTable(-3);
			}
		}

		void registerLib(const module & members, const std::string & name, const int nup=0){
			findLib(name, members.size, nup);
			registerLib(members);
			stack.pop(nup);
		}

		/*
			Errors
		*/
		void error(const char * fmt, ...){
			char buffer[1024];
			va_list args;
			va_start (args, fmt);
			vsprintf (buffer, fmt, args);
			luaL_error(state, "%s", buffer);
			va_end (args);
		}


	};
};