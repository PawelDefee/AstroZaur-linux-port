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

#ifndef _MAP_H_
#define _MAP_H_

#include <map>
#include <list>
#include <string>
#include <fltk/Color.h>
#include "astro.h"
#include "point.h"
#include "utils.h"

using namespace std;

// Main map class

class CProgram;
class CDrawArea;
class CTransform;
class CMapEdit;

class CMap : public CAstro { 
    friend class CHouse;
    friend class CAxis;
private:
    list<CMap*>		iDepends;
    list<CTransform*>	iTransform;
    long int		iRotateSec;
    CMapEdit*		iEditor;
    CAstro		iPointsRef;

protected:
    char*		iType;
    char*		iInfo;		// Text info
    char*		iId;		// Text ID (as key)
    double		iCusps[13];	// Internal cuspides
    double		iAxis[10];	// Internal axes
    fltk::Color		iColor;
    
    void XMLPoints(xmlNodePtr node);
    
    virtual void Calc();

public:
    map<string, CPoint*>	iPointsMap;
    list<CPoint*>		iPointsList;
    int				iWidth;
    
    CMap();
    ~CMap();
    
    void Info(const char *data);
    char* Info();
    char* Id();
    void Id(const char* data);
    char* IdInfo();

    char* Type();
    void Type(const char* type);
    
    fltk::Color Color();
    void  Color(fltk::Color color);

    void AddDepends(CMap* map);
    void DelDepends(CMap* map);
    
    CPoint* NewPoint(const char* type, const char* name);
    CTransform* NewTransform(const char* type);
    
    void SetRotate(long int sec);
    void RotateForward();
    void RotateBack();
    void DoCalc();
    void OpenEdit();
    
    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);
    void Event(CAstro* obj, eventEnum event, void* data = NULL);

    void Lua();
    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
    
    virtual void Draw(CDrawArea* area);
    virtual void DrawPoints(CDrawArea* area);
    virtual void DrawInfo(CDrawArea* area);
    virtual void ResolveShadow() {};

    virtual char* MoreInfo() { return NULL; };
    virtual double JD() { return 0; };
    virtual void Rotate(long int sec) {};
    virtual void RefrashEdit() {};
    virtual CMapEdit* MakeEditor() { return NULL; };
};

#endif
