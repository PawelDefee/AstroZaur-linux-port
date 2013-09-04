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

#include "program.h"
#include "map_eqlipse.h"
#include "map_eqlipse_edit.h"
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/events.h>

// Callbacks

static void MapEqlipseTypeCB(fltk::Choice* input, CMapEqlipse* map) {
    map->Lunar(input->value());
    program.CalcMaps();
    program.Draw();
}

static void MapEqlipseDirectCB(fltk::Choice* input, CMapEqlipse* map) {
    map->Backward(input->value());
    program.CalcMaps();
    program.Draw();
}

// Editor class

CMapEqlipseEdit::~CMapEqlipseEdit() {
    delete iParentEdit;
}

void CMapEqlipseEdit::InsertContent(fltk::MenuBar* menu) {
    CMapNatalEdit::InsertContent(menu);

    iType = new fltk::Choice(0, 0, 200, 23, "Type");
    iType->callback((fltk::Callback*)MapEqlipseTypeCB, (void*)iMap);
    iType->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
    iType->when(fltk::WHEN_CHANGED);
    iType->begin();
    new fltk::Item(program.Msg("Solar"));
    new fltk::Item(program.Msg("Lunar"));
    iType->end();

    iDirect = new fltk::Choice(0, 0, 200, 23, "Direction");
    iDirect->callback((fltk::Callback*)MapEqlipseDirectCB, (void*)iMap);
    iDirect->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
    iDirect->when(fltk::WHEN_CHANGED);
    iDirect->begin();
    new fltk::Item(program.Msg("Next"));
    new fltk::Item(program.Msg("Prev"));
    iDirect->end();

    fltk::ItemGroup* g = new fltk::ItemGroup(program.Msg("Parent chart"));
    g->resize(0,0,200,23);
    g->begin();

    iParentEdit = ((CMapDevelop*)iMap)->Parent()->MakeEditor();
    iParentEdit->InsertContent(menu);

    g->end();
}

void CMapEqlipseEdit::SetData() {
    CMapNatalEdit::SetData();

    iParentEdit->SetData();
    iType->value(((CMapEqlipse*)iMap)->Lunar());
    iDirect->value(((CMapEqlipse*)iMap)->Backward());
}

void CMapEqlipseEdit::Event(CAstro* obj, eventEnum event, void* data) {
    CMapNatalEdit::Event(obj, event, data);
    iParentEdit->Event(obj, event, data);
}

bool CMapEqlipseEdit::Cloned(fltk::Widget* widget) {
    return CMapNatalEdit::Cloned(widget) 
	|| ((CMapNatalEdit*)iParentEdit)->Cloned(widget);
}
