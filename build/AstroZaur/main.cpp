/*
    AstroZaur - Universal astrologic processor
    Copyright (C) 2006 Belousov Oleg <belousov.oleg@gmail.com>
    http://www.strijar.ru/astrozaur

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "astro.h"
#include "program.h"
#include "division.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

#ifdef DBUS
    #include "dbus.h"
#endif

#include <fltk/FileIcon.h>

using namespace fltk;

CStaticHolder static_holder;

int main(int argc, char **argv) {
//    FileIcon::load_system_icons();

    CLua		local_lua;
    static_holder.Set(&local_lua);
    CDivision	local_division;
    static_holder.Set(&local_division);
    CProgram	local_program;
    static_holder.Set(&local_program);
	
    if (lua.Load("data/astrozaur.lua")) {
        lua.Call(0);
    }

#ifdef DBUS    
    dbus.RequestName("ru.strijar.astrozaur");
    dbus.AddMatch("type='signal',interface='ru.strijar'");
    dbus.Start();
#endif

    return 0;
}
