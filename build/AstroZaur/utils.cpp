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

#include <time.h>
#include "utils.h"
#include "string.h"

string leadcut(const string &base, const string &delim) {
    int pos = base.find_first_not_of(delim);
    return (pos != -1) ? base.substr(pos) : "";
}

string trailcut(const string &base, const string &delim) {
    int pos = base.find_last_not_of(delim);
    return (pos != -1) ? base.substr(0, pos+1) : "";
}

bool has_content(xmlNodePtr node) {
    if (node->children)
	if (node->children->content) return true;
    return false;
}

char* xml_level(int level) {
    static char str[128];
    int i = 0;
    
    str[0] = '\n';
    while (i < level) {
	str[i+1] = '\t';
	i++;
    }
    str[i++] = 0;
    
    return str;
}

void Time(bool start) {
    static time_t now;
    
    if (start) {
        now = time(NULL);
    } else {
	printf("Time %i sec\n",time(NULL)-now);
    }
}

double Dist(double i1, double i2) {
    double res, res_abs;

    res = i2 - i1;
    if (res < 0.0) res_abs = -res;
	else res_abs = res;
    
    if (i1 > i2) {
	if (res_abs > 180.0) {
	    res = 360.0-res_abs;
	    if (i1 < 180.0) res = -res;
	}
    } else {
	if (res_abs > 180.0) {
	    res = 360.0-res_abs;
	    res = -res;
	}
    }
    return res;
}

void TranslChars(const char *src, char *dst) {
    int		i;
    char	c;
    
    for (i = 0; *src != 0; src++,i++) {
	c = (*src);
	switch (*src) {
	    case '\\':
		src++;
		switch (*src) {
		    case 'n' : 
			c = '\n';
			break;
		    case 'r' :
			c = '\r';
			break;
		    case 't' :
			c = '\t';
			break;
		    default:
			src--;
			c = *src;
		}
	    case '[':
		dst[i] = '<';
		break;
	    case ']':
		dst[i] = '>';
		break;
	    default:
		dst[i]= c;
	}
    }
}

void DoubleToStr(double d, char* str) {
    sprintf(str,"%f",d);
    
    for (int i = strlen(str)-1; i>0; i--)
	if (str[i] == '0') {
	    str[i] = 0;
	} else {
	    if (str[i] == '.') str[i] = 0;
	    break;
	}
}

unsigned int Hash(const char *str, unsigned int start) {
    char 		*key = (char*) str;
    unsigned int	h = start;

    while (*key)
	h = h * 37 ^ (*key++ - ' ');
	
    return h;
}
