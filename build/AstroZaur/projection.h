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

#ifndef _PROJECTION_H_
#define _PROJECTION_H_

#include "astro.h"
#include <fltk/draw.h>
#include <fltk/SharedImage.h>

class CMap;
class CDrawArea;
class CPoint;

// Base projection

class CProjection : public CAstro {
protected:
    int			iWidth;
    bool		iCW;
    fltk::Color		iColor;
    CPoint*		iStart;
    
public:
    CProjection();

    void StartPoint(CPoint* point);
    CPoint* StartPoint();

    virtual void DrawSelf(CDrawArea* area);
    virtual void DrawMap(CMap* map, CDrawArea* area, bool active);
    virtual void GetXY(double lon, int r, int *x, int *y);
    virtual char* InfoStr(CPoint* point);
    virtual void BeforeDraw() {};
    virtual void AfterDraw() {};

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

// Eqliptic

class CEqliptic : public CProjection {
private:
    fltk::Image*	iBitmap[12];

public:
    ~CEqliptic();
    
    void XML(xmlNodePtr node);
    void DrawSelf(CDrawArea* area);
    void DrawMap(CMap* map, CDrawArea* area, bool active);
    void GetXY(double lon, int r, int *x, int *y);
    char* InfoStr(CPoint* point);

    bool LuaSet(const char* var);
};

// Horizont

class CHorizont : public CProjection {
private:
    fltk::Image*	iBitmap[4];

public:
    void XML(xmlNodePtr node);
    void DrawSelf(CDrawArea* area);
    void DrawMap(CMap* map, CDrawArea* area, bool active);
    void GetXY(double lon, int r, int *x, int *y);
    char* InfoStr(CPoint* point);

    bool LuaSet(const char* var);

#ifdef DBUS
    void BeforeDraw();
    void AfterDraw();
#endif    
};

// Dial

class CDial : public CProjection {
private:
    double	iSize;
    double	iStep;
    int		iFontSize;

public:
    CDial();

    void XML(xmlNodePtr node);
    void DrawSelf(CDrawArea* area);
    void DrawMap(CMap* map, CDrawArea* area, bool active);
    void GetXY(double lon, int r, int *x, int *y);
    char* InfoStr(CPoint* point);

    bool LuaSet(const char* var);
};

#endif
