/*
    LUA Sqlite
    Copyright (C) 2007 Belousov Oleg <belousov.oleg@gmail.com>

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
    #include <sqlite3.h>
}


static int l_close(lua_State *L) {
    sqlite3*	db  = (sqlite3*) lua_touserdata(L, lua_upvalueindex(1));

    if (db) sqlite3_close(db);
    return 0;
}

static int l_exec(lua_State *L) {
    sqlite3*	db = (sqlite3*) lua_touserdata(L, lua_upvalueindex(1));
    const char*	sql = luaL_checkstring(L, 1);
    
    if (db) {
	int 		res;
	sqlite3_stmt*	stmt;
	
	res = sqlite3_prepare(db, sql, -1, &stmt, NULL);

	if (res == SQLITE_OK) {
	    int ncol, col, row = 1;
	    
	    ncol = sqlite3_column_count(stmt);
	    lua_newtable(L);

	    res = sqlite3_step(stmt);
    
	    while (res == SQLITE_ROW) {
		lua_pushnumber(L, row);
		lua_newtable(L);

		for (col = 0; col < ncol; col++) {
		    lua_pushstring(L, sqlite3_column_name(stmt, col));
		    lua_pushstring(L, (const char*) sqlite3_column_text(stmt, col));
		    lua_settable(L, -3);
		}
		lua_settable(L, -3);
		    
		row++;
		res = sqlite3_step(stmt);
	    }
	    sqlite3_finalize(stmt);
	    return 1;
	}
    }
    
    return 0;
}

static int l_error(lua_State *L) {
    sqlite3* db = (sqlite3*) lua_touserdata(L, lua_upvalueindex(1));
    
    if (sqlite3_errcode(db)) {
	lua_pushstring(L, sqlite3_errmsg(db));
    } else {
	lua_pushnil(L);
    }
    
    return 1;
}

static int l_open(lua_State *L) {
    const char*	name = luaL_checkstring(L, 1);
    sqlite3*	db;
    int		res;

    res = sqlite3_open(name, &db);
    
    if (res != SQLITE_OK) {
    	lua_pushnil(L);
    } else {
	lua_newtable(L);

	lua_pushstring(L, "exec");
	lua_pushlightuserdata(L, db);
	lua_pushcclosure(L, l_exec, 1);
	lua_rawset(L, -3);

	lua_pushstring(L, "error");
	lua_pushlightuserdata(L, db);
	lua_pushcclosure(L, l_error, 1);
	lua_rawset(L, -3);

	lua_pushstring(L, "close");
	lua_pushlightuserdata(L, db);
	lua_pushcclosure(L, l_close, 1);
	lua_rawset(L, -3);
    }

    return 1;
}

static const struct luaL_reg sqlitelib[] = {
    {"open", l_open},
    {NULL, NULL}
};

int luaopen_sqlite(lua_State * L) {
    luaL_openlib(L, "sqlite", sqlitelib, 0);

    return 1;
}
