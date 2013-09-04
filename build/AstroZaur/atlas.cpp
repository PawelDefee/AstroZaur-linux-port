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

#include "atlas.h"
#include "map_natal.h"
#include "program.h"
#include "lua.h"
#include "lua_gui.h"
#include <fltk/Item.h>
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

CAtlas::CAtlas() {
    iDb = NULL;
    iItem = NULL;
    iItemNum = 0;
    iMap = NULL;
}

CAtlas::~CAtlas() {
    if (iDb) sqlite3_close(iDb);
    iWindow->destroy();
    ItemFree();
}

static void InputCB(fltk::Input* input, CAtlas* atlas) {
    atlas->FillList();
}

static void ListCB(fltk::Browser* browser, CAtlas* atlas) {
    atlas->ListSelect();
}
    
void CAtlas::Title(char* str) {
    char	title[128];
    
    strcpy(title, "Atlas: ");
    strcat(title, str);
    iWindow->copy_label(title);
}

void CAtlas::Open(char* file, CMapNatal* map) {
    const char 	*labels[] = {"City", "Region", 0};
    int 	widths[]   = {140, -1, 0};
    
    int res = sqlite3_open(file, &iDb);
    
    if (res == SQLITE_OK) {
	PushTable();

	iWindow = new CAstroWindow(360, 210, "Atlas");
	iWindow->user_data((void*)(this));
	iWindow->begin();

	lua.NewTable("window");
	gui_group(lua.State(), iWindow);
	lua.EndTable();
    
	iText = new fltk::Input(0, 0, 360, 25);
	iText->callback((fltk::Callback*)InputCB, (void*)this);
	iText->when(fltk::WHEN_CHANGED);

	iList = new fltk::Browser(0, 25, 360, 210-25);
	iList->callback((fltk::Callback*)ListCB, (void*)this);
	iList->column_widths(widths);
	iList->column_labels(labels);

	iWindow->end();
	iWindow->resizable(iList);
	
	lua.Pop(1);

	if (map) {
	    iMap = map;
	    Title(map->Info());
	}
    } else {
	printf("SQL error\n");
    }
}

void CAtlas::ItemFree() {
    if (iItem) {
	unsigned int n;
    
	for (n = 0; n < iItemNum; n++) {
	    free(iItem[n].iName);
	    free(iItem[n].iZone);
	}
	
	free(iItem);
	iItem = NULL;
	iItemNum = 0;
    }
}

void CAtlas::FillList() {
    char 		str[200];
    sqlite3_stmt*	stmt;
    int			res;
    
    sprintf(str, "select city.name,reg.name,city.lon,city.lat,reg.zone"
		    " from city,reg"
		    " where city.name like '%s%s' and reg.id = city.reg"
		    " order by city.name limit 200",
	    iText->value(), "%");
	    
    res = sqlite3_prepare(iDb, str, -1, &stmt, NULL);
    
    if (res == SQLITE_OK) {
	unsigned int n = 0;

	ItemFree();	
	iList->clear();
	iList->begin();
	
        res = sqlite3_step(stmt);

	while (res == SQLITE_ROW) {
	    char* zone = (char*) sqlite3_column_text(stmt, 4);
	
	    strcpy(str, (const char*) sqlite3_column_text(stmt, 0));
	    strcat(str, "\t");
	    strcat(str, (const char*) sqlite3_column_text(stmt, 1));

	    fltk::Item* item = new fltk::Item(strdup(str));
	    
	    iItem = (ListItemType*) realloc(iItem, sizeof(ListItemType) * (n+1));

	    iItem[n].iName = strdup((char*) sqlite3_column_text(stmt, 0));
	    iItem[n].iLon = sqlite3_column_double(stmt, 2);
	    iItem[n].iLat = sqlite3_column_double(stmt, 3);
	    iItem[n].iZone = strdup(zone != NULL ? zone : "GMT");
	    
    	    res = sqlite3_step(stmt);
	    n++;
	}
	iItemNum = n;
	sqlite3_finalize(stmt);
	iList->end();
    } else {
	printf("SQL error\n");
    }
}

void CAtlas::ListSelect() {
    if (!iItemNum) return;

    ListItemType* item  = &iItem[iList->value()];

    if (iMap) {
	iMap->Place()->Lon(item->iLon);
	iMap->Place()->Lat(item->iLat);
	iMap->Place()->Info(item->iName);
	iMap->Date()->TimeZone(item->iZone);
    
	iMap->Ready(false);

	program.Event(iMap->Place(), eventChangePlaceLon);
	program.Event(iMap->Place(), eventChangePlaceLat);
    
	program.CalcMaps();
	program.Draw();
    } else {
	PushTable();
    
	lua.PushString("callback");
	lua.GetTable(-2);

	if (lua.Type(-1) == LUA_TFUNCTION) {
	    // move self
	    lua.PushValue(-2);
	    lua.Remove(-3);
	    
	    lua.SetField("info", item->iName);
	    lua.SetField("lon", item->iLon);
	    lua.SetField("lat", item->iLat);
	    lua.SetField("zone", item->iZone);
	    
	    lua.Call(1);
	} else {
	    lua.Pop(2);
	}
    }
}
