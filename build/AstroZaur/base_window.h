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

#ifndef _BASE_WINDOW_H_
#define _BASE_WINDOW_H_

#include "astro.h"

#include <list>
#include <fltk/DoubleBufferWindow.h>
#include <fltk/MenuBar.h>

class CMenuItem;

class CAstroWindow : public fltk::DoubleBufferWindow {
public:
    CAstroWindow(int x, int y);
    CAstroWindow(int x, int y, const char* title);

    int handle(int event);
};

class CBaseWindow : public CAstro {
protected:
    char*			iType;
    char*			iTitle;
    CAstroWindow*		iWindow;
    fltk::MenuBar* 		iMenu;

public:
    CBaseWindow();
    ~CBaseWindow();

    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);

    virtual void Show();
    bool Visible();
    void Title(const char* title);
};

#endif
