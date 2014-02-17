#if !defined(LUTOK_BUFFER_HPP)
#define LUTOK_BUFFER_HPP

#include "state.hpp"
#include <lua/lua.hpp>
#include <string>

namespace lutok {
	class Buffer {
	private:
		luaL_Buffer buffer;
	public:
		Buffer(lutok::state & state);
		void putchar(char ch);
		void addlstring(const char * str, size_t len);
		void addstring(std::string & str);
		void push();
	};
};

#endif