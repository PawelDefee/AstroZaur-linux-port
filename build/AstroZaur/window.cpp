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

#include "window.h"
#include "program.h"
#include "projection.h"
#include "aspects.h"
#include "map_progres.h"
#include "map_direct.h"
#include "map_eqlipse.h"
#include "map_raw.h"
#include "log.h"
#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "lua_gui.h"
#include <fltk/file_chooser.h>
#include <fltk/ask.h>
#include <fltk/events.h>
#include <fltk/damage.h>
#include <fltk/InvisibleBox.h>
#include <fltk/run.h>

#include <png.h>

// Callbacks

void SaveBitmapWindow(fltk::Item*, CWindow* window) {
    window->ScreenShot();
}

void SelectMapCB(fltk::Item*, MenuData* data) {
    data->iWindow->Maps()->ActiveMap(data->iMap);
    data->iWindow->Redraw();
    program.DoInfo();
}

void AspectMapCB(fltk::Item*, MenuData* data) {
    data->iWindow->Maps()->AspectedMap(data->iMap);
    data->iWindow->Maps()->CalcAspectTable();
    data->iWindow->Redraw();
    program.DoInfo();
}

void RotateForwardCB(fltk::Item* , CWindow* win) {
    if (win->Maps()->ActiveMap()) {
	win->Maps()->ActiveMap()->RotateForward();
	program.CalcMaps();
	program.Draw();
    } else {
	fltk::message(program.Msg("No active chart"));
    }
}

void RotateBackCB(fltk::Item* menu, CWindow* win) {
    if (win->Maps()->ActiveMap()) {
	win->Maps()->ActiveMap()->RotateBack();
	program.CalcMaps();
	program.Draw();
    } else {
	fltk::message(program.Msg("No active chart"));
    }
}

void RotateM(fltk::Item*, CWindow* win) {
    if (win->Maps()->ActiveMap()) {
	win->Maps()->ActiveMap()->SetRotate(60);
    } else {
	fltk::message(program.Msg("No active chart"));
    }
}

void RotateH(fltk::Item*, CWindow* win) {
    if (win->Maps()->ActiveMap()) {
	win->Maps()->ActiveMap()->SetRotate(60*60);
    } else {
	fltk::message(program.Msg("No active chart"));
    }
}

void RotateD(fltk::Item*, CWindow* win) {
    if (win->Maps()->ActiveMap()) {
        win->Maps()->ActiveMap()->SetRotate(60*60*24);
    } else {
	fltk::message(program.Msg("No active chart"));
    }
}

void RotateY(fltk::Item*, CWindow* win) {
    if (win->Maps()->ActiveMap()) {
	win->Maps()->ActiveMap()->SetRotate(60*60*24*365);
    } else {
	fltk::message(program.Msg("No active chart"));
    }
}

void LoadWindowCB(fltk::Item*, CWindow* win) {
    static char filename[256] = "";
    const char *name = fltk::file_chooser(program.Msg("Load into window"),"*.xml",filename);

    if (name) {
	strcpy(filename, name);
//	win->Maps()->LoadFile((char*) name);
	win->LoadFile((char*) name);
	
	program.ResolveShadow();
	program.CalcMaps();
	win->Redraw();
    }
}

void LoadMapCB(fltk::Item*, CWindow* win) {
    if (win->Maps()->ActiveMap() == NULL) {
	fltk::message(program.Msg("No active chart"));
	return;
    }

    static char filename[256] = "";
    const char *name = fltk::file_chooser(program.Msg("Load into active chart"),"*.xml",filename);

    if (name) {
	strcpy(filename, name);
	win->Maps()->ActiveMap()->LoadFile((char*) name);
	program.ResolveShadow();
	win->Maps()->ActiveMap()->Ready(false);
	program.CalcMaps();
	program.Draw();
    }
}

void SaveMapCB(fltk::Item*, CWindow* win) {
    if (win->Maps()->ActiveMap() == NULL) {
	fltk::message(program.Msg("No active chart"));
	return;
    }
    
    static char filename[256] = "";
    const char *name = fltk::file_chooser(program.Msg("Save active chart"),"*.xml",filename);

    if (name) {
	strcpy(filename, name);
	win->Maps()->ActiveMap()->SaveFile((char*) name);
    }
}

void LoadWorkspaceCB(fltk::Item*, CWindow *win) {
    static char filename[256] = "";
    const char *name = fltk::file_chooser(program.Msg("Load workspace"),"*.xml",filename);

    if (name) {
	strcpy(filename, name);
	program.LoadFile((char*) name);
	program.ResolveShadow();
	program.CalcMaps();
	program.Draw();
    }
}

void SaveWorkspaceCB(fltk::Item*, CWindow* win) {
    static char filename[256] = "";
    const char *name = fltk::file_chooser(program.Msg("Save workspace"),"*.xml",filename);

    if (name) {
	strcpy(filename, name);
	program.SaveFile((char*) name);
    }
}

// Lua method

static int l_projection(lua_State *L) {
    CMaps* 	maps = (CMaps*) lua.GetObj();
    
    const char	*type = lua.ToString(1);

    if (type) {
	maps->Projection(type);
	
	if (maps->Projection()) {
	    maps->Projection()->PushTable();
	} else {
	    return 0;
	}
    } else {
	maps->Projection()->PushTable();
    }
    return 1;
}

static int l_show(lua_State *L) {
    CMaps* 	maps = (CMaps*) lua.GetObj();
    int		args = lua.GetTop();
    
    if (lua.Type(1) == LUA_TTABLE) {
	lua.PushString(".ptr_chart");
	lua.GetTable(1);

	if (lua.Type(-1) == LUA_TLIGHTUSERDATA) {
	    CMap *map = (CMap*) lua.ToUser(-1);

	    if (args > 1) {
		maps->View(map, lua.ToBool(2));
		lua.Pop(1);
		return 0;
	    } else {
		lua.Pop(1);
		lua.PushBool(maps->View(map));
		return 1;
	    }
	}
    }
    
    return 0;
}

// Window class

CWindow::CWindow() {
    iMaps = new CMaps;
    iDraw = NULL;

    PushTable();
    lua.Method("projection", l_projection, iMaps);
    lua.Method("show", l_show, iMaps);
    lua.Pop(1);
}

CWindow::~CWindow() {
    delete iMaps;
}

void CWindow::MakeWindow() {
    iWindow = new CAstroWindow(400, 425, "AstroZaur");
    iWindow->type(241);
    iWindow->set_vertical();
    iWindow->begin();

    PushTable();
	
    iMenu = new fltk::MenuBar(0, 0, 400, 25);
    iMenu->begin();

    lua.NewTable("menu");
    gui_group(lua.State(), iMenu);
    
    {
	fltk::ItemGroup* o = new fltk::ItemGroup(program.Msg("Workspace"));
        o->begin();
	
	lua.NewTable(1);
	gui_group(lua.State(), o);
	lua.EndTable();

        {
	    fltk::Item* o = new fltk::Item(program.Msg("Load..."));
            o->shortcut(0xffc6);
            o->callback((fltk::Callback*)LoadWorkspaceCB, (void*)(this));
        }{
	    fltk::Item* o = new fltk::Item(program.Msg("Save..."));
            o->callback((fltk::Callback*)SaveWorkspaceCB, (void*)(this));
        }
        o->end();
    }{
	fltk::ItemGroup* o = new fltk::ItemGroup(program.Msg("Window"));
        o->begin();

	lua.NewTable(2);
	gui_group(lua.State(), o);
	lua.EndTable();

        {
	    fltk::Item* o = new fltk::Item(program.Msg("Load..."));
            o->shortcut(0xffc4);
            o->callback((fltk::Callback*)LoadWindowCB, (void*)(this));
        }{
	    fltk::Item* o = new fltk::Item(program.Msg("Save bitmap"));
            o->callback((fltk::Callback*)SaveBitmapWindow, (void*)(this));
	
        }
        o->end();
    }{
	fltk::ItemGroup* o = new fltk::ItemGroup(program.Msg("Chart"));
        o->begin();

	lua.NewTable(3);
	gui_group(lua.State(), o);
	lua.EndTable();

	{
	    iSelectGroup = new fltk::ItemGroup(program.Msg("Select"));
	    iAspectGroup = new fltk::ItemGroup(program.Msg("Aspect"));
        }{
	    fltk::ItemGroup* o = new fltk::ItemGroup(program.Msg("Rotate"));
            o->begin();
            {
		fltk::Item* o = new fltk::Item(program.Msg("Back"));
                o->shortcut(0xff51);
                o->callback((fltk::Callback*)RotateBackCB, (void*)(this));
            }{
		fltk::Item* o = new fltk::Item(program.Msg("Forward"));
                o->shortcut(0xff53);
                o->callback((fltk::Callback*)RotateForwardCB, (void*)(this));
            }
                new fltk::Divider();
            {
		fltk::Item* o = new fltk::Item(program.Msg("1 Minute"));
                o->shortcut(0x6d);
                o->callback((fltk::Callback*)RotateM, (void*)(this));
            }{
		fltk::Item* o = new fltk::Item(program.Msg("1 Hour"));
                o->shortcut(0x68);
                o->callback((fltk::Callback*)RotateH, (void*)(this));
            }{
		fltk::Item* o = new fltk::Item(program.Msg("1 Day"));
                o->shortcut(0x64);
                o->callback((fltk::Callback*)RotateD, (void*)(this));
            }{
		fltk::Item* o = new fltk::Item(program.Msg("1 Year"));
                o->shortcut(0x79);
                o->callback((fltk::Callback*)RotateY, (void*)(this));
            }
            o->end();
        }
            new fltk::Divider();
        {
	    fltk::Item* o = new fltk::Item(program.Msg("Save..."));
            o->shortcut(0xffbf);
            o->callback((fltk::Callback*)SaveMapCB, (void*)(this));
        }{
	    fltk::Item* o = new fltk::Item(program.Msg("Load..."));
            o->shortcut(0xffc0);
            o->callback((fltk::Callback*)LoadMapCB, (void*)(this));
        }
        o->end();
    }

    iMenu->end();
    lua.EndTable();

    iDraw = new CDrawArea(0, 25, 400, 400);

    iWindow->resizable(iDraw);
    iWindow->end();

    lua.NewTable("window");
    gui_group(lua.State(), iWindow);
    lua.EndTable();

    lua.PushString("draw");
    iDraw->PushTable();
    gui_widget(lua.State(), iDraw);
    lua.EndTable();

    lua.Pop(1);
}

void CWindow::AddMenuItem(CMap* map) {
    MenuData*		item = (MenuData*) malloc(sizeof(MenuData));
    char* 		info = map->Info();
    int			n = iSelectGroup->size();
	
    item->iMap = map;
    item->iWindow = this;

    item->iWidgetSelect = iSelectGroup->add(info, '1' + n, (fltk::Callback *)SelectMapCB, item);
    item->iWidgetAspect = iAspectGroup->add(info, fltk::CTRL + '1' + n, (fltk::Callback *)AspectMapCB, item);
    
    iInfoItems[map] = item;
}

void CWindow::Event(CAstro* obj, eventEnum event, void* data) {
    switch (event) {
	case eventChangeChartInfo:
	    {
		CMap*		map = (CMap*) obj;
		MenuData*	item = iInfoItems[map];
    
		if (item) {
		    item->iWidgetSelect->label(map->Info());
		    item->iWidgetAspect->label(map->Info());

		    item->iWidgetSelect->w(0);
		    item->iWidgetAspect->w(0);
		}
	    }
	    break;
	    
	case eventNewChart:
	    AddMenuItem((CMap*)obj);
	    break;
	    
	default:
	    break;
    }
}

void CWindow::Redraw() {
    iDraw->redraw();
}

extern fltk::Image* fl_current_Image;

void CWindow::ScreenShot() {
    uchar 		*pixels;
    fltk::Rectangle	r;

    /*
    char filename[256] = "";
    const char *name = fltk::file_chooser(program.Msg("Save active chart"),"*.png", filename);
    
    if (name == NULL) return;
    */
	    
    pixels = (uchar*) malloc(iDraw->h() * iDraw->w() * 3);

    r.x(iDraw->x());	r.y(iDraw->y());
    r.w(iDraw->w());	r.h(iDraw->h());

    fltk::flush();
    iDraw->make_current();

    fltk::readimage(pixels, fltk::RGB, r);

    FILE *fp;

    if ((fp = fopen("astrozaur.png", "wb")) == NULL) {
	free(pixels);
	fltk::alert("Error writing");
	return;
    }

    png_structp pptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_infop iptr = png_create_info_struct(pptr);
    png_bytep ptr = (png_bytep)pixels;

    png_init_io(pptr, fp);
    png_set_IHDR(pptr, iptr, r.w(), r.h(), 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_sRGB(pptr, iptr, PNG_sRGB_INTENT_PERCEPTUAL);

    png_write_info(pptr, iptr);

    for (int i = r.h(); i > 0; i --, ptr += r.w() * 3)
	png_write_row(pptr, ptr);

    png_write_end(pptr, iptr);
    png_destroy_write_struct(&pptr, &iptr);
    fclose(fp);
    free(pixels);
    
    fltk::message("Writing Ok");
}

void CWindow::Make() {
    MakeWindow();
    iDraw->Maps(iMaps);
}

void CWindow::XML(xmlNodePtr node) {
    xmlNodePtr	subNode = node;

    // We in root?    
    while (subNode) {
	if (!strcmp((char*)subNode->name,"window")) break;
	subNode = subNode->next;
    }
    
    if (subNode) {	// Found in root
	subNode = subNode->children;
    } else {		// Not found
	subNode = node;
    }
    
    iMaps->XML(subNode);
    
    while (subNode) {
	char* name = (char *) subNode->name;

	if (!strcmp(name,"title")) {
	    if (has_content(subNode)) {
		Title((char *)subNode->children->content);
	    }
	}

	if (!strcmp(name,"lua")) {
	    XMLLua(subNode);
	}
	
	subNode = subNode->next;
    }

    LuaExec("INIT", true);
    CBaseWindow::XML(node);
}

xmlNodePtr CWindow::XML(xmlNodePtr parent, int level) {
    xmlNodePtr sub = CBaseWindow::XML(parent, level);
    
    iMaps->XML(sub, level);

    return sub;
}

CMaps* CWindow::Maps() {
    return iMaps;
}

void CWindow::Hide() {
    iDraw->Hide();
}

void CWindow::Lua() {
    LuaFunction("INFO");
}

bool CWindow::LuaSet(const char* var) {
    if (strcmp(var, "title") == 0) {
	Title(lua.ToString(-1));
	return true;
    }

    return iMaps->LuaSet(var);
}

bool CWindow::LuaGet(const char* var) {
    if (strcmp(var, "title") == 0) {
	if (iTitle) {
	    lua.PushString(iTitle);
	} else {
	    lua.PushNil();
	}
	return true;
    }

    return iMaps->LuaGet(var);
}
