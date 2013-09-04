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
#include "map_stationar.h"

CMapStationar::CMapStationar() {
    Type("stationar");
    Info(iType);
    iPoint = NULL;
    iBackward = false;
    iPrevJD = 0.0;
}

void CMapStationar::XML(xmlNodePtr node) {
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

next:	subNode = subNode->next;
    }

    CMapNatal::XML(node);
}

double CMapStationar::JD() {
    double 	jd, pos, speed, speed0, to_speed;
    double 	data[6];
    char	serr[AS_MAXCH];  


    jd = Date()->JD();
    if (jd == iPrevJD) return iJD;
    iJD = iPrevJD = jd;
    
    iPoint->Calc(iJD, SEFLG_SPEED | SEFLG_HELCTR);
    
    pos = iPoint->Ecliptic()->Lon();
    speed0 = speed = iPoint->LonSpeed();

    printf("===\n");    
    printf("JD:%f\tLon:%.2f\tSpeed:%f\n", iJD, pos, speed);

    int loop = 20;
    
    to_speed = speed > 0 ? -0.001 : 0.001;
    
    while(loop-- > 0) {
//	if ((abs(speed) < abs(to_speed)) && (speed*to_speed > 0)) break;

	iJD += 1.0;

	if (swe_calc_ut(iJD, 14, SEFLG_SPEED | SEFLG_HELCTR, data, serr) == ERR) {
	    printf("CSwiss error %s\n",serr);
	} else {
	    /*
	    iEcliptic.Lon(data[0]);
	    iEcliptic.Lat(data[1]);
	    iLonSpeed = data[3];
	    iLatSpeed = data[4];
	    */
	}
	
	iPoint->Calc(iJD, SEFLG_SPEED | SEFLG_HELCTR);
	pos = iPoint->Ecliptic()->Lon();
	speed = iPoint->LonSpeed();
	
	double d = speed-speed0;

	printf("JD:%f\tLon:%.2f\tSpeed:%f\t", 
	    iJD, pos, speed);

	printf("Lon:%.2f\tSpeed:%f\n", 
	    data[0], data[3]);
	
	speed0 = speed;
    }

    return iJD;
}
