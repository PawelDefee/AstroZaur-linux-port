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
#include "map_ingres.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

CMapIngres::CMapIngres() {
    Type("ingres");
    Info(iType);
    iPoint = NULL;
    iBackward = false;
    iPrevJD = 0.0;
    iLon = NULL;
    iLonNum = 0;
}

void CMapIngres::XML(xmlNodePtr node) {
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
		
	    if (name) {
		iPoint = iPointsMap[name];
	    }
		
	    if (name) xmlFree(name);

	    goto next;
	}

	if (!strcmp(name,"backward")) {
	    iBackward = true;
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

    CMapNatal::XML(node);
}

double CMapIngres::JD() {
    double jd, pos, to_pos, dist, delta;

    jd = Date()->JD();

    if (jd == iPrevJD) return iJD;
    
    iJD = iPrevJD = jd;
    
    iPoint->Calc(iJD);
    pos = iPoint->Ecliptic()->Lon();
    
    unsigned int i;
    
    for (i = 0; i < iLonNum; i++) {
	if (pos < iLon[i]) break;
    }
	
    if (iBackward && i != 0) i--;
    
    to_pos = iLon[i];
    int loop = 5;
    
    while(loop-- > 0) {
	dist = Dist(pos, to_pos);
	delta = dist / iPoint->LonSpeed();
	
	if (fabs(delta) < 1.0/(24.0*60.0*60.0)) break;
	
	iJD += delta;
	iPoint->Calc(iJD);
	pos = iPoint->Ecliptic()->Lon();
    }
    iMoment.JD(iJD);
    return iJD;
}

void CMapIngres::Lua() {
    CMapNatal::Lua();
    
    lua.NewTable("moment");
    iMoment.Lua();
    lua.EndTable();
}
