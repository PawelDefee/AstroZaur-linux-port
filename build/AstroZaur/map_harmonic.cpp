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
#include "program.h"
#include "map_harmonic.h"

CMapHarmonic::CMapHarmonic() {
    Type("harmonic");
    Info(iType);
    iN = 1.0;
}

void CMapHarmonic::Calc() {
    list<CPoint*>::iterator p;
    
    CMapNatal::Calc();
    
    for (p = iPointsList.begin(); p != iPointsList.end(); p++) {
	(*p)->Ready(false);
	(*p)->Ecliptic()->Lon((*p)->Ecliptic()->Lon() * iN);
	(*p)->LonSpeed((*p)->LonSpeed() * iN);
    }
}

void CMapHarmonic::XML(xmlNodePtr node) {
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
    
	if (!strcmp(name,"n")) {
	    if (has_content(subNode))
		iN = atof((char *) subNode->children->content);
	    goto next;
	}

next:	subNode = subNode->next;
    }

    CMapNatal::XML(node);
}
