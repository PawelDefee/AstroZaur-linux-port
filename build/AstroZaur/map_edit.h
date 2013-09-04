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

#ifndef _MAP_EDIT_H_
#define _MAP_EDIT_H_

#include "map.h"
#include "base_window.h"
#include <fltk/Window.h>
#include <fltk/Browser.h>
#include <fltk/MultiBrowser.h>
#include <fltk/CheckButton.h>
#include <fltk/InvisibleBox.h>
#include <fltk/Input.h>
#include <fltk/Choice.h>
#include <fltk/MenuBar.h>
#include <fltk/ItemGroup.h>

class CMapEdit : public CBaseWindow  {
private:
    fltk::Input*	iInfo;
    fltk::Input*	iId;
    fltk::ItemGroup*	iPointList;
    fltk::InvisibleBox*	iColor;

    void InsertPoint(CPoint* point);

protected:
    fltk::MultiBrowser*	iBrowser;
    CMap*		iMap;

public:
    list<fltk::Widget*>	iPointWidget;
    fltk::CheckButton*	iPointView;
    fltk::CheckButton*	iPointAspected;
    fltk::CheckButton*	iPointAspecting;
    fltk::CheckButton*	iPointInfo;
    
    CMapEdit();
    virtual ~CMapEdit();
    
    void Map(CMap* map);
    void MakeWindow();
    void Event(CAstro* obj, eventEnum event, void* data = NULL);
    
    virtual void BrowserClick(fltk::Widget* widget) {};
    virtual void InsertContent(fltk::MenuBar* menu);
    virtual void SetData();
};

#endif
