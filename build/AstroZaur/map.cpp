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

#include <algorithm>
#include <math.h>
#include "astro.h"
#include "map.h"
#include "map_edit.h"
#include "program.h"
#include "utils.h"
#include "draw_area.h"
#include "transform_horizont.h"
#include "transform_sidereal.h"
#include "transform_inmundo.h"
#include "transform_equator.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "static_holder.h"
#include "point_aspect.h"

#define PI 3.14159265358979323846

static int l_newpoint(lua_State *L) {
    CMap* 	map = (CMap*) lua.GetObj();
    
    const char	*type = lua.ToString(1);
    const char	*name = lua.ToString(2);

    if (type && name) {
	CPoint* point = map->NewPoint(type, name);
    
	if (point) {
	    map->Event(map, eventNewPoint, (void*) point);
	    program.Event(map, eventNewPoint, (void*) point);

	    point->PushTable();
	    return 1;
	}
    }

    return 0;
}

static int l_edit(lua_State *L) {
    CMap* 	map = (CMap*) lua.GetObj();

    map->OpenEdit();
    
    return 0;    
}

static int l_newtransform(lua_State *L) {
    CMap* 	map = (CMap*) lua.GetObj();
    
    const char	*type = lua.ToString(1);

    if (type) {
	CTransform* transform = map->NewTransform(type);
    
	if (transform) {
	    transform->PushTable();
	    return 1;
	}
    }

    return 0;
}

CMap::CMap() {
    iType = NULL;
    iInfo = NULL;
    iId = NULL;
    iRotateSec = 60*60;
    iColor = fltk::WHITE;
    iEditor = NULL;
    iWidth = 25;
    
    PushTable();
    lua.Method("newpoint", l_newpoint, this);
    lua.Method("newtransform", l_newtransform, this);
    lua.Method("edit", l_edit, this);
    lua.SetField(".ptr_chart", this);

    lua.PushString("point");
    iPointsRef.PushTable();
    lua.EndTable();

    lua.Pop(1);
}


CMap::~CMap() {
    if (iInfo) free(iInfo);
    if (iId) free(iId);
    if (iEditor) delete iEditor;
    if (iType) free(iType);

    list<CPoint*>::iterator point;

    for (point = iPointsList.begin(); point != iPointsList.end(); point++)
	delete *point;

    list<CTransform*>::iterator transform;

    for (transform = iTransform.begin(); transform != iTransform.end(); transform++)
	delete *transform;
	
    iPointsList.clear();
    iPointsMap.clear();
    iDepends.clear();
    iTransform.clear();
}

void CMap::Info(const char *data) {
    free(iInfo);
    iInfo = strdup(data);
    program.Event(this, eventChangeChartInfo);
}

char* CMap::Info() {
    return iInfo;
}

char* CMap::Id() {
    return iId;
}

void CMap::Id(const char* data) {
    if (iId) free(iId);
    iId = strdup(data);
    program.Event(this, eventChangeChartId);
}

void CMap::Type(const char* type) {
    if (iType) free(iType);
    iType = strdup(type);

    PushTable();
    lua.SetField("type", iType);
    lua.Pop(1);
}

char* CMap::Type() {
    return iType;
}

char* CMap::IdInfo() {
    static char str[64];
    
    if (iId && strlen(iId)) {
	sprintf(str, "id:%s", iId);
	return str;
    }
    
    return iInfo;
}

void CMap::OpenEdit() {
    if (iEditor == NULL) {
	iEditor = MakeEditor();
	iEditor->MakeWindow();
	iEditor->Show();

	PushTable();
	lua.PushString("editor");
	iEditor->PushTable();
	lua.EndTable();
	lua.Pop(1);
    }
    iEditor->Show();
    iEditor->SetData();
}

fltk::Color CMap::Color() {
    return iColor;
}

void  CMap::Color(fltk::Color color) {
    iColor = color;
    program.Event(this, eventChangeChartColor);
}

void CMap::AddDepends(CMap* map) {
    iDepends.push_back(map);
}

void CMap::DelDepends(CMap* map) {
    iDepends.erase(find(iDepends.begin(), iDepends.end(), map));
}

xmlNodePtr CMap::XML(xmlNodePtr parent, int level) {
    xmlNodePtr		node;
    char		str[16];
    
    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level)));
    
    node = xmlNewTextChild(parent, NULL, (xmlChar*)"chart", NULL);
    xmlNewProp(node, (xmlChar*)"type", (xmlChar*)iType);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));

    xmlNewTextChild(node, NULL, (xmlChar*)"info", (xmlChar*)iInfo);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    
    if (iId) {
	xmlNewTextChild(node, NULL, (xmlChar*)"id", (xmlChar*)iId);
	xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    }

    unsigned char	r, g, b;

    fltk::split_color(iColor, r, g, b);
    sprintf(str,"#%02X%02X%02X", r, g, b);
    xmlNewTextChild(node, NULL, (xmlChar*)"color", (xmlChar*)str);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));

    sprintf(str,"%i",iRotateSec);
    xmlNewTextChild(node, NULL, (xmlChar*)"rotate_sec", (xmlChar*)str);

    // Points

    xmlNodePtr 			sub;
    list<CPoint*>::iterator 	point		= iPointsList.begin();
    bool			view		= (*point)->View();
    bool			aspected	= (*point)->Aspected();
    bool			aspecting	= (*point)->Aspecting();
    bool			info		= (*point)->Info();
    bool			first		= true;

    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    sub = xmlNewTextChild(node, NULL, (xmlChar*)"points", NULL);

    while (point != iPointsList.end()) {
	xmlChar* str;
    
	if ((*point)->View() != view || first) {
	    str = (xmlChar*) ((*point)->View() ? "true":"false");
	    xmlAddChild(sub, xmlNewText((xmlChar*)xml_level(level+2)));
	    xmlNewTextChild(sub, NULL, (xmlChar*)"view", str);
	}

	if ((*point)->Aspected() != aspected || first) {
	    str = (xmlChar*) ((*point)->Aspected() ? "true":"false");
	    xmlAddChild(sub, xmlNewText((xmlChar*)xml_level(level+2)));
	    xmlNewTextChild(sub, NULL, (xmlChar*)"aspected", str);
	}

	if ((*point)->Aspecting() != aspecting || first) {
	    str = (xmlChar*) ((*point)->Aspecting() ? "true":"false");
	    xmlAddChild(sub, xmlNewText((xmlChar*)xml_level(level+2)));
	    xmlNewTextChild(sub, NULL, (xmlChar*)"aspecting", str);
	}

	if ((*point)->Info() != info || first) {
	    str = (xmlChar*) ((*point)->Info() ? "true":"false");
	    xmlAddChild(sub, xmlNewText((xmlChar*)xml_level(level+2)));
	    xmlNewTextChild(sub, NULL, (xmlChar*)"info", str);
	}
    
	(*point)->XML(sub, level+2);
	
	view		= (*point)->View();
    	aspected	= (*point)->Aspected();
    	aspecting	= (*point)->Aspecting();
    	info		= (*point)->Info();
	first		= false;
	
	point++;
    }

    if (iEditor) iEditor->XML(node, level+1);
    
    xmlAddChild(sub, xmlNewText((xmlChar*)xml_level(level+1)));

    return node;
}

CTransform* CMap::NewTransform(const char* type) {
    CTransform* transform = NULL;
    
    if (!strcmp(type, "horizont")) transform = new CTransformHorizont;
    if (!strcmp(type, "sidereal")) transform = new CTransformSidereal;
    if (!strcmp(type, "inmundo")) transform = new CTransformInMundo;
    if (!strcmp(type, "equator")) transform = new CTransformEquator;
		
    if (transform) 
	iTransform.push_back(transform);
    
    return transform;    
}

void CMap::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

//    printf("CMap::XML\n");

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
	char* name = (char *)subNode->name;

	if (!strcmp(name,"file")) {
	    if (has_content(subNode))
		LoadFile((char *)subNode->children->content);
	    goto next;
	}
    
	if (!strcmp(name,"points")) {
	    XMLPoints(subNode);
	    goto next;
	}

	if (!strcmp(name,"lua")) {
	    XMLLua(subNode);
	    goto next;
	}

	if (!strcmp(name,"info")) {
	    if (has_content(subNode)) {
		string info = (char *) subNode->children->content;
		
		info = leadcut(info);
		info = trailcut(info);
		Info(info.c_str());
	    }
	    goto next;
	}

	if (!strcmp(name,"id")) {
	    if (has_content(subNode))
		Id((char *) subNode->children->content);

	    goto next;
	}

	if (!strcmp(name,"color")) {
	    iColor = fltk::color((char *) subNode->children->content);
	}

	if (!strcmp(name,"rotate_sec") && has_content(subNode)) {
	    iRotateSec = atoi((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"transform")) {
	    char* type = (char*) xmlGetProp(subNode,(xmlChar *) "type");

	    if (type) {
		CTransform* transform = NewTransform((const char*) type);
	    
		if (transform && has_content(subNode))
		    transform->XML(subNode->children);

		xmlFree(type);
	    }
	    goto next;
	}

	if (!strcmp(name,"editor")) {
	    OpenEdit();
	    iEditor->XML(subNode);
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

void CMap::XMLPoints(xmlNodePtr node) {
    bool	viewDefault = true;
    bool	aspectedDefault = true;
    bool	aspectingDefault = true;
    bool	infoDefault = false;
    bool	viewDefaultSet = false;
    bool	aspectedDefaultSet = false;
    bool	aspectingDefaultSet = false;
    bool	infoDefaultSet = false;
    xmlNodePtr	pointNode = node;

//    printf("CMap::XMLPoints\n");

    while (pointNode) {
	if (!strcmp((char *)pointNode->name,"points")) break;
	pointNode = pointNode->next;
    }
    
    if (pointNode) {
	pointNode = pointNode->children;
    } else return;

    while (pointNode) {
	char* item = (char *)pointNode->name;

	if (!strcmp(item,"file")) {
	    if (has_content(pointNode)) {
		list<CPoint*>::iterator point = iPointsList.begin();
		
		while (point != iPointsList.end()) {
		    (*point)->LoadFile((char *)pointNode->children->content);
		    point++;
		}
		goto next;
	    }
	}

	if (!strcmp(item,"aspect")) {
	    if (has_content(pointNode)) {
		aspectingDefault = aspectedDefault = strcmp((char*)pointNode->children->content,"true") == 0;
		aspectingDefaultSet = aspectedDefaultSet = true;
	    }
	    goto next;
	}

	if (!strcmp(item,"aspected")) {
	    if (has_content(pointNode)) {
		aspectedDefault = strcmp((char*)pointNode->children->content,"true") == 0;
		aspectedDefaultSet = true;
	    }
	    goto next;
	}

	if (!strcmp(item,"aspecting")) {
	    if (has_content(pointNode)) {
		aspectingDefault = strcmp((char*)pointNode->children->content,"true") == 0;
		aspectingDefaultSet = true;
	    }
	    goto next;
	}

	if (!strcmp(item,"view")) {
	    if (has_content(pointNode)) {
		viewDefault = strcmp((char*)pointNode->children->content,"true") == 0;
		viewDefaultSet = true;
	    }
	    goto next;
	}

	if (!strcmp(item,"info")) {
	    if (has_content(pointNode)) {
		infoDefault = strcmp((char*)pointNode->children->content,"true") == 0;
		infoDefaultSet = true;
	    }
	    goto next;
	}
	    
	if (!strcmp(item,"point")) {
	    char* name = (char*)xmlGetProp(pointNode,(xmlChar *) "name"); 
		    
	    if (name) {
		CPoint* point = iPointsMap[name];
		bool	newPoint = false;

		if (point == NULL) {	// Not found - insert new
		    char* type = (char*)xmlGetProp(pointNode,(xmlChar *) "type");

		    if (type) {
			point = NewPoint(type, name);

			if (point) {
			    newPoint = true;
			}
		    }
		} else point->StateNew(false); // Found it

		if (point) {
		    if (viewDefaultSet) point->View(viewDefault);
		    if (aspectedDefaultSet) point->Aspected(aspectedDefault);
		    if (aspectingDefaultSet) point->Aspecting(aspectingDefault);
		    if (infoDefaultSet) point->Info(infoDefault);
		    point->XML(pointNode);
		    
		    if (newPoint) {
			point->LuaExec("INIT", true);
			Event(this, eventNewPoint, (void*) point);
			program.Event(this, eventNewPoint, (void*) point);
		    }
		}
	    }
	}
next:	pointNode = pointNode->next;
    }
}

CPoint* CMap::NewPoint(const char* type, const char* name) {
    CPoint* point = NULL;

    if (type) {
	if (!strcmp(type,"swiss")) point = new CSwissPlanet();
	if (!strcmp(type,"house")) point = new CHouse();
	if (!strcmp(type,"axis")) point = new CAxis();
	if (!strcmp(type,"star")) point = new CStar();
	if (!strcmp(type,"lua")) point = new CLuaPoint();
	if (!strcmp(type,"midpoint")) point = new CMidPoint();
	if (!strcmp(type,"aspect")) point = new CAspectPoint();
				
	if (point) {
	    point->Name((char*) name);
	    point->Map(this);
	    iPointsMap[name] = point;
	    iPointsList.push_back(point);

	    iPointsRef.PushTable();
	    lua.PushString(point->Name());
	    point->PushTable();
	    lua.EndTable();
	    lua.Pop(1);
	}
    }

    return point;
}

void CMap::DoCalc() {
    if (! Ready()) {
	Calc();
	Ready(true);

	list<CTransform*>::iterator transform = iTransform.begin(); 
	
	while (transform != iTransform.end()) {
	    (*transform)->Do(this);
	    transform++;
	}
    } else {
	list<CPoint*>::iterator point = iPointsList.begin();

	while (point != iPointsList.end()) {
	    (*point)->Touch();
	    point++;
	}
    }
}

void CMap::Calc() {
    list<CMap*>::iterator i;
    
    for (i = iDepends.begin(); i != iDepends.end(); i++) {
	(*i)->Ready(false);
	(*i)->DoCalc();
    }
}

static int PointCompare(const void *a, const void *b) {
    CPoint	**A = (CPoint**) a;
    CPoint	**B = (CPoint**) b;
    
    if ((*A)->Projected()->Lon() < (*B)->Projected()->Lon()) {
	return -1;
    } else {
	return 1;
    }
}

void CMap::Draw(CDrawArea* area) {
    list<CPoint*>::iterator 	point;
    CPoint**			sortList = NULL;
    unsigned int		i = 0, listSize = 0, maxSize = 0;

    for (point = iPointsList.begin(); point != iPointsList.end(); point++) 
	if ((*point)->Visible() && (*point)->DrawWidth()) {
	    listSize++;
	    sortList = (CPoint**) realloc(sortList, listSize * sizeof(CPoint*));
	    sortList[listSize-1] = *point;
	    
	    (*point)->DrawShift(0);
	    
	    if ((*point)->DrawWidth() > maxSize) 
		maxSize = (*point)->DrawWidth();
	}

    qsort(sortList, listSize, sizeof(CPoint*), PointCompare);
    
    float maxDelta = atan((float) maxSize / (float) area->iSize) / PI * 180.0;

    for (i = 1; i < listSize; i++) {
	float delta;
	
	delta = sortList[i]->Projected()->Lon() - (sortList[i-1]->Projected()->Lon() + sortList[i-1]->DrawShift());
	
	if (delta < maxDelta) sortList[i]->DrawShift(maxDelta - delta);
    }
    
    free(sortList);

    for (point = iPointsList.begin(); point != iPointsList.end(); point++)
	if ((*point)->Visible()) (*point)->Draw(area);
}

void CMap::DrawPoints(CDrawArea* area) {
    list<CPoint*>::iterator point;

    for (point = iPointsList.begin(); point != iPointsList.end(); point++)
	if ((*point)->Visible()) (*point)->DrawPoint(area);
}

void CMap::DrawInfo(CDrawArea* area) {
    list<CPoint*>::iterator point;

    for (point = iPointsList.begin(); point != iPointsList.end(); point++) 
	if ((*point)->Info()) (*point)->DrawInfo(area);
}

void CMap::SetRotate(long int sec) {
    iRotateSec = sec;
}

void CMap::RotateForward() {
    Rotate(iRotateSec);
}

void CMap::RotateBack() {
    Rotate(-iRotateSec);
}

void CMap::Event(CAstro* obj, eventEnum event, void* data) {
    if (iEditor) iEditor->Event(obj, event, data);
}

void CMap::Lua() {
    LuaFunction("INFO");
}

bool CMap::LuaSet(const char *var) {
    if (strcmp(var, "info") == 0) {
        Info(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "id") == 0) {
        Id(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "width") == 0) {
        iWidth = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "color") == 0) {
	iColor = fltk::color(lua.ToString(-1));
	return true;
    }
    
    return false;
}

bool CMap::LuaGet(const char* var) {
    if (strcmp(var, "info") == 0) {
	lua.PushString(iInfo);
	return true;
    } else if (strcmp(var, "id") == 0) {
	lua.PushString(iId);
	return true;
    }
    return false;
}
