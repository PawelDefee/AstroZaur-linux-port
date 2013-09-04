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

#include "aspects.h"
#include "aspect_eql_lon.h"
#include "aspect_equator.h"
#include "aspect_reception.h"
#include "aspect_symetric.h"
#include "map.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

// CAspectData

CAspectData::CAspectData() {
    iActive = false;
    iAspect = NULL;
    iPoint1 = NULL;
    iPoint2 = NULL;
    iName = NULL;
    iType = NULL;
}

CAspectData::~CAspectData() {
    if (iName) free(iName);
}

const char* CAspectData::Name() {
    return iName;
}

void CAspectData::Name(const char *data) {
    if (iName) free(iName);
    iName = strdup(data);
    
    PushTable();
    lua.SetField("name", iName);
    lua.Pop(1);
}

const char* CAspectData::Type() {
    return iType;
}

bool CAspectData::ThisType(const char* type) {
    return strcmp(iType, type) == 0;
}

void CAspectData::Type(const char *data) {
    if (iType) free(iType);
    iType = strdup(data);

    PushTable();
    lua.SetField("type", iType);
    lua.Pop(1);
}
    
CPoint* CAspectData::Point1() {
    return iPoint1;
}

CPoint* CAspectData::Point2() {
    return iPoint2;
}

void CAspectData::Points(CPoint* point1, CPoint* point2) {
    iPoint1 = point1;
    iPoint2 = point2;

    PushTable();
    lua.SetField("point1", iPoint1->Name());
    lua.SetField("point2", iPoint2->Name());
    lua.Pop(1);
}

bool CAspectData::Active() {
    return iActive;
}

void CAspectData::Active(bool data) {
    iActive = data;
}

void CAspectData::Aspect(CAspect* aspect) {
    iAspect = aspect;
}

void CAspectData::Calc() {
    Active(false);
    
    if (iAspect && iAspect->View() &&
	((iPoint1->Aspecting() && iPoint2->Aspected()) ||
	(iPoint1->Aspected() && iPoint2->Aspecting())))
    {
	iAspect->Calc(this);
    }
}

// CAspect

CAspect::CAspect() {
    iType = NULL;
    iView = true;
}

CAspect::~CAspect() {
    if (iType) free(iType);
}

void CAspect::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    char*	str;
    
    while (subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"exclude")) {
	    if (has_content(subNode)) {
		xmlNodePtr node1 = subNode->children;
		
		while (node1) {
		    if (!strcmp((char*)node1->name,"point")) {
			char	name1[128];
			
			name1[0] = 0;
			
			str = (char*) xmlGetProp(node1,(xmlChar *) "name");
			if (str) {
			    strcpy(name1, str);
			} else {
			    str = (char*) xmlGetProp(node1,(xmlChar *) "category");
			
			    if (str) {
				strcpy(name1, "cat:");
				strcat(name1, str);
			    }
			}
			if (has_content(node1)) {
			    xmlNodePtr 	node2 = node1->children;
			    
			    while (node2) {
				if (!strcmp((char*)node2->name,"point")) {
				    char	name2[128];
				    
				    name2[0] = 0;
				    
				    str = (char*) xmlGetProp(node2,(xmlChar *) "name");
				    if (str) {
					strcpy(name2, str);
				    } else {
					str = (char*) xmlGetProp(node2,(xmlChar *) "category");
			
					if (str) {
					    strcpy(name2, "cat:");
					    strcat(name2, str);
					}
				    }
				    strcat(name1, name2);
				    iExclude.insert(Hash(name1));
				}
				node2 = node2->next;
			    }
			} else {
			    iExclude.insert(Hash(name1));
			}
		    }
		    node1 = node1->next;
		}
	    }
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

const char* CAspect::Type() {
    return iType;
}

bool CAspect::ThisType(const char* type) {
    return strcmp(iType, type) == 0;
}

void CAspect::Type(const char *data) {
    if (iType) free(iType);
    iType = strdup(data);
}

void CAspect::View(bool data) {
    iView = data;
}

bool CAspect::View() {
    return iView;
}

bool CAspect::LuaSet(const char* var) {
    if (strcmp(var, "exclude") == 0 && lua.Type(-1) == LUA_TTABLE) {
	lua.PushNil();
	
	while (lua.Next(-2)) {
	    if (lua.Type(-1) == LUA_TSTRING) {
		const char*	point = lua.ToString(-1);

		iExclude.insert(Hash(point));
	    } else if (lua.Type(-1) == LUA_TTABLE) {
		const char*	point1 = lua.ToString(-2);
		
		lua.PushNil();

		while (lua.Next(-2)) {
		    const char*	point2 = lua.ToString(-1);
		    char	points[128];
		    
		    strcpy(points, point1);
		    strcat(points, point2);

		    iExclude.insert(Hash(points));
		    lua.Pop(1);
		}
	    }
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "view") == 0) {
	iView = lua.ToBool(-1);
	return true;
    }

    return false;
}

bool CAspect::Exclude(CPoint* point1, CPoint* point2) {
    char	str[128];

    strcpy(str, point1->Name());
    strcat(str, point2->Name());
    if (iExclude.find(Hash(str)) != iExclude.end()) return true;

    strcpy(str, point2->Name());
    strcat(str, point1->Name());
    if (iExclude.find(Hash(str)) != iExclude.end()) return true;

    if (iExclude.find(Hash(point1->Name())) != iExclude.end()) return true;
    if (iExclude.find(Hash(point2->Name())) != iExclude.end()) return true;

    strcpy(str, "cat:");
    strcat(str, point1->Name());
    strcat(str, "cat:");
    strcat(str, point2->Name());
    if (iExclude.find(Hash(str)) != iExclude.end()) return true;

    strcpy(str, "cat:");
    strcat(str, point2->Name());
    strcat(str, "cat:");
    strcat(str, point1->Name());
    if (iExclude.find(Hash(str)) != iExclude.end()) return true;

    strcpy(str, "cat:");
    strcat(str, point1->Name());
    if (iExclude.find(Hash(str)) != iExclude.end()) return true;

    strcpy(str, "cat:");
    strcat(str, point2->Name());
    if (iExclude.find(Hash(str)) != iExclude.end()) return true;
    
    return false;
}

// CAspectTable

static int l_clear(lua_State *L) {
    CAspectTable* 	table = (CAspectTable*) lua.GetObj();
    
    const char	*type = lua.ToString(1);

    if (type) {
	table->FreeItems(type);
	table->FreeAspects(type);
    } else {
	table->FreeItems();
	table->FreeAspects();
    }
    
    return 0;
}

static int l_new(lua_State *L) {
    CAspectTable* 	table = (CAspectTable*) lua.GetObj();
    
    const char	*type = lua.ToString(1);

    CAspect* aspect =  table->NewAspect(type);
    
    if (aspect) {
	aspect->PushTable();
    } else {
	lua.PushNil();
    }
    
    return 1;
}

CAspectTable::CAspectTable() {
    PushTable();
    lua.Method("clear", l_clear, this);
    lua.Method("new", l_new, this);
    lua.Remove(-1);
}

CAspectTable::~CAspectTable() {
    FreeItems();
    FreeAspects();
}

void CAspectTable::Calc() {
    list<CAspectData*>::iterator item;
    
    for (item = iItem.begin(); item != iItem.end(); item++)
	(*item)->Calc();
}

void CAspectTable::Draw(CDrawArea* area) {
    list<CAspectData*>::iterator item;
    
    for (item = iItem.begin(); item != iItem.end(); item++)
	if ((*item)->Active()) {
	    (*item)->Draw(area);
	}
}

CAspect* CAspectTable::NewAspect(const char *type) {
    CAspect	*aspect = NULL;
    
    if (!strcmp(type, "eqliptic")) aspect = new CAspectEqlLon;
    if (!strcmp(type, "equator")) aspect = new CAspectEquator;
    if (!strcmp(type, "reception")) aspect = new CAspectReception;
    if (!strcmp(type, "symetric")) aspect = new CAspectSymetric;

    if (aspect) {
	aspect->Type(type);
	iAspects.push_back(aspect);
    }
    
    return aspect;
}

void CAspectTable::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;

    // We in root?    
    while (subNode) {
	if (!strcmp((char*)subNode->name,"aspects")) break;
	subNode = subNode->next;
    }
    
    if (subNode) {	// Found in root
	subNode = subNode->children;
    } else {		// Not found
	subNode = node;
    }

    while (subNode) {
	const char* name = (const char *)subNode->name;
	CAspect* aspect;

	if (!strcmp(name,"clear")) {
	    FreeItems();
	    FreeAspects();
	    goto next;
	}

	aspect = NewAspect(name);
	
	if (aspect) {
	    FreeItems(name);
	    aspect->XML(subNode->children);
	}

next:	subNode = subNode->next;
    }
}

void CAspectTable::FreeItems() {
    list<CAspectData*>::iterator item;
    
    for (item = iItem.begin(); item != iItem.end(); item++)
	delete (*item);
    iItem.clear();
}

void CAspectTable::FreeAspects() {
    list<CAspect*>::iterator item;
    
    for (item = iAspects.begin(); item != iAspects.end(); item++)
	delete (*item);
    iAspects.clear();
}

CAspect* CAspectTable::FindAspect(const char* type) {
    list<CAspect*>::iterator item = iAspects.begin();

    while (item != iAspects.end())
	if ((*item)->ThisType(type)) {
	    return (*item);
	} else item++;
	
    return NULL;
}

void CAspectTable::FreeItems(const char *type) {
    list<CAspectData*>::iterator item = iItem.begin();

    while (item != iItem.end()) {
	if ((*item)->ThisType(type)) {
	    delete (*item);
	    item = iItem.erase(item);
	} else item++;
    }
}

void CAspectTable::FreeAspects(const char *type) {
    list<CAspect*>::iterator item = iAspects.begin();

    while (item != iAspects.end()) {
	if ((*item)->ThisType(type)) {
	    delete (*item);
	    item = iAspects.erase(item);
	} else item++;
    }
}

void CAspectTable::CreateItems(CMap* map) {
    list<CPoint*>::iterator p1;
    list<CPoint*>::iterator p2;
    
    FreeItems();
    for (p1 = map->iPointsList.begin(); p1 != map->iPointsList.end(); p1++)
	for (p2 = p1, p2++; p2 != map->iPointsList.end(); p2++) {
	    list<CAspect*>::iterator aspect;
	    
	    for (aspect = iAspects.begin(); aspect != iAspects.end(); aspect++) {
		CAspectData* data = (*aspect)->Data((*p1), (*p2));
		
		if (data) {
		    data->Points((*p1), (*p2));
		    data->Aspect((*aspect));
		    data->Type((*aspect)->Type());
		    iItem.push_back(data);
		}
	    }
	}
}

void CAspectTable::CreateItems(CMap* map1, CMap* map2) {
    list<CPoint*>::iterator p1;
    list<CPoint*>::iterator p2;
    
    FreeItems();
    for (p1 = map1->iPointsList.begin(); p1 != map1->iPointsList.end(); p1++)
	for (p2 = map2->iPointsList.begin(); p2 != map2->iPointsList.end(); p2++) {
	    list<CAspect*>::iterator aspect;
	    
	    for (aspect = iAspects.begin(); aspect != iAspects.end(); aspect++) {
		CAspectData* data = (*aspect)->Data((*p1), (*p2));

		if (data) {
		    data->Points((*p1), (*p2));
		    data->Aspect((*aspect));
		    data->Type((*aspect)->Type());
		    iItem.push_back(data);
		}
	    }
	}
}

bool CAspectTable::LuaGet(const char* var) {
    if (strcmp(var, "item") == 0) {
	list<CAspectData*>::iterator item = iItem.begin();
	int i = 1;

	lua.NewTable();

	while (item != iItem.end()) {
	    if ((*item)->Active()) {
		lua.PushNumber(i++);
		(*item)->PushTable();
		lua.EndTable();
	    }
	    item++;
	}
	return true;
    } else if (strcmp(var, "aspects") == 0) {
	list<CAspect*>::iterator aspect = iAspects.begin();
	int i = 1;

	lua.NewTable();

	while (aspect != iAspects.end()) {
	    lua.PushNumber(i++);
	    (*aspect)->PushTable();
	    lua.EndTable();
	    aspect++;
	}
	return true;
    }
    
    return false;
}

bool CAspectTable::LuaSet(const char* var) {
    return false;
}
