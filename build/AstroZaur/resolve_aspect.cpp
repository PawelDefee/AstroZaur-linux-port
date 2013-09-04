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
#include "resolve_aspect.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

CResolveAspect::CResolveAspect() {
    iPoint1 = NULL;
    iPoint2 = NULL;
    iPrevJD = 0.0;
    iLon = NULL;
    iLonNum = 0;
}

void CResolveAspect::Map(CMap* map) {
    iMap = map;
}

void CResolveAspect::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    while (subNode) {
	char* name = (char *) subNode->name;
    
	if (!strcmp(name,"point")) {
	    char* name = (char*)xmlGetProp(subNode,(xmlChar *) "name");
	    
	    if (name) {
		if (iPoint1 == NULL) {
		    iPoint1 = iMap->iPointsMap[name];
		} else {
		    iPoint2 = iMap->iPointsMap[name];
		}
	    }
		
	    if (name) xmlFree(name);

	    goto next;
	}

	if (!strcmp(name,"lon")) {
	    if (has_content(subNode)) {
		iLonNum++;
		iLon = (double*) realloc(iLon, sizeof(double) * iLonNum);
		iLon[iLonNum-1] = atof((char*) subNode->children->content);
	    }
		
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

double CResolveAspect::GetDist() {
    double dist = Dist(iPoint1->Ecliptic()->Lon(), iPoint2->Ecliptic()->Lon());
    
    if (dist < 0) dist += 360;
    
    return dist;
}

double CResolveAspect::GetSpeed() {
    return iPoint1->LonSpeed() - iPoint2->LonSpeed();
}

double CResolveAspect::JD(double jd) {
    double 		delta, dist, to_dist;
    unsigned int	i;
    int 		loop = 5;

    if (jd == iPrevJD) return iJD;
    
    iJD = iPrevJD = jd;

    iPoint1->Calc(iJD);
    iPoint2->Calc(iJD);
    dist = GetDist();
    
    for (i = 0; i < iLonNum; i++)
	if (dist < iLon[i]) break;
	
    to_dist = iLon[i];

    delta = (Dist(to_dist, dist)) / GetSpeed();
    iJD += delta;

    while (loop-- > 0) {
	iPoint1->Calc(iJD);
	iPoint2->Calc(iJD);
	
	dist = GetDist();
	delta = Dist(to_dist, dist) / GetSpeed();
	
	if (fabs(delta) < 1.0/(24.0*60.0*60.0)) break;
	
	iJD += delta;
    }

    iMoment.JD(iJD);
    
    iEcliptic1 = *(iPoint1->Ecliptic());
    iPoint1->Ready(false);
    iPoint1->Calc();

    iEcliptic2 = *(iPoint2->Ecliptic());
    iPoint2->Ready(false);
    iPoint2->Calc();
    
    return iJD;
}

CCoord*	CResolveAspect::Ecliptic1() {
    return &iEcliptic1;
}

CCoord*	CResolveAspect::Ecliptic2() {
    return &iEcliptic2;
}

void CResolveAspect::Lua() {
    lua.NewTable("moment");
    iMoment.Lua();
    lua.EndTable();
}
