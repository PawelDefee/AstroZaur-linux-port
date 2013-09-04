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

#include <string.h>
#include <fltk/events.h>
#include "base_window.h"
#include "utils.h"
#include "lua.h"

CAstroWindow::CAstroWindow(int x, int y) : fltk::DoubleBufferWindow(x, y) {
}

CAstroWindow::CAstroWindow(int x, int y, const char* title) : fltk::DoubleBufferWindow(x, y, title) {
}

int CAstroWindow::handle(int event) {
    if (event == fltk::HIDE) {
    }

    return DoubleBufferWindow::handle(event);
}

//

CBaseWindow::CBaseWindow() {
    iType = "window";
    iWindow = NULL;
    iTitle = NULL;
    iMenu = NULL;
}

CBaseWindow::~CBaseWindow() {
    iWindow->destroy();
    if (iTitle) free(iTitle);
}

void CBaseWindow::Title(const char* title) {
    if (iTitle) free(iTitle);
    iTitle = strdup((char*) title);
    iWindow->label(iTitle);
}
    
void CBaseWindow::XML(xmlNodePtr node) {
    xmlChar*	x = xmlGetProp(node,(xmlChar *) "x");
    xmlChar*	y = xmlGetProp(node,(xmlChar *) "y");
    xmlChar*	w = xmlGetProp(node,(xmlChar *) "w");
    xmlChar*	h = xmlGetProp(node,(xmlChar *) "h");
    xmlChar*	title = xmlGetProp(node,(xmlChar *) "title");
    
    if (x && y) {
	iWindow->position(atoi((char*) x), atoi((char*) y));
	xmlFree(x);
	xmlFree(y);
    }
			
    if (w && h) {
	iWindow->resize(atoi((char*) w), atoi((char*) h));
	xmlFree(w);
	xmlFree(h);
    }

    if (title) {
	Title((char*)title);
	xmlFree(title);
    }
}

xmlNodePtr CBaseWindow::XML(xmlNodePtr parent, int level) {
    xmlNodePtr	sub;
    char	str[32];
    int 	x, y, w, h;
    
    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level)));
    sub = xmlNewTextChild(parent, NULL, (xmlChar*)iType, NULL);
	
    x = iWindow->x();
    y = iWindow->y();
    w = iWindow->w();
    h = iWindow->h();
	
    sprintf(str, "%i", x);
    xmlNewProp(sub, (xmlChar*)"x", (xmlChar*)str);

    sprintf(str, "%i", y);
    xmlNewProp(sub, (xmlChar*)"y", (xmlChar*)str);

    sprintf(str, "%i", w);
    xmlNewProp(sub, (xmlChar*)"w", (xmlChar*)str);

    sprintf(str, "%i", h);
    xmlNewProp(sub, (xmlChar*)"h", (xmlChar*)str);
    
    if (iTitle) xmlNewProp(sub, (xmlChar*)"title", (xmlChar*)iTitle);

    return sub;
}

void CBaseWindow::Show() {
    iWindow->show();
}

bool CBaseWindow::Visible() {
    return iWindow->visible();
}

