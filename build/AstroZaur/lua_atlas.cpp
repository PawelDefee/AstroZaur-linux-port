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

#include "atlas.h"
#include "program.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}


static int l_path(lua_State *L) {
    const char*	path = luaL_checkstring(L, 1);

    program.AtlasFile(path);

    return 0;
}

static int l_close(lua_State *L) {
    CAtlas* atlas = (CAtlas*) lua_touserdata(L, lua_upvalueindex(1));
    
    delete atlas;
    
    return 0;
}

static int l_open(lua_State *L) {
    CAtlas* atlas = new CAtlas;

    atlas->Open(program.AtlasFile());
    atlas->Show();
    atlas->PushTable();

    lua_pushstring(L, "close");
    lua_pushlightuserdata(L, atlas);
    lua_pushcclosure(L, l_close, 1);
    lua_rawset(L, -3);

    return 1;
}

static const struct luaL_reg atlaslib[] = {
    {"path", l_path},
    {"open", l_open},
    {NULL, NULL}
};

int luaopen_atlas(lua_State * L) {
    luaL_openlib(L, "atlas", atlaslib, 0);

    return 1;
}
