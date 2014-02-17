#include "buffer.hpp"

namespace lutok {
	Buffer::Buffer(lutok::state & state){
		luaL_buffinit(static_cast<lua_State*>(state.raw_state()), &buffer);
	}

	void Buffer::putchar(char ch){
		luaL_putchar(&buffer, ch);
	}
	void Buffer::addlstring (const char * str, size_t len){
		luaL_addlstring(&buffer, str, len);
	}
	void Buffer::addstring(std::string & str){
		luaL_addstring(&buffer, str.c_str());
	}
	void Buffer::push(){
		luaL_pushresult(&buffer);
	}
};