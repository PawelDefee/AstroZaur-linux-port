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

#include "map_edit.h"
#include "program.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "lua_gui.h"
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/Choice.h>
#include <fltk/ColorChooser.h>
#include <fltk/events.h>

#include <stdio.h>

typedef struct {
    CMapEdit*	editor;
    CPoint*	point;
} pointItemType;

// Callbacks

static void BrowserCB(fltk::Browser* browser, CMapEdit* edit) {
    fltk::Widget*	item = browser->item();
    
    if (item) {
	item->do_callback();
	edit->BrowserClick(item);
    }
}

static void MapInfoCB(fltk::Input* input, CMap* map) {
    map->Info(input->value());
    program.Draw();
}

static void MapIdCB(fltk::Input* input, CMap* map) {
    map->Id(input->value());
    program.Draw();
}

static void MapColorCB(fltk::Widget* input, CMap* map) {
    fltk::Color color = map->Color();
    
    if (fltk::color_chooser("Chart color", color)) {
	map->Color(color);
	input->color(color);
	program.Draw();
    }
}

static void PointListSelectCB(fltk::Item* item, pointItemType* data) {
    int			selected = 0;

    list<fltk::Widget*>::iterator w = data->editor->iPointWidget.begin();
    
    while (w != data->editor->iPointWidget.end()) {
	if ((*w)->selected())
	    if (selected++ > 1) break;
	w++;
    }

    if (selected) {
	if (selected == 1) {
	    data->editor->iPointAspected->value(data->point->Aspected());
	    data->editor->iPointAspecting->value(data->point->Aspecting());
	    data->editor->iPointView->value(data->point->View());
	    data->editor->iPointInfo->value(data->point->Info());
	} else {
	    data->editor->iPointAspected->value(0);
	    data->editor->iPointAspecting->value(0);
	    data->editor->iPointView->value(0);
	    data->editor->iPointInfo->value(0);
	}
    }
}

static void PointMenuCB(fltk::Item* item, fltk::CheckButton* button) {
    button->value(! button->value());
    button->do_callback();
}

static void PointViewButtonCB(fltk::CheckButton* button, CMapEdit* editor) {
    list<fltk::Widget*>::iterator w = editor->iPointWidget.begin();
    
    while (w != editor->iPointWidget.end()) {
	if ((*w)->selected()) {
	    pointItemType* data = (pointItemType*) (*w)->user_data();
	    
	    data->point->View(button->value());
	}
	w++;
    }
    
    program.CalcMaps();
    program.Draw();
}

static void PointAspectedButtonCB(fltk::CheckButton* button, CMapEdit* editor) {
    list<fltk::Widget*>::iterator w = editor->iPointWidget.begin();
    
    while (w != editor->iPointWidget.end()) {
	if ((*w)->selected()) {
	    pointItemType* data = (pointItemType*) (*w)->user_data();
	    
	    data->point->Aspected(button->value());
	}
	w++;
    }
    
    program.CalcMaps();
    program.Draw();
}

static void PointAspectingButtonCB(fltk::CheckButton* button, CMapEdit* editor) {
    list<fltk::Widget*>::iterator w = editor->iPointWidget.begin();
    
    while (w != editor->iPointWidget.end()) {
	if ((*w)->selected()) {
	    pointItemType* data = (pointItemType*) (*w)->user_data();
	    
	    data->point->Aspecting(button->value());
	}
	w++;
    }
    
    program.CalcMaps();
    program.Draw();
}

static void PointInfoButtonCB(fltk::CheckButton* button, CMapEdit* editor) {
    list<fltk::Widget*>::iterator w = editor->iPointWidget.begin();
    
    while (w != editor->iPointWidget.end()) {
	if ((*w)->selected()) {
	    pointItemType* data = (pointItemType*) (*w)->user_data();
	    
	    data->point->Info(button->value());
	}
	w++;
    }
    
    program.CalcMaps();
    program.Draw();
}

// Editor class

CMapEdit::CMapEdit() {
    iWindow = NULL;
    iType = "editor";
}

CMapEdit::~CMapEdit() {
    iPointWidget.clear();
}

void CMapEdit::Map(CMap* map) {
    iMap = map;
}

void CMapEdit::MakeWindow() {
    iWindow = new CAstroWindow(360, 210, iMap->Type());

    PushTable();

    iWindow->user_data((void*)(this));
    iWindow->begin();

    lua.NewTable("window");
    gui_group(lua.State(), iWindow);
    lua.EndTable();
    
    iMenu = new fltk::MenuBar(0, 0, 360, 25);

    lua.NewTable("menu");
    gui_group(lua.State(), iMenu);
    lua.EndTable();
    
    iBrowser = new fltk::MultiBrowser(0, 25, 360, 210-25);
    iBrowser->indented(true);
    iBrowser->callback((fltk::Callback*)BrowserCB, (void*)this);
    iBrowser->when(fltk::WHEN_CHANGED);

    iWindow->resizable(iBrowser);
    iBrowser->begin();

    InsertContent(iMenu);

    fltk::ItemGroup* g;

    g = new fltk::ItemGroup(program.Msg("Points control"));
    g->resize(0,0,200,23);
    g->begin();
    g->set_flag(fltk::OPENED);

    iPointView = new fltk::CheckButton(0, 0, 200, 23, program.Msg("View"));
    iPointAspected = new fltk::CheckButton(0, 0, 200, 23, program.Msg("Aspected"));
    iPointAspecting = new fltk::CheckButton(0, 0, 200, 23, program.Msg("Aspecting"));
    iPointInfo = new fltk::CheckButton(0, 0, 200, 23, program.Msg("Info"));

    iPointView->callback((fltk::Callback*)PointViewButtonCB, (void*)this);
    iPointAspected->callback((fltk::Callback*)PointAspectedButtonCB, (void*)this);
    iPointAspecting->callback((fltk::Callback*)PointAspectingButtonCB, (void*)this);
    iPointInfo->callback((fltk::Callback*)PointInfoButtonCB, (void*)this);

    iPointList = new fltk::ItemGroup(program.Msg("Points"));
    iPointList->resize(0,0,200,23);
    iPointList->set_flag(fltk::OPENED);
    
    g->end();

    iMenu->begin();
    {
	fltk::ItemGroup* g = new fltk::ItemGroup(program.Msg("Points"));
	g->begin();
	{
	    fltk::Item* i = new fltk::Item(program.Msg("View"));
	    i->shortcut(fltk::ALT + 'v');
	    i->callback((fltk::Callback*)PointMenuCB, (void*)iPointView);
	}{
	    fltk::Item* i = new fltk::Item(program.Msg("Aspected"));
	    i->shortcut(fltk::ALT + 'd');
	    i->callback((fltk::Callback*)PointMenuCB, (void*)iPointAspected);
	}{
	    fltk::Item* i = new fltk::Item(program.Msg("Aspecting"));
	    i->shortcut(fltk::ALT + 'g');
	    i->callback((fltk::Callback*)PointMenuCB, (void*)iPointAspecting);
	}{
	    fltk::Item* i = new fltk::Item(program.Msg("Info"));
	    i->shortcut(fltk::ALT + 'i');
	    i->callback((fltk::Callback*)PointMenuCB, (void*)iPointInfo);
	}	
	g->end();
    }
    iMenu->end();
    
    list<CPoint*>::iterator 	point = iMap->iPointsList.begin();
    
    while (point != iMap->iPointsList.end()) {
	InsertPoint((*point));
	point++;
    }
    
    iBrowser->end();
    iWindow->end();
    
    lua.Pop(1);
}

void CMapEdit::InsertPoint(CPoint* point) {
    char		str[128];
    fltk::Widget*	item;
    pointItemType*	data = new pointItemType;

    data->editor = this;
    data->point = point;
	
    strcpy(str, point->Category());
    if (strlen(str)) {
        strcat(str, "/");
	strcat(str, point->Name());
    } else {
	strcpy(str, point->Name());
    }
	
    item = iPointList->add(str, 0, (fltk::Callback*)PointListSelectCB, (void*)(data));
    item->image(point->Bitmap());
	
    iPointWidget.push_back(item);
}

void CMapEdit::Event(CAstro* obj, eventEnum event, void* data) {
    if (obj == iMap) {
	switch (event) {
	    case eventNewPoint: {
		CPoint* point = (CPoint*) data;
		InsertPoint(point);
		break;
	    }
		
	    case eventChangeChartInfo:
		iInfo->value(iMap->Info());
		break;

	    case eventChangeChartId:
		iId->value(iMap->Id());
		break;

	    case eventChangeChartColor:
		iColor->color(iMap->Color());
		iColor->redraw();
		break;
		
	    default:
		break;
	}
    }
}

void CMapEdit::InsertContent(fltk::MenuBar* menu) {
    iInfo = new fltk::Input(0, 0, 200, 23, program.Msg("Info"));
    iInfo->callback((fltk::Callback*)MapInfoCB, (void*)(iMap));
    iInfo->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);

    iId = new fltk::Input(0, 0, 200, 23, program.Msg("ID"));
    iId->callback((fltk::Callback*)MapIdCB, (void*)(iMap));
    iId->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
    
    iColor = new fltk::InvisibleBox(fltk::THIN_UP_BOX, 0, 0, 200, 23, program.Msg("Color"));
    iColor->callback((fltk::Callback*)MapColorCB, (void*)(iMap));
    iColor->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
}

void CMapEdit::SetData() {
    iInfo->value(iMap->Info());
    iId->value(iMap->Id());
    iColor->color(iMap->Color());
    iColor->redraw();
}
