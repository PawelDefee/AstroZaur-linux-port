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

#include <string.h>
#include "line.h"
#include "utils.h"
#include "draw_area.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include <fltk/draw.h>
#include <math.h>

#define PI 3.14159265358979323846

static char	dash[] =	{5,6,0};
static char 	dot[] = 	{1,6,0};
static char 	dashdot[] =	{5,6,1,6,0};
static char 	dashdotdot[] =	{7,6,1,6,1,6,0};

CLine::CLine() {
    iStyle = NULL; // Solid
    iColor = fltk::BLACK;
    iWidth = 1;
    iCurve = 0;
}

void CLine::Style(const char* style) {
    if (strcmp(style, "solid") == 0) {
	iStyle = NULL;
    } else if (strcmp(style, "dash") == 0) {
	iStyle = dash;
    } else if (strcmp(style, "dot") == 0) { 
	iStyle = dot;
    } else if (strcmp(style, "dashdot") == 0) {
	iStyle = dashdot;
    } else if (strcmp(style, "dashdotdot") == 0) {
	iStyle = dashdotdot;
    }
}

void CLine::XML(xmlNodePtr node) {
    xmlNodePtr subNode = node;

    while (subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"color")) {
	    if (has_content(subNode)) {
		iColor = fltk::color((char *) subNode->children->content);
	    }
	    goto next;
	}
	    
	if (!strcmp(name,"style")) {
	    if (has_content(subNode)) {
		string style = (char *) subNode->children->content;
		    
		style = leadcut(style);
		style = trailcut(style);
		
		Style(style.c_str());
	    }	    
	    goto next;
	}

	if (!strcmp(name,"curve") && has_content(subNode)) {
	    iCurve = atof((char *) subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"width") && has_content(subNode))
	    iWidth = atoi((char *) subNode->children->content);

next:	subNode = subNode->next;
    }
}

void CLine::Curve(double data) {
    iCurve = data;
}

void CLine::DrawCurve(double lon1, double lon2, CDrawArea* area) {
    int			x0,y0, x1,y1, x2,y2, x3,y3, t;
    double		a0 = lon1;
    double		a1 = lon2;
    double		a, r, dist;
    double		R = area->iSize;
    double		delta = R/iCurve;

    if (Dist(a0,a1) < 0) {
	a = a0;
	a0 = a1;
	a1 = a;
    }
    
    area->GetXY(a0, area->iSize, &x0, &y0);
    area->GetXY(a1, area->iSize, &x3, &y3);

    dist = Dist(a0,a1);
    a = a0 + dist/2.0;
    
    r = R * cos((dist/180.0*PI)/2.0) - delta;

    area->GetXY(a-15, r, &x1, &y1);
    area->GetXY(a+15, r, &x2, &y2);

    if (r<0) {
	t = x1; x1 = x2; x2 = t;
	t = y1; y1 = y2; y2 = t;
    }
    
    fltk::setcolor(iColor);
    fltk::line_style(fltk::SOLID | fltk::CAP_ROUND, iWidth, iStyle);
    
    fltk::addcurve(x0,y0,x1,y1,x2,y2,x3,y3);
    fltk::strokepath();
}

void CLine::DrawHord(double lon1, double lon2, CDrawArea* area) {
    int	x0,y0, x1,y1;

    area->GetXY(lon1, area->iSize, &x0, &y0);
    area->GetXY(lon2, area->iSize, &x1, &y1);

    fltk::setcolor(iColor);
    fltk::line_style(fltk::SOLID | fltk::CAP_ROUND, iWidth, iStyle);
    fltk::drawline(x0, y0, x1, y1);
}

bool CLine::LuaSet(const char* var) {
    if (strcmp(var, "style") == 0) {
	Style(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "width") == 0) {
	iWidth = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "color") == 0) {
	iColor = fltk::color(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "curve") == 0) {
	iCurve = lua.ToNumber(-1);
	return true;
    }
    
    return false;
}
