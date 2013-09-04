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

#ifndef _ATLAS_H_
#define _ATLAS_H_

#include "base_window.h"
#include <sqlite3.h>
#include <fltk/Input.h>
#include <fltk/Browser.h>

class CMapNatal;

typedef struct {
    char*	iName;
    char*	iZone;
    double	iLon;
    double	iLat;
} ListItemType;

class CAtlas : public CBaseWindow {
private:
    sqlite3*		iDb;
    CMapNatal*		iMap;
    fltk::Input* 	iText;
    fltk::Browser* 	iList;
    ListItemType*	iItem;
    unsigned int	iItemNum;

    void ItemFree();

public:
    CAtlas();
    ~CAtlas();
    
    void Open(char* file, CMapNatal* map = NULL);
    void FillList();
    void ListSelect();
    void Title(char* str);
};

#endif
