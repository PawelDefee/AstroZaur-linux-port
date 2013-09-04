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

#ifndef _MAP_NATAL_EDIT_H_
#define _MAP_NATAL_EDIT_H_

#include "map_edit.h"
#include <fltk/Window.h>
#include <fltk/MultiBrowser.h>
#include <fltk/CheckButton.h>
#include <fltk/Input.h>
#include <fltk/Item.h>
#include <fltk/Output.h>
#include <fltk/Choice.h>
#include <fltk/MenuBar.h>

class CAtlas;

class CMapNatalEdit : public CMapEdit  {
private:
    map<CMap*, fltk::Widget*>	iClonedItems;
    fltk::ItemGroup*		iClonedGroup;
    fltk::Widget*		iClonedWidget;
    fltk::ItemGroup*		iMomentGroup;
    fltk::Input*		iDate;
    fltk::Input*		iTime;
    fltk::Input*		iTimeZone;
    fltk::Output*		iTimeOffset;
    fltk::ItemGroup*		iPlaceGroup;
    fltk::Input*		iPlace;
    fltk::Input*		iLon;
    fltk::Input*		iLat;
    fltk::Choice*		iHouse;
    
    CAtlas* 			iAtlas;
    
    void SetMomentLabel();
    void SetPlaceLabel();
    
public:
    CMapNatalEdit();
    ~CMapNatalEdit();
    
    void InsertContent(fltk::MenuBar* menu);
    void SetData();
    void Event(CAstro* obj, eventEnum event, void* data = NULL);
    void AddMenuItem(CMap* map);
    
    void BrowserClick(fltk::Widget* widget);
    void ClonedClick(fltk::Item* item);
    void OpenAtlas();

    virtual bool Cloned(fltk::Widget* widget);
};

#endif
