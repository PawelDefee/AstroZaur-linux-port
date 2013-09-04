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
#include "map_eqlipse.h"
#include "map_eqlipse_edit.h"
#include "program.h"

extern "C" {
    #include <sweph/swephexp.h>
}

CMapEqlipse::CMapEqlipse() {
    Type("eqlipse");
    iBackward = false;
    iLunar = false;
    iPrevJD = 0;
}

void CMapEqlipse::XML(xmlNodePtr node) {
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
    
	if (!strcmp(name,"lunar")) {
	    iLunar = true;
	    goto next;
	}

	if (!strcmp(name,"backward")) { 
	    iBackward = true;
	    goto next;
	}
    
next:	subNode = subNode->next;
    }

    CMapDevelop::XML(node);
}

char* CMapEqlipse::MoreInfo() {
    char *res = (char *) malloc(1024);
    char *date = iMoment.Date();
    char *time = iMoment.Time();
    char type[64] = "";

    if (iType & SE_ECL_CENTRAL)		strcat(type, "Central ");
    if (iType & SE_ECL_NONCENTRAL)	strcat(type, "Noncentral ");
    if (iType & SE_ECL_TOTAL)		strcat(type, "Total ");
    if (iType & SE_ECL_ANNULAR)		strcat(type, "Annular ");
    if (iType & SE_ECL_PARTIAL)		strcat(type, "Partial ");
    if (iType & SE_ECL_ANNULAR_TOTAL)	strcat(type, "Annular Total ");
    if (iType & SE_ECL_PENUMBRAL)	strcat(type, "Penumbral ");

    sprintf(res,"%s%s %s", type, date, time);
	
    free(date);
    free(time);

    return res;
}

double CMapEqlipse::JD() {
    double		jd;
    int32		iflag = 0;
    double		res[10], moment;
    char		serr[AS_MAXCH];
    double		prevMoment = iMoment.JD();

    if (iParent) {
	jd = iParent->JD();
    } else {
	jd = Date()->JD();
    }

    if (iPrevJD == jd) return prevMoment;

    if (iLunar) iType = swe_lun_eclipse_when(jd, iflag, 0, res, iBackward, serr);
	else iType = swe_sol_eclipse_when_glob(jd, iflag, 0, res, iBackward, serr);
	
    if (iType == ERR) {
    	printf("CSwiss error %s\n",serr);
	moment = jd;
    } else {
	moment = res[0];
    }
    iPrevJD = jd;
    iMoment.JD(moment);

    return iMoment.JD();
}

CMapEdit* CMapEqlipse::MakeEditor() {
    CMapNatalEdit* edit = new CMapEqlipseEdit();
    edit->Map(this);
    
    return edit;
}

bool CMapEqlipse::Backward() {
    return iBackward;
}

bool CMapEqlipse::Lunar() {
    return iLunar;
}

void CMapEqlipse::Backward(bool data) {
    iBackward = data;
    iPrevJD = 0;
    Ready(false);
}

void CMapEqlipse::Lunar(bool data) {
    iLunar = data;
    iPrevJD = 0;
    Ready(false);
}
