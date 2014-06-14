# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( lutok )
set (LIB lutok)
set (lutok_src lutok/buffer.cpp lutok/c_gate.cpp lutok/debug.cpp lutok/exceptions.cpp lutok/lobject.cpp lutok/operations.cpp lutok/stack_cleaner.cpp lutok/state.cpp)
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )
include ( cmake/lua.cmake )
include_directories ( lutok/include ${LUA_INCLUDE_DIR} )
add_definitions( "-std=c++11" )

# Build
#install_lua_module ( lutok  LINK )
add_library (${LIB} STATIC ${${LIB}_src})
# install_data ( COPYRIGHT README )

