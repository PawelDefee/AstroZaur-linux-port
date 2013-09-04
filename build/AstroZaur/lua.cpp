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

#include <fltk/run.h>
#include <fltk/Threads.h>

#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "lua_sqlite.h"
#include "lua_gui.h"
#include "lua_swe.h"
#include "lua_atlas.h"
#include "program.h"
#include "division.h"
#include "utils.h"

#ifdef WIN32
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

extern "C" {
    #include <lauxlib.h>
}

static int l_angle(lua_State *L) {
    double a = luaL_checknumber(L, 1);
    double b = luaL_checknumber(L, 2);
    
    lua_pushnumber(L, Dist(a, b));
    return 1;
}

static int l_between(lua_State *L) {
    double 	p = luaL_checknumber(L, 1);
    double	a, b;
    int		i;
    
    for (i = 2; i < lua_gettop(L); i++) {
	a = luaL_checknumber(L, i);
	b = luaL_checknumber(L, i + 1);
	
	if (Dist(a, p) >= 0 && Dist(p, b) > 0) {
	    lua_pushnumber(L, i - 1);
	    return 1;
	}
    }
    
    lua_pushnil(L);
    return 1;
}

static int l_division(lua_State *L) {
    const char	*info = luaL_checkstring(L, 1);

    if (lua_type(L, 2) == LUA_TTABLE) {
	const char	*data;
	double		from = 0, to = 0, size = 0;

	for (int i = 1; ; i++) {
	    lua_rawgeti(L, 2, i);
	    
	    switch (lua_type(L, -1)) {
		case  LUA_TTABLE:
		    // data
		
		    lua_pushnumber(L, 1);
		    lua_gettable(L, -2);

		    if (lua_type(L, -1) == LUA_TSTRING) {
			data = lua_tostring(L, -1);
		    } else {
			lua_remove(L, -1);
			break;
		    }
		    lua_remove(L, -1);
		    
		    // from
		    
		    lua_pushstring(L, "from");
		    lua_gettable(L, -2);
		    
		    if (lua_type(L, -1) == LUA_TNUMBER) {
			from = lua_tonumber(L, -1);
		    } else {
			from = to;	// Prev
		    }
		    lua_remove(L, -1);
		    
		    // size
		    
		    lua_pushstring(L, "size");
		    lua_gettable(L, -2);
		    
		    if (lua_type(L, -1) == LUA_TNUMBER) {
			size = lua_tonumber(L, -1);
			to = from + size;
			lua_remove(L, -1);
		    } else {
			lua_remove(L, -1);

			// to
		    
			lua_pushstring(L, "to");
			lua_gettable(L, -2);
		    
			if (lua_type(L, -1) == LUA_TNUMBER) {
			    to = lua_tonumber(L, -1);
			    size = to - from;
			} else {
			    to = from + size;
			}
		    
			lua_remove(L, -1);
		    }
		    division.New(info, from, to, data);
		    break;
		    
		case LUA_TNIL:
		    lua_remove(L, -1);
		    division.Sort();
		    
		    return 0;
	    }
	    lua_remove(L, -1);
	}
    } else {
	double		lon = luaL_checknumber(L, 2);
	char		*div;

	if (division.Get(lon, info, &div)) {
	    lua_pushstring(L, div);
	    return 1;
	}
    }
    
    return 0;
}

static const luaL_reg astrolib[] = {
    {"angle", l_angle},
    {"between", l_between},
    {"division", l_division},
    {NULL, NULL}
};

//

static int obj_set(lua_State *L) {
    const char	*var = luaL_checkstring(L, 2);
    CAstro	*obj = (CAstro*) lua_touserdata(L, lua_upvalueindex(1));

    if (!obj->LuaSet(var)) {
	lua_rawset(L, -3);
    }
    
    return 0;
}

static int obj_get(lua_State *L) {
    const char	*var = luaL_checkstring(L, 2);
    CAstro	*obj = (CAstro*) lua_touserdata(L, lua_upvalueindex(1));

    if (!obj->LuaGet(var)) {
	lua_rawget(L, -2);
    }
    
    return 1;
}

static int obj_call(lua_State *L) {
    CAstro	*obj = (CAstro*) lua_touserdata(L, lua_upvalueindex(1));

    return obj->LuaCall();
}

CLua::CLua() {
    Start();
}

CLua::~CLua() {
//    Stop();
}

void CLua::Start() {
    iL = lua_open();
    
    luaL_openlibs(iL);
    luaL_openlib(iL, "astro", astrolib, 0);
    
    luaopen_gui(iL);
    luaopen_sqlite(iL);
    luaopen_swe(iL);
    luaopen_atlas(iL);
}

void CLua::Stop() {
    lua_close(iL);
    iL = NULL;
}

bool CLua::Load(char* name) {
    if (luaL_loadfile(iL, name)) {
	printf("Lua: cannot load file: %s\n", lua_tostring(iL, -1));
	return false;
    }
    
    return true;
}

int CLua::Call(int arg) {
    int stat = lua_pcall(iL, arg, LUA_MULTRET, 0);

    if (stat) {
	printf("Lua: cannot call: %s\n", lua_tostring(iL, -1));
	return -1;
    }
    
    return stat;
}

void CLua::PrintType(int index) {
    printf("Type: %s\n",lua_typename(iL, lua_type(iL, index)));
}

void CLua::PrintStack(const char* info) {
    int idx;
    
    if (info) printf(info);
    
    for (idx = lua_gettop(iL); idx >= 1; idx--) {
	printf("%2i %s (", idx, lua_typename(iL, lua_type(iL, idx)));
	
	switch (lua_type(iL, idx)) {
	    case LUA_TSTRING:
		printf("%s", lua_tostring(iL, idx));
		break;

	    case LUA_TNUMBER:
		printf("%f", lua_tonumber(iL, idx));
		break;
		
	    default:
		printf("%p", lua_topointer(iL, idx));
		break;
	}
	printf(")\n");
    }
    printf("\n");
}

void CLua::InitMeta(CAstro* obj) {
    lua_newtable(iL);
    lua_pushstring(iL, "__newindex");
    lua_pushlightuserdata(iL, obj);
    lua_pushcclosure(iL, obj_set, 1);
    lua_rawset(iL, -3);
    lua_setmetatable(iL, -2);

    lua_getmetatable(iL, -1);
    lua_pushstring(iL, "__index");
    lua_pushlightuserdata(iL, obj);
    lua_pushcclosure(iL, obj_get, 1);
    lua_rawset(iL, -3);
    lua_setmetatable(iL, -2);

    lua_getmetatable(iL, -1);
    lua_pushstring(iL, "__call");
    lua_pushlightuserdata(iL, obj);
    lua_pushcclosure(iL, obj_call, 1);
    lua_rawset(iL, -3);
    lua_setmetatable(iL, -2);
}

void CLua::Method(const char* name, lua_CFunction func, CAstro *obj) {
    lua_pushstring(iL, name);
    lua_pushlightuserdata(iL, obj);
    lua_pushcclosure(iL, func, 1);
    lua_rawset(iL, -3);
}

CAstro* CLua::GetObj() {
    return (CAstro*) lua_touserdata(iL, lua_upvalueindex(1));
}

void CLua::ClearTable() {
    for (int i = 1; i < luaL_getn(iL, -1); i++) {
	 lua_pushnil(iL);
	 lua_rawseti(iL, -2, i);
    }
}

void CLua::Exec(char* cmd) {
    fltk::lock();
    luaL_dostring(iL, cmd);
    fltk::unlock();
    fltk::awake(0);
}

void CLua::GetGlobal(char* name) {
    lua_getglobal(iL, name);
}

void CLua::SetGlobal(char* name) {
    lua_setglobal(iL, name);
}

void CLua::NewTable() {
    lua_newtable(iL);
}

void CLua::NewTable(char* name) {
    lua_pushstring(iL, name);
    lua_newtable(iL);
}

void CLua::NewTable(int index) {
    lua_pushnumber(iL, index);
    lua_newtable(iL);
}

void CLua::EndTable() {
    lua_rawset(iL, -3);
}

void CLua::SetTable(int index) {
    lua_settable(iL, index);
}

void CLua::GetTable(int index) {
    lua_gettable(iL, index);
}

void CLua::PushString(const char* str) {
    lua_pushstring(iL, str);
}

void CLua::PushValue(int data) {
    lua_pushvalue(iL, data);
}

void CLua::PushNumber(double data) {
    lua_pushnumber(iL, data);
}

void CLua::PushNil() {
    lua_pushnil(iL);
}

void CLua::PushFunc(void *data) {
    lua_pushcfunction(iL, (lua_CFunction)data);
}

void CLua::PushBool(bool data) {
    lua_pushboolean(iL, data);
}

bool CLua::Next(int index) {
    return lua_next(iL, index);
}

void CLua::NewMetaTable(char* name) {
    luaL_newmetatable(iL, name);
}

void CLua::SetField(const char* name, char* str) {
    lua_pushstring(iL, name);
    lua_pushstring(iL, str);
    lua_rawset(iL, -3);
}

void CLua::SetField(int index, char* str) {
    lua_pushnumber(iL, index);
    lua_pushstring(iL, str);
    lua_rawset(iL, -3);
}

void CLua::SetField(const char* name, double data) {
    lua_pushstring(iL, name);
    lua_pushnumber(iL, data);
    lua_rawset(iL, -3);
}

void CLua::SetField(const char* name, bool data) {
    lua_pushstring(iL, name);
    lua_pushboolean(iL, data);
    lua_rawset(iL, -3);
}

void CLua::SetField(const char* name, CAstro* data) {
    lua_pushstring(iL, name);
    lua_pushlightuserdata(iL, data);
    lua_rawset(iL, -3);
}

bool CLua::GetNumber(double *res) {
    if (lua_isnumber(iL, -1)) {
	*res = lua_tonumber(iL, -1);
	lua_pop(iL, 1);
	return true;
    } else {
	return false;
    }
    
}

int CLua::GetTop() {
    return lua_gettop(iL);
}

const char* CLua::ToString(int index) {
    return lua_tostring(iL, index);
}

const double CLua::ToNumber(int index) {
    return lua_tonumber(iL, index);
}

const bool CLua::ToBool(int index) {
    return lua_toboolean(iL, index);
}

const void* CLua::ToUser(int index) {
    return lua_touserdata(iL, index);
}

void CLua::Pop(int index) {	
    lua_pop(iL, index);
}

void CLua::Remove(int index) {
    lua_remove(iL, index);
}

void CLua::RawSet(int index) {
    lua_rawset(iL, index);
}

void CLua::RawGetI(int index, int n) {
    lua_rawgeti(iL, index, n);
}

int CLua::Type(int index) {
    return lua_type(iL, index);
}

bool CLua::LoadString(char *str) {
    return  luaL_loadstring(iL, str) == 0;
}

int CLua::Ref() {
    return luaL_ref(iL, LUA_REGISTRYINDEX);
}

void CLua::Unref(int ref) {
    luaL_unref(iL, LUA_REGISTRYINDEX, ref);
}

lua_State* CLua::State() {
    return iL;
}

//

CLuaChunk::CLuaChunk() {
    iRef = -1;
    iChunk = NULL;
}

CLuaChunk::~CLuaChunk() {
    if (iRef > 0) lua.Unref(iRef);
    
    if (iChunk) free(iChunk);
}
    
bool CLuaChunk::Set(char *text) {
    if (lua.LoadString(text)) {
	if (lua.Type(-1) == LUA_TFUNCTION) {
	    if (iChunk) free(iChunk);
	    if (iRef > 0) lua.Unref(iRef);
	
	    iChunk = strdup(text);
	    iRef = lua.Ref();
	    
	    return true;
	} else {
	    printf("Error: chunk not function\n");
	}
    } else {
	printf("Error: load chunk\n");
    }
    
    return false;
}

void CLuaChunk::Push() {
    lua.RawGetI(LUA_REGISTRYINDEX, iRef);
}

