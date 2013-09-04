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

#include "map_natal.h"
#include "transform_horizont.h"

extern "C" {
    #include <sweph/swephexp.h>
}

void CTransformHorizont::Do(CMap* map) {
    list<CPoint*>::iterator	p;
    CMapNatal			*natal = (CMapNatal*) map;
    double			jd, jd_et;
    double			armc, eps, lat;
    double			x[6];
    
    jd = natal->JD();
    armc = swe_degnorm(swe_sidtime(jd) * 15.0 + natal->Place()->Lon() + 90.0);
    jd_et = jd + swe_deltat(jd);
    swe_calc(jd_et, SE_ECL_NUT, 0, x, NULL);
    eps = x[0];
    lat = 90 - natal->Place()->Lat();
    
    for (p = natal->iPointsList.begin(); p != natal->iPointsList.end(); p++) {
	x[0] = (*p)->Ecliptic()->Lon();
	x[1] = (*p)->Ecliptic()->Lat();
	x[2] = 1;
	
	swe_cotrans(x, x, -eps);
	
	x[0] = x[0] - armc;
	x[2] = 1;
	
	/* azimuth from east, counterclock */
	swe_cotrans(x, x, lat);
	
	/* azimuth from south to west */
	(*p)->Ecliptic()->Lon(360.0 - (x[0] + 90.0));
	(*p)->Ecliptic()->Lat(x[1]);
    }
}
