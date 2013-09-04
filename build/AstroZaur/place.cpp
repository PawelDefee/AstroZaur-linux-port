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
#include <string.h>
#include "place.h"
#include "utils.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "program.h"

CPlace::CPlace() {
    iInfo = NULL;

    PushTable();
    lua.SetField(".ptr_place", this);
    lua.Pop(1);
}

CPlace::~CPlace() {
    if (iInfo) free(iInfo);
}

void CPlace::Info(const char *data) {
    if (iInfo) free(iInfo);
    iInfo = strdup(data);
    program.Event(this, eventChangePlaceInfo);
}

char* CPlace::Info() {
    return iInfo;
}

char* CPlace::LonStr() {
    char buf[32];
    int lon = iLon;
    
    if (lon > 180*3600) lon = - 360*3600 + lon;
    
    ToStr(lon,"EW",buf);
    return strdup(buf);
}

char* CPlace::LatStr() {
    char buf[32];
    
    ToStr(iLat,"NS",buf);
    return strdup(buf);
}

void CPlace::LonStr(const char *data) {
    FromStr(data, &iLon);
    Ready(false);
    program.Event(this, eventChangePlaceLon);
}

void CPlace::LatStr(const char *data) {
    FromStr(data, &iLat);
    Ready(false);
    program.Event(this, eventChangePlaceLat);
}

void CPlace::ToStr(long int coord, char *grad_char, char *str) {
    int c, g, m, s;
    
    c = abs(coord);
    g = c/3600;
    m = c%3600 / 60;
    s = c % 60;

    sprintf(str, "%i%c%s%i'%s%i\"",
	g, grad_char[coord > 0 ? 0:1],
	m < 10 ? "0":"",m,
	s < 10 ? "0":"",s);
}

void CPlace::FromStr(const char *str, long int *coord) {
    char	field[32];
    int		i = 0, f = 3600;
    long int	res = 0;
    int		sign = 1;

    while(1) {
	switch (*str) {
	    case 'S':
	    case 'W':
		sign = -1;
	    case 'N':
	    case 'E':
		f = 3600;
	    case 0:
		goto stop;
	    case '\'':
		f = 60;
		goto stop;
	    case '"':
		f = 1;
	    stop:
		if (i) {
		    field[i] = 0;
		    res = res + atof(field) * f;
		    i = 0;
		}
	    case ' ':
		break;
	    default:
		field[i++] = *str;
	}
	if (! *str) {
	    *coord = res * sign;
	    return;
	}
	str++;
    }
}

void CPlace::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    while (subNode) {
	if (!strcmp((char *)subNode->name,"place")) break;
	subNode = subNode->next;
    }

    if (subNode == NULL) return;
    
    xmlChar *lon = xmlGetProp(subNode,(xmlChar *) "lon");
    xmlChar *lat = xmlGetProp(subNode,(xmlChar *) "lat");
	
    if (lon) {
	LonStr((char *) lon);
	xmlFree(lon);
    }

    if (lat) {
	LatStr((char *) lat);
	xmlFree(lat);
    }

    if (has_content(subNode))
	Info((char *) subNode->children->content);
}

xmlNodePtr CPlace::XML(xmlNodePtr parent, int level) {
    xmlNodePtr node;

    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level)));
    node = xmlNewTextChild(parent, NULL, (xmlChar*)"place", (xmlChar*)Info());
    xmlNewProp(node, (xmlChar*)"lon", (xmlChar*)LonStr());
    xmlNewProp(node, (xmlChar*)"lat", (xmlChar*)LatStr());

    return node;
}

bool CPlace::LuaSet(const char* var) {
    if (strcmp(var, "info") == 0) {
	Info(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "lon") == 0) {
	LonStr(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "lat") == 0) {
	LatStr(lua.ToString(-1));
	return true;
    }
    return false;
}

bool CPlace::LuaGet(const char* var) {
    if (strcmp(var, "info") == 0) {
        lua.PushString(iInfo);
	return true;
    } else if (strcmp(var, "lon") == 0) {
	lua.PushNumber(Lon());
	return true;
    } else if (strcmp(var, "lat") == 0) {
	lua.PushNumber(Lat());
	return true;
    }
    return false;
}
