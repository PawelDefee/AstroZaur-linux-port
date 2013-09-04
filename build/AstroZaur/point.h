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

#ifndef _POINT_H_
#define _POINT_H_

#include <list>
#include "coord.h"

#include <fltk/Color.h>
#include <fltk/Image.h>

extern "C" {
    #include <sweph/swephexp.h>
}

using namespace std;

// Main point class

// Predefine

class CMap; 
class CDrawArea;
class CAspectData;
class CLine;

class CPoint : public CAstro {
protected:
    CCoord	iEcliptic;
    CCoord	iEquator;
    CCoord	iProjected;

    double	iLatSpeed;
    double	iLonSpeed;
    char	*iName;
    char	*iCategory;
    bool	iView;
    bool	iAspecting;
    bool	iAspected;
    bool	iInfo;
    fltk::Color	iColor;
    
protected:
    CMap		*iMap;
    int			iId;
    bool		iStateNew;
    double		iLonShift;

public:
    CPoint();
    ~CPoint();

    CCoord* Ecliptic();
    CCoord* Equator();
    CCoord* Projected();
    
    double LonSpeed();
    double LatSpeed();
    
    char* Name();
    void Name(char* name);
    
    bool View();
    bool Info();
    CMap* Map();
    const char* Category();

    void LonSpeed(double data);
    void LatSpeed(double data);
    void View(bool data);
    void Info(bool data);
    void Map(CMap *data);
    void StateNew(bool data);

    void operator<<(const CPoint& a);

    void Aspecting(bool data);
    void Aspected(bool data);

    virtual bool Aspecting();
    virtual bool Aspected();
    virtual void HasAspected(CAspectData* aspect) {};
    virtual void Touch() {};
    
    void DrawShift(double data);
    double DrawShift();
    virtual int DrawWidth();

    virtual void Calc() {};
    virtual void Calc(double jd, int32 iflag = SEFLG_SPEED) {};

    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);
    void Ready(bool data);

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);

    virtual fltk::Image* Bitmap() { return NULL; };
    virtual char* BitmapFile() { return NULL; };
    fltk::Color Color();
    
    virtual void Draw(CDrawArea* area) {};
    virtual void DrawPoint(CDrawArea* area) {};
    virtual void DrawInfo(CDrawArea* area);

    virtual bool Visible();
};

// Swiss planet class

class CSwissPlanet : public CPoint {
protected:
    fltk::Image*	iBitmap;
    char*		iBitmapFile;
    int			iBitmapSize;
    int			iDotSize;
    int			iDotSizeBorder;
    
public:
    CSwissPlanet();
    ~CSwissPlanet();
    
    void XML(xmlNodePtr node);
    void Draw(CDrawArea* area);
    void DrawPoint(CDrawArea* area);
    void DrawInfo(CDrawArea* area);
    void Calc();
    void Calc(double jd, int32 iflag = SEFLG_SPEED);
    int DrawWidth();
    fltk::Image* Bitmap();
    char* BitmapFile();

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

// House cuspide class

class CHouse : public CPoint { 
private:
    int	iLineWidth;
    int iLineStyle;
    
public:
    CHouse();
    void Draw(CDrawArea* area);
    void XML(xmlNodePtr node);
    void Calc();

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

// Axis class

class CAxis : public CHouse { 
protected:
    fltk::Image*	iBitmap;
    int			iBitmapSize;
    char*		iBitmapFile;
    
public:
    CAxis();
    ~CAxis();
    void Draw(CDrawArea* area);
    void XML(xmlNodePtr node);
    void Calc();
    void DrawInfo(CDrawArea* area);
    fltk::Image* Bitmap();
    char* BitmapFile();

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

// Fixed star class

class CStar : public CSwissPlanet {
private:
    bool	iTurnOn;
    double	iPrevJD;
    double	iData[6];
    
public:
    CStar();

    void Ready(bool data);
    void Calc();
    void HasAspected(CAspectData* aspect);
    bool Visible();
    void DrawInfo(CDrawArea* area);
};

// Lua point

class CLuaPoint : public CAxis {
private:
    bool	iPersist;
    bool	iTurnOn;
    int		iDotSize;
    int		iDotSizeBorder;
    
public:
    CLuaPoint();
    
    void Draw(CDrawArea* area);
    void DrawInfo(CDrawArea* area);
    void DrawPoint(CDrawArea* area);
    void Calc();
    void Ready(bool data);
    void HasAspected(CAspectData* aspect);
    void Touch();
    bool Visible();
    int DrawWidth();
    void XML(xmlNodePtr node);

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

// Midpoint

class CMidPoint : public CLuaPoint {
private:
    CPoint*	iPoint1;
    CPoint*	iPoint2;
    CLine*	iLine;
    
public:
    CMidPoint();
    
    void Calc();
    void XML(xmlNodePtr node);
    void DrawPoint(CDrawArea* area);
};

#endif
