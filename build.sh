#!/bin/bash
# Copyright (c) Pawel Defée 2013
# Usage: Run the script as is. If running on a system requiring sudo
# access for installation, run with USE_SUDO_INSTALL=sudo ./build.sh

# Set error handling
set -o errexit

# Create output directory
mkdir -p build

# Build Lua 5.1.5
tar xvf download/lua-5.1.5.tar.gz -C build
pushd build/lua-5.1.5
make linux
$USE_SUDO_INSTALL make linux install
popd

# Patch and build Lua File System
unzip download/luafilesystem-master.zip -d build
pushd build
patch -p0 < ../patches/luafilesystem-master.patch
pushd luafilesystem-master
make
$USE_SUDO_INSTALL make install
popd
popd

# Patch and build Fast Light Toolkit
tar xvf download/fltk-2.0.x-r5272.tar.gz -C build
pushd build
patch -p0 < ../patches/fltk-2.0.x-r5272.patch
pushd fltk-2.0.x-r5272
./configure --x-includes=/usr/include/X11 --x-libraries=/usr/libraries/X11 --enable-debug
make
$USE_SUDO_INSTALL make install
popd
popd

# Build Swiss Ephemeris
mkdir -p build/sweph/src
tar xvf download/swe_unix_src_1.79.00.tar.gz -C build/sweph
pushd build/sweph/src
make
mkdir -p ../../AstroZaur/sweph
cp * ../../AstroZaur/sweph
popd

# Patch and build AstroZaur
tar xvf download/AstroZaur.tar.gz -C build
pushd build
patch -p0 < ../patches/AstroZaur.patch
pushd AstroZaur
make
popd

