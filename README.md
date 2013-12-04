Lutok
=====

Lutok is a lightweight C++ API library for Lua.

Lutok provides thin C++ wrappers around the Lua C API to ease the
interaction between C++ and Lua.  These wrappers make intensive use of
RAII to prevent resource leakage, expose C++-friendly data types, report
errors by means of exceptions and ensure that the Lua stack is always
left untouched in the face of errors.  The library also provides a small
subset of miscellaneous utility functions built on top of the wrappers.

Lutok focuses on providing a clean and safe C++ interface; the drawback
is that it is not suitable for performance-critical environments.  In
order to implement error-safe C++ wrappers on top of a Lua C binary
library, Lutok adds several layers or abstraction and error checking
that go against the original spirit of the Lua C API and thus degrade
performance.

This version is slightly extended so you can use more Lua functions in your C++ project.
There is also added C++ object wrapper which is based on Luna object wrapper

Dependencies
============
To build and use Lutok successfully you need:

* A standards-compliant C++ complier. (Visual Studio 2012 is preferred)
* Lua 5.1.x, LuaJIT 2.0.x
* update lua5.1.props so that you've got correct paths for Lua header and library files

Examples
========
Examples of use are included in files:
-	example/example_object.cpp
-	example/example_object.hpp
-	example/example_object.lua

Authors
=======
* Julio Merino <jmmv@google.com> - original developer
* Mário Kašuba <soulik42@gmail.com>

Links for further info
======================
For general project information, please visit:

-	https://github.com/soulik/lutok.git - current github for this version of Lutok
-	http://code.google.com/p/lutok/ - original version
-	http://lua-users.org/wiki/LunaWrapper - original version of Luna wrapper by nornagon

Copying
=======
Copyright 2012, 2013 Mário Kašuba
All rights reserved.

Copyright 2011, 2012 Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of Google Inc. nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
