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

#ifndef _ASPECT_SYMETRIC_H
#define _ASPECT_SYMETRIC_H

#include "aspects.h"

class CDrawArea;
class CPoint;
class CLine;

class CAspectSymetricData : public CAspectData {
    friend class CAspectSymetric;
    CLine*	iLine;
    
public:
    void Draw(CDrawArea* area);
};

class CAspectSymetric : public CAspect {
private:
    CLine*	iLine;
    char*	iName;
    double	iOrbis;
    double	iAxis;

public:
    CAspectSymetric();
    ~CAspectSymetric();
    
    CAspectData* Data(CPoint* point1, CPoint *point2);
    void Calc(CAspectData* data);
    void XML(xmlNodePtr node);
    void Name(const char* name);

    bool LuaSet(const char* var);
};

#endif
