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

#include "log.h"
#include "string.h"
#include "utils.h"
#include "program.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "lua_gui.h"
#include <fltk/MenuBar.h>
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/Divider.h>
#include <fltk/HelpView.h>
#include <fltk/events.h>
#include <fltk/file_chooser.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class CTabItem {
private:
    char*		iText;
    long int		iTextSize;
    fltk::HelpView*	iHtml;
    
public:
    CTabItem(int w, int h);
    ~CTabItem();
    
    void Append(const char* text);
    void Load(char* name);
    void Save(char* name);
    void View();
    void Clear();
};

// CTabItem

CTabItem::CTabItem(int w, int h) {
    iHtml = new fltk::HelpView(0, 0, w, h);
    iHtml->textsize(14);
    iHtml->value("");
    iText = NULL;
    iTextSize = 0;
}

CTabItem::~CTabItem() {
    if (iText) free(iText);
}

void CTabItem::Append(const char* text) {
    int	strsize = strlen(text);
    
    iText = (char*) realloc(iText, iTextSize + strsize + 1);

    TranslChars(text, iText + iTextSize);

    iTextSize += strsize;
    iText[iTextSize] = 0;
}

void CTabItem::Load(char* name) {
    Clear();
    iHtml->load(name);
}

void CTabItem::Save(char* name) {
    int	file = open(name, O_CREAT | O_WRONLY, 0644);

    if (file > 0) {
	write(file, iText, iTextSize);
	close(file);
    }
}

void CTabItem::View() {
    iHtml->value(iText ? iText : "");
}

void CTabItem::Clear() {
    free(iText);
    iText = NULL;
    iTextSize = 0;
}

// CallBacks

static void NewTabCB(fltk::Item*, CLog* log) {
    log->NewTab();
}

static void SaveTabCB(fltk::Item*, CLog* log) {
    static char filename[256] = "";
    const char *name = fltk::file_chooser(program.Msg("Save tab"),"*.html",filename);

    if (name) {
	strcpy(filename, name);
	log->Save((char*) name);
    }
}

// Methods

static int l_print(lua_State *L) {
    CLog* log = (CLog*) lua.GetObj();

    int n = lua.GetTop();

    for (int i = 1; i <= n; i++) {
	const char *str = lua.ToString(i);
	
	log->Print(str);
    }
    return 0;
}

static int l_clear(lua_State *L) {
    CLog* log = (CLog*) lua.GetObj();

    log->Clear();
    return 0;
}

static int l_show(lua_State *L) {
    CLog* log = (CLog*) lua.GetObj();

    log->Show();
    return 0;
}

// CLog

CLog::CLog() {
    iWindow = new CAstroWindow(320, 150, program.Msg("Log"));
    iWindow->user_data((void*)(this));
    iWindow->begin();

    fltk::MenuBar* menu = new fltk::MenuBar(0, 0, 320, 25);
    menu->begin();
    {
	fltk::ItemGroup* o = new fltk::ItemGroup(program.Msg("Tab"));
        o->begin();
        {
	    fltk::Item* o = new fltk::Item(program.Msg("New"));
            o->callback((fltk::Callback*)NewTabCB, (void*)(this));
        }{
	    fltk::Item* o = new fltk::Item(program.Msg("Save..."));
            o->shortcut(fltk::F2Key);
            o->callback((fltk::Callback*)SaveTabCB, (void*)(this));
        }{
	    /*
	    fltk::Item* o = new fltk::Item(program.Msg("Close"));
            o->callback((fltk::Callback*)LoadWorkspaceCB, (void*)(this));
	    */
        }
	o->end();
    }
    menu->end();
    
    iTabsGroup = new fltk::TabGroup(0, 25, 320, 150-25);
    
    iWindow->end();
    iWindow->resizable(iTabsGroup);
    
    NewTab();
    
    iType = "log";

    PushTable();

    lua.NewTable("window");
    gui_group(lua.State(), iWindow);
    lua.EndTable();

    lua.Method("print", l_print, this);
    lua.Method("clear", l_clear, this);
    lua.Method("show", l_show, this);
    lua.SetGlobal("log");
}

CLog::~CLog() {
}

void CLog::NewTab() {
    iTabsGroup->begin();

    fltk::Group* tab = new fltk::Group(0, 20, iTabsGroup->w(), iTabsGroup->h()-20, "Info");
    tab->begin();
    
    CTabItem* item = new CTabItem(iTabsGroup->w(), iTabsGroup->h()-20);
	
    tab->user_data((void*) item);
    tab->end();

    iTabsGroup->resizable(tab);
    iTabsGroup->end();
}

void CLog::Print(const char *text) {
    fltk::Widget*	tab = iTabsGroup->selected_child();
    CTabItem*		item = (CTabItem*) tab->user_data();

    if (text) item->Append(text);
}

void CLog::Show() {
    iWindow->show();
    View();
}

void CLog::View() {
    if (iWindow->visible()) {
	fltk::Widget*	tab = iTabsGroup->selected_child();
	CTabItem*	item = (CTabItem*) tab->user_data();

	item->View();
    }
}

void CLog::Clear() {
    fltk::Widget*	tab = iTabsGroup->selected_child();
    CTabItem*		item = (CTabItem*) tab->user_data();

    item->Clear();
}

void CLog::Load(char* name) {
    fltk::Widget*	tab = iTabsGroup->selected_child();
    CTabItem*		item = (CTabItem*) tab->user_data();

    item->Load(name);
    iWindow->show();
}

void CLog::Save(char* name) {
    fltk::Widget*	tab = iTabsGroup->selected_child();
    CTabItem*		item = (CTabItem*) tab->user_data();

    item->Save(name);
}
