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

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
    #include <sweph/swephexp.h>
}

static int l_path(lua_State *L) {
    const char*	path = luaL_checkstring(L, 1);

    swe_set_ephe_path((char*) path);

    return 0;
}

static const struct luaL_reg swelib[] = {
    {"path", l_path},
    {NULL, NULL}
};

int luaopen_swe(lua_State * L) {
    luaL_openlib(L, "swe", swelib, 0);

    return 1;
}
