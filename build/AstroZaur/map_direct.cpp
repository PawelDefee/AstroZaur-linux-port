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

#include "utils.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "program.h"
#include "map_direct.h"

extern "C" {
    #include <sweph/swephexp.h>
}

// CMapZodiacDirect //

CMapZodiacDirect::CMapZodiacDirect() {
    Type("zodiac direct");
    Info(iType);
}

void CMapZodiacDirect::Calc() {
    double k = iK1/iK2;
    double delta = ((JD() - iParent->JD())/365.2422) * k;
    list<CPoint*>::iterator from;
    list<CPoint*>::iterator to;
    
    from = iParent->iPointsList.begin();
    for (to = iPointsList.begin(); to != iPointsList.end(); to++, from++) {
	(*to)->Ready(false);
	(*to)->Ecliptic()->Lon((*from)->Ecliptic()->Lon() + delta);
	(*to)->LonSpeed(k);
    }
}

// CMapEquatorDirect //

CMapEquatorDirect::CMapEquatorDirect() {
    Type("equator direct");
    Info(iType);
}

void CMapEquatorDirect::Calc() {
    double	k = iK1/iK2;
    double	delta = ((JD() - iParent->JD())/365.2422) * k;
    double	xobl[6];
    char	serr[AS_MAXCH];  
    list<CPoint*>::iterator from_point;
    list<CPoint*>::iterator to_point;
    
    swe_calc(JD(), SE_ECL_NUT, 0, xobl, serr);
    
    from_point = iParent->iPointsList.begin();
    for (to_point = iPointsList.begin(); to_point != iPointsList.end(); to_point++, from_point++) {
	double	from[3];
	double	to[3];
	
	from[0] = (*from_point)->Ecliptic()->Lon();
	from[1] = (*from_point)->Ecliptic()->Lat();
	from[2] = 1.0;
	
	swe_cotrans(from, to, -xobl[0]);
	to[0] += delta;
	to[2] = 1.0;
	swe_cotrans(to, from, xobl[0]);

	(*to_point)->Ready(false);
	(*to_point)->Ecliptic()->Lon(from[0]);
	(*to_point)->Ecliptic()->Lat(from[1]);
	(*to_point)->LonSpeed(k);
    }
}

// CMapTrueDirect //

CMapTrueDirect::CMapTrueDirect() {
    Type("true direct");
    Info(iType);
}

double CMapTrueDirect::JD() {
    double parent_jd = iParent->JD();
    
    iDelta = ((CMapNatal::JD() - parent_jd)/365.2422) * (iK1/iK2);
    return parent_jd + iDelta / 359.017043904 /* 360.98564736629 */;
}

void CMapTrueDirect::Calc() {
    double	xobl[6];
    char	serr[AS_MAXCH];  
    double	k = - (iK1/iK2) / 365.2422;
    list<CPoint*>::iterator point;
    
    CMapDevelop::Calc();
    
    swe_calc(JD(), SE_ECL_NUT, 0, xobl, serr);
    
    for (point = iPointsList.begin(); point != iPointsList.end(); point++) {
	double	from[3];
	double	to[3];
	
	from[0] = (*point)->Ecliptic()->Lon();
	from[1] = (*point)->Ecliptic()->Lat();
	from[2] = 1.0;
	
	swe_cotrans(from, to, -xobl[0]);
	to[0] -= iDelta;
	to[2] = 1.0;
	swe_cotrans(to, from, xobl[0]);

	(*point)->Ready(true);
	(*point)->Ecliptic()->Lon(from[0]);
	(*point)->Ecliptic()->Lat(from[1]);
	(*point)->LonSpeed((*point)->LonSpeed() * k);
    }
}

// CMapArcDirect

CMapArcDirect::CMapArcDirect() {
    Type("arc direct");
    Info(iType);
    iPoint = NULL;
}

void CMapArcDirect::Calc() {
    if (iPoint == NULL) return;
    
    double k = iK1/iK2;
    double days = ((JD() - iParent->JD())/365.2422) * k;
    double lon0, delta;
    
    iPoint->Calc(iParent->JD());
    lon0 = iPoint->Ecliptic()->Lon();

    iPoint->Calc(iParent->JD() + days);
    delta = iPoint->Ecliptic()->Lon() - lon0;
    
    list<CPoint*>::iterator from;
    list<CPoint*>::iterator to;
    
    from = iParent->iPointsList.begin();
    for (to = iPointsList.begin(); to != iPointsList.end(); to++, from++) {
	(*to)->Ready(false);
	(*to)->Ecliptic()->Lon((*from)->Ecliptic()->Lon() + delta);
	(*to)->LonSpeed(k);
    }
}

void CMapArcDirect::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    while (subNode) {
	char* name = (char *) subNode->name;
    
	if (!strcmp(name,"point")) {
	    if (has_content(subNode))
		iPoint = iPointsMap[(char*) subNode->children->content];
	    goto next;
	}
	
next:	subNode = subNode->next;
    }

    CMapDevelop::XML(node);
}

bool CMapArcDirect::LuaSet(const char* var) {
    if (strcmp(var, "arc") == 0) {
	if (lua.Type(-1) == LUA_TTABLE) {
	    lua.PushString(".ptr_point");
	    lua.GetTable(-2);

	    if (lua.Type(-1) == LUA_TLIGHTUSERDATA) {
		iPoint = (CPoint*) lua.ToUser(-1);
	    }
		
	    lua.Pop(1);
	}
	return true;
    }

    return CMapDevelop::LuaSet(var);
}

bool CMapArcDirect::LuaGet(const char* var) {
    return CMapDevelop::LuaGet(var);
}
