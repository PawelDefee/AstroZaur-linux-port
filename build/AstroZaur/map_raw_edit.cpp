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

#include "map_raw.h"
#include "map_raw_edit.h"
#include "program.h"
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/events.h>

static void PointLonCB(fltk::Input* item, CPoint* point) {
    point->Ecliptic()->Lon(atof((char*)item->value()));
    point->DrawShift(0);

    program.CalcMaps();
    program.Draw();
}

static void PointLatCB(fltk::Input* item, CPoint* point) {
    point->Ecliptic()->Lat(atof((char*)item->value()));

    program.CalcMaps();
    program.Draw();
}

static void PointLonSpeedCB(fltk::Input* item, CPoint* point) {
    point->LonSpeed(atof((char*)item->value()));

    program.CalcMaps();
    program.Draw();
}

static void PointLatSpeedCB(fltk::Input* item, CPoint* point) {
    point->LatSpeed(atof((char*)item->value()));

    program.CalcMaps();
    program.Draw();
}

// Editor class

CMapRawEdit::~CMapRawEdit() {
    iInput.clear();
}

void CMapRawEdit::InsertContent(fltk::MenuBar* menu) {
    CMapEdit::InsertContent(menu);

    fltk::ItemGroup* g = new fltk::ItemGroup(program.Msg("Points"));
    g->resize(0,0,200,23);
    g->begin();
	
    list<CPoint*>::iterator point = iMap->iPointsList.begin();
    
    while (point != iMap->iPointsList.end()) {
	fltk::Input* 		i;
	fltk::ItemGroup* 	g = new fltk::ItemGroup((*point)->Name());
	pointInputType*		input = new pointInputType;
	
	g->resize(0,0,200,23);
	g->begin();
	
	i = new fltk::Input(0, 0, 200, 23, program.Msg("Lon"));
        i->callback((fltk::Callback*)PointLonCB, (void*)(*point));
	i->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
	input->iLon = i;
	
	i = new fltk::Input(0, 0, 200, 23, program.Msg("Lat"));
        i->callback((fltk::Callback*)PointLatCB, (void*)(*point));
	i->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
	input->iLat = i;

	i = new fltk::Input(0, 0, 200, 23, program.Msg("Lon speed"));
        i->callback((fltk::Callback*)PointLonSpeedCB, (void*)(*point));
	i->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
	input->iLonSpeed = i;

	i = new fltk::Input(0, 0, 200, 23, program.Msg("Lat speed"));
        i->callback((fltk::Callback*)PointLatSpeedCB, (void*)(*point));
	i->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
	input->iLatSpeed = i;

	iInput[(*point)] = input;

	g->end();
	point++;
    }
    g->end();
}

void CMapRawEdit::SetData() {
    CMapEdit::SetData();

    list<CPoint*>::iterator point = iMap->iPointsList.begin();
    
    while (point != iMap->iPointsList.end()) {
	char		str[16];
	pointInputType*	input = iInput[(*point)];
	
	DoubleToStr((*point)->Ecliptic()->Lon(), str);
	input->iLon->value(str);

	DoubleToStr((*point)->Ecliptic()->Lat(), str);
	input->iLat->value(str);

	DoubleToStr((*point)->LonSpeed(), str);
	input->iLonSpeed->value(str);

	DoubleToStr((*point)->LatSpeed(), str);
	input->iLatSpeed->value(str);
	
	point++;
    }
}
