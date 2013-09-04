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

#include <math.h>
#include <string.h>
#include "coord.h"

CCoord::CCoord() {
    iLon = 0;
    iLat = 0;
}

double CCoord::Lon() {
    return iLon/3600.0;
}

double CCoord::Lat() {
    return iLat/3600.0;
}

void CCoord::Lon(double data) {
    int n;
    
    iLon = data*3600;

    if (iLon < 0) {
	n = (iLon / (3600*360)) - 1;
	iLon = iLon - n*3600*360;
    } else if (iLon > 3600*360) {
	n = iLon / (3600*360);
	iLon = iLon - n*3600*360;
    }
    Ready(false);
}

void CCoord::Lat(double data) {
    iLat = data*3600.0;
    Ready(false);
}

char* CCoord::LonStr() {
    const char	*signName[] = {"Ar", "Ta", "Ge", "Cn", "Le", "Vi", 
			       "Li", "Sc", "Sg", "Cp", "Aq", "Pi"};
    char buf[32];
    int g, m, s;
    
    g = iLon / 3600;
    m = iLon % 3600 / 60;
    s = iLon % 60;

    sprintf(buf, "%s%i %s %s%i'%s%i\"",
	(g % 30) < 10 ? " ":"",
	g % 30,
	signName[g/30],
	m < 10 ? "0":"",m,
	s < 10 ? "0":"",s);

    return strdup(buf);
}

char* CCoord::LatStr() {
    char buf[32], *str;
    int	c, g, m, s;
    
    c = abs(iLat);
    g = c / 3600;
    m = c % 3600 / 60;
    s = c % 60;

    sprintf(buf, "%c%i %s%i'%s%i\"",
	iLat > 0 ? '+':'-',
	g,
	m < 10 ? "0":"",m,
	s < 10 ? "0":"",s);
    str = strdup(buf);
    return str;
}
