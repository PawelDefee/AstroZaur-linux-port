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

Note: other libraries (such as libxml, ncurses etc) may be needed to compile this software. This repository only includes those packages that needed explicit patching or specific versions for a functional port to Linux.

* The original [AstroZaur] [6] application deployed for Sharp Zaurus PDA.

* ./download/fltk-2.0.x-r5272.tar.gz

Fast Light Toolkit 2.0 [revision 5272] [2].

./download/lua-5.1.5.tar.gz

The [Lua] [3] programming language runtime version 5.1.5.

./download/luafilesystem-master.zip

The [Lua File System] [4] library.

./download/swe_unix_src_1.79.00.tar.gz

The state-of-the-art [SWISS EPHEMERIS] [5] library by AstroDienst.

  [1]: http://strijar.ru/astrozaur/        "AstroZaur"
  [2]: http://strijar.ru/astrozaur/compile/  "FLTK patched"
  [3]: http://www.lua.org    "LUA 5.1.5"
  [4]: http://keplerproject.github.io/luafilesystem/    "LFS"
  [5]: http://www.astro.com/ftp/swisseph/	"SwissEph"
  [6]: https://github.com/PawelDefee/AstroZaur-linux-port/blob/569e1181575e3de9b27bea2529e6f3bac617753f/download/AstroZaur.tar.gz	"AstroZaur"

Original patches in the Linux port
----------------------------------


