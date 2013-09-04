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
#include "map_develop.h"
#include "map_develop_edit.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

CMapDevelop::CMapDevelop() {
    iParent = NULL;
    iK1 = 1.0;
    iK2 = 1.0;
}

CMapDevelop::~CMapDevelop() {
    if (iParent->Owner() == this) delete iParent;
}

void CMapDevelop::K1(double data) {
    iK1 = data;
    Ready(false);
}

void CMapDevelop::K2(double data) {
    iK2 = data;
    Ready(false);
}

double CMapDevelop::K1() {
    return iK1;
}

double CMapDevelop::K2() {
    return iK2;
}

CMap* CMapDevelop::Parent() {
    return iParent;
}

void CMapDevelop::Parent(CMap* map) {
    if (iParent) {
	iParent->DelDepends(this);
	if (iParent->Owner() == this) delete iParent;
    }
    iParent = map;
    iParent->AddDepends(this);
}

void CMapDevelop::XML(xmlNodePtr node) {
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
    
	if (!strcmp(name,"k1")) {
	    if (has_content(subNode))
		iK1 = atof((char *) subNode->children->content);
	    goto next;
	}
	
	if (!strcmp(name,"k2")) { 
	    if (has_content(subNode))
	        iK2 = atof((char *) subNode->children->content);
	    goto next;
	}
    
	if (!strcmp(name,"parent_chart")) {
	    xmlChar*	xmlInfo = xmlGetProp(subNode,(xmlChar *) "clone");
	    
	    if (xmlInfo) {
		iParent = program.Map((char *)xmlInfo);
		if (iParent) {
		    iParent->AddDepends(this);
		} else {
		    printf("Error: not found parent chart %s\n",(char *)xmlInfo);
		}
		xmlFree(xmlInfo);
	    } else {
		if (has_content(subNode)) {
		    iParent = new CMapNatal;
		    iParent->Owner(this);
		    iParent->XML(subNode->children);
		}
	    }
	    goto next;
	}
next:	subNode = subNode->next;
    }
    
    CMapNatal::XML(node);
}

xmlNodePtr CMapDevelop::XML(xmlNodePtr parent, int level) {
    xmlNodePtr	node, sub;
    char	str[32];
    
    node = CMapNatal::XML(parent, level);

    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    sub = xmlNewTextChild(node, NULL, (xmlChar*)"parent_chart", NULL);
    xmlNewProp(sub, (xmlChar*)"clone", (xmlChar*)iParent->IdInfo());
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));

    DoubleToStr(iK1, str);
    xmlNewTextChild(node, NULL, (xmlChar*)"k1", (xmlChar*)str);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));

    DoubleToStr(iK2, str);
    xmlNewTextChild(node, NULL, (xmlChar*)"k2", (xmlChar*)str);

    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level)));
    
    return node;
}

CMapEdit* CMapDevelop::MakeEditor() {
    CMapDevelopEdit* edit = new CMapDevelopEdit();
    edit->Map(this);
    
    return edit;
}

bool CMapDevelop::LuaSet(const char* var) {
    if (strcmp(var, "parent") == 0) {
	if (lua.Type(-1) == LUA_TTABLE) {
	    lua.PushString(".ptr_chart");
	    lua.GetTable(-2);

	    if (lua.Type(-1) == LUA_TLIGHTUSERDATA) {
		CMap *map = (CMap*) lua.ToUser(-1);
		
		Parent(map);
	    }
	    lua.Pop(1);
	}
	return true;
    } else if (strcmp(var, "k1") == 0) {
	iK1 = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "k2") == 0) {
	iK2 = lua.ToNumber(-1);
	return true;
    }
    
    return CMapNatal::LuaSet(var);
}

bool CMapDevelop::LuaGet(const char* var) {
    if (strcmp(var, "parent") == 0) {
	if (iParent) iParent->PushTable();
	return true;
    } else if (strcmp(var, "k1") == 0) {
	lua.PushNumber(iK1);
	return true;
    } else if (strcmp(var, "k2") == 0) {
	lua.PushNumber(iK2);
	return true;
    }
    
    return CMapNatal::LuaGet(var);
}
