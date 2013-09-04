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

#include "map.h"
#include "map_natal.h"
#include "point.h"
#include "draw_area.h"
#include "utils.h"
#include "program.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "aspects.h"
#include "line.h"
#include <string>
#include <fltk/draw.h>
#include <fltk/SharedImage.h>

// CPoint

CPoint::CPoint() {
    iLatSpeed = 0;
    iLonSpeed = 0;
    iName = NULL;
    iCategory = NULL;
    iAspected = true;
    iAspecting = true;
    iView = true;
    iInfo = false;
    iMap = NULL;
    iId = 0;
    iStateNew = true;
    iColor = fltk::BLACK;
    iLonShift = 0;
    
    PushTable();
    lua.SetField(".ptr_point", this);
    lua.Pop(1);
}

CPoint::~CPoint() {
    if (iName) free(iName);
}

void CPoint::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;
    char	*name = NULL;

    // We in root?    
    while (subNode) {
	if (!strcmp((char*)subNode->name,"point")) break;
	subNode = subNode->next;
    }
    
    if (subNode) {	// Found in root
	name = (char*)xmlGetProp(subNode,(xmlChar *) "name");
	subNode = subNode->children;
    } else {		// Not found
	subNode = node;
	name = (char*)xmlGetProp(subNode,(xmlChar *) "name");
    }

    if (name && iStateNew) {
	Name(name);
	if (iName) free(iName);
	iName = strdup(name);
    }
    
    while (subNode) {
	char* name = (char *)subNode->name;

	if (!strcmp(name,"lua")) {
	    XMLLua(subNode);
	    goto next;
	}
	
	if (!strcmp(name,"file")) {
	    if (has_content(subNode))
		LoadFile((char *)subNode->children->content);
	    goto next;
	}
	
	if (!strcmp(name,"id")) {
	    if (has_content(subNode))
		iId = atoi((char *)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"category")) {
	    if (has_content(subNode))
		if (iCategory) free(iCategory);
		iCategory = strdup((char *)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"view")) {
	    if (has_content(subNode))
		iView = strcmp((char*)subNode->children->content,"true") == 0;
	    goto next;
	}

	if (!strcmp(name,"aspect")) {
	    if (has_content(subNode))
		iAspecting = iAspected = strcmp((char*)subNode->children->content,"true") == 0;
	    goto next;
	}

	if (!strcmp(name,"aspecting")) {
	    if (has_content(subNode))
		iAspecting = strcmp((char*)subNode->children->content,"true") == 0;
	    goto next;
	}

	if (!strcmp(name,"aspected")) {
	    if (has_content(subNode))
		iAspected = strcmp((char*)subNode->children->content,"true") == 0;
	    goto next;
	}

	if (!strcmp(name,"info")) {
	    if (has_content(subNode))
		iInfo = strcmp((char*)subNode->children->content,"true") == 0;
	    goto next;
	}
	    
	if (!strcmp(name,"color")) {
	    iColor = fltk::color((char *) subNode->children->content);
	}

	if (!strcmp(name,"lon") && has_content(subNode)) {
	    iEcliptic.Lon(atof((char*)subNode->children->content));
	    goto next;
	}

	if (!strcmp(name,"lat") && has_content(subNode)) {
	    iEcliptic.Lat(atof((char*)subNode->children->content));
	    goto next;
	}

	if (!strcmp(name,"lon_speed") && has_content(subNode)) {
	    iLonSpeed = atof((char*)subNode->children->content);
	    goto next;
	}

	if (!strcmp(name,"lat_speed") && has_content(subNode)) {
	    iLatSpeed = atof((char*)subNode->children->content);
	    goto next;
	}

next:	subNode = subNode->next;
    }
}

xmlNodePtr CPoint::XML(xmlNodePtr parent, int level) {
    xmlNodePtr	node;
    char	str[16];

    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level)));
    node = xmlNewTextChild(parent, NULL, (xmlChar*)"point", NULL);
    xmlNewProp(node, (xmlChar*)"name", (xmlChar*)iName);

    DoubleToStr(iEcliptic.Lon(), str);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    xmlNewTextChild(node, NULL, (xmlChar*)"lon", (xmlChar*)str);

    DoubleToStr(iEcliptic.Lat(), str);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    xmlNewTextChild(node, NULL, (xmlChar*)"lat", (xmlChar*)str);

    DoubleToStr(iLonSpeed, str);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    xmlNewTextChild(node, NULL, (xmlChar*)"lon_speed", (xmlChar*)str);

    DoubleToStr(iLatSpeed, str);
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level+1)));
    xmlNewTextChild(node, NULL, (xmlChar*)"lat_speed", (xmlChar*)str);
    
    xmlAddChild(node, xmlNewText((xmlChar*)xml_level(level)));

    return node;
}

void CPoint::Ready(bool data) {
    CAstro::Ready(data);
    
    iEquator.Ready(false);
}

CCoord* CPoint::Ecliptic() {
    return &iEcliptic;
}

CCoord* CPoint::Projected() {
    return &iProjected;
}

CCoord* CPoint::Equator() {
    if (! iEquator.Ready()) {
	double	xobl[6];
	char	serr[AS_MAXCH];  
	double	x[3];
    
	swe_calc(iMap->JD(), SE_ECL_NUT, 0, xobl, serr);
	
	x[0] = iEcliptic.Lon();
	x[1] = iEcliptic.Lat();
	x[2] = 1.0;
	
	swe_cotrans(x, x, -xobl[0]);
	
	iEquator.Lon(x[0]);
	iEquator.Lat(x[1]);
	
	iEquator.Ready(true);
    }
    
    return &iEquator;
}

bool CPoint::View() {
    return iView;
}

bool CPoint::Visible() {
    return iView;
}

bool CPoint::Aspected() {
    return iView && iAspected;
}

bool CPoint::Aspecting() {
    return iView && iAspecting;
}

void CPoint::Aspected(bool data) {
    iAspected = data;
}

void CPoint::Aspecting(bool data) {
    iAspecting = data;
}

bool CPoint::Info() {
    return iInfo;
}

void CPoint::View(bool data) {
    iView = data;
}

void CPoint::Info(bool data) {
    iInfo = data;
}

char* CPoint::Name() {
    return iName;
}

void CPoint::Name(char* name) {
    if (iName) free(iName);
    iName = strdup(name);

    PushTable();
    lua.SetField("name", iName);
    lua.Pop(1);
}

fltk::Color CPoint::Color() {
    return iColor;
}

const char* CPoint::Category() {
    return iCategory ? iCategory : "";
}

void CPoint::Map(CMap *data) {
    iMap = data;

    PushTable();
    lua.SetField("chart", iMap->IdInfo());
    lua.Pop(1);
}

CMap* CPoint::Map() {
    return iMap;
}

void CPoint::StateNew(bool data) {
    iStateNew = data;
}

double CPoint::LonSpeed() {
    return iLonSpeed;
}

double CPoint::LatSpeed() {
    return iLatSpeed;
}

void CPoint::LatSpeed(double data) {
    iLatSpeed = data;
}

void CPoint::LonSpeed(double data) {
    iLonSpeed = data;
}

void CPoint::operator<<(const CPoint& a) {
    iEcliptic = a.iEcliptic;

    iLatSpeed = a.iLatSpeed;
    iLonSpeed = a.iLonSpeed;
    iMap = a.iMap;
/*    
    if (iName) free(iName);
    iName = strdup(a.iName);
*/
}

void CPoint::DrawShift(double data) {
    iLonShift = data;
}

double CPoint::DrawShift() {
    return iLonShift;
}

int CPoint::DrawWidth() {
    return 0;
}

bool CPoint::LuaSet(const char* var) {
    if (strcmp(var, "view") == 0) {
	iView = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "info") == 0) {
	iInfo = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "aspected") == 0) {
	iAspected = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "aspecting") == 0) {
	iAspecting = lua.ToBool(-1);
	return true;
    } else if (strcmp(var, "color") == 0) {
	iColor = fltk::color(lua.ToString(-1));
	return true;
    } else if (strcmp(var, "id") == 0) {
	iId = lua.ToNumber(-1);
	return true;
    } else if (strcmp(var, "category") == 0) {
	if (iCategory) {
	    free(iCategory);
	}
	
	iCategory = strdup(lua.ToString(-1));
	return true;
    }
    
    return false;
}

bool CPoint::LuaGet(const char* var) {
    if (strcmp(var, "view") == 0) {
	lua.PushBool(iView);
	return true;
    } else if (strcmp(var, "info") == 0) {
	lua.PushBool(iInfo);
	return true;
    } else if (strcmp(var, "aspected") == 0) {
	lua.PushBool(iAspected);
	return true;
    } else if (strcmp(var, "aspecting") == 0) {
	lua.PushBool(iAspecting);
	return true;
    } else if (strcmp(var, "lon") == 0) {
	lua.PushNumber(iEcliptic.Lon());
	return true;
    } else if (strcmp(var, "lat") == 0) {
	lua.PushNumber(iEcliptic.Lat());
	return true;
    } else if (strcmp(var, "speed") == 0) {
	lua.PushNumber(iLonSpeed);
	return true;
    } 
    
    return false;
}

void CPoint::DrawInfo(CDrawArea* area) {
    if (iInfo) {
	char	*text;
	int	x = area->iInfoX;
	
	x += 15;

        text = area->InfoStr(this);
	fltk::drawtext(text,x, area->iInfoY + area->iFontSize);
	x += fltk::getwidth(text) + 5;
	fltk::drawtext(iName,x, area->iInfoY + area->iFontSize);
    
	area->iInfoY += area->iFontSize + 2;
	free(text);
    }
}

// CSwiss

CSwissPlanet::CSwissPlanet() {
    iBitmap = NULL;
    iBitmapSize = 0;
    iBitmapFile = NULL;

    iDotSize = 5;
    iDotSizeBorder = 2;
}

CSwissPlanet::~CSwissPlanet() {
//    if (iBitmap) delete iBitmap; FIXME!
    if (iBitmapFile) free(iBitmapFile);
}

void CSwissPlanet::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node->children;

    CPoint::XML(node);
    
    while(subNode) {
	char* name = (char *) subNode->name;
	
	if (!strcmp(name,"bitmap") && has_content(subNode)) {
	    fltk::ImageType *it = fltk::guess_image((char *) subNode->children->content);
				
	    if (it) {
		iBitmapFile = strdup((char *) subNode->children->content);
		iBitmap = it->get(iBitmapFile, 0);

		int w = 0, h = 0;
		iBitmap->measure(w, h);
		iBitmapSize = w>h ? w:h;
	    }
	}
	subNode = subNode->next;
    }
}

void CSwissPlanet::Calc() {
    Calc(iMap->JD());
}

void CSwissPlanet::Calc(double jd, int32 iflag) {
//    int32	iflag = SEFLG_SPEED /* | SEFLG_HELCTR */ ;
    double	data[6];
    char	serr[AS_MAXCH];  

    if (swe_calc_ut(jd, iId, iflag, data, serr) == ERR) {
	printf("CSwiss error %s\n",serr);
    } else {
	iEcliptic.Lon(data[0]);
	iEcliptic.Lat(data[1]);
	iLonSpeed = data[3];
	iLatSpeed = data[4];
    }
    iEquator.Ready(false);
}

int CSwissPlanet::DrawWidth() {
    return iBitmapSize;
}

void CSwissPlanet::Draw(CDrawArea* area) {
    int x, y, px, py;

    if (iBitmap) {
	area->GetXY(iProjected.Lon() + iLonShift, area->iSize - iMap->iWidth/2, &x,&y);
	px = iBitmap->w()/2;
	py = iBitmap->h()/2;
	iBitmap->draw(x - px, y - py);
    }
}

void CSwissPlanet::DrawInfo(CDrawArea* area) {
    char	*text;
	
    text = area->InfoStr(this);

    iBitmap->draw(area->iInfoX, area->iInfoY + (area->iFontSize - iBitmap->h()));
    fltk::drawtext(text,area->iInfoX+15, area->iInfoY + area->iFontSize);
    
    area->iInfoY += area->iFontSize + 2;
    free(text);
}

void CSwissPlanet::DrawPoint(CDrawArea* area) {
    if (iDotSize > 0) {
	int x, y, size = iDotSize;

	fltk::line_style(fltk::SOLID, iDotSizeBorder);
	fltk::setcolor(iMap->Color());
	area->GetXY(iProjected.Lon(),area->iSize, &x,&y);
	fltk::addarc(x - size/2, y - size/2, size, size, 0,-360); 
	fltk::fillstrokepath(iColor);
    }
}

fltk::Image* CSwissPlanet::Bitmap() {
    return iBitmap;
}

char* CSwissPlanet::BitmapFile() {
    return iBitmapFile;
}

bool CSwissPlanet::LuaSet(const char* var) {
    if (CPoint::LuaSet(var)) return true;
    
    if (strcmp(var, "dotsize") == 0) {
	double size = lua.ToNumber(-1);
    
        iDotSize = size;
        iDotSizeBorder = size/5 + 1;

        return true;
    } else if (strcmp(var, "bitmap") == 0) {
	const char* file = lua.ToString(-1);
    
	fltk::ImageType *it = fltk::guess_image(file);
				
	if (it) {
	    iBitmapFile = strdup(file);
	    iBitmap = it->get(iBitmapFile, 0);

	    int w = 0, h = 0;
	    iBitmap->measure(w, h);
	    iBitmapSize = w>h ? w:h;
	}
	return true;
    }
    return false;
}

bool CSwissPlanet::LuaGet(const char* var) {
    if (CPoint::LuaGet(var)) return true;
    
    if (strcmp(var, "dotsize") == 0) {
	lua.PushNumber(iDotSize);
	return true;
    } else if (strcmp(var, "bitmap") == 0) {
	lua.PushString(iBitmapFile);
	return true;
    }
    
    return false;
}

// CHouse

CHouse::CHouse() {
    iLineWidth = 1;
    iLineStyle = fltk::SOLID;
    LonSpeed(360.0);
    iLatSpeed = 0;
}

void CHouse::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node->children;

    CPoint::XML(node);
    
    while(subNode) {
	char* name = (char *) subNode->name;
	
	if (!strcmp(name,"line_width") && has_content(subNode))
	    iLineWidth = atoi((char *) subNode->children->content);
	    
	subNode = subNode->next;
    }
}

bool CHouse::LuaSet(const char* var) {
    if (CPoint::LuaSet(var)) return true;
    
    if (strcmp(var, "linewidth") == 0) {
	double size = lua.ToNumber(-1);
    
        iLineWidth = size;

        return true;
    }
    
    return false;
}

bool CHouse::LuaGet(const char* var) {
    if (CPoint::LuaGet(var)) return true;
    
    if (strcmp(var, "linewidth") == 0) {
	lua.PushNumber(iLineWidth);
	return true;
    }
    
    return false;
}


void CHouse::Calc() {
    iEcliptic.Lon(iMap->iCusps[iId]);
    iEcliptic.Lat(0);
    iEquator.Ready(false);
}

void CHouse::Draw(CDrawArea* area) {
    int x1, y1, x2, y2;
    
    if (iLineWidth) {    
	area->GetXY(iProjected.Lon(), area->iSize, &x1,&y1);
	area->GetXY(iProjected.Lon(), area->iSize - iMap->iWidth, &x2,&y2);
	fltk::line_style(fltk::SOLID, iLineWidth);
	fltk::setcolor(iColor);
	fltk::drawline(x1, y1, x2, y2);
    }
}

// CAxis

CAxis::CAxis() {
    iBitmap = NULL;
    iBitmapFile = NULL;
    iBitmapSize = 0;
    LonSpeed(360.0);
    iLatSpeed = 0;
}

CAxis::~CAxis() {
    if (iBitmapFile) free(iBitmapFile);
}

void CAxis::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node->children;

    CHouse::XML(node);
    
    while(subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"bitmap") && has_content(subNode)) {
	    fltk::ImageType *it = fltk::guess_image((char *) subNode->children->content);
				
	    if (it) {
		iBitmapFile = strdup((char *) subNode->children->content);
		iBitmap = it->get(iBitmapFile, 0);

		int w = 0, h = 0;
		iBitmap->measure(w, h);
		iBitmapSize = w>h ? w:h;
	    }
	}
	subNode = subNode->next;
    }
}

bool CAxis::LuaSet(const char* var) {
    if (CHouse::LuaSet(var)) return true;
    
    if (strcmp(var, "bitmap") == 0) {
	const char* file = lua.ToString(-1);
    
	fltk::ImageType *it = fltk::guess_image(file);
				
	if (it) {
	    iBitmapFile = strdup(file);
	    iBitmap = it->get(iBitmapFile, 0);

	    int w = 0, h = 0;
	    iBitmap->measure(w, h);
	    iBitmapSize = w>h ? w:h;
	}
	return true;
    }
    
    return false;
}

bool CAxis::LuaGet(const char* var) {
    if (CHouse::LuaGet(var)) return true;

    if (strcmp(var, "bitmap") == 0) {
	lua.PushString(iBitmapFile);
	return true;
    }
    
    return false;
}


void CAxis::Calc() {
    iEcliptic.Lon(iMap->iAxis[iId]);
    iEcliptic.Lat(0);
    iEquator.Ready(false);
}

void CAxis::Draw(CDrawArea* area) {
    CHouse::Draw(area);

    if (iBitmap) {
        int x, y;

	area->GetXY(iProjected.Lon(), area->iSize - iMap->iWidth/2, &x,&y);
	iBitmap->draw(x - iBitmap->w()/2, y - iBitmap->h()/2);
    }
}

void CAxis::DrawInfo(CDrawArea* area) {
    if (iBitmap) {
	char	*text;

	text = area->InfoStr(this);
	iBitmap->draw(area->iInfoX, area->iInfoY + (area->iFontSize - iBitmap->h()));
	fltk::drawtext(text,area->iInfoX+15, area->iInfoY + area->iFontSize);
	area->iInfoY += area->iFontSize + 2;
	free(text);
    }
}

fltk::Image* CAxis::Bitmap() {
    return iBitmap;
}

char* CAxis::BitmapFile() {
    return iBitmapFile;
}

// CStar

CStar::CStar() {
    iPrevJD = 0;
}

void CStar::Calc() {
    int32	iflag = 0;
    double	jd;
    char	serr[AS_MAXCH];  
    char	name[64];

    iTurnOn = false;
    jd = iMap->JD();
    
    if (fabs(jd - iPrevJD) > 3) {
	strcpy(name,iName);

	if (swe_fixstar_ut(name, jd, iflag, iData, serr) == ERR) {
	    printf("CSwiss error %s\n",serr);
	    return;
	} else iPrevJD = jd;
    }

    Ready(true);
    iEcliptic.Lon(iData[0]);
    iEcliptic.Lat(iData[1]);
    iLonSpeed = 0;
    iLatSpeed = 0;
}

void CStar::Ready(bool data) {
    CSwissPlanet::Ready(data);
    iTurnOn = false;
}

void CStar::HasAspected(CAspectData* aspect) {
    iTurnOn = true;
}

bool CStar::Visible() {
    return iTurnOn && iView;
}

void CStar::DrawInfo(CDrawArea* area) {
    if (iTurnOn && iInfo) {
	char	*text;
	int	x = area->iInfoX;
	
	text = area->InfoStr(this);

	iBitmap->draw(x, area->iInfoY + (area->iFontSize - iBitmap->h()));
	x += 15;
	fltk::drawtext(text,x, area->iInfoY + area->iFontSize);
	x += fltk::getwidth(text) + 5;
	fltk::drawtext(iName,x, area->iInfoY + area->iFontSize);
    
	area->iInfoY += area->iFontSize + 2;
	free(text);
    }
}

// Lua

CLuaPoint::CLuaPoint() {
    iPersist = true;
    iTurnOn = false;

    iDotSize = 5;
    iDotSizeBorder = 2;
}

void CLuaPoint::Draw(CDrawArea* area) {
    if (iBitmap) {
        int x, y;

	area->GetXY(iProjected.Lon() + iLonShift, area->iSize - iMap->iWidth/2, &x,&y);
	iBitmap->draw(x - iBitmap->w()/2, y - iBitmap->h()/2);
    } else {
	CHouse::Draw(area);
    }
}

void CLuaPoint::DrawInfo(CDrawArea* area) {
    if (Visible() && iInfo) {
	char	*text;
	int	x = area->iInfoX;
	
	if (iBitmap) {
	    iBitmap->draw(x, area->iInfoY + (area->iFontSize - iBitmap->h()));
	}
	x += 15;

        text = area->InfoStr(this);
	fltk::drawtext(text,x, area->iInfoY + area->iFontSize);
	x += fltk::getwidth(text) + 5;
	
	if (!iBitmap) fltk::drawtext(iName,x, area->iInfoY + area->iFontSize);
    
	area->iInfoY += area->iFontSize + 2;
	free(text);
    }
}

void CLuaPoint::DrawPoint(CDrawArea* area) {
    if (iDotSize > 0) {
	int x, y, size = iDotSize;

	fltk::line_style(fltk::SOLID, iDotSizeBorder);
	fltk::setcolor(iMap->Color());
	area->GetXY(iProjected.Lon(),area->iSize, &x,&y);
	fltk::addarc(x - size/2, y - size/2, size, size, 0,-360); 
	fltk::fillstrokepath(iColor);
    }
}

void CLuaPoint::Calc() {
    CLuaChunk	*chunk = iLuaChunk["CALC"];
    double 	lon = 0;
    double 	lat = 0;

    if (chunk) {
	chunk->Push();

	// First arg self
	PushTable();
    } else {
	PushTable();
	lua.PushString("CALC");
	lua.GetTable(-2);
	
	if (lua.Type(-1) == LUA_TFUNCTION) {
	    lua.PushValue(-2);	// self
	    lua.Remove(-3);	// table
	} else {
	    lua.Pop(2);
	    /*
	    lua.Remove(-1);	// nil
	    lua.Remove(-1); 	// table
	    */
	    
	    return;
	}
    }

    // Second arg chart
    iMap->PushTable();
    iMap->Lua();

    lua.Call(2);

    if (lua.GetNumber(&iLonSpeed)) {
        if (lua.GetNumber(&lat)) {
    	    iEcliptic.Lat(lat);
		
	    if (lua.GetNumber(&lon)) {
	        iEcliptic.Lon(lon);
	        Ready(true);
	    } else {
	        printf("Lua point: Lon error\n");
	    }
	} else {
	    printf("Lua point: Lat error\n");
	}
    } else {
        printf("Lua point: Speed error\n");
    }
}

bool CLuaPoint::LuaSet(const char* var) {
    if (CAxis::LuaSet(var)) return true;
    
    if (strcmp(var, "dotsize") == 0) {
	double size = lua.ToNumber(-1);
    
        iDotSize = size;
        iDotSizeBorder = size/5 + 1;

        return true;
    } else if (strcmp(var,"persist") == 0) {
	iPersist = lua.ToBool(-1);
	return true;
    }
    
    return false;
}

bool CLuaPoint::LuaGet(const char* var) {
    if (CAxis::LuaGet(var)) return true;
    
    if (strcmp(var, "dotsize") == 0) {
	lua.PushNumber(iDotSize);
	return true;
    } else if (strcmp(var, "persist") == 0) {
	lua.PushBool(iPersist);
	return true;
    }
    
    return false;
}

void CLuaPoint::Ready(bool data) {
    CAxis::Ready(data);
    
    if (!iPersist) iTurnOn = false;
}

void CLuaPoint::HasAspected(CAspectData* aspect) {
    iTurnOn = true;
}

void CLuaPoint::Touch() {
    iTurnOn = false;
}

bool CLuaPoint::Visible() {
    return iPersist ? iView : iTurnOn && iView;
}

int CLuaPoint::DrawWidth() {
    return iBitmapSize;
}

void CLuaPoint::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node->children;

    CAxis::XML(node);
    
    while(subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"persist")) {
	    iPersist = true;
	}
	subNode = subNode->next;
    }
}

// MidPoint

CMidPoint::CMidPoint() {
    iPoint1 = NULL;
    iPoint2 = NULL;
    iLine = NULL;
}

void CMidPoint::Calc() {
    if (iPoint1 && iPoint2) {
	double l1, l2;
	
	l1 = iPoint1->Ecliptic()->Lon();
	l2 = iPoint2->Ecliptic()->Lon();
	iEcliptic.Lon(l1 + Dist(l1, l2)/2);

	l1 = iPoint1->Ecliptic()->Lat();
	l2 = iPoint2->Ecliptic()->Lat();
	iEcliptic.Lat((l1 + l2)/2.0);

	l1 = iPoint1->LonSpeed();
	l2 = iPoint2->LonSpeed();
	iLonSpeed = (l1 + l2)/2.0;
	
	Ready(true);
    }
}

void CMidPoint::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node->children;

    CLuaPoint::XML(node);
    
    while(subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"point") && has_content(subNode)) {
	    if (iPoint1 == NULL) {
		iPoint1 = iMap->iPointsMap[(char*)subNode->children->content];
	    } else {
		iPoint2 = iMap->iPointsMap[(char*)subNode->children->content];
	    }
	    goto next;
	}

	if (!strcmp(name,"line")) {
	    if (!iLine) iLine = new CLine;
	    
	    iLine->XML(subNode->children);
	    goto next;
	}
	
next:	subNode = subNode->next;
    }
}

void CMidPoint::DrawPoint(CDrawArea* area) {
    CLuaPoint::DrawPoint(area);
    
    if (iLine) {
	iLine->DrawHord(iPoint1->Projected()->Lon(), iPoint2->Projected()->Lon(), area);
    }
}
