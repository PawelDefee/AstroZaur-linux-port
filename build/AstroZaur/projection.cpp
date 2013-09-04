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

#include "projection.h"
#include "program.h"
#include "map_natal.h"
#include "draw_area.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

#ifdef DBUS
    #include "dbus.h"
#endif

#include <math.h>
#include <fltk/Font.h>

#define PI 3.14159265358979323846

extern "C" {
    #include <sweph/swephexp.h>
}

//

CProjection::CProjection() {
    iStart = NULL;
    
}

void CProjection::StartPoint(CPoint* point) {
    iStart = point;
}

CPoint* CProjection::StartPoint() {
    return iStart;
}

void CProjection::GetXY(double lon, int r, int *x, int *y) {
    double	tlon = lon;

    if (iCW) tlon -= 90.0;
    
    tlon = tlon / 180.0 * PI;

    if (iCW) {
	*x = r * sin(tlon);
	*y = r * cos(tlon);
    } else {
	*x = r * cos(tlon);
	*y = r * sin(tlon);
    }
}

char* CProjection::InfoStr(CPoint* point) {
    return NULL;
}

void CProjection::DrawSelf(CDrawArea* area) {
    fltk::line_style(fltk::SOLID, 2);

    fltk::setcolor(iColor);
    fltk::addarc(area->iCenterX - area->iSize, area->iCenterY - area->iSize,
		 area->iSize*2, area->iSize*2, 0, -360); 
    fltk::fillstrokepath(fltk::BLACK);
    fltk::setcolor(fltk::BLACK);
}

void CProjection::DrawMap(CMap* map, CDrawArea* area, bool active) {
    fltk::setcolor(active ? map->Color() : fltk::inactive(map->Color()));
    fltk::addarc(area->iCenterX - area->iSize, area->iCenterY - area->iSize,
		area->iSize*2, area->iSize*2, 0, -360); 
    fltk::fillstrokepath(fltk::BLACK);
    fltk::setcolor(fltk::BLACK);
	
    map->Draw(area);
    area->iSize -= map->iWidth;
    
    fltk::line_style(fltk::SOLID, 1);

    if (active) {
	fltk::setcolor(fltk::WHITE);
	fltk::addarc(area->iCenterX - area->iSize, area->iCenterY - area->iSize,
		     area->iSize*2, area->iSize*2, 0, -360); 
	fltk::fillstrokepath(fltk::BLACK);
    } else {
	fltk::setcolor(fltk::BLACK);
	fltk::addarc(area->iCenterX - area->iSize, area->iCenterY - area->iSize,
	    	     area->iSize*2, area->iSize*2, 0, -360); 
	fltk::strokepath();
    }
}
    
bool CProjection::LuaSet(const char* var) {
    if (strcmp(var, "cw") == 0) {
	iCW = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "width") == 0) {
	iWidth = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "color") == 0) {
	iColor = fltk::color(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "start") == 0) {
	CPoint *point = NULL;
	
	if (lua.Type(-1) == LUA_TTABLE) {
	    lua.PushString(".ptr_point");
	    lua.GetTable(-2);

	    if (lua.Type(-1) == LUA_TLIGHTUSERDATA)
		point = (CPoint*) lua.ToUser(-1);
		
	    lua.Pop(1);
	}
	
	StartPoint(point);
	return true;
    }
    
    return false;
}

bool CProjection::LuaGet(const char* var) {
    if (strcmp(var, "cw") == 0) {
	lua.PushBool(iCW);
	return true;
    } else if (strcmp(var, "width") == 0) {
	lua.PushNumber(iWidth);
	return true;
    } 
    
    return false;
}

//

CEqliptic::~CEqliptic() {
}

void CEqliptic::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    int		iZ = 0;
    
    while (subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name, "bitmap")) {
	    if (has_content(subNode)) {
		fltk::ImageType *it = fltk::guess_image((char *) subNode->children->content);
				
		if (it) iBitmap[iZ++] = it->get((char *) subNode->children->content, 0);
	    }
	    goto next;
	}
	
	if (!strcmp(name,"direction")) {
	    if (has_content(subNode)) iCW = !strcasecmp((char*)subNode->children->content,"cw");
	    goto next;
	}

	if (!strcmp(name,"color")) {
	    if (has_content(subNode)) iColor = fltk::color((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"width")) {
	    if (has_content(subNode)) iWidth = atoi((char*)subNode->children->content);
	    goto next;
	}
	
next:	subNode = subNode->next;
    }
}

bool CEqliptic::LuaSet(const char* var) {
    if (CProjection::LuaSet(var)) return true;
    
    if (strcmp(var, "bitmap") == 0 && lua.Type(-1) == LUA_TTABLE) {
	int iZ = 0;
    
	while (1) {
	    lua.RawGetI(3, iZ+1);
	    
	    if (lua.Type(-1) == LUA_TNIL || iZ > 12) {
		lua.Pop(1);
		break;
	    }
	
	    const char* name = lua.ToString(-1);
	    lua.Pop(1);

	    fltk::ImageType *it = fltk::guess_image((char *) name);
	    if (it) iBitmap[iZ++] = it->get((char *) name, 0);
	}
	return true;
    }
    return false;
}

void CEqliptic::DrawSelf(CDrawArea* area) {
    CProjection::DrawSelf(area);
    
    for (int i = 0; i < 12; i++) {
	int		x1, y1, x2, y2;
	fltk::Image*	image = iBitmap[i];
    
	area->GetXY(i*30, area->iSize, &x1,&y1);
	area->GetXY(i*30, area->iSize - iWidth, &x2,&y2);
	fltk::drawline(x1, y1, x2, y2);

	if (image) {
	    int w = 0, h = 0;

	    image->measure(w, h);
	    area->GetXY(i*30+15,area->iSize - iWidth/2, &x1,&y1);
	    iBitmap[i]->draw(x1 - w/2, y1 - h/2);
	}
    }

    area->iSize -= iWidth;
}

void CEqliptic::DrawMap(CMap* map, CDrawArea* area, bool active) {
    list<CPoint*>::iterator p = map->iPointsList.begin();
    
    while (p != map->iPointsList.end()) {
	(*p)->Projected()->Lon((*p)->Ecliptic()->Lon());
	(*p)->Projected()->Lat((*p)->Ecliptic()->Lat());
	p++;
    }

    CProjection::DrawMap(map, area, active);
}

void CEqliptic::GetXY(double lon, int r, int *x, int *y) {
    double start = 0;
    
    if (iStart) start = iStart->Ecliptic()->Lon();

    CProjection::GetXY(lon - start, r, x, y);
}

char* CEqliptic::InfoStr(CPoint* point) {
    char *text = point->Ecliptic()->LonStr();

    if (point->LonSpeed() < 0) {
        // We realloc strlen+2 due to NULL termination!
	text = (char*) realloc((void*) text, strlen(text)+2);
	strcat(text, "R");
    }

    return text;
}

//

void CHorizont::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    int		iZ = 0;
    
    while (subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name, "bitmap")) {
	    if (has_content(subNode)) {
		fltk::ImageType *it = fltk::guess_image((char *) subNode->children->content);
				
		if (it) iBitmap[iZ++] = it->get((char *) subNode->children->content, 0);
	    }
	    goto next;
	}
	
	if (!strcmp(name,"color")) {
	    if (has_content(subNode)) iColor = fltk::color((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"width")) {
	    if (has_content(subNode)) iWidth = atoi((char*)subNode->children->content);
	    goto next;
	}
	
next:	subNode = subNode->next;
    }
}

void CHorizont::DrawSelf(CDrawArea* area) {
    int i, l;

    CProjection::DrawSelf(area);
    
    for (i = 0, l = 45; i < 4; i++, l += 90) {
	int		x1, y1, x2, y2;
	fltk::Image*	image = iBitmap[i];
    
	area->GetXY(l, area->iSize, &x1,&y1);
	area->GetXY(l, area->iSize - iWidth, &x2,&y2);
	fltk::drawline(x1, y1, x2, y2);

	if (image) {
	    int w = 0, h = 0;

	    image->measure(w, h);
	    area->GetXY(i*90,area->iSize - iWidth/2, &x1,&y1);
	    iBitmap[i]->draw(x1 - w/2, y1 - h/2);
	}
    }
    area->iSize -= iWidth;
}

bool CHorizont::LuaSet(const char* var) {
    if (CProjection::LuaSet(var)) return true;
    
    if (strcmp(var, "bitmap") == 0 && lua.Type(-1) == LUA_TTABLE) {
	int iZ = 0;
    
	while (1) {
	    lua.RawGetI(3, iZ+1);
	    
	    if (lua.Type(-1) == LUA_TNIL || iZ > 4) {
		lua.Pop(1);
		break;
	    }
	
	    const char* name = lua.ToString(-1);
	    lua.Pop(1);

	    fltk::ImageType *it = fltk::guess_image((char *) name);
	    if (it) iBitmap[iZ++] = it->get((char *) name, 0);
	}
	return true;
    }
    return false;
}

#ifdef DBUS
void CHorizont::BeforeDraw() {
    dbus.NewSignal("/localspace/objects/clear", "ru.strijar", "localspace");
    dbus.Send();
}

void CHorizont::AfterDraw() {
    dbus.NewSignal("/localspace/objects/draw", "ru.strijar", "localspace");
    dbus.Send();
}
#endif

void CHorizont::DrawMap(CMap* map, CDrawArea* area, bool active) {
    list<CPoint*>::iterator	p;
    CMapNatal			*natal = (CMapNatal*) map;
    double			jd, jd_et;
    double			armc, eps, lat;
    double			x[6];
    
    jd = natal->JD();
    jd_et = jd + swe_deltat(jd);
    
    armc = swe_degnorm(swe_sidtime(jd) * 15.0 + natal->Place()->Lon() + 90.0);
    swe_calc(jd_et, SE_ECL_NUT, 0, x, NULL);
    eps = x[0];
    lat = 90 - natal->Place()->Lat();
    
#ifdef DBUS
    dbus.NewSignal("/localspace/objects/new", "ru.strijar", "localspace");
    dbus.AppendArg(natal->Place()->Lon());
    dbus.AppendArg(natal->Place()->Lat());
    dbus.AppendArg(natal->Color());
#endif
    
    for (p = natal->iPointsList.begin(); p != natal->iPointsList.end(); p++) {
	x[0] = (*p)->Ecliptic()->Lon();
	x[1] = (*p)->Ecliptic()->Lat();
	x[2] = 1;
	
	swe_cotrans(x, x, -eps);
	
	x[0] = x[0] - armc;
	x[2] = 1;
	
	/* azimuth from east, counterclock */
	swe_cotrans(x, x, lat);
	
	/* azimuth from south to west */
	double az = 360.0 - (x[0] - 90);
	
	(*p)->Projected()->Lon(az);
	(*p)->Projected()->Lat(x[1]);
	
	#ifdef DBUS
	if ((*p)->Visible()) {
	    dbus.AppendArg((*p)->Name());
	    dbus.AppendArg(az);
	    dbus.AppendArg((*p)->Color());
	}
	#endif
    }
    CProjection::DrawMap(map, area, active);
    #ifdef DBUS
    dbus.Send();
    #endif
}

void CHorizont::GetXY(double lon, int r, int *x, int *y) {
    double	tlon = lon - 90.0;

    tlon = tlon / 180.0 * PI;

    *x = r * cos(tlon);
    *y = r * sin(tlon);
}

char* CHorizont::InfoStr(CPoint* point) {
    char	buf[32];
    int		g, m;
    double	lon = point->Projected()->Lon();

    g = lon;
    m = (int)(lon * 60) % 60;
    sprintf(buf,"%03i %02i'", g, m);
    
    return strdup(buf);
}

//

CDial::CDial() {
    iSize = 360;
    iStep = 10;
    iFontSize = 14;
}

void CDial::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    
    while (subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"direction")) {
	    if (has_content(subNode)) iCW = !strcasecmp((char*)subNode->children->content,"cw");
	    goto next;
	}

	if (!strcmp(name,"color")) {
	    if (has_content(subNode)) iColor = fltk::color((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"width")) {
	    if (has_content(subNode)) iWidth = atoi((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"size")) {
	    if (has_content(subNode)) iSize = atof((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"step")) {
	    if (has_content(subNode)) iStep = atof((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"font_size")) {
	    if (has_content(subNode)) iFontSize = atoi((char*)subNode->children->content);
	    goto next;
	}
	
next:	subNode = subNode->next;
    }
}

bool CDial::LuaSet(const char* var) {
    if (CProjection::LuaSet(var)) return true;
    
    if (strcmp(var, "size") == 0) {
	iSize = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "step") == 0) {
	iStep = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "fontsize") == 0) {
	iFontSize = lua.ToNumber(-1);
	return true;
    }
    
    return false;
}

void CDial::DrawSelf(CDrawArea* area) {
    char	str[8];
    double	k = 360 / iSize;

    CProjection::DrawSelf(area);
    fltk::setfont(fltk::COURIER->bold(), iFontSize);

    for (double i = 0; i < iSize; i += iStep) {
	int		x1, y1, x2, y2, w = 0, h = 0;
    
	area->GetXY(i*k, area->iSize - iWidth/2, &x1,&y1);
	area->GetXY(i*k, area->iSize - iWidth/2, &x2,&y2);

	DoubleToStr(i, str);
	fltk::measure((const char*) str, w, h);
	fltk::drawtext(str, x1 - w/2, y1 + iFontSize/2);
    }

    area->iSize -= iWidth;
}

void CDial::DrawMap(CMap* map, CDrawArea* area, bool active) {
    double	k = 360 / iSize;
    list<CPoint*>::iterator p = map->iPointsList.begin();
    
    while (p != map->iPointsList.end()) {
	(*p)->Projected()->Lon((*p)->Ecliptic()->Lon() * k);
	(*p)->Projected()->Lat((*p)->Ecliptic()->Lat());
	p++;
    }

    CProjection::DrawMap(map, area, active);
}

void CDial::GetXY(double lon, int r, int *x, int *y) {
    CProjection::GetXY(lon - 90, r, x, y);
}

char* CDial::InfoStr(CPoint* point) {
    char	buf[32];
    int		g, m;
    double	lon = point->Projected()->Lon();

    g = lon * iSize / 360.0;
    m = (int)(lon * 60) % 60;
    sprintf(buf,"%03i %02i'", g, m);
    
    return strdup(buf);
}

