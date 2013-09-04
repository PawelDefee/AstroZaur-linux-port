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

#ifndef _ASPECTS_H_
#define _ASPECTS_H_

#include <map>
#include <list>
#include <set>
#include "astro.h"

using namespace std;

class CPoint;
class CDrawArea;
class CMap;
class CAspect;
// base class of data for abstract aspect

class CAspectData : public CAstro {
protected:
    bool	iActive;
    CAspect*	iAspect;	
    CPoint*	iPoint1;
    CPoint*	iPoint2;
    char*	iName;
    char*	iType;
    
public:
    CAspectData();
    ~CAspectData();

    const char* Name();
    const char* Type();
    
    virtual void Draw(CDrawArea* area) {};
    
    bool ThisType(const char *type);
    
    void Name(const char *data);
    void Type(const char *data);
    
    void Points(CPoint* point1, CPoint *point2);
    CPoint* Point1();
    CPoint* Point2();
    
    void Aspect(CAspect* aspect);
    bool Active();
    void Active(bool data);
    void Calc();
};

// base class of abstract aspect

class CAspect : public CAstro {
private:
    char*		iType;
    bool		iView;
    set<unsigned int>	iExclude; // hash
    
protected:
    bool	Exclude(CPoint *point1, CPoint *point2);
    
public:
    CAspect();
    ~CAspect();

    void XML(xmlNodePtr node);
    
    const char* Type();
    void Type(const char *data);
    bool ThisType(const char *type);
    
    void View(bool data);
    bool View();

    virtual CAspectData* Data(CPoint* point1, CPoint *point2) { return NULL; };
    virtual void Calc(CAspectData* data) {};

    bool LuaSet(const char* var);
};

// main aspect table class

class CAspectTable : public CAstro {
public:
    list<CAspectData*>		iItem;		// created point-point table
    list<CAspect*>		iAspects;	// all XML-loaded aspects
    
private:
    CAspect* FindAspect(const char* type);

public:
    CAspectTable();
    ~CAspectTable();

    void Calc();
    void Draw(CDrawArea* area);
    void XML(xmlNodePtr node);
    
    CAspect* NewAspect(const char *type);
    void FreeAspects();
    void FreeAspects(const char *type);
    
    void CreateItems(CMap* map);
    void CreateItems(CMap* map1, CMap* map2);
    void FreeItems();
    void FreeItems(const char *type);
    
    bool LuaGet(const char* var);
    bool LuaSet(const char* var);
};

#endif
