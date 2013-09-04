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

#include <string>
#include <fltk/run.h>
#include "program.h"
#include "projection.h"
#include "window.h"
#include "aspects.h"
#include "maps.h"
#include "draw_area.h"
#include "utils.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

CMaps::CMaps() {
    iAspectTable = new CAspectTable;
    iAspectMap1 = NULL;
    iAspectMap2 = NULL;
    iActiveMap = NULL;
    iProjection = new CEqliptic;
    
}

CMaps::~CMaps() {
    delete iAspectTable;
}

void CMaps::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    bool	aspectReload = false;
    
    while (subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"file")) {
	    if (has_content(subNode)) {
		LoadFile((char *)subNode->children->content);
	    }
	    goto next;
	}

	if (!strcmp(name,"aspects")) {
	    iAspectTable->XML(subNode);
	    aspectReload = true;
		
	    goto next;
	}

	if (!strcmp(name,"active_chart")) {
	    char*	name = (char*)xmlGetProp(subNode,(xmlChar *) "name");

	    if (name) {
		ActiveMap(name);
		free(name);
	    }
	    
	    if (iActiveMap) {
		xmlNodePtr chartNode = subNode->children;
		    
		while (chartNode) {
		    if (!strcmp((char*)chartNode->name,"chart")) {
			iActiveMap->Ready(false);
			iActiveMap->XML(chartNode->children);
//			program.CalcMaps();
		    }
		    chartNode = chartNode->next;
		}
	    }
	    goto next;
	}

	if (!strcmp(name,"aspect_chart")) {
	    char*	name = (char*)xmlGetProp(subNode,(xmlChar *) "name");

	    if (name) {
		AspectedMap(name);
		free(name);
	    }
	    goto next;
	}

	if (!strcmp(name,"hide_chart")) {
	    char*	name = (char*)xmlGetProp(subNode,(xmlChar *) "name");

	    if (name) {
		CMap* map = program.Map(name);
		
		if (map) iView[map] = false;
		free(name);
	    }
	    goto next;
	}

	if (!strcmp(name,"show_chart")) {
	    char*	name = (char*)xmlGetProp(subNode,(xmlChar *) "name");

	    if (name) {
		CMap* map = program.Map(name);
		
		if (map) iView[map] = true;
		free(name);
	    }
	    goto next;
	}

	if (!strcmp(name,"start_chart")) {
	    char*	name = (char*)xmlGetProp(subNode,(xmlChar *) "name");
	    char*	point = (char*)xmlGetProp(subNode,(xmlChar *) "point");

	    if (name && point) {
		CMap* map = program.Map(name);
		
		if (map) iProjection->StartPoint(map->iPointsMap[point]);

		free(name);
		free(point);
	    }
	    goto next;
	}

	if (!strcmp(name,"chart")) {
	    CMap* map = program.AddMap(subNode);
	    
	    iView[map] = true;

	    goto next;
	}

	if (!strcmp(name,"projection")) {
	    const char* type = (const char*) xmlGetProp(subNode,(xmlChar *) "type");
	    
	    if (type) {
		Projection(type);
		if (iProjection) iProjection->XML(subNode->children);
	    }

	    goto next;
	}

next:	subNode = subNode->next;
    }
    
    if (aspectReload && iAspectMap1 && iAspectMap2)
	AspectedMap(iAspectMap1, iAspectMap2);
}

xmlNodePtr CMaps::XML(xmlNodePtr parent, int level) {
    xmlNodePtr sub;

    CAstro::XML(parent, level);

    for (unsigned int i = 0; i < program.MapNum(); i++) {
	CMap* map = program.Map(i);
	
	if (iView[map]) {
	    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level+1)));
	    sub = xmlNewTextChild(parent, NULL, (xmlChar*)"show_chart", NULL);
	    xmlNewProp(sub, (xmlChar*)"name", (xmlChar*)map->IdInfo());
	}
    }

    if (iAspectMap1) {
	xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level+1)));
	sub = xmlNewTextChild(parent, NULL, (xmlChar*)"active_chart", NULL);
	xmlNewProp(sub, (xmlChar*)"name", (xmlChar*)iAspectMap1->IdInfo());
    }

    if (iActiveMap != iAspectMap1) {
	if (iAspectMap2) {
	    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level+1)));
	    sub = xmlNewTextChild(parent, NULL, (xmlChar*)"aspect_chart", NULL);
	    xmlNewProp(sub, (xmlChar*)"name", (xmlChar*)iAspectMap2->IdInfo());
	}
	if (iActiveMap) {
	    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level+1)));
	    sub = xmlNewTextChild(parent, NULL, (xmlChar*)"active_chart", NULL);
	    xmlNewProp(sub, (xmlChar*)"name", (xmlChar*)iActiveMap->IdInfo());
	}
    } else {
	if (iAspectMap2) {
	    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level+1)));
	    sub = xmlNewTextChild(parent, NULL, (xmlChar*)"aspect_chart", NULL);
	    xmlNewProp(sub, (xmlChar*)"name", (xmlChar*)iAspectMap2->IdInfo());
	}
    }

    CPoint* start = iProjection->StartPoint();

    if (start) {
	xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level+1)));
	sub = xmlNewTextChild(parent, NULL, (xmlChar*)"start_chart", NULL);
	xmlNewProp(sub, (xmlChar*)"point", (xmlChar*)start->Name());
	xmlNewProp(sub, (xmlChar*)"name", (xmlChar*)start->Map()->IdInfo());
    }
    
    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level)));
    return NULL;
}

void CMaps::CalcAspectTable() {
    iAspectTable->Calc();
}

void CMaps::AspectedMap(CMap* a, CMap *b) {
    iAspectMap1 = a;
    iAspectMap2 = b;

    if (a == b)
	iAspectTable->CreateItems(a);
    else
	iAspectTable->CreateItems(a, b);
}

void CMaps::AspectedMap(char* name) {
    CMap* map = program.Map(name);
    
    if (map && iActiveMap)
	AspectedMap(iActiveMap,map);
}

void CMaps::AspectedMap(CMap* map) {
    if (map && iActiveMap)
	AspectedMap(iActiveMap,map);
}

void CMaps::StartPoint(char *point) {
    if (iActiveMap)
	iProjection->StartPoint(iActiveMap->iPointsMap[point]);
}

void CMaps::Projection(const char* type) {
    if (!strcmp(type, "eqliptic")) {
        delete iProjection;
	iProjection = new CEqliptic;
    } else if (!strcmp(type, "horizont")) {
        delete iProjection;
        iProjection = new CHorizont;
    } else if (!strcmp(type, "dial")) {
        delete iProjection;
        iProjection = new CDial;
    }
}

CProjection* CMaps::Projection() {
    return iProjection;
}

void CMaps::Draw(CDrawArea* area) {
    unsigned int i;

    iProjection->BeforeDraw();
    iProjection->DrawSelf(area);

    for (i = 0; i < program.MapNum(); i++) {
	CMap* map = program.Map(i);
	
	if (iView[map]) iProjection->DrawMap(map, area, map == iActiveMap);
    }

    fltk::setcolor(area->color());
    fltk::addarc(area->iCenterX - area->iSize, area->iCenterY - area->iSize,
		 area->iSize*2, area->iSize*2, 0, -360); 
    fltk::fillstrokepath(fltk::BLACK);
    fltk::setcolor(fltk::BLACK);

    iAspectTable->Draw(area);

    for (i = 0; i < program.MapNum(); i++) {
	CMap* map = program.Map(i);
	if (iView[map]) map->DrawPoints(area);
    }
    iProjection->AfterDraw();
}

void CMaps::Hide(CMap* map) {
    iView[map] = ! iView[map];
}

void CMaps::View(CMap* map, bool data) {
    iView[map] = data;
}

bool CMaps::View(CMap* map) {
    return iView[map];
}

CMap* CMaps::ActiveMap() {
    return iActiveMap;
}

void CMaps::ActiveMap(CMap* map) {
    iActiveMap = map;
}

void CMaps::ActiveMap(char* name) {
    CMap* map = program.Map(name);
    
    if (map) iActiveMap = map;
}

bool CMaps::Empty() {
    return iView.size() == 0;
}

CAspectTable* CMaps::AspectTable() {
    return iAspectTable;
}

bool CMaps::LuaSet(const char* var) {
    if (strcmp(var, "active") == 0) {
	if (lua.Type(-1) == LUA_TTABLE) {
	    lua.PushString(".ptr_chart");
	    lua.GetTable(-2);

	    if (lua.Type(-1) == LUA_TLIGHTUSERDATA) {
		CMap *map = (CMap*) lua.ToUser(-1);
		
		ActiveMap(map);
	    }
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "aspected") == 0) {
	if (lua.Type(-1) == LUA_TTABLE) {
	    CMap *map1 = NULL, *map2 = NULL;
	
	    lua.PushNumber(1);
	    lua.GetTable(-2);
	    
	    if (lua.Type(-1) == LUA_TTABLE) {
		lua.PushString(".ptr_chart");
		lua.GetTable(-2);
		
		if (lua.Type(-1) == LUA_TLIGHTUSERDATA)
		    map1 = (CMap*) lua.ToUser(-1);
		
		lua.Pop(1);
	    }
	    lua.Pop(1);

	    lua.PushNumber(2);
	    lua.GetTable(-2);
	    
	    if (lua.Type(-1) == LUA_TTABLE) {
		lua.PushString(".ptr_chart");
		lua.GetTable(-2);
		
		if (lua.Type(-1) == LUA_TLIGHTUSERDATA)
		    map2 = (CMap*) lua.ToUser(-1);
		
		lua.Pop(1);
	    }
	    lua.Pop(1);
	    
	    if (map1) {
		if (map2) {
		    AspectedMap(map1, map2);
		} else {
		    AspectedMap(map1, map1);
		}
	    }
	}
	return true;
    }

    return false;
}

bool CMaps::LuaGet(const char* var) {
    if (strcmp(var, "active") == 0) {
	if (iActiveMap) {
	    iActiveMap->PushTable();
	} else {
	    lua.PushNil();
	}
	return true;
    } else if (strcmp(var, "aspected") == 0) {
	lua.NewTable();
	if (iAspectMap1) {
	    lua.PushNumber(1);
	    iAspectMap1->PushTable();
	    lua.EndTable();
	
	    if (iAspectMap2 != iAspectMap1) {
		lua.PushNumber(2);
		iAspectMap2->PushTable();
		lua.EndTable();
	    }
	}
	return true;
    } else if (strcmp(var, "aspect") == 0) {
	iAspectTable->PushTable();
	return true;	
    }
    
    return false;
}

void CMaps::Event(CAstro* obj, eventEnum event, void* data) {
    if (event == eventNewPoint && (obj == iAspectMap1 || obj == iAspectMap2)) {
	AspectedMap(iAspectMap1, iAspectMap2);
    }
}
