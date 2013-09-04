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

#ifndef _ASTRO_H_
#define _ASTRO_H_

#include <libxml/parser.h>
#include <string>
#include <list>
#include <map>

using namespace std;

// Events

typedef enum {
    eventChangeChartInfo = 0,
    eventChangeChartId,
    eventChangeChartColor,
    eventChangePlaceInfo,
    eventChangePlaceLon,
    eventChangePlaceLat,
    eventChangeTime,
    eventChangeDate,
    eventChangeTimeZone,
    eventNewChart,
    eventNewPoint
} eventEnum;

// Abstract base class

class CAstro;
class CLuaChunk;

class CAstro {
protected:
    map<string, CLuaChunk*>	iLuaChunk;

    void XMLLua(xmlNodePtr node);
    void LuaFunction(char *name);

private:
    CAstro*		iOwner;
    list<string>	iLoadedFile;
    bool		iLoadedFileStat;
    bool		iReady;
    int			iLuaRef;
    
public:
    CAstro();
    virtual ~CAstro();

    void LoadFile(char *name);
    void SaveFile(char *name);

    void LoadedFile(bool stat);

    virtual xmlNodePtr XML(xmlNodePtr parent, int level);
    virtual void XML(xmlNodePtr node);
    
    CAstro* Owner();
    void Owner(CAstro* owner);
    
    virtual void Ready(bool data);
    virtual bool Ready();
    
    virtual void Event(CAstro* obj, eventEnum event, void* data = NULL) {};
    
    virtual void Lua() {};
    virtual bool LuaGet(const char* var) { return false; };
    virtual bool LuaSet(const char* var) { return false; };
    virtual int LuaCall();

    void LuaExec(char *name, bool destroy = false);
    void LuaExec(char *name, CAstro* obj);
    void PushTable();
};

#endif
