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

#include "program.h"
#include "map_natal.h"
#include "map_natal_edit.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

extern "C" {
    #include <sweph/swephexp.h>
}

CMapNatal::CMapNatal() {
    Type("natal");

    iDate = new CDate;
    iDate->Owner(this);
    
    iPlace = new CPlace;
    iPlace->Owner(this);
    
    iHouseSys = 'P';
}

CMapNatal::~CMapNatal() {
    if (iDate && iDate->Owner() == this) delete iDate;
    if (iPlace && iPlace->Owner() == this) delete iPlace;
}

xmlNodePtr CMapNatal::XML(xmlNodePtr parent, int level) {
    xmlNodePtr	node, sub;
    char	str[16];
    CMapNatal*	natal;

    node = CMap::XML(parent, level);

    xmlAddChild(node, sub = iDate->XML(node, level+1));
    natal = (CMapNatal*) iDate->Owner();
    if (natal != this) xmlNewProp(sub, (xmlChar*)"clone", (xmlChar*)natal->IdInfo());
    
    xmlAddChild(node, sub = iPlace->XML(node, level+1));
    natal = (CMapNatal*) iPlace->Owner();
    if (natal != this) xmlNewProp(sub, (xmlChar*)"clone", (xmlChar*)natal->IdInfo());

    sprintf(str,"%i",HouseSys());
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    xmlNewTextChild(node, NULL, (xmlChar*)"house", (xmlChar*)str);

    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level)));
    
    return node;
}

void CMapNatal::XML(xmlNodePtr node) {
//    printf("CMapNatal::XML\n");

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
	char* name = (char*) subNode->name;
    
	if (!strcmp(name,"date")) { 
	    char* xmlInfo = (char*)xmlGetProp(subNode,(xmlChar *) "clone");
		
	    if (xmlInfo) {
		CMapNatal* natal = (CMapNatal*) program.Map((char*)xmlInfo);
		
		if (natal) {
		    Date(natal->Date());
		    natal->AddDepends(this);
		}
		xmlFree(xmlInfo);
	    } else {
		if (has_content(subNode)) iDate->XML(subNode);
	    }
	    goto next;
	}

	if (!strcmp(name,"time")) {
	    char* xmlInfo = (char*)xmlGetProp(subNode,(xmlChar *) "clone");
	    
	    if (xmlInfo) {
		CMapNatal* natal = (CMapNatal*) program.Map((char*)xmlInfo);
		
		if (natal) {
		    Date(natal->Date());
		    natal->AddDepends(this);
		}
		xmlFree(xmlInfo);
	    } else {
		iDate->XML(subNode);
	    }
	    goto next;
	}

	if (!strcmp(name,"place")) {
	    char* xmlInfo = (char*)xmlGetProp(subNode,(xmlChar *) "clone");
		
	    if (xmlInfo) {
		CMapNatal* natal = (CMapNatal*) program.Map((char*)xmlInfo);
		
		if (natal) {
		    Place(natal->Place());
		    natal->AddDepends(this);
		}
		xmlFree(xmlInfo);
	    } else {
		iPlace->XML(subNode);
	    }
	    goto next;
	}

	if (!strcmp(name,"house") && has_content(subNode)) {
	    HouseSys(atoi((char*)subNode->children->content));
	    goto next;
	}

next:	subNode = subNode->next;
    }

    CMap::XML(node);
}

void CMapNatal::Calc() {
    swe_houses(JD(), iPlace->Lat(), iPlace->Lon(), iHouseSys, iCusps, iAxis);

    list<CPoint*>::iterator point;

    for (point = iPointsList.begin(); point != iPointsList.end(); point++)
	(*point)->Calc();

    CMap::Calc();
}

double CMapNatal::JD() {
    return iDate->JD();
}

void CMapNatal::Rotate(long int sec) {
    iDate->Rotate(sec);
}

char* CMapNatal::MoreInfo() {
    char *res = (char *) malloc(1024);
    char *date = iDate->Date();
    char *time = iDate->Time();

    sprintf(res,"%s %s", date, time);	
    free(date);
    free(time);
    
    return res;
}

CDate* CMapNatal::Date() {
    return iDate;
}

CPlace* CMapNatal::Place() {
    return iPlace;
}

void CMapNatal::Date(CDate* date) {
    if (iDate) {
	if (iDate->Owner() != this) {
	    ((CMapNatal*)iDate->Owner())->DelDepends(this);
	} else {
	    delete iDate;
	}
    }
    iDate = date;

    PushTable();

    lua.PushString("date");
    iDate->PushTable();
    lua.EndTable();

    lua.Pop(1);
}

void CMapNatal::Place(CPlace* place) {
    if (iPlace) {
	if (iPlace->Owner() != this) {
	    ((CMapNatal*)iPlace->Owner())->DelDepends(this);
	} else {
	    delete iPlace;
	}
    }
    iPlace = place;

    PushTable();

    lua.PushString("place");
    iPlace->PushTable();
    lua.EndTable();
    
    lua.Pop(1);
}

void CMapNatal::HouseSys(const int data) {
    int sys[] = {'P','K','R','B','O','C','E','M','V','X','H','T'};
    
    iHouseSys = sys[data];
    Ready(false);
}

int CMapNatal::HouseSys() {
    int sys[] = {'P','K','R','B','O','C','E','M','V','X','H','T'};
    int i;
    
    for (i=0; i < sizeof(sys); i++)
	if (iHouseSys == sys[i]) return i;

    return 0;
}

CMapEdit* CMapNatal::MakeEditor() {
    CMapNatalEdit* edit = new CMapNatalEdit();
    edit->Map(this);
    
    return edit;
}

void CMapNatal::Ready(bool data) {
    CMap::Ready(data);

    CMapNatal* natal;

    natal = (CMapNatal*) iDate->Owner();
    if (natal && natal != this) natal->Ready(false);
    
    natal = (CMapNatal*) iPlace->Owner();
    if (natal && natal != this) natal->Ready(false);
}

void CMapNatal::Event(CAstro* obj, eventEnum event, void* data) {
    CMap::Event(obj, event, data);
    
    if (obj == iDate && (event == eventChangeTime || event == eventChangeDate)) {
	Ready(false);
    }
}

bool CMapNatal::LuaSet(const char* var) {
    if (strcmp(var, "date") == 0) {
	if (lua.Type(-1) == LUA_TTABLE) {
	    lua.PushString(".ptr_date");
	    lua.GetTable(-2);

	    if (lua.Type(-1) == LUA_TLIGHTUSERDATA) {
		CDate *date = (CDate*) lua.ToUser(-1);
		
		if (date->Owner() != this) Date(date);
	    }
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "place") == 0) {
	if (lua.Type(-1) == LUA_TTABLE) {
	    lua.PushString(".ptr_place");
	    lua.GetTable(-2);

	    if (lua.Type(-1) == LUA_TLIGHTUSERDATA) {
		CPlace *place = (CPlace*) lua.ToUser(-1);
		
		if (place->Owner() != this) Place(place);
	    }
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "house") == 0) {
	HouseSys(lua.ToNumber(-1));
	return true;
    }
    
    return CMap::LuaSet(var);
}

bool CMapNatal::LuaGet(const char* var) {
    if (strcmp(var, "date") == 0) {
	iDate->PushTable();
	return true;
    } else if (strcmp(var, "place") == 0) {
	iPlace->PushTable();
	return true;
    } else if (strcmp(var, "house") == 0) {
	lua.PushNumber(HouseSys());
	return true;
    }
    
    return CMap::LuaGet(var);
}
