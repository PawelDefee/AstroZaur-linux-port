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

#include "lua.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()
#include "lua_gui.h"
#include "program.h"

#include <string.h>
#include <fltk/DoubleBufferWindow.h>
#include <fltk/TabGroup.h>
#include <fltk/TiledGroup.h>
#include <fltk/Button.h>
#include <fltk/CheckButton.h>
#include <fltk/RadioButton.h>
#include <fltk/Choice.h>
#include <fltk/Item.h>
#include <fltk/ItemGroup.h>
#include <fltk/Browser.h>
#include <fltk/Input.h>
#include <fltk/Output.h>
#include <fltk/MultiLineInput.h>
#include <fltk/MultiLineOutput.h>
#include <fltk/ValueInput.h>
#include <fltk/InvisibleBox.h>
#include <fltk/MenuBar.h>
#include <fltk/HelpView.h>
#include <fltk/ProgressBar.h>
#include <fltk/Divider.h>
#include <fltk/file_chooser.h>
#include <fltk/ask.h>
#include <fltk/run.h>
#include <fltk/draw.h>
#include <fltk/events.h>
#include <fltk/Color.h>
#include <fltk/SharedImage.h>

extern "C" {
    #include <lua.h>
    #include <lualib.h>

    #include <lauxlib.h>
    #include <sqlite3.h>
}

static int l_ready(lua_State *L) {
    lua_pushboolean(L, fltk::ready());
    
    return 1;
}

static int l_check(lua_State *L) {
    fltk::check();
    
    return 0;
}

static int l_run(lua_State *L) {
    fltk::run();
    
    return 0;
}

static int l_monitor(lua_State *L) {
    lua_pushnumber(L, fltk::monitor_w());
    lua_pushnumber(L, fltk::monitor_h());
    
    return 2;
}

static int l_filechooser(lua_State *L) {
    const char	*title = luaL_checkstring(L, 1);
    const char	*filter = luaL_checkstring(L, 2);
    const char	*dir = luaL_checkstring(L, 3);

    const char *name = fltk::file_chooser(program.Msg(title), filter, dir);

    lua_pushstring(L, name);
    
    return 1;
}

static int l_message_dialog(lua_State *L) {
    const char	*text = luaL_checkstring(L, 1);
    
    fltk::message(program.Msg(text));
    return 0;
}

static int l_alert_dialog(lua_State *L) {
    const char	*text = luaL_checkstring(L, 1);
    
    fltk::alert(program.Msg(text));
    return 0;
}

static int l_choice_dialog(lua_State *L) {
    const char	*text = luaL_checkstring(L, 1);
    const char	*label1 = luaL_checkstring(L, 2);
    const char	*label2 = luaL_checkstring(L, 3);
    const char	*label3 = lua_tostring(L, 4);

    int		res;
    
    res = fltk::choice(program.Msg(text), program.Msg(label1), program.Msg(label2), program.Msg(label3));
    lua.PushNumber(res);
    return 1;
}

//

static void callback(fltk::Widget* widget, void* data) {
    fltk::check();

    lua.RawGetI(LUA_REGISTRYINDEX, (int)data);
    lua.PushString("callback");
    lua.GetTable(-2);
    
    if (lua.Type(-1) == LUA_TFUNCTION) {
	lua.PushValue(-2);	// self
	lua.Remove(-3);		// table
	lua.Call(1);
    } else {
	lua.Pop(2);
    }
}

// Widget

static int l_widget_x(lua_State *L) {
    int res = 0;
    int x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));
    
    if (lua_isnumber(L, 1)) {
	x = (int)lua_tonumber(L, 1);
	ptr->x(x);
    } else {
	x = ptr->x();
	lua_pushnumber(L, x);
	res = 1;
    }
    return res;
}

static int l_widget_y(lua_State *L) {
    int res = 0;
    int x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_isnumber(L, 1)) {
	x = (int)lua_tonumber(L, 1);
	ptr->y(x);
    } else {
	x = ptr->y();
	lua_pushnumber(L, x);
	res = 1;
    }
    return res;
}

static int l_widget_w(lua_State *L) {
    int res = 0;
    int x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));
	    
    if (lua_isnumber(L, 1)) {
	x = (int)lua_tonumber(L, 1);
	ptr->w(x);
    } else {
	x = ptr->w();
	lua_pushnumber(L, x);
	res = 1;
    }
    return res;
}

static int l_widget_h(lua_State *L) {
    int res = 0;
    int x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_isnumber(L, 1)) {
	x = (int)lua_tonumber(L, 1);
	ptr->h(x);
    } else {
	x = ptr->h();
	lua_pushnumber(L, x);
	res = 1;
    }
    return res;
}

static int l_widget_r(lua_State *L) {
    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    lua_pushnumber(L, ptr->x() + ptr->w());

    return 1;
}

static int l_widget_b(lua_State *L) {
    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    lua_pushnumber(L, ptr->y() + ptr->h());

    return 1;
}

static int l_widget_redraw(lua_State *L) {
    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->redraw();

    return 0;
}

static int l_widget_label(lua_State *L) {
    int res = 0;
    char *x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	x = (char *)lua_tostring(L, 1);
	ptr->copy_label(program.Msg(x));
	ptr->redraw();
    } else {
	x = (char *)ptr->label();
	lua_pushstring(L, x);
	res = 1;
    }
    return res;
}

static int l_widget_key(lua_State *L) {
    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	char *key = (char *)lua_tostring(L, 1);
	if (key) ptr->shortcut(fltk::key(key));
    }
    
    return 0;
}

static int l_widget_backcolor(lua_State *L) {
    int res = 0;
    char *x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	x = (char *)lua_tostring(L, 1);
	ptr->color(fltk::color(x));
    } else {
    }
    return res;
}

static int l_widget_textcolor(lua_State *L) {
    int res = 0;
    char *x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	x = (char *)lua_tostring(L, 1);
	ptr->textcolor(fltk::color(x));
    } else {
    }
    return res;
}

static int l_widget_labelcolor(lua_State *L) {
    int res = 0;
    char *x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	x = (char *)lua_tostring(L, 1);
	ptr->labelcolor(fltk::color(x));
    } else {
    }
    return res;
}

static int l_widget_textsize(lua_State *L) {
    int res = 0;
    unsigned x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	x = lua_tonumber(L, 1);
	ptr->textsize(x);
    } else {
    }
    return res;
}

static int l_widget_labelsize(lua_State *L) {
    int res = 0;
    unsigned x;

    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	x = lua_tonumber(L, 1);
	ptr->labelsize(x);
    } else {
    }
    return res;
}

static int l_widget_show(lua_State *L) {
    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->show();
    return 0;
}

static int l_widget_hide(lua_State *L) {
    fltk::Widget *ptr = (fltk::Widget*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->hide();
    return 0;
}

static const luaL_reg widget_method[] = {
    {"x", l_widget_x},
    {"y", l_widget_y},
    {"w", l_widget_w},
    {"h", l_widget_h},
    {"r", l_widget_r},
    {"b", l_widget_b},
    {"redraw", l_widget_redraw},
    {"label", l_widget_label},
    {"backcolor", l_widget_backcolor},
    {"textcolor", l_widget_textcolor},
    {"labelcolor", l_widget_labelcolor},
    {"textsize", l_widget_textsize},
    {"labelsize", l_widget_labelsize},
    {"show", l_widget_show},
    {"hide", l_widget_hide},
    {"key", l_widget_key},
    {NULL, NULL}
};

void gui_widget(lua_State *L, fltk::Widget *ptr) {
    lua_pushstring(L, ".ptr_widget");
    lua_pushlightuserdata(L, (void*) ptr);
    lua_rawset(L, -3);

    lua_pushlightuserdata(L, ptr);
    luaL_openlib(L, NULL, widget_method, 1);
}

// Item

static int l_item(lua_State *L) {
    fltk::Widget	*ptr;
    const char 		*label;
    int			idx = 1, n = 0;
    int			args = lua_gettop(L);

    fltk::Menu 		*g = (fltk::Menu*) lua_touserdata(L, lua_upvalueindex(1));

    while (idx <= args) {
	if (lua_type(L, idx) == LUA_TSTRING) {
    	    label = (char *) lua_tostring(L, idx);
	    
	    lua_newtable(L);

	    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

	    ptr = g->add(program.Msg((char*) label), 0, callback, (void*)ref);

	    gui_widget(L, ptr);
	    n++;
	} else if (lua_type(L, idx) == LUA_TTABLE) {
	    lua_pushstring(L, "label");
	    lua_gettable(L, idx);
	    
	    if (lua_type(L, -1) != LUA_TSTRING) {
		printf("Not label\n");
		lua_pop(L, 1);
		idx++;
		continue;
	    }
	    
    	    label = lua_tostring(L, -1);
	    
	    if (label[0] == '-') {
    		lua_pop(L, 1);
		
		g->begin();
    		new fltk::Divider();
		g->end();

		idx++;
		continue;
	    }

    	    lua_pop(L, 1);
	    lua_newtable(L);

	    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	    
	    ptr = g->add(program.Msg((char*) label), 0, callback, (void*)ref);

	    gui_widget(L, ptr);

	    n++;
		
	    lua_pushstring(L, "callback");
	    lua_gettable(L, idx);
		
	    if (lua_type(L, -1) == LUA_TFUNCTION) {
	        lua_pushstring(L, "callback");
	        lua_insert(L, -2);
	        lua_rawset(L, -3);
	    } else {
	        lua_pop(L, 1); 
	    }

	    lua_pushstring(L, "key");
	    lua_gettable(L, idx);
	    
	    if (lua_type(L, -1) == LUA_TSTRING) {
		char *key = (char *) lua_tostring(L, -1);
		lua_pop(L, 1);
		
		ptr->shortcut(fltk::key(key));
	    } else {
		lua_pop(L, 1);
	    }
	} else {
	    printf("Item param not string, not table\n");
	}
	
	idx++;
    }

    return n;
}

// Groups

static int l_tabgroup(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::TabGroup	*ptr = new fltk::TabGroup(x, y, w, h);
    g->end();

    lua_newtable(L);
    gui_group(L, ptr);
    
    return 1;
}

static int l_tiledgroup(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::TiledGroup	*ptr = new fltk::TiledGroup(x, y, w, h);
    g->end();

    lua_newtable(L);
    gui_group(L, ptr);
    
    return 1;
}

static int l_itemgroup(lua_State *L) {
    char 		*label = (char *)luaL_checkstring(L, 1);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::ItemGroup	*ptr = new fltk::ItemGroup();
    g->end();

    ptr->copy_label(program.Msg(label));
    lua_newtable(L);

    gui_group(L, ptr);
    
    return 1;
}

static int l_menubar(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::MenuBar	*ptr = new fltk::MenuBar(x, y, w, h);
    g->end();

    lua_newtable(L);
    gui_group(L, ptr);
    
    return 1;
}

// Choice

static int l_choice_value(lua_State *L) {
    int res = 0, val;

    fltk::Choice *ptr = (fltk::Choice*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L) && lua_isnumber(L, 1)) {
	val = (int) lua_tonumber(L, 1);
	ptr->value(val);
    } else {
	val = ptr->value();
	lua_pushnumber(L, val);
	res = 1;
    }
    return res;
}

static int l_choice(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::Choice	*ptr = new fltk::Choice(x, y, w, h);
    g->end();

    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    lua_pushstring(L, "item");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_item, 1);
    lua_rawset(L, -3);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_choice_value, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

// Browser

static int l_browser_value(lua_State *L) {
    int res = 0, val;

    fltk::Browser *ptr = (fltk::Browser*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L) && lua_isnumber(L, 1)) {
	val = (int) lua_tonumber(L, 1);
	ptr->value(val);
    } else {
	val = ptr->value();
	lua_pushnumber(L, val);
	res = 1;
    }
    return res;
}

static int l_browser_clear(lua_State *L) {
    fltk::Browser *ptr = (fltk::Browser*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->clear();
    return 0;
}

static int l_browser_remove(lua_State *L) {
    int x = (int)luaL_checknumber(L, 1);

    fltk::Browser *ptr = (fltk::Browser*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->remove(x);
    return 0;
}

static int l_browser_indented(lua_State *L) {
    fltk::Browser *ptr = (fltk::Browser*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->indented(true);

    return 0;
}

static const luaL_reg browser_method[] = {
    {"item", l_item},
    {"value", l_browser_value},
    {"clear", l_browser_clear},
    {"remove", l_browser_remove},
    {"indented", l_browser_indented},
    {NULL, NULL}
};

static int l_browser(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);
    const char		*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::Browser	*ptr = new fltk::Browser(x, y, w, h, program.Msg(title));
    g->end();

    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    lua_pushlightuserdata(L, ptr);
    luaL_openlib(L, NULL, browser_method, 1);
    
    gui_group(L, ptr);
    
    return 1;
}

// Buttons

static int l_button_value(lua_State *L) {
    int res = 0, val;

    fltk::Button *ptr = (fltk::Button*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L) && lua_isnumber(L, 1)) {
	val = (int) lua_tonumber(L, 1);
	ptr->value(val);
    } else {
	val = ptr->value();
	lua_pushnumber(L, val);
	res = 1;
    }
    return res;
}

static int l_button(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);
    const char		*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::Button	*ptr = new fltk::Button(x, y, w, h, program.Msg(title));
    g->end();

    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    gui_widget(L, ptr);

    return 1;
}

static int l_checkbutton(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);
    const char		*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::CheckButton	*ptr = new fltk::CheckButton(x, y, w, h, program.Msg(title));
    g->end();

    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_button_value, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

static int l_radiobutton(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);
    const char		*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::RadioButton	*ptr = new fltk::RadioButton(x, y, w, h, program.Msg(title));
    g->end();

    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_button_value, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

// Progress bar

static int l_progress_range(lua_State *L) {
    double 	min = luaL_checknumber(L, 1);
    double 	max = luaL_checknumber(L, 2);

    fltk::ProgressBar *ptr = (fltk::ProgressBar*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->range(min, max);

    return 0;
}

static int l_progress_step(lua_State *L) {
    double 	step = luaL_checknumber(L, 1);

    fltk::ProgressBar *ptr = (fltk::ProgressBar*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->step(step);

    return 0;
}

static int l_progress(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);
    const char		*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::ProgressBar	*ptr = new fltk::ProgressBar(x, y, w, h, program.Msg(title));
    g->end();

    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    lua_pushstring(L, "range");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_progress_range, 1);
    lua_rawset(L, -3);

    lua_pushstring(L, "step");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_progress_step, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}


// DrawBox

using namespace fltk;

class CDrawBox : public Widget {
public:
    CDrawBox(int x, int y, int w, int h, const char* label=0);

    void draw();
    int handle(int event);
};

CDrawBox::CDrawBox(int x,int y,int w,int h, const char* label) : Widget(x, y, w, h, label) {
};

void CDrawBox::draw() {
    Widget::draw();

    // get table
    lua.RawGetI(LUA_REGISTRYINDEX, (int)user_data());
    
    // get table.draw
    lua.PushString("draw");
    lua.GetTable(-2);

    if (lua.Type(-1) == LUA_TFUNCTION) {
	// move self
	lua.PushValue(-2);
	lua.Remove(-3);
	
	fltk::push_clip(0, 0, w(), h());
	lua.Call(1);
	fltk::pop_clip();
    } else {
	lua.Pop(2);
	/*
	lua.Remove(-1);		// nil
	lua.Remove(-1); 	// table
	*/
    }
}

int CDrawBox::handle(int event) {
    lua.RawGetI(LUA_REGISTRYINDEX, (int)user_data());
    
    lua.PushString("handle");
    lua.GetTable(-2);
    
    if (lua.Type(-1) != LUA_TFUNCTION) {
	lua.Pop(2);
	/*
	lua.Remove(-1);		// nil
	lua.Remove(-1); 	// table
	*/
	
	return 0;
    }

    lua.PushValue(-2);
    lua.Remove(-3);

    lua.NewTable();
    
    switch (event) {
	case fltk::PUSH:
	    lua.SetField("x", (double) fltk::event_x());
	    lua.SetField("y", (double) fltk::event_y());
	    lua.SetField("button", (double) fltk::event_button());
	    lua.SetField("click", (double) fltk::event_clicks());
	    break;
	    
	case fltk::SHORTCUT: {
	    char	*text;
	
	    switch (fltk::event_key()) {
		case fltk::EscapeKey: 		text = "Escape";	break;
		case fltk::BackSpaceKey: 	text = "BackSpace";	break;
		case fltk::TabKey: 		text = "Tab";		break;
		case fltk::ReturnKey: 		text = "Return";	break;
		case fltk::PrintKey: 		text = "Print";		break;
		case fltk::ScrollLockKey: 	text = "ScrollLock";	break;
		case fltk::PauseKey: 		text = "Pause";		break;
		case fltk::InsertKey: 		text = "Insert";	break;
		case fltk::HomeKey: 		text = "Home";		break;
		case fltk::PageUpKey: 		text = "PageUp";	break;
		case fltk::DeleteKey: 		text = "Delete";	break;
		case fltk::EndKey: 		text = "End";		break;
		case fltk::PageDownKey: 	text = "PageDown";	break;
		case fltk::LeftKey: 		text = "Left";		break;
		case fltk::UpKey: 		text = "Up";		break;
		case fltk::RightKey: 		text = "Right";		break;
		case fltk::DownKey: 		text = "Down";		break;
	    
		default:
		    text = (char*) fltk::event_text();
		    break;
	    }

	    char out[32] = "";

	    if (event_state() & fltk::SHIFT)
		strcat(out, "Shift+");
	    
	    if (event_state() & fltk::CTRL)
		strcat(out, "Ctrl+");

	    if (event_state() & fltk::ALT)
		strcat(out, "Alt+");

	    strcat(out, text);

	    lua.SetField("key", out);
	    break;
	}
	
	default:
	    printf("Event %i\n", event);
	    break;
    }

    lua.Call(2);

    return 0;
}

static int l_drawbox_line(lua_State *L) {
    int	x1 = (int)luaL_checknumber(L, 1);
    int y1 = (int)luaL_checknumber(L, 2);
    int x2 = (int)luaL_checknumber(L, 3);
    int y2 = (int)luaL_checknumber(L, 4);
	    
    fltk::drawline(x1, y1, x2, y2);
    return 0;
}

static int l_drawbox_rect(lua_State *L) {
    int	x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int w = (int)luaL_checknumber(L, 3);
    int h = (int)luaL_checknumber(L, 4);
	    
    fltk::strokerect(x, y, w, h);
    return 0;
}

static int l_drawbox_rectfill(lua_State *L) {
    int	x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int w = (int)luaL_checknumber(L, 3);
    int h = (int)luaL_checknumber(L, 4);
	    
    fltk::fillrect(x, y, w, h);
    return 0;
}

static int l_drawbox_text(lua_State *L) {
    int	x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);

    fltk::drawtext(text, x, y);
    return 0;
}

static int l_drawbox_color(lua_State *L) {
    char *color = (char*)luaL_checkstring(L, 1);

    fltk::line_style(fltk::SOLID, 1);
    fltk::setcolor(fltk::color(color));
    return 0;
}

static int l_drawbox_addvertex(lua_State *L) {
    int	x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
	    
    fltk::addvertex(x, y);
    return 0;
}

static int l_drawbox_addcurve(lua_State *L) {
    int	x1 = (int)luaL_checknumber(L, 1);
    int y1 = (int)luaL_checknumber(L, 2);
    int	x2 = (int)luaL_checknumber(L, 3);
    int y2 = (int)luaL_checknumber(L, 4);
    int	x3 = (int)luaL_checknumber(L, 5);
    int y3 = (int)luaL_checknumber(L, 6);
    int	x4 = (int)luaL_checknumber(L, 7);
    int y4 = (int)luaL_checknumber(L, 8);
	    
    fltk::addcurve(x1,y1, x2,y2, x3,y3, x4,y4);
    return 0;
}

static int l_drawbox_addarc(lua_State *L) {
    int	x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int	w = (int)luaL_checknumber(L, 3);
    int h = (int)luaL_checknumber(L, 4);
    int	a1 = (int)luaL_checknumber(L, 5);
    int a2 = (int)luaL_checknumber(L, 6);
	    
    fltk::addarc(x,y, w,h, a1,a2);
    return 0;
}

static int l_drawbox_strokepath(lua_State *L) {
    fltk::strokepath();
    return 0;
}

static int l_drawbox_fillpath(lua_State *L) {
    char *color = (char*)luaL_checkstring(L, 1);

    fltk::fillstrokepath(fltk::color(color));
    return 0;
}

static const luaL_reg drawbox_method[] = {
    {"line", l_drawbox_line},
    {"rect", l_drawbox_rect},
    {"rectfill", l_drawbox_rectfill},
    {"text", l_drawbox_text},
    {"color", l_drawbox_color},
    {"addvertex", l_drawbox_addvertex},
    {"addcurve", l_drawbox_addcurve},
    {"addarc", l_drawbox_addarc},
    {"strokepath", l_drawbox_strokepath},
    {"fillpath", l_drawbox_fillpath},
    {NULL, NULL}
};

static int l_drawbox(lua_State *L) {
    int	x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int w = (int)luaL_checknumber(L, 3);
    int h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    CDrawBox		*ptr = new CDrawBox(x, y, w, h);
    g->end();

    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);

    lua_pushlightuserdata(L, ptr);
    luaL_openlib(L, NULL, drawbox_method, 1);

    gui_widget(L, ptr);

    return 1;
}

// Image

static int l_image_draw(lua_State *L) {
    fltk::Image *ptr = (fltk::Image*) lua_touserdata(L, lua_upvalueindex(1));
    
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
		
    ptr->draw(x, y);
    
    return 0;
}

static int l_image(lua_State *L) {
    char *file = (char*)luaL_checkstring(L, 1);

    fltk::ImageType *it = fltk::guess_image(file);
				
    if (it) {
	fltk::Image* ptr = it->get(file, 0);

	if (ptr) {
	    int w = 0, h = 0;
	    ptr->measure(w, h);

	    lua_newtable(L);

	    lua_pushstring(L, "draw");
	    lua_pushlightuserdata(L, ptr);
	    lua_pushcclosure(L, l_image_draw, 1);
	    lua_rawset(L, -3);
	
	    return 1;
	}
    }
    
    return 0;
}

// Input

static int l_input_value(lua_State *L) {
    int res = 0;

    fltk::Input *ptr = (fltk::Input*) lua_touserdata(L, lua_upvalueindex(1));
    char*	val = (char*) lua_tostring(L, 1);

    if (val) {
	ptr->value((const char*) val);
    } else {
	val = (char *) ptr->value();
	lua_pushstring(L, val);
	res = 1;
    }
    return res;
}

static int l_multilineinput(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);
    int 	w = (int)luaL_checknumber(L, 3);
    int 	h = (int)luaL_checknumber(L, 4);
    const char	*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::MultiLineInput *ptr = new fltk::MultiLineInput(x, y, w, h, program.Msg(title));
    g->end();
    
    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    ptr->when(fltk::WHEN_CHANGED);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_input_value, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

static int l_multilineoutput(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);
    int 	w = (int)luaL_checknumber(L, 3);
    int 	h = (int)luaL_checknumber(L, 4);
    const char	*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::MultiLineOutput *ptr = new fltk::MultiLineOutput(x, y, w, h, program.Msg(title));
    g->end();
    
    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    ptr->when(fltk::WHEN_CHANGED);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_input_value, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

static int l_input(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);
    int 	w = (int)luaL_checknumber(L, 3);
    int 	h = (int)luaL_checknumber(L, 4);
    const char	*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::Input		*ptr = new fltk::Input(x, y, w, h, program.Msg(title));
    g->end();
    
    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    ptr->when(fltk::WHEN_CHANGED);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_input_value, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

static int l_output(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);
    int 	w = (int)luaL_checknumber(L, 3);
    int 	h = (int)luaL_checknumber(L, 4);
    const char	*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::Output	*ptr = new fltk::Output(x, y, w, h, program.Msg(title));
    g->end();
    
    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    ptr->when(fltk::WHEN_CHANGED);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_input_value, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

static int l_valuator_value(lua_State *L) {
    int 	res = 0;
    double	val;

    fltk::Valuator *ptr = (fltk::Valuator*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L) && lua_isnumber(L, 1)) {
	val = lua_tonumber(L, 1);
	ptr->value(val);
    } else {
	val = ptr->value();
	lua_pushnumber(L, val);
	res = 1;
    }
    return res;
}

static int l_valuator_range(lua_State *L) {
    double 	min = luaL_checknumber(L, 1);
    double 	max = luaL_checknumber(L, 2);

    fltk::Valuator *ptr = (fltk::Valuator*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->range(min, max);

    return 0;
}

static int l_valuator_step(lua_State *L) {
    double 	step = luaL_checknumber(L, 1);

    fltk::Valuator *ptr = (fltk::Valuator*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->linesize(step);

    return 0;
}

static int l_valueinput(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);
    int 	w = (int)luaL_checknumber(L, 3);
    int 	h = (int)luaL_checknumber(L, 4);
    const char	*title = luaL_checkstring(L, 5);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::ValueInput	*ptr = new fltk::ValueInput(x, y, w, h, program.Msg(title));
    g->end();
    
    lua_newtable(L);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    ptr->user_data((void*)ref);
    ptr->callback(callback);

    ptr->when(fltk::WHEN_CHANGED);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_valuator_value, 1);
    lua_rawset(L, -3);

    lua_pushstring(L, "range");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_valuator_range, 1);
    lua_rawset(L, -3);

    lua_pushstring(L, "step");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_valuator_step, 1);
    lua_rawset(L, -3);

    gui_widget(L, ptr);

    return 1;
}

// Box

static int l_box(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);
    int 	w = (int)luaL_checknumber(L, 3);
    int 	h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::InvisibleBox	*ptr = new fltk::InvisibleBox(x, y, w, h);
    g->end();

    ptr->align(fltk::ALIGN_LEFT | fltk::ALIGN_INSIDE);
    ptr->box(fltk::THIN_DOWN_BOX);
    
    lua_newtable(L);

    gui_widget(L, ptr);

    return 1;
}

// Html

static int l_html_value(lua_State *L) {
    fltk::HelpView 	*ptr = (fltk::HelpView*) lua_touserdata(L, lua_upvalueindex(1));
    char		*val = (char*) lua_tostring(L, 1);
    int 		res = 0;

    if (val) {
	ptr->value((const char*) val);
    } else {
	val = (char *) ptr->value();
	lua_pushstring(L, val);
	res = 1;
    }
    return res;
}

static int l_html_file(lua_State *L) {
    fltk::HelpView	*ptr = (fltk::HelpView*) lua_touserdata(L, lua_upvalueindex(1));
    char		*name = (char*) lua_tostring(L, 1);

    if (name) {
	ptr->load((const char*) name);
    }
    return 0;
}

static int l_html_textsize(lua_State *L) {
    int res = 0;
    unsigned x;

    fltk::HelpView *ptr = (fltk::HelpView*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L)) {
	x = lua_tonumber(L, 1);
	ptr->textsize(x);
    } else {
    }
    return res;
}

static int l_html(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);
    int 	w = (int)luaL_checknumber(L, 3);
    int 	h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::HelpView	*ptr = new fltk::HelpView(x, y, w, h);
    g->end();

    lua_newtable(L);

    gui_widget(L, ptr);

    lua_pushstring(L, "value");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_html_value, 1);
    lua_rawset(L, -3);

    lua_pushstring(L, "file");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_html_file, 1);
    lua_rawset(L, -3);

    lua_pushstring(L, "textsize");
    lua_pushlightuserdata(L, ptr);
    lua_pushcclosure(L, l_html_textsize, 1);
    lua_rawset(L, -3);

    return 1;
}

static int l_divider(lua_State *L) {
    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::Divider	*ptr = new fltk::Divider();
    g->end();

    lua_newtable(L);
    gui_widget(L, ptr);

    return 1;
}

// Group

static int l_group_resizable(lua_State *L) {
    fltk::Group *ptr = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_type(L, 1) == LUA_TTABLE) {
	lua_pushstring(L, ".ptr_widget");
	lua_gettable(L, 1);
	
	if (lua.Type(2) == LUA_TLIGHTUSERDATA) {
	    fltk::Widget *widget = (fltk::Widget*) lua_touserdata(L, 2);
	    
	    ptr->resizable(widget);
	}
    }
    
    return 0;
}

static int l_group_add(lua_State *L) {
    fltk::Group *ptr = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_type(L, 1) == LUA_TTABLE) {
	lua_pushstring(L, ".ptr_widget");
	lua_gettable(L, 1);
	
	if (lua.Type(2) == LUA_TLIGHTUSERDATA) {
	    fltk::Widget *widget = (fltk::Widget*) lua_touserdata(L, 2);
	    
	    ptr->add(widget);
	}
    }
    
    return 0;
}

static int l_group_position(lua_State *L) {
    int 	x = (int)luaL_checknumber(L, 1);
    int 	y = (int)luaL_checknumber(L, 2);

    fltk::Group *ptr = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));

    if (lua_gettop(L) == 2) {
	ptr->position(x, y);
    } else if (lua_gettop(L) == 4) {
	int 	w = (int)luaL_checknumber(L, 3);
	int 	h = (int)luaL_checknumber(L, 4);

	ptr->resize(x, y, w, h);
    }

    return 0;
}

static int l_group_resize(lua_State *L) {
    int 	w = (int)luaL_checknumber(L, 1);
    int 	h = (int)luaL_checknumber(L, 2);

    fltk::Group *ptr = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));

    ptr->resize(w, h);

    return 0;
}

static int l_group(lua_State *L) {
    int 		x = (int)luaL_checknumber(L, 1);
    int 		y = (int)luaL_checknumber(L, 2);
    int 		w = (int)luaL_checknumber(L, 3);
    int 		h = (int)luaL_checknumber(L, 4);

    fltk::Group 	*g = (fltk::Group*) lua_touserdata(L, lua_upvalueindex(1));
    g->begin();
    fltk::Group		*ptr = new fltk::Group(x, y, w, h);
    g->end();

    lua_newtable(L);
    gui_group(L, ptr);
    
    return 1;
}

static const luaL_reg group_method[] = {
    {"resizable", l_group_resizable},
    {"add", l_group_add},
    {"position", l_group_position},
    {"resize", l_group_resize},
    {"choice", l_choice},
    {"button", l_button},
    {"checkbutton", l_checkbutton},
    {"radiobutton", l_radiobutton},
    {"drawbox", l_drawbox},
    {"input", l_input},
    {"output", l_output},
    {"multilineinput", l_multilineinput},
    {"multilineoutput", l_multilineoutput},
    {"valueinput", l_valueinput},
    {"group", l_group},
    {"tabgroup", l_tabgroup},
    {"itemgroup", l_itemgroup},
    {"tiledgroup", l_tiledgroup},
    {"browser", l_browser},
    {"box", l_box},
    {"menubar", l_menubar},
    {"html", l_html},
    {"progress", l_progress},
    {"item", l_item},
    {"divider", l_divider},
    {NULL, NULL}
};

void gui_group(lua_State *L, fltk::Group *ptr) {
    lua_pushlightuserdata(L, ptr);
    luaL_openlib(L, NULL, group_method, 1);

    gui_widget(L, ptr);
}

// Window


static int l_window(lua_State *L) {
    fltk::DoubleBufferWindow *ptr;

    int 	w = (int)luaL_checknumber(L, 1);
    int 	h = (int)luaL_checknumber(L, 2);
    const char	*title = luaL_checkstring(L, 3);
    
    ptr = new fltk::DoubleBufferWindow(w, h, program.Msg(title));

    lua_newtable(L);

    gui_group(L, ptr);

    return 1;
}

//

static const luaL_reg guilib[] = {
    {"ready", l_ready},
    {"check", l_check},
    {"run", l_run},
    {"filechooser", l_filechooser},
    {"message", l_message_dialog},
    {"alert", l_alert_dialog},
    {"choice", l_choice_dialog},
    {"window", l_window},
    {"image", l_image},
    {"monitor", l_monitor},
    {NULL, NULL}
};

int luaopen_gui(lua_State * L) {
    luaL_openlib(L, "gui", guilib, 0);

    return 1;
}
