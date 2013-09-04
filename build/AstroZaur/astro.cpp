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

#include "astro.h"
#include "utils.h"
#include "program.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

CAstro::CAstro() {
    iOwner = NULL;
    iReady = false;
    iLoadedFileStat = false;

    lua.NewTable();
    lua.InitMeta(this);
    iLuaRef = lua.Ref();
}

CAstro::~CAstro() {
    lua.Unref(iLuaRef);
}

void CAstro::LoadFile(char *name) {
//    printf("CAstro::LoadFile (%s)\n",name);

    xmlNodePtr	node;
    xmlDocPtr	doc;
    
    doc = xmlParseFile(name);
    
    if (iLoadedFileStat)
	iLoadedFile.push_back(name);
    
    if (doc) {
	node = xmlDocGetRootElement(doc);
	while (node) {
	    if (!strcmp((char*)node->name,"astrozaur"))
		if (node->children) XML(node->children);

	    node = node->next;
	}
    }
    xmlFreeDoc(doc);
}

void CAstro::SaveFile(char *name) {
    xmlDocPtr		doc;
    xmlNodePtr		tree;

    doc = xmlNewDoc((xmlChar *)"1.0");
    tree = xmlNewTextChild((xmlNodePtr)doc, NULL, (xmlChar*)"astrozaur", NULL);

    xmlAddChild(tree, XML(tree, 2));

    xmlAddChild(tree, xmlNewText((xmlChar*)"\n"));
//    xmlSaveFormatFileEnc(name, doc, fltk::get_encoding(), 1);
    xmlSaveFormatFileEnc(name, doc, NULL, 1);
    xmlFreeDoc(doc);
}

void CAstro::XML(xmlNodePtr node) {
}

void CAstro::LoadedFile(bool stat) {
    iLoadedFileStat = stat;
}

xmlNodePtr CAstro::XML(xmlNodePtr parent, int level) { 
    list<string>::iterator file;

    for (file = iLoadedFile.begin(); file != iLoadedFile.end(); file++) {
	char* name = (char*) (*file).c_str();
    
	xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level+1)));
	xmlNewTextChild(parent, NULL, (xmlChar*)"file", (xmlChar*) name);
    }
    return NULL;
}

CAstro* CAstro::Owner() {
    return iOwner;
}

void CAstro::Owner(CAstro* owner) {
    iOwner = owner;
}

void CAstro::Ready(bool data) {
    iReady = data;
    
    if (iOwner && !data)
	iOwner->Ready(false);
}

bool CAstro::Ready() {
    return iReady;
}

void CAstro::LuaFunction(char *name) {
    CLuaChunk	*chunk = iLuaChunk[name];

    if (chunk) {
	lua.PushString(name);
	chunk->Push();	
	lua.RawSet(-3);
    }
}

void CAstro::LuaExec(char *name, bool destroy) {
    CLuaChunk	*chunk = iLuaChunk[name];

    if (chunk) {
	chunk->Push();
	
	if (iLuaRef > 0) {
	    PushTable();
	    Lua();
	    lua.Call(1);
	} else {
	    lua.Call(0);
	}
	
	if (destroy) {
	    iLuaChunk.erase(name);
	    delete chunk;
	}
    } else {
	PushTable();
        lua.PushString(name);
	lua.GetTable(-2);
    
	if (lua.Type(-1) == LUA_TFUNCTION) {
	    lua.PushValue(-2);	// self
	    lua.Remove(-3);	// table
	    lua.Call(1);
	} else {
	    lua.Pop(2);
	    /*
	    lua.Remove(-1);	// nil
	    lua.Remove(-1); 	// table
	    */
	}
    }
}

void CAstro::LuaExec(char *name, CAstro* obj) {
    CLuaChunk	*chunk = iLuaChunk[name];

    if (chunk) {
	chunk->Push();
	
	obj->PushTable();
	obj->Lua();
	lua.Call(1);
    } else {
	PushTable();
        lua.PushString(name);
	lua.GetTable(-2);

	if (lua.Type(-1) == LUA_TFUNCTION) {
	    lua.Remove(-2);	// self
	    obj->PushTable();	// obj
	    lua.Call(1);
	} else {
	    lua.Pop(2);
	    /*
	    lua.Remove(-1);	// nil
	    lua.Remove(-1); 	// table
	    */
	}
    }
}

void CAstro::XMLLua(xmlNodePtr node) {
    xmlNodePtr	subNode = node->children;
    
    while (subNode) {
	char* name = (char*) subNode->name;
	
	if (!strcmp(name,"load")) {
	    if (has_content(subNode)) {
		if (lua.Load((char*) subNode->children->content)) {
		    lua.Call(0);
		}
	    }
	    goto next;
	}

	if (!strcmp(name,"chunk")) {
	    if (has_content(subNode)) {
		char *when = (char*)xmlGetProp(subNode,(xmlChar *) "when");
		
		if (when) {
		    CLuaChunk *chunk = new CLuaChunk();
		    
		    if (chunk->Set((char*) subNode->children->content))
			iLuaChunk[when] = chunk;
		}
	    }
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

void CAstro::PushTable() {
    lua.RawGetI(LUA_REGISTRYINDEX, iLuaRef);
}

int CAstro::LuaCall() {
    if (lua.Type(-1) == LUA_TTABLE) {
	lua.PushNil();
	
	while (lua.Next(-2)) {
	    const char	*var = lua.ToString(-2);

	    if (!LuaSet(var)) lua.RawSet(-3);

	    lua.Pop(1);
	}
    }
    return 0;
}
