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

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "maps.h"
#include "draw_area.h"
#include "base_window.h"

#include <fltk/DoubleBufferWindow.h>
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/Divider.h>
#include <map>

class CWindow;

typedef struct {
    CWindow*		iWindow;
    CMap*		iMap;
    fltk::Widget*	iWidgetSelect;
    fltk::Widget*	iWidgetAspect;
} MenuData;

class CWindow : public CBaseWindow {
private:
    fltk::ItemGroup*		iSelectGroup;
    fltk::ItemGroup*		iAspectGroup;
    map<CMap*, MenuData*>	iInfoItems;
    
    void MakeWindow();

protected:
    CMaps*	iMaps;
    CDrawArea*	iDraw;

public:
    CWindow();
    ~CWindow();
    
    void Hide();
    
    void AddMenuItem(CMap* map);

    void Make();
    CMaps* Maps();
    void Redraw();
    void ScreenShot();
    void Event(CAstro* obj, eventEnum event, void* data = NULL);

    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);
    
    void Lua();
    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};
#endif
