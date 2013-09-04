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

#include "map_multy.h"
#include "utils.h"
#include "program.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

void CMapMulty::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    // We in root?
    while (subNode) {
	if (!strcmp((char*)subNode->name,"chart")) break;
	subNode = subNode->next;
    }
    
    if (subNode) {	// Found in root
	subNode = subNode->children;
    } else {		// Not found
	subNode = node;
    }

    while (subNode) {
	if (!strcmp((char*)subNode->name,"parts")) break;
	subNode = subNode->next;
    }

    if (subNode == NULL) return;
    subNode = subNode->children;

    while (subNode) {
	char* name = (char *) subNode->name;
    
	if (!strcmp(name,"chart") && has_content(subNode)) {
	    CMapNatal* map = new CMapNatal;
	    
	    map->Owner(this);
	    map->XML(subNode->children);
	    iMaps.push_back(map);
	    
	    CMapNatal* natal;
	    
	    natal = (CMapNatal*) map->Date()->Owner();
	    if (natal != map) natal->AddDepends(this);

	    natal = (CMapNatal*) map->Place()->Owner();
	    if (natal != map) natal->AddDepends(this);
	}
	subNode = subNode->next;
    }

    CMapNatal::XML(node);
}

bool CMapMulty::LuaSet(const char* var) {
    if (strcmp(var, "parts") == 0 && lua.Type(-1) == LUA_TTABLE) {
	lua.PushNil();
	
	while (lua.Next(-2)) {
	    if (lua.Type(-1) == LUA_TTABLE) {
		lua.PushString(".ptr_chart");
		lua.GetTable(-2);

		if (lua.Type(-1) == LUA_TLIGHTUSERDATA) {
		    CMapNatal *map = (CMapNatal*) lua.ToUser(-1);

		    map->AddDepends(this);
		    iMaps.push_back(map);
		}
		lua.Pop(1);
	    }
	    lua.Pop(1);
	}
    
	return true;
    }

    return CMapNatal::LuaSet(var);
}

bool CMapMulty::LuaGet(const char* var) {
    return CMapNatal::LuaGet(var);
}
