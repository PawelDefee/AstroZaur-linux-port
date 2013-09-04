AstroZaur-linux-port
====================

This is a Linux port of the natal chart inspection application AstroZaur based on the state-of-the-art astronomical Swiss Ephemeris library. For more information, including usage instructions, see: http://strijar.ru/astrozaur/.

Building
--------

To build, simply run:
		
		./build.sh 

The resulting application binary can be found at ./build/AstroZaur/astrozaur . If you are building on a system that requires sudo access for installation, build with:

		USE_SUDO_INSTALL=sudo ./build.sh
		
The ported software has been succesfully built and run on RedHat RHEL 6 and Ubuntu 12.10 Linux distributions.

Sourced contents of the Linux port
----------------------------------

Note: other libraries (such as libxml, ncurses etc) may be needed to compile this software. This repository only includes those packages that needed explicit patching or specific versions for a functional port for Linux.

* The original [AstroZaur] [1] application deployed for Sharp Zaurus PDA:
	
		./download/AstroZaur.tar.gz

* Fast Light Toolkit 2.0 [revision 5272] [2].

		./download/fltk-2.0.x-r5272.tar.gz

* The [Lua] [3] programming language runtime version 5.1.5.

		./download/lua-5.1.5.tar.gz

* The [Lua File System] [4] library.

		./download/luafilesystem-master.zip

* The state-of-the-art [SWISS EPHEMERIS] [5] library by AstroDienst.

		./download/swe_unix_src_1.79.00.tar.gz


  [1]: http://strijar.ru/astrozaur/        "AstroZaur"
  [2]: http://strijar.ru/astrozaur/compile/  "FLTK patched"
  [3]: http://www.lua.org    "LUA 5.1.5"
  [4]: http://keplerproject.github.io/luafilesystem/    "LFS"
  [5]: http://www.astro.com/ftp/swisseph/    "SwissEph"

Original patches in this Linux port
----------------------------------

Copyright (c) Pawel Defée 2013

* The AstroZaur patch at:

			./patches/AstroZaur.patch

	that corrects the following defects:
	* Compilation errors due to missing standard include files
	* Compilation errors due to invalid Make definitions
	* Crash due to memory corruption in Ubuntu Linux due to undeterministic usage of globally initialized objects.
	* Forced English language due to incorrect display of UTF-8 (Cyrillic) characters

* The FLTK patch at:

			./patches/fltk-2.0.x-r5272.patch

	that corrects the following defects:
	* Linux system functions used improperly (scandir)
	* Out-of-bounds access for memory buffers in pixel blitting functions
	* Invalid pointer access in font handling functions

* The LFS patch at:

			./patches/luafilesystem-master.patch

	that corrects the following defects:
	* Compilation errors when client Lua runtime is statically linked with the executable

* The Lua patch at:

			./patches/lua-5.1.5.patch

	that corrects the following defects:
	* Modified compilation to generate position independent code.
