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

#include <math.h>
#include "point.h"
#include "utils.h"
#include "draw_area.h"
#include "aspect_eql_lon.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

#include <fltk/draw.h>
#include <fltk/Color.h>

static char	dash[] =	{5,6,0};
static char 	dot[] = 	{1,6,0};
static char 	dashdot[] =	{5,6,1,6,0};
static char 	dashdotdot[] =	{7,6,1,6,1,6,0};

// CAspectEqlLonData

void CAspectEqlLonData::Draw(CDrawArea* area) {
    int x1, y1, x2, y2;
    
    area->GetXY(iPoint1->Projected()->Lon(), area->iSize, &x1, &y1);
    area->GetXY(iPoint2->Projected()->Lon(), area->iSize, &x2, &y2);

    fltk::setcolor(iLineColor);
    fltk::line_style(fltk::SOLID | fltk::CAP_ROUND, iAction == separate ? 1:3, iLineStyle);
    fltk::drawline(x1, y1, x2, y2);
}

// CAspectEqlLonItem

CAspectEqlLonItem::CAspectEqlLonItem() {
    iAngle = 0;
    iView = true;
    iLineStyle = NULL;
    iName = NULL;
}

void CAspectEqlLonItem::Name(const char* name) {
    if (iName) free(iName);
    iName = strdup(name);
}

void CAspectEqlLonItem::LineStyle(const char* style) {
    if (strcmp(style, "solid") == 0) {
	iLineStyle = NULL;
    } else if (strcmp(style, "dash") == 0) {
	iLineStyle = dash;
    } else if (strcmp(style, "dot") == 0) { 
	iLineStyle = dot;
    } else if (strcmp(style, "dashdot") == 0) {
	iLineStyle = dashdot;
    } else if (strcmp(style, "dashdotdot") == 0) {
	iLineStyle = dashdotdot;
    }
}

bool CAspectEqlLonItem::LuaSet(const char* var) {
    if (strcmp(var, "angle") == 0) {
	iAngle = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "view") == 0) {
	iView = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "orbis") == 0 && lua.Type(-1) == LUA_TTABLE) {
	iOrb.clear();
	lua.PushNil();
	
	while (lua.Next(-2)) {
	    const char* id = lua.ToString(-2);
	    double	orbis = lua.ToNumber(-1);

	    iOrb[Hash(id)] = orbis;
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "line_style") == 0) {
	LineStyle(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "color") == 0 && lua.Type(-1) == LUA_TTABLE ) {
	iLineColor.clear();
	lua.PushNil();
	
	while (lua.Next(-2)) {
	    double	part = lua.ToNumber(-2);
	    const char*	color = lua.ToString(-1);

	    iLineColor[part] = fltk::color(color);
	    lua.Pop(1);
	}
	return true;
    }
    
    return false;
}

void CAspectEqlLonItem::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    if (subNode->content) { // Need name!
	string name = (char *) subNode->content;
	
	name = leadcut(name);
	name = trailcut(name);

	Name(name.c_str());
    
	while(subNode) {
	    char* name = (char *) subNode->name;

	    if (!strcmp(name,"color")) {
		if (has_content(subNode)) {
		    char* xmlPart = (char *)xmlGetProp(subNode,(xmlChar *) "part");
		
		    if (xmlPart) {
			double part = atof(xmlPart);
		    
			xmlFree(xmlPart);
		    
			iLineColor[part] = fltk::color((char *) subNode->children->content);
		    }
		}
		goto next;
	    }
	    
	    if (!strcmp(name,"line_style")) {
		if (has_content(subNode)) {
		    string style = (char *) subNode->children->content;
		    
		    style = leadcut(style);
		    style = trailcut(style);
		    
		    LineStyle(style.c_str());
		}
		goto next;
	    }
	    
	    if (!strcmp(name,"angle")) {
		if (has_content(subNode)) {
		    string angle = (char *) subNode->children->content;
		
		    angle = leadcut(angle);
		    angle = trailcut(angle);
		    iAngle = atof(angle.c_str());
		}
		goto next;
	    }

	    if (!strcmp(name,"view")) {
		if (has_content(subNode))
		    iView = strcmp((char*)subNode->children->content,"true") == 0;
		goto next;
	    }

	    if (!strcmp(name,"orbis")) {
		if (has_content(subNode)) {
		    char* xmlId = (char *)xmlGetProp(subNode,(xmlChar *) "id");
		
		    if (xmlId) {
			string id = xmlId;
			xmlFree(xmlId);

			string orbis = (char *) subNode->children->content;
		
			orbis = leadcut(orbis);
			orbis = trailcut(orbis);
			iOrb[Hash(id.c_str())] = atof(orbis.c_str());
		    }
		}
	    }
next:	    subNode = subNode->next;
	}
    }
}

// CAspectEqlLon - main aspect class

static int l_lon(lua_State *L) {
    CAspectEqlLon* 	aspect = (CAspectEqlLon*) lua.GetObj();
    const char*		name = lua.ToString(1);

    CAspectEqlLonItem*	item = aspect->NewLon();

    item->Name(name);
    item->PushTable();

    return 1;
}

CAspectEqlLon::CAspectEqlLon() {
    iPoint1Fixed = false;
    iPoint2Fixed = false;

    PushTable();
    lua.Method("lon", l_lon, this);
    lua.Pop(1);
}

const unsigned int CAspectEqlLon::OrbId(CPoint* p1, CPoint* p2) {
    unsigned int 	res;
    char		str[64];

    strcpy(str, "cat:");
    strcat(str, p1->Category());
    strcat(str, "cat:");
    strcat(str, p2->Category());
    
    res = iOrbId[Hash(str)];
    if (res) return res;

    strcpy(str, "cat:");
    strcat(str, p2->Category());
    strcat(str, "cat:");
    strcat(str, p1->Category());

    res = iOrbId[Hash(str)];
    if (res) return res;

    strcpy(str, "cat:");
    strcat(str, p1->Category());

    res = iOrbId[Hash(str)];
    if (res) return res;

    strcpy(str, "cat:");
    strcat(str, p2->Category());

    res = iOrbId[Hash(str)];
    if (res) return res;

    strcpy(str, p1->Name());
    strcat(str, p2->Name());

    res = iOrbId[Hash(str)];
    if (res) return res;
    
    strcpy(str, p2->Name());
    strcat(str, p1->Name());

    res = iOrbId[Hash(str)];
    if (res) return res;

    res = iOrbId[Hash(p1->Name())];
    if (res) return res;

    res = iOrbId[Hash(p2->Name())];
    if (res) return res;

    res = iOrbId[Hash("*")];
    return res;
}

CAspectData* CAspectEqlLon::Data(CPoint* point1, CPoint *point2) {
    if (Exclude(point1, point2)) return NULL;

    CAspectEqlLonData* data = new CAspectEqlLonData;

    data->iOrbId = OrbId(point1, point2);
    
    return data;
}

void CAspectEqlLon::Calc(CAspectData* data) {
    CAspectEqlLonData*	iData = (CAspectEqlLonData*)data;
    CPoint* 		point1 = iData->iPoint1;
    CPoint*		point2 = iData->iPoint2;
    double		res;

    res = Dist(point1->Ecliptic()->Lon(), point2->Ecliptic()->Lon());

    list<CAspectEqlLonItem*>::iterator item;
    
    for (item = iItem.begin(); item != iItem.end(); item++) {
	if ((*item)->iView) {
	    double itemOrb = (*item)->iOrb[iData->iOrbId];

	    if (itemOrb == 0) continue;
	    
	    double aspectOrb = fabs(fabs(res) - (*item)->iAngle);

	    if (aspectOrb < itemOrb) {
		// Calc separation/application
		double a = (*item)->iAngle;
		double v;
		
		v = (iPoint1Fixed ? 0 : point1->LonSpeed()) - 
		    (iPoint2Fixed ? 0 : point2->LonSpeed());
		
		if (res < 0) a = -a;
		iData->iAction = (res >= a && v <= 0) || (res <= a && v > 0) ? separate:application;

		// Calc orbis part for color
		double	part = aspectOrb/itemOrb;
		map<double, fltk::Color>::iterator parts;
	
		for (parts = (*item)->iLineColor.begin(); parts != (*item)->iLineColor.end(); parts++)
		    if (part < parts->first) {
			iData->iLineColor = parts->second;
			break;
		    }
		
		// Copy other info
		iData->Name((*item)->iName);
		iData->iLineStyle = (*item)->iLineStyle;
		iData->iOrb = aspectOrb;

		iData->Active(true);
		point1->HasAspected(data);
		point2->HasAspected(data);

		iData->PushTable();
		lua.SetField("applicate", iData->iAction == application);
		lua.SetField("orbis", iData->iOrb);
		lua.Pop(1);

		break;
	    }
	}
    }
}

CAspectEqlLonItem* CAspectEqlLon::NewLon() {
    CAspectEqlLonItem* item = new CAspectEqlLonItem;
    iItem.push_back(item);
    
    return item;
}

bool CAspectEqlLon::LuaSet(const char* var) {
    if (strcmp(var, "orbises") == 0 && lua.Type(-1) == LUA_TTABLE) {
	iOrbId.clear();
	lua.PushNil();

	while (lua.Next(-2)) {
	    if (lua.Type(-1) == LUA_TSTRING) {
		const char*	point = lua.ToString(-2);
		const char*	id = lua.ToString(-1);

		iOrbId[Hash(point)] = Hash(id);
	    } else if (lua.Type(-1) == LUA_TTABLE) {
		const char*	point1 = lua.ToString(-2);
		
		lua.PushNil();

		while (lua.Next(-2)) {
		    const char*	point2 = lua.ToString(-2);
		    const char*	id = lua.ToString(-1);
		    char	points[128];
		    
		    strcpy(points, point1);
		    strcat(points, point2);
		    
		    iOrbId[Hash(points)] = Hash(id);
		    lua.Pop(1);
		}
	    }
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "point1_fixed") == 0) {
	iPoint1Fixed = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "point2_fixed") == 0) {
	iPoint2Fixed = lua.ToBool(-1);
	return true;
    }

    return CAspect::LuaSet(var);
}

void CAspectEqlLon::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;
    
    CAspect::XML(node);
    
    while (subNode) {
	char* name = (char *) subNode->name;
	    
	if (!strcmp(name,"lon")) {
	    CAspectEqlLonItem* item = NewLon();
	    item->XML(subNode->children);
	}
	
	if (!strcmp(name,"orbises")) {
	    XMLOrbises(subNode->children);
	}

	if (!strcmp(name,"point1_fixed")) {
	    iPoint1Fixed = true;
	}
	if (!strcmp(name,"point2_fixed")) {
	    iPoint2Fixed = true;
	}
	
	subNode = subNode->next;
    }
}

void CAspectEqlLon::XMLOrbises(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    char*	xmlDefId = (char *) subNode->content;
    char	point[128];
    string	id;

    FreeOrbId();	
    if (xmlDefId) {
	id = xmlDefId;
	id = leadcut(id);
	id = trailcut(id);
	iOrbId[Hash("*")] = Hash(id.c_str());
    }

    while (subNode) {
	char* name = (char *)subNode->name;
    
	if (!strcmp(name,"point") && has_content(subNode)) {
	    xmlChar* xmlPoint = xmlGetProp(subNode,(xmlChar *) "name");
	    
	    if (xmlPoint) {
		strcpy(point, (char *)xmlPoint);
		xmlFree(xmlPoint);
	    } else {
		xmlPoint = xmlGetProp(subNode,(xmlChar *) "category");
		
		if (xmlPoint) {
		    strcpy(point, "cat:");
		    strcat(point, (char *)xmlPoint);
		    xmlFree(xmlPoint);
		}
	    }
		    
	    id = (char *) subNode->children->content;
	    id = leadcut(id);
	    id = trailcut(id);
	    if (!id.empty()) iOrbId[Hash(point)] = Hash(id.c_str());
		    
	    if (subNode->children) {
		xmlNodePtr orbisSubNode = subNode->children;
			
		while (orbisSubNode) {
		    if (!strcmp((char*)orbisSubNode->name,"point")) {
			char subPoint[128];
			xmlPoint = xmlGetProp(orbisSubNode,(xmlChar *) "name");

			if (xmlPoint) {
			    strcpy(subPoint, (char *)xmlPoint);
			    xmlFree(xmlPoint);
			} else {
			    xmlPoint = xmlGetProp(subNode,(xmlChar *) "category");
		
			    if (xmlPoint) {
				strcpy(subPoint, "cat:");
				strcat(subPoint, (char *) xmlPoint);
				xmlFree(xmlPoint);
			    }
			}
				
	    		id = (char *) orbisSubNode->children->content;
			id = leadcut(id);
			id = trailcut(id);

			if (!id.empty()) {
			    strcat(point, subPoint);
			    iOrbId[Hash(point)] = Hash(id.c_str());
			}
		    }
		    orbisSubNode = orbisSubNode->next;
		}
	    }
	}
	subNode = subNode->next;
    }
}

void CAspectEqlLon::FreeOrbId() {
    iOrbId.clear();
}
