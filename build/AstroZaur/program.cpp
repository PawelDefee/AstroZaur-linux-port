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

#include "maps.h"
#include "program.h"
#include "window.h"
#include "log.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "division.h"
#include "map_natal.h"
#include "map_progres.h"
#include "map_direct.h"
#include "map_date_select.h"
#include "map_eqlipse.h"
#include "map_middle.h"
#include "map_harmonic.h"
#include "map_ingres.h"
#include "map_raw.h"
#include "map_revolution.h"
#include "map_stationar.h"
#include "map_aspect.h"
#include <fltk/Menu.h>
#include <fltk/SharedImage.h>
#include "static_holder.h"

extern "C" {
    #include <sweph/swephexp.h>
}

static int l_newchart(lua_State *L) {
    const char	*type = lua.ToString(1);
    const char	*name = lua.ToString(2);

    if (type && name) {
	CMap* map = program.AddMap(type);
    
	if (map) {
	    map->Info(name);
	    program.AddMap(map);
	    map->PushTable();
	    return 1;
	}
    }

    return 0;
}

static int l_newwindow(lua_State *L) {
    const char	*title = lua.ToString(1);
    CWindow* win = program.NewWindow(title);
    
    win->PushTable();
    
    return 1;
}

static int l_draw(lua_State *L) {
    program.Draw();
    return 0;
}

static int l_clear(lua_State *L) {
    program.Clear();
    return 0;
}

static int l_exit(lua_State *L) {
    program.Exit();
    return 0;
}

static int l_calc(lua_State *L) {
    program.CalcMaps();
    return 0;
}

static int l_timezones(lua_State *L) {
    const char	*path = lua.ToString(1);

    program.TimeZones(path);

    return 0;
}


CProgram::CProgram() {
    iTimeZones = strdup("/usr/share/zoneinfo");
    iAtlasFile = strdup("data/atlas/atlas.db");
    
    PushTable();
    lua.Method("newchart", l_newchart, this);
    lua.Method("newwindow", l_newwindow, this);
    lua.Method("clear", l_clear, this);
    lua.Method("exit", l_exit, this);
    lua.Method("draw", l_draw, this);
    lua.Method("calc", l_calc, this);
    lua.Method("timezones", l_timezones, this);

    lua.SetGlobal("program");
    
    GetDir();

    iLog = new CLog;
}

CProgram::~CProgram() {
    LuaExec("QUIT");

    delete iLog;
    
    free(iTimeZones);
}

void CProgram::GetDir() {
    char*	env;
    
    env = getenv("ASTROZAUR");
    if (env) {
	strcpy(iDir, env);
	if (chdir(iDir) == 0) return;
    }
    
    env = getenv("HOME");
    if (env) {
	strcpy(iDir, env);
	strcat(iDir, "/astrozaur");
	
	if (chdir(iDir) == 0) return;
    }
    
    strcpy(iDir, "/usr/local/share/astrozaur");
    if (chdir(iDir) == 0) return;
    
    getcwd(iDir, sizeof(iDir));
}

CWindow* CProgram::NewWindow(const char *title) {
    CWindow*	window = new CWindow;

    if (window) {
	window->Make();
	
	if (title) window->Title(title);
	
	window->LoadedFile(true);
	window->Maps()->LoadedFile(true);
	window->Show();

	AddWindow(window);
    } else {
	printf("Error: new CWindow\n");
    }
    
    return window;
}

void CProgram::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    
    while (subNode) {
	if (!strcmp((char*)subNode->name,"file")) {
	    if (has_content(subNode))
		LoadFile((char *)subNode->children->content);
	    goto next;
	}

	if (!strcmp((char*)subNode->name,"chart")) {
	    AddMap(subNode);
	    goto next;
	}

	if (!strcmp((char*)subNode->name,"log")) {
	    iLog->XML(subNode);
	    goto next;
	}

	if (!strcmp((char *) subNode->name, "window")) {
	    if (has_content(subNode)) {
		list<CWindow*>::iterator i = iWindows.begin();
		CWindow* window = (*i);
		
		if (window->Maps()->Empty()) {
		    window->XML(subNode);
		} else {
		    window = NewWindow();
		    
		    if (window) window->XML(subNode);
		}
//		window->LuaExec("INIT", true);
	    }
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

xmlNodePtr CProgram::XML(xmlNodePtr parent, int level) {
    for (unsigned int i = 0; i < iMapsList.size(); i++)
	iMapsList[i]->XML(parent, level);
	
    for (list<CWindow*>::iterator i = iWindows.begin(); i != iWindows.end(); i++)
	(*i)->XML(parent, level);
	
    iLog->XML(parent, level);
    return NULL;
}

void CProgram::Draw() {
    DoInfo();

    for (list<CWindow*>::iterator i = iWindows.begin(); i != iWindows.end(); i++)
	(*i)->Redraw();
}

void CProgram::Clear() {
    for (list<CWindow*>::iterator i = iWindows.begin(); i != iWindows.end(); i++) 
	delete (*i);
	
    iWindows.clear();
    iLog->Clear();
    iLog->View();
    FreeMaps();
}

void CProgram::Exit() {
    Clear();
    exit(0);
}

void CProgram::FreeMaps() {
    for (unsigned int i = 0; i < iMapsList.size(); i++)
	delete iMapsList[i];

    iMapsList.clear();
    iMapsMap.clear();
}

void CProgram::ResolveShadow() {
    for (unsigned int i = 0; i < iMapsList.size(); i++)
	iMapsList[i]->ResolveShadow();
}

void CProgram::AddWindow(CWindow *window) {
    iWindows.push_back(window);
    for (unsigned int i = 0; i < iMapsList.size(); i++)
	window->AddMenuItem(iMapsList[i]);

    LuaExec("NEW WINDOW", window);
    window->LuaExec("INIT", true);
}

bool CProgram::AddMap(CMap *map) {
    if (map) {
	LuaExec("NEW CHART", map);
    }

    string name = map->Info();	// FIX ME (Empty name)

    iMapsList.push_back(map);
    iMapsMap[name] = map;
	
    if (map->Id()) {
        char id[32];
    
        sprintf(id,"id:%s",map->Id());
        iMapsMap[id] = map;
    }
    program.Event(map, eventNewChart);
    
    map->LuaExec("INIT", true);

    return true;
}

CMap* CProgram::AddMap(const char* type) {
    CMap*	map = NULL;

    if (!strcmp(type,"natal")) map = new CMapNatal;
    if (!strcmp(type,"progres")) map = new CMapProgres;
    if (!strcmp(type,"zodiac direct")) map = new CMapZodiacDirect;
    if (!strcmp(type,"equator direct")) map = new CMapEquatorDirect;
    if (!strcmp(type,"true direct")) map = new CMapTrueDirect;
    if (!strcmp(type,"arc direct")) map = new CMapArcDirect;
    if (!strcmp(type,"date select")) map = new CMapDateSelect;
    if (!strcmp(type,"eqlipse")) map = new CMapEqlipse;
    if (!strcmp(type,"middle")) map = new CMapMiddle;
    if (!strcmp(type,"harmonic")) map = new CMapHarmonic;
    if (!strcmp(type,"raw")) map = new CMapRaw;
    if (!strcmp(type,"revolution")) map = new CMapRevolution;
    if (!strcmp(type,"ingres")) map = new CMapIngres;
    if (!strcmp(type,"aspect")) map = new CMapAspect;
    if (!strcmp(type,"stationar")) map = new CMapStationar;

    return map;
}

CMap* CProgram::AddMap(xmlNodePtr node) {
    xmlChar*	xmlType = xmlGetProp(node,(xmlChar *) "type");
    char*	type = (char *) xmlType;
    CMap*	map = AddMap(type);

    xmlFree(xmlType);
    
    if (map) {
	map->XML(node->children);
	AddMap(map);
    }
    
    return map;
}

void CProgram::CalcMaps() {
    for (unsigned int i = 0; i < iMapsList.size(); i++)
	iMapsList[i]->DoCalc();

    for (list<CWindow*>::iterator i = iWindows.begin(); i != iWindows.end(); i++)
	(*i)->Maps()->CalcAspectTable();
	
    DoInfo();
}

CMap* CProgram::Map(unsigned int data) {
    return data < iMapsList.size() ? iMapsList[data] : NULL;
}

CMap* CProgram::Map(char *data) {
    string name = data;

    return iMapsMap[name];
}

unsigned int CProgram::MapNum() {
    return iMapsList.size();
}

bool CProgram::LuaSet(const char* var) {
    return false;
}

bool CProgram::LuaGet(const char* var) {
    int n;

    if (strcmp(var, "chart") == 0) {
	lua.NewTable();
	
	for (n = 0; n < iMapsList.size(); n++) {
	    CMap*	map = iMapsList[n];
    
	    //lua.PushString(map->IdInfo());
	    lua.PushNumber(n+1);
	    map->PushTable();
	    map->Lua(); // ???
	    lua.EndTable();
	}
	return true;
    } else if (strcmp(var, "window") == 0) {
	    list<CWindow*>::iterator iw = iWindows.begin();
	    n = 1;

	    lua.NewTable();
	    while (iw != iWindows.end()) {
		if ((*iw)->Visible()) {
		    lua.PushNumber(n++);
		    (*iw)->PushTable();
		    (*iw)->Lua(); // ???
		    lua.EndTable();
		}
    		iw++;
	    }
	return true;
    } 
    
    return false;
}

void CProgram::DoInfo() {
    chdir(iDir);

    LuaExec("INFO");
    iLog->View();
}

void CProgram::Event(CAstro* obj, eventEnum event, void* data) {
    switch (event) {
	case eventChangeChartInfo:
	    for (map<string, CMap*>::iterator p = iMapsMap.begin(); p != iMapsMap.end(); p++)
		if (p->second == obj) {
		    CMap*	map = (CMap*) obj;
		    string	name = map->Info();

		    iMapsMap.erase(p);
		    iMapsMap[name] = map;
		    break;
		}
	    // Not break - ok...
		
	case eventNewChart:
	    for (list<CWindow*>::iterator i = iWindows.begin(); i != iWindows.end(); i++)
		(*i)->Event(obj, event, data);
	    // Not break - ok...
	
	case eventChangePlaceInfo:
	case eventChangePlaceLon:
	case eventChangePlaceLat:
	case eventChangeTime:
	case eventChangeDate:
	case eventChangeTimeZone:
	    for (unsigned int i = 0; i < iMapsList.size(); i++)
		iMapsList[i]->Event(obj, event, data);
		
	    break;
	    
	case eventChangeChartId: {
	    for (map<string, CMap*>::iterator p = iMapsMap.begin(); p != iMapsMap.end(); p++)
		if (p->second == obj)
		    iMapsMap.erase(p);

	    CMap* map = (CMap*) obj;
	    
	    if (map->Id() && strlen(map->Id()) > 0) {
		char 	id[32];

		sprintf(id, "id:%s", map->Id());
		iMapsMap[id] = map;
	    }

	    if (map->Info() && strlen(map->Info()) > 0)
		iMapsMap[map->Info()] = map;

	    map->Event(obj, event, data);
		
	    break;
	}
	
	case eventNewPoint:
	    for (list<CWindow*>::iterator i = iWindows.begin(); i != iWindows.end(); i++)
		(*i)->Maps()->Event(obj, event, data);

	    break;
	
	default:
	    break;
    }
}

const char* CProgram::Msg(const char* key) {
    lua.GetGlobal("program");
	
    if (lua.Type(-1) == LUA_TTABLE) {
        lua.PushString("msg");
        lua.GetTable(-2);
        lua.Remove(-2);
	    
        if (lua.Type(-1) == LUA_TTABLE) {
    	    lua.PushString(key);
	    lua.GetTable(-2);
	    lua.Remove(-2);
		
	    if (lua.Type(-1) != LUA_TNIL) {
	        const char *text = strdup(lua.ToString(-1));
		
	        lua.Pop(1);
	        return text;
	    } else {
	        lua.Pop(1);	// nil
	    }
	} else {
	    lua.Pop(1);		// not table
	}
    } else {
        lua.Pop(1); 		// not table
    }
    
    return key;
}

void CProgram::AtlasFile(const char *path) {
    if (iAtlasFile) free(iAtlasFile);
    iAtlasFile = strdup(path);
}

char* CProgram::AtlasFile() {
    return iAtlasFile;
}

void CProgram::TimeZones(const char *path) {
    if (iTimeZones) free(iTimeZones);
    iTimeZones = strdup(path);
}

char* CProgram::TimeZones() {
    return iTimeZones;
}
