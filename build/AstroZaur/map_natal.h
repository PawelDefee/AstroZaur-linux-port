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

#ifndef _MAP_NATAL_H_
#define _MAP_NATAL_H_

#include "map.h"
#include "date.h"
#include "place.h"

class CDrawArea;

// Natal map class

class CMapNatal : public CMap { 
private:
    CDate*		iDate;
    CPlace*		iPlace;
    int			iHouseSys;
    
protected:
    void Calc();
    
public:
    CMapNatal();
    ~CMapNatal();

    void Ready(bool data);

    CDate* Date();
    CPlace* Place();

    void Date(CDate* date);
    void Place(CPlace* place);

    void HouseSys(const int data);
    int HouseSys();
    
    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);

    void Rotate(long int sec);
    double JD();
    char* MoreInfo();
    
    CMapEdit* MakeEditor();

    void Event(CAstro* obj, eventEnum event, void* data = NULL);

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

#endif
