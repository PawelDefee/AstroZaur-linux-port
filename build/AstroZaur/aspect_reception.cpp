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

#include "aspect_reception.h"
#include "point.h"
#include "draw_area.h"
#include "line.h"
#include "map.h"
#include "utils.h"
#include "division.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

#include <fltk/Color.h>
#include <fltk/draw.h>
#include <math.h>

void CAspectReceptionData::Draw(CDrawArea* area) {
    if (iLine) {
	iLine->DrawCurve(iPoint1->Projected()->Lon(), iPoint2->Projected()->Lon(), area);
    }
}

// CAspectReception

CAspectReception::CAspectReception() {
    iOneView = false;
    iOneLine = new CLine;
    iOneLine->Curve(8.0);

    iBothView = false;
    iBothLine = new CLine;
    iBothLine->Curve(8.0);
    
    PushTable();

    lua.PushString("one_line");
    iOneLine->PushTable();
    lua.EndTable();

    lua.PushString("both_line");
    iBothLine->PushTable();
    lua.EndTable();
    
    lua.Pop(1);
}

CAspectReception::~CAspectReception() {
    if (iOneLine) delete iOneLine;
    if (iBothLine) delete iBothLine;
}
    
CAspectData* CAspectReception::Data(CPoint* point1, CPoint *point2) {
    if (Exclude(point1, point2)) return NULL;

    return new CAspectReceptionData;
}

bool CAspectReception::Reception(CPoint* point1, CPoint* point2) {
    char*			data = NULL;
    list<string>::iterator	item = iDivision.begin();

    while (item != iDivision.end()) {
	if (division.Get(point1->Ecliptic()->Lon(),item->c_str(), &data)) {
	    if (!strcmp(point2->Name(),data)) return true;
	}
	item++;
    }
    return false;
}

void CAspectReception::Calc(CAspectData* data) {
    CAspectReceptionData*	iData = (CAspectReceptionData*) data;
    bool			Ok1 = Reception(data->Point1(), data->Point2());
    bool			Ok2 = Reception(data->Point2(), data->Point1());
    
    if (Ok1 && Ok2 && iBothView) {
	data->Name("<->");
	data->Active(true);
	iData->iLine = iBothLine;
	return;
    } else if (Ok1 && iOneView) {
	data->Name("->");
	data->Active(true);
	iData->iLine = iOneLine;
	return;
    } else if (Ok2 && iOneView) {
	data->Name("<-");
	data->Active(true);
	iData->iLine = iOneLine;
	return;
    }
    
    data->Active(false);
}

void CAspectReception::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    CAspect::XML(node);
    
    while (subNode) {
	char* name = (char *) subNode->name;
	    
	if (!strcmp(name,"division")) { 
	    if (has_content(subNode)) {
		string text = (char *) subNode->children->content;
		
		text = leadcut(text);
		text = trailcut(text);
		
		iDivision.push_back(text);
	    }
	    goto next;
	}

	if (!strcmp(name,"one")) { 
	    if (has_content(subNode)) {
		xmlNodePtr node = subNode->children;

		while (node) {
		    if (!strcmp((char*)node->name,"line")) {
			iOneLine->XML(node->children);
			iOneView = true;
		    }
		    node = node->next;
		}
	    }
	    goto next;
	}

	if (!strcmp(name,"both")) { 
	    if (has_content(subNode)) {
		xmlNodePtr node = subNode->children;
		
		while (node) {
		    if (!strcmp((char*)node->name,"line")) {
			iBothLine->XML(node->children);
			iBothView = true;
		    }
		    node = node->next;
		}
	    }
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

bool CAspectReception::LuaSet(const char* var) {
    if (strcmp(var, "division") == 0 && lua.Type(-1) == LUA_TTABLE) {
	iDivision.clear();
	lua.PushNil();
	
	while (lua.Next(-2)) {
	    const char* text = lua.ToString(-1);

	    iDivision.push_back(text);
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "one_view") == 0) {
	iOneView = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "both_view") == 0) {
	iBothView = lua.ToBool(-1);
	return true;
    }

    return CAspect::LuaSet(var);
}
