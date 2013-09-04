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

#ifndef _PROGRAM_H_
#define _PROGRAM_H_

// cygwin bug
#undef max
#include <vector>

#include "astro.h"
#include "maps.h"
#include <set>
#include <map>
#include <list>
#include <fltk/draw.h>

class CWindow;
class CMap;
class CDrawArea;
class CLog;

using namespace std;

class CProgram : public CAstro {
private:
    CLog*		iLog;
    list<CWindow*>	iWindows;
    vector<CMap*>	iMapsList;
    map<string, CMap*>	iMapsMap;
    char		iDir[512];
    char*		iAtlasFile;
    char*		iTimeZones;
    
    void AddWindow(CWindow *window);
    void GetDir();
    
public:
    void 		AtlasFile(const char *path);
    void 		TimeZones(const char *path);

    char*		TimeZones();
    char*		AtlasFile();

public:
    CProgram();
    ~CProgram();
    
    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);

    bool AddMap(CMap *map);
    CMap* AddMap(const char* type);
    CMap* AddMap(xmlNodePtr node);

    void CalcMaps();
    void Draw();
    
    void DoInfo();
    
    CMap* Map(unsigned int data);
    CMap* Map(char *data);
    unsigned int MapNum();

    CWindow* NewWindow(const char* title = NULL);
    
    void Clear();
    void Exit();
    
    void FreeMaps();
    void ResolveShadow();

    const char* Msg(const char* key);

    void Event(CAstro* obj, eventEnum event, void* data = NULL);

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

#include "static_holder.h"
#define program CStaticHolder::GetProgram()

#endif
