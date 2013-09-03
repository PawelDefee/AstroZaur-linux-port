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

Contents of the Linux port
--------------------------

