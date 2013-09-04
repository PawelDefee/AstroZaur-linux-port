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

#ifndef _LOG_H_
#define _LOG_H_

#include "base_window.h"
#include <fltk/DoubleBufferWindow.h>
#include <fltk/TabGroup.h>
#include <fltk/Group.h>

class CLog : public CBaseWindow {
private:
    fltk::TabGroup* 	iTabsGroup;

public:
    CLog();
    ~CLog();
    
    void Print(const char *text);
    void Clear();
    void View();
    void Show();
    
    void Load(char* name);
    void Save(char* name);
    void NewTab();
};

#endif
