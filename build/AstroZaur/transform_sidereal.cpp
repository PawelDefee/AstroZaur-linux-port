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

#include "date.h"
#include "transform_sidereal.h"

CTransformSidereal::CTransformSidereal() {
    iSize = 0;
    iStartJD = 0;
    iSpeed = 0;
}

void CTransformSidereal::Do(CMap* map) {
    list<CPoint*>::iterator p;
    double	delta;
    
    delta = iSize + ((map->JD() - iStartJD) * iSpeed) / 365.25/3600.0;
    
    for (p = map->iPointsList.begin(); p != map->iPointsList.end(); p++) {
	(*p)->Ready(false);
	(*p)->Ecliptic()->Lon((*p)->Ecliptic()->Lon() - delta);
    }
}

void CTransformSidereal::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;
    
    while (subNode) {
    	char* name = (char *) subNode->name;

	if (!strcmp(name,"date") && has_content(subNode)) {
	    CDate* date = new CDate;
	    
	    date->XML(subNode);
	    iStartJD = date->JD();
	    
	    delete date;
	
	    goto next;
	}

	if (!strcmp(name,"size") && has_content(subNode)) {
	    iSize = atof((char *) subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"speed") && has_content(subNode)) {
	    iSpeed = atof((char *) subNode->children->content);
	    goto next;
	}

next:	subNode = subNode->next;
    }
}
