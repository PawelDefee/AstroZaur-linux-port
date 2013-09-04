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

#include "map_natal.h"
#include "map_natal_edit.h"
#include "program.h"
#include "atlas.h"
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/Button.h>
#include <fltk/PopupMenu.h>
#include <fltk/events.h>
#include <fltk/ask.h>

#include <stdio.h>

// Callbacks

typedef enum {
    widgetMoment = 0,
    widgetPlace
} widgetType;

typedef struct {
    widgetType		type;
    CAstro*		obj;
    CMapNatal*		map;
} clonedItemType;

static void ClonedCB(fltk::Item* item, CMapNatalEdit* edit) {
    edit->ClonedClick(item);
}

static void MapNatalDateCB(fltk::Input* input, CMapNatal* map) {
    map->Date()->Date(input->value());
    program.CalcMaps();
    program.Draw();
}

static void MapNatalTimeCB(fltk::Input* input, CMapNatal* map) {
    map->Date()->Time(input->value());
    program.CalcMaps();
    program.Draw();
}

static void MapNatalTimeZoneCB(fltk::Input* input, CMapNatal* map) {
    map->Date()->TimeZone(input->value());
    program.CalcMaps();
    program.Draw();
}

static void MapNatalPlaceCB(fltk::Input* input, CMapNatal* map) {
    map->Place()->Info(input->value());
    program.Draw();
}

static void MapNatalLonCB(fltk::Input* input, CMapNatal* map) {
    map->Place()->LonStr(input->value());
    program.CalcMaps();
    program.Draw();
}

static void MapNatalLatCB(fltk::Input* input, CMapNatal* map) {
    map->Place()->LatStr(input->value());
    program.CalcMaps();
    program.Draw();
}

static void MapNatalHouseCB(fltk::Choice* input, CMapNatal* map) {
    map->HouseSys(input->value());
    program.CalcMaps();
    program.Draw();
}

static void AtlasCB(fltk::Item* item, CMapNatalEdit* edit) {
    edit->OpenAtlas();
}

// Editor class

CMapNatalEdit::CMapNatalEdit() {
    iAtlas = NULL;
}

CMapNatalEdit::~CMapNatalEdit() {
    if (iAtlas) delete iAtlas;
}

void CMapNatalEdit::OpenAtlas() {
    if (!iAtlas) {
	iAtlas = new CAtlas;
	iAtlas->Open(program.AtlasFile(), (CMapNatal*) iMap);
    }
    iAtlas->Show();
}

void CMapNatalEdit::InsertContent(fltk::MenuBar* menu) {
    CMapEdit::InsertContent(menu);
    
    iClonedGroup = (fltk::ItemGroup*) menu->find(program.Msg("Cloned/"));
    if (iClonedGroup == NULL) {
	menu->add(program.Msg("Cloned/<None>"), 0, (fltk::Callback *)ClonedCB, (void*)(this));
	
	iClonedGroup = (fltk::ItemGroup*) menu->find(program.Msg("Cloned/"));
	iClonedGroup->deactivate();

	for (unsigned int i = 0; i < program.MapNum(); i++)
	    AddMenuItem(program.Map(i));

    } else {
	iMenu = menu;
    }
    menu->add(program.Msg("Atlas"), 0, (fltk::Callback *)AtlasCB, (void*)(this));
    
    iMomentGroup = new fltk::ItemGroup(program.Msg("Moment"));
    iMomentGroup->resize(0,0,200,23);
    iMomentGroup->begin();
    iMomentGroup->set_flag(fltk::OPENED);

    iDate = new fltk::Input(0, 0, 200, 23, program.Msg("Date"));
    iDate->callback((fltk::Callback*)MapNatalDateCB, (void*)iMap);
    iDate->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
        
    iTime = new fltk::Input(0, 0, 200, 23, program.Msg("Time"));
    iTime->callback((fltk::Callback*)MapNatalTimeCB, (void*)iMap);
    iTime->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
	
    iTimeZone = new fltk::Input(0, 0, 200, 23, program.Msg("Zone"));
    iTimeZone->callback((fltk::Callback*)MapNatalTimeZoneCB, (void*)iMap);
    iTimeZone->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);

    iTimeOffset = new fltk::Output(0, 0, 200, 23, program.Msg("Offset"));
    iTimeOffset->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);

    iMomentGroup->end();
	
    iPlaceGroup = new fltk::ItemGroup(program.Msg("Place"));
    iPlaceGroup->resize(0,0,200,23);
    iPlaceGroup->begin();
    iPlaceGroup->set_flag(fltk::OPENED);

    iPlace = new fltk::Input(0, 0, 200, 23, program.Msg("Info"));
    iPlace->callback((fltk::Callback*)MapNatalPlaceCB, (void*)iMap);
    iPlace->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);

    iLon = new fltk::Input(0, 0, 200, 23, program.Msg("Lon"));
    iLon->callback((fltk::Callback*)MapNatalLonCB, (void*)iMap);
    iLon->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);

    iLat = new fltk::Input(0, 0, 200, 23, program.Msg("Lat"));
    iLat->callback((fltk::Callback*)MapNatalLatCB, (void*)iMap);
    iLat->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);

    iPlaceGroup->end();

    iHouse = new fltk::Choice(0, 0, 200, 23, program.Msg("House"));
    iHouse->callback((fltk::Callback*)MapNatalHouseCB, (void*)iMap);
    iHouse->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
    iHouse->when(fltk::WHEN_CHANGED);
    iHouse->begin();
    new fltk::Item("Placidus");
    new fltk::Item("Koch");
    new fltk::Item("Regiomontan");
    new fltk::Item("Alcabitus");
    new fltk::Item("Porphyrius");
    new fltk::Item("Campanus");
    new fltk::Item("Equal (Asc)");
    new fltk::Item("Morinus");
    new fltk::Item("Vehlow equal");
    new fltk::Item("Axial rotation");
    new fltk::Item("Azimuthal (horizontal)");
    new fltk::Item("Polich/Page (topocentric)");
    
    iHouse->end();

    {
	clonedItemType* item = (clonedItemType*) malloc(sizeof(clonedItemType));
	item->type = widgetMoment;
	item->obj = ((CMapNatal*)iMap)->Date();
	item->map = (CMapNatal*)iMap;
	iMomentGroup->user_data(item);
    }{
	clonedItemType* item = (clonedItemType*) malloc(sizeof(clonedItemType));
	item->type = widgetPlace;
	item->obj = ((CMapNatal*)iMap)->Place();
	item->map = (CMapNatal*)iMap;
	iPlaceGroup->user_data(item);
    }
}

void CMapNatalEdit::SetMomentLabel() {
    char	str[128];
    CMapNatal*	natal;
    
    natal = (CMapNatal*) ((CMapNatal*)iMap)->Date()->Owner();
    strcpy(str, program.Msg("Moment"));
    if (natal != iMap) {
	strcat(str, " (");
	strcat(str, natal->Info());
	strcat(str, ")");
    }
    iMomentGroup->copy_label(str);
}

void CMapNatalEdit::SetPlaceLabel() {
    char	str[128];
    CMapNatal*	natal;
    
    natal = (CMapNatal*) ((CMapNatal*)iMap)->Place()->Owner();
    strcpy(str, program.Msg("Place"));
    if (natal != iMap) {
	strcat(str, " (");
	strcat(str, natal->Info());
	strcat(str, ")");
    }
    iPlaceGroup->copy_label(str);
}

void CMapNatalEdit::SetData() {
    CMapEdit::SetData();
    
    iDate->value(((CMapNatal*)iMap)->Date()->Date());
    iTime->value(((CMapNatal*)iMap)->Date()->Time());
    iTimeZone->value(((CMapNatal*)iMap)->Date()->TimeZone());
    iTimeOffset->value(((CMapNatal*)iMap)->Date()->TimeOffset());
    SetMomentLabel();       
       
    iPlace->value(((CMapNatal*)iMap)->Place()->Info());
    iLon->value(((CMapNatal*)iMap)->Place()->LonStr());
    iLat->value(((CMapNatal*)iMap)->Place()->LatStr());
    SetPlaceLabel();     
     
    iHouse->value(((CMapNatal*)iMap)->HouseSys());
}

void CMapNatalEdit::Event(CAstro* obj, eventEnum event, void* data) {
    CMapEdit::Event(obj, event, data);
    
    switch (event) {
	case eventChangeChartInfo:
	    {
		if (obj == ((CMapNatal*)iMap)->Date()->Owner()) {
		    SetMomentLabel();
		}
		
		if (obj == ((CMapNatal*)iMap)->Place()->Owner()) {
		    SetPlaceLabel();
		}

		CMap*		map = (CMap*) obj;
		fltk::Widget*	item = iClonedItems[map];
    
		if (item) {
		    item->label(map->Info());
		    item->w(0);
		}
		if (obj == iMap && iAtlas) iAtlas->Title(iMap->Info());
	    }
	    break;

	case eventChangeDate:
	    if (obj == ((CMapNatal*)iMap)->Date()) {
		iDate->value(((CMapNatal*)iMap)->Date()->Date());
		iDate->redraw();
	    }
	    break;

	case eventChangeTime:
	    if (obj == ((CMapNatal*)iMap)->Date()) {
		iTime->value(((CMapNatal*)iMap)->Date()->Time());
		iTime->redraw();
	    }
	    break;

	case eventChangePlaceInfo:
	    if (obj == ((CMapNatal*)iMap)->Place()) {
		iPlace->value(((CMapNatal*)iMap)->Place()->Info());
		iPlace->redraw();
	    }
	    break;

	case eventChangePlaceLon:
	    if (obj == ((CMapNatal*)iMap)->Place()) {
		iLon->value(((CMapNatal*)iMap)->Place()->LonStr());
		iLon->redraw();
	    }
	    break;

	case eventChangePlaceLat:
	    if (obj == ((CMapNatal*)iMap)->Place())
		iLat->value(((CMapNatal*)iMap)->Place()->LatStr());
		iLat->redraw();
	    break;

	case eventChangeTimeZone:
	    if (obj == ((CMapNatal*)iMap)->Date()) {
		iTimeZone->value(((CMapNatal*)iMap)->Date()->TimeZone());
		iTimeOffset->value(((CMapNatal*)iMap)->Date()->TimeOffset());
		iTimeZone->redraw();
		iTimeOffset->redraw();
	    }
	    break;

	case eventNewChart:
	    AddMenuItem((CMap*)obj);
	    break;
	    
	default:
	    break;
    }
}

bool CMapNatalEdit::Cloned(fltk::Widget* widget) {
    return widget == iMomentGroup || widget == iPlaceGroup;
}

void CMapNatalEdit::BrowserClick(fltk::Widget* widget) {
    if (Cloned(widget)) {
	iClonedGroup->activate();
	iClonedWidget = widget;
    } else {
	iClonedGroup->deactivate();
    }
    iMenu->redraw();
}

void CMapNatalEdit::ClonedClick(fltk::Item* item) {
    CMapNatal*		map = (CMapNatal*) program.Map((char*)item->label());
    clonedItemType* 	cloned = (clonedItemType*) iClonedWidget->user_data();
    
    if (cloned->type == widgetMoment) {
	if (map) {
	    if (map == cloned->map) {
		fltk::message(program.Msg("It's impossible!"));
		return;
	    }
	    cloned->map->Date(map->Date());
	    map->AddDepends(cloned->map);
	} else {
	    CDate* date = new CDate;
	    date->Owner(cloned->map);
	    cloned->map->Date(date);
	}
	SetData();
	iBrowser->redraw();
	cloned->map->Ready(false);
	program.CalcMaps();
	program.Draw();
    }

    if (cloned->type == widgetPlace) {
	if (map) {
	    if (map == cloned->map) {
		fltk::message(program.Msg("It's impossible!"));
		return;
	    }
	    cloned->map->Place(map->Place());
	    map->AddDepends(cloned->map);
	} else {
	    CPlace* place = new CPlace;
	    place->Owner(cloned->map);
	    cloned->map->Place(place);
	}
	SetData();
	iBrowser->redraw();
	cloned->map->Ready(false);
	program.CalcMaps();
	program.Draw();
    }
}

void CMapNatalEdit::AddMenuItem(CMap* map) {
    char* 		info = strdup(map->Info());
    fltk::Widget*	item;

    item = iClonedGroup->add(info, 0, (fltk::Callback *)ClonedCB, (void*)(this));
    iClonedItems[map] = item;
}
