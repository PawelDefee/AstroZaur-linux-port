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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <libxml/parser.h>

using namespace std;

string leadcut(const string &base, const string &delim = "\t\n\r ");
string trailcut(const string &base, const string &delim = "\t\n\r ");

bool has_content(xmlNodePtr node);

char* xml_level(int i);

void Time(bool start);
double Dist(double i1, double i2);
void TranslChars(const char *src, char *dst);
void DoubleToStr(double d, char* str);
unsigned int Hash(const char *str, unsigned int start = 0);

#endif
