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

#ifndef _MAPS_H_
#define _MAPS_H_

#include <map>
#include <string>
#include "astro.h"
#include "map.h"

using namespace std;

class CDrawArea;
class CAspectTable;
class CProjection;

// Set of maps

class CMaps : public CAstro { 
private:
    CAspectTable*		iAspectTable;
    CMap*			iAspectMap1;
    CMap*			iAspectMap2;
    
    map<CMap*, bool>		iView;
    CMap*			iActiveMap;
    CProjection*		iProjection;

public:
    CMaps();
    ~CMaps();
    
    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);
    
    void Draw(CDrawArea* area);
    void AspectedMap(CMap* a, CMap* b);
    
    //whith active map
    void AspectedMap(CMap* b);
    void AspectedMap(char* name);
    void CalcAspectTable();
    void StartPoint(char *point);

    void Projection(const char* type);
    CProjection* Projection();
    
    void Hide(CMap* map);
    void View(CMap* map, bool data);
    bool View(CMap* map);

    bool Empty();
    
    CMap* ActiveMap();
    void ActiveMap(CMap* map);
    void ActiveMap(char* name);

    CAspectTable* AspectTable();

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);

    void Event(CAstro* obj, eventEnum event, void* data = NULL);
};

#endif
