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

#ifndef _ASPECT_EQL_LON_H_
#define _ASPECT_EQL_LON_H_

#include <map>
#include <list>
#include <string>
#include "aspects.h"

#include <fltk/Color.h>

using namespace std;

typedef enum {
    application = 0,
    separate
} ActionEnum;

// CAspectEqlLon - aspect data (aspect table)

class CAspectEqlLonItem;

class CAspectEqlLonData : public CAspectData {
    friend class CAspectEqlLon;

protected:
    // Set from CAspectEqlLon::Data();
    unsigned int		iOrbId;		// hash
        
    // Copy from CAspectEqlLonItem when active
    char*			iLineStyle;
    fltk::Color			iLineColor;
    
    // Calc
    ActionEnum			iAction;
    double			iOrb;
    
public:
    void Draw(CDrawArea* area);
};

// CAspectEqlLonItem - aspect item (loaded from XML)

class CAspectEqlLonItem : public CAstro {
    friend class CAspectEqlLon;
    friend class CAspectEqlLonData;
    
protected:
    bool			iView;
    char*			iName;
    double			iAngle;
    char*			iLineStyle;
    map<double,fltk::Color>	iLineColor;
    map<unsigned int,double>	iOrb;		// hash
    
public:
    CAspectEqlLonItem();

    void XML(xmlNodePtr node);
    void Name(const char* name);
    void LineStyle(const char* style);
    
    bool LuaSet(const char* var);
};

// CAspectEqlLon - main class

class CAspectEqlLon : public CAspect {
private:
    map<unsigned int, unsigned int>	iOrbId;		// hash to hash
    list<CAspectEqlLonItem*>		iItem;
    bool				iPoint1Fixed;
    bool				iPoint2Fixed;
    
    void XMLOrbises(xmlNodePtr node);
    void FreeOrbId();
    void FreeItem();
    const unsigned int OrbId(CPoint* p1, CPoint* p2);

public:
    CAspectEqlLon();
    CAspectData* Data(CPoint* point1, CPoint *point2);
    void Calc(CAspectData* data);
    void XML(xmlNodePtr node);

    CAspectEqlLonItem* NewLon();
    bool LuaSet(const char* var);
};

#endif
