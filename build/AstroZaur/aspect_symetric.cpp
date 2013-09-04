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

#include "aspect_symetric.h"
#include "point.h"
#include "draw_area.h"
#include "line.h"
#include "map.h"
#include "utils.h"
#include "program.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

#include <fltk/Color.h>
#include <fltk/draw.h>
#include <math.h>

void CAspectSymetricData::Draw(CDrawArea* area) {
    if (iLine) {
	iLine->DrawCurve(iPoint1->Projected()->Lon(), iPoint2->Projected()->Lon(), area);
    }
}

// CAspectSymetric

CAspectSymetric::CAspectSymetric() {
    iLine = new CLine;
    iLine->Curve(8.0);

    iOrbis = 1.0;
    iAxis = 0.0;
    iName = NULL;

    PushTable();

    lua.PushString("line");
    iLine->PushTable();
    lua.EndTable();
    
    lua.Pop(1);
}

CAspectSymetric::~CAspectSymetric() {
    if (iLine) delete iLine;
    if (iName) free(iName);
}
    
CAspectData* CAspectSymetric::Data(CPoint* point1, CPoint *point2) {
    if (Exclude(point1, point2)) return NULL;

    CAspectSymetricData* data = new CAspectSymetricData;
    data->iLine = iLine;
    data->Name(iName);

    return data;
}

void CAspectSymetric::Calc(CAspectData* data) {
    double			orb;
    
    orb = fabs(Dist(data->Point1()->Ecliptic()->Lon(), iAxis) - 
	    Dist(iAxis, data->Point2()->Ecliptic()->Lon()));

    data->Active(orb < iOrbis);
}

void CAspectSymetric::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    CAspect::XML(node);

    if (!subNode->content) return;

    string name = (char *) subNode->content;
	
    name = leadcut(name);
    name = trailcut(name);

    Name(name.c_str());
	
    while (subNode) {
	char* name = (char *) subNode->name;
	    
        if (!strcmp((char*)name, "line")) {
	    iLine->XML(subNode->children);
	    
	    goto next;
	}

        if (!strcmp((char*)name, "axis") && has_content(subNode)) {
	    iAxis = atof((char *) subNode->children->content);
	    
	    goto next;
	}

        if (!strcmp((char*)name, "orbis") && has_content(subNode)) {
	    iOrbis = atof((char *) subNode->children->content);
	    
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

void CAspectSymetric::Name(const char* name) {
    if (iName) free(iName);
    iName = strdup(name);
}

bool CAspectSymetric::LuaSet(const char* var) {
    if (strcmp(var, "axis") == 0) {
	iAxis = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "orbis") == 0) {
	iOrbis = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "name") == 0) {
	Name(lua.ToString(-1));
	return true;
    }

    return CAspect::LuaSet(var);
}
