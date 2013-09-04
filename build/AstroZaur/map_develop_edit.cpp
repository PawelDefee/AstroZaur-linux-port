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
#include "map_develop.h"
#include "map_develop_edit.h"
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/Choice.h>
#include <fltk/events.h>

// Callbacks

void InputK1CB(fltk::Input* input, CMapDevelop* map) {
    double k = atof(input->value());

    map->K1(k);
    program.CalcMaps();
    program.Draw();
}

void InputK2CB(fltk::Input* input, CMapDevelop* map) {
    double k = atof(input->value());
    
    map->K2(k);
    program.CalcMaps();
    program.Draw();
}

// Editor class

CMapDevelopEdit::~CMapDevelopEdit() {
    delete iParentEdit;
}

void CMapDevelopEdit::InsertContent(fltk::MenuBar* menu) {
    CMapNatalEdit::InsertContent(menu);

    iK1 = new fltk::Input(0, 0, 200, 23, "K1");
    iK1->callback((fltk::Callback*)InputK1CB, (void*)(iMap));
    iK1->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
    
    iK2 = new fltk::Input(0, 0, 200, 23, "K2");
    iK2->callback((fltk::Callback*)InputK2CB, (void*)(iMap));
    iK2->align(fltk::ALIGN_LEFT|fltk::ALIGN_INSIDE|fltk::ALIGN_CLIP);
    
    fltk::ItemGroup* g = new fltk::ItemGroup(program.Msg("Parent chart"));
    g->resize(0,0,200,23);
    g->begin();
    g->set_flag(fltk::OPENED);

    iParentEdit = ((CMapDevelop*)iMap)->Parent()->MakeEditor();
    iParentEdit->InsertContent(menu);

    g->end();
}

void CMapDevelopEdit::SetData() {
    char str[32];

    CMapNatalEdit::SetData();

    DoubleToStr(((CMapDevelop*)iMap)->K1(), str);
    iK1->value(str);

    DoubleToStr(((CMapDevelop*)iMap)->K2(), str);
    iK2->value(str);

    iParentEdit->SetData();
}

void CMapDevelopEdit::Event(CAstro* obj, eventEnum event, void* data) {
    CMapNatalEdit::Event(obj, event, data);
    iParentEdit->Event(obj, event, data);
}

bool CMapDevelopEdit::Cloned(fltk::Widget* widget) {
    return CMapNatalEdit::Cloned(widget) 
	|| ((CMapNatalEdit*)iParentEdit)->Cloned(widget);
}
