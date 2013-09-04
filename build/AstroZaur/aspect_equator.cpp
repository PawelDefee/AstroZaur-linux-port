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

#include "aspect_equator.h"
#include "point.h"
#include "draw_area.h"
#include "line.h"
#include "map.h"
#include "utils.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

#include <fltk/Color.h>
#include <fltk/draw.h>
#include <math.h>

void CAspectEquatorData::Draw(CDrawArea* area) {
    if (iLine) {
	iLine->DrawCurve(iPoint1->Projected()->Lon(), iPoint2->Projected()->Lon(), area);
    }
}

//

CAspectEquator::CAspectEquator() {
    iOrb = 1.0;
    iParallel = new CLine;
    iContrParallel = new CLine;
    
    iParallel->Curve(6.0);
    iContrParallel->Curve(6.0);

    PushTable();

    lua.PushString("parallel_line");
    iParallel->PushTable();
    lua.EndTable();

    lua.PushString("contrparallel_line");
    iContrParallel->PushTable();
    lua.EndTable();
    
    lua.Pop(1);

}

CAspectEquator::~CAspectEquator() {
    delete iParallel;
    delete iContrParallel;
}

CAspectData* CAspectEquator::Data(CPoint* point1, CPoint *point2) {
    if (Exclude(point1, point2)) return NULL;
    
    return new CAspectEquatorData;
}

void CAspectEquator::Calc(CAspectData* data) {
    CAspectEquatorData*	iData = (CAspectEquatorData*) data;
    double		decl1 = data->Point1()->Equator()->Lat();
    double		decl2 = data->Point2()->Equator()->Lat();
    
    if (fabs(fabs(decl1) - fabs(decl2)) < iOrb) {
	if (decl1*decl2 > 0) {
	    iData->Name("Parallel");
	    iData->iLine = iParallel;
	} else {
	    data->Name("ContrParallel");
	    iData->iLine = iContrParallel;
	}
	data->Active(true);
    }
}


void CAspectEquator::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    CAspect::XML(node);
    
    while (subNode) {
	char* name = (char *) subNode->name;
	    
	if (!strcmp(name,"orbis")) { 
	    if (has_content(subNode)) {
		string orbis = (char *) subNode->children->content;
		
		orbis = leadcut(orbis);
		orbis = trailcut(orbis);
		iOrb = atof(orbis.c_str());
	    }
	    goto next;
	}

	if (!strcmp(name,"parallel")) { 
	    if (has_content(subNode)) {
		xmlNodePtr node = subNode->children;

		while (node) {
		    if (!strcmp((char*)node->name,"line"))
			iParallel->XML(node->children);
		    
		    node = node->next;
		}
	    }
	    goto next;
	}

	if (!strcmp(name,"contrparallel")) { 
	    if (has_content(subNode)) {
		xmlNodePtr node = subNode->children;
		
		while (node) {
		    if (!strcmp((char*)node->name,"line"))
			iContrParallel->XML(node->children);
		    
		    node = node->next;
		}
	    }
	    goto next;
	}
	
next:	subNode = subNode->next;
    }
}

bool CAspectEquator::LuaSet(const char* var) {
    if (strcmp(var, "orbis") == 0) {
	iOrb = lua.ToNumber(-1);
	return true;
    }
    
    return CAspect::LuaSet(var);
}
