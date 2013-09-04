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

#ifndef _ASPECT_EQUATOR_H
#define _ASPECT_EQUATOR_H

#include "aspects.h"

class CDrawArea;
class CPoint;
class CLine;

class CAspectEquatorData : public CAspectData {
    friend class CAspectEquator;
    CLine* iLine;
    
public:
    void Draw(CDrawArea* area);
};

class CAspectEquator : public CAspect {
private:
    double	iOrb;
    CLine*	iParallel;
    CLine*	iContrParallel;
    
public:
    CAspectEquator();
    ~CAspectEquator();
    
    CAspectData* Data(CPoint* point1, CPoint *point2);
    void Calc(CAspectData* data);
    void XML(xmlNodePtr node);

    bool LuaSet(const char* var);
};

#endif
