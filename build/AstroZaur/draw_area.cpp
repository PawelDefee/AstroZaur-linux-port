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

#include "window.h"
#include "draw_area.h"
#include "program.h"
#include "projection.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include <math.h>
#include <fltk/draw.h>
#include <fltk/Font.h>
#include <fltk/rgbImage.h>

#define PI 3.14159265358979323846

CDrawArea::CDrawArea(int x,int y,int w,int h, const char* label) : Widget(x,y,w,h,label) {
    iBorderWidth = 10;
    iFontSize = 14;

};

void CDrawArea::Maps(CMaps *maps) {
    iMaps = maps;
}

void CDrawArea::GetXY(double lon, int r, int *x, int *y) {
    iMaps->Projection()->GetXY(lon, r, x, y);
    *x += iCenterX;
    *y += iCenterY;
}

char* CDrawArea::InfoStr(CPoint* point) {
    return iMaps->Projection()->InfoStr(point);
}

void CDrawArea::draw() {
    Widget::draw();

    push_clip(0, 0, w(), h());

    CMap* active = iMaps->ActiveMap();

    if (w() < h()) {
	iSize = w()/2;
	iInfoX = iBorderWidth;
	iInfoY = w() + iBorderWidth;
    } else {
	iSize = h()/2;
	iInfoX = h() + iBorderWidth;
	iInfoY = iBorderWidth;
    }

    iCenterX = iSize;
    iCenterY = iSize;
    iSize -= iBorderWidth;

    iMaps->Draw(this);

    fltk::setfont(fltk::COURIER->bold(), iFontSize);
    
    if (active) {
	char		*more;
	char		*info = active->Info();
	int		ix, iy;
    
	ix = iBorderWidth;
	iy = iFontSize + iBorderWidth;

	fltk::setcolor(fltk::WHITE);
	fltk::drawtext(info, ix, iy-1);
	fltk::drawtext(info, ix, iy+1);
	fltk::drawtext(info, ix-1, iy);
	fltk::drawtext(info, ix+1, iy);
	fltk::setcolor(iMaps->View(active) ? fltk::BLACK : fltk::GRAY50);
	fltk::drawtext(info, ix, iy);
	
	more = active->MoreInfo();
	if (more) {
	    iy += iFontSize;
	    fltk::setcolor(fltk::WHITE);
	    fltk::drawtext(more, ix, iy-1);
	    fltk::drawtext(more, ix, iy+1);
	    fltk::drawtext(more, ix-1, iy);
	    fltk::drawtext(more, ix+1, iy);
	    fltk::setcolor(iMaps->View(active) ? fltk::BLACK : fltk::GRAY50);
	    fltk::drawtext(more, ix, iy);
	    free(more);
	}
	active->DrawInfo(this);
    }
    fltk::line_style(fltk::SOLID | fltk::CAP_SQUARE, 1);
    pop_clip();
}

void CDrawArea::Hide() {
    CMap* active = iMaps->ActiveMap();

    if (active) iMaps->Hide(active);
}

bool CDrawArea::LuaSet(const char* var) {
    if (strcmp(var, "border") == 0) {
	iBorderWidth = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "fontsize") == 0) {
	iFontSize =  lua.ToNumber(-1);
	return true;
    }
    return false;
}
