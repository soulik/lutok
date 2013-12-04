#ifdef _WIN32
#include <io.h>
#else
extern "C" {
#include <unistd.h>
}
#endif

#include <cassert>
#include <cstring>
#include <typeinfo>

#include "c_gate.hpp"
#include "exceptions.hpp"
#include "state.ipp"
#include "lobject.hpp"

namespace lutok {
	/*
	template <class C> std::unique_ptr<LSingleton<C>> LSingleton<C>::m_instance;
	template <class C> std::once_flag LSingleton<C>::m_onceFlag;
	*/
	/*
	std::unique_ptr<LSingleton> LSingleton::m_instance;
	std::once_flag LSingleton::m_onceFlag;
	*/
}