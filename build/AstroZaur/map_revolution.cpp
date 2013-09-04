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
#include "utils.h"
#include "program.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "map_revolution.h"

CMapRevolution::CMapRevolution() {
    Type("revolution");
    Info(iType);
    iFixed = NULL;
    iMoved = NULL;
    iBackward = false;
    iPrevJD = 0.0;
    iPrevFixed = -1.0;
}

void CMapRevolution::XML(xmlNodePtr node) {
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
	char* name = (char *) subNode->name;
    
	if (!strcmp(name,"point")) {
	    char* name = (char*)xmlGetProp(subNode,(xmlChar *) "name");
	    char* map = (char*)xmlGetProp(subNode,(xmlChar *) "chart");
		
	    if (name && map) {
		program.Map(map)->AddDepends(this);
		iFixed = program.Map(map)->iPointsMap[name];
		iMoved = iPointsMap[name];
	    }
		
	    if (name) xmlFree(name);
	    if (map) xmlFree(map);

	    goto next;
	}

	if (!strcmp(name,"backward")) {
	    iBackward = true;
	    goto next;
	}

next:	subNode = subNode->next;
    }

    CMapNatal::XML(node);
}

double CMapRevolution::JD() {
    double jd, fixed, moved, dist;

    jd = Date()->JD();
    fixed = iFixed->Ecliptic()->Lon();
    if (jd == iPrevJD && fixed == iPrevFixed) return iJD;
    
    iJD = iPrevJD = jd;
    iPrevFixed = fixed;
    
    iMoved->Calc(iJD);
    moved = iMoved->Ecliptic()->Lon();

    dist = Dist(moved, fixed);
    if (iBackward) dist = -dist;
    if (dist < 0.0) dist += 360.0;

    dist = dist / iMoved->LonSpeed();
    if (iBackward) dist = -dist;
    iJD += dist;
    
    while(1) {
	iMoved->Calc(iJD);
	moved = iMoved->Ecliptic()->Lon();

	dist = Dist(moved, fixed);

	if (fabs(dist) < 1.0/3600.0) break;
	iJD += Dist(moved, fixed) / iMoved->LonSpeed();
    }
    return iJD;
}

bool CMapRevolution::LuaSet(const char* var) {
    if (strcmp(var, "revolution") == 0) {
	CPoint *point = NULL;
	
	if (lua.Type(-1) == LUA_TTABLE) {
	    lua.PushString(".ptr_point");
	    lua.GetTable(-2);

	    if (lua.Type(-1) == LUA_TLIGHTUSERDATA)
		point = (CPoint*) lua.ToUser(-1);
		
	    lua.Pop(1);
	}
	
	if (point) {
	    point->Map()->AddDepends(this);
	    iFixed = point;
	    iMoved = iPointsMap[point->Name()];
	}
	
	return true;
    } else if (strcmp(var, "backward") == 0) {
	iBackward = lua.ToBool(-1);
	return true;
    }

    return CMapNatal::LuaSet(var);
}

bool CMapRevolution::LuaGet(const char* var) {
    return CMapNatal::LuaGet(var);
}
