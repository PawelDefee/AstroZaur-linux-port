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

#ifndef _MAP_DIRECT_H_
#define _MAP_DIRECT_H_

#include "map_develop.h"

// Zodiacal direction map class

class CMapZodiacDirect : public CMapDevelop { 
protected:
    void Calc();
    
public:
    CMapZodiacDirect();
};

// Equatorial direction map class

class CMapEquatorDirect : public CMapDevelop { 
protected:
    void Calc();

public:
    CMapEquatorDirect();
};

// True direction map class

class CMapTrueDirect : public CMapDevelop { 
private:
    double iDelta;

protected:
    void Calc();

public:
    CMapTrueDirect();
    double JD();
};

// Arc direction

class CMapArcDirect : public CMapDevelop { 
protected:
    CPoint*	iPoint;
    
    void Calc();
    
public:
    CMapArcDirect();
    void XML(xmlNodePtr node);

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

#endif
