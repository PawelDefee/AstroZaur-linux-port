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

#ifndef _DRAW_AREA_H_
#define _DRAW_AREA_H_

#include "astro.h"
#include <fltk/Widget.h>
#include <fltk/Image.h>

class CMaps;
class CPoint;

using namespace fltk;

class CDrawArea : public Widget, public CAstro {
private:
    CMaps*	iMaps;
    int		iBorderWidth;

public:
    int		iCenterX;
    int		iCenterY;
    int		iInfoX;
    int		iInfoY;
    int		iSize;
    int		iFontSize;

public:
    CDrawArea(int x,int y,int w,int h, const char* label=0);
    void Maps(CMaps *maps);
    void draw();
    
    void GetXY(double lon, int r, int *x, int *y);
    char* InfoStr(CPoint* point);
    
    // With active map
    void Hide();
    
    bool LuaSet(const char* var);
};

#endif
