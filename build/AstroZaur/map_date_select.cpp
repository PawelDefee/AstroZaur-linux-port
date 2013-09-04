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

#include "map_date_select.h"
#include "utils.h"
#include "program.h"

void CMapDateSelect::Calc() {
    double jd = JD();
    CMap* mapPtr;
    list<CMapNatal*>::iterator map;
    list<CPoint*>::iterator to;
    list<CPoint*>::iterator from;

    for (map = iMaps.begin(); map != iMaps.end(); map++)
	if (jd < (*map)->JD()) break;
	
    if (map != iMaps.begin()) map--;
    
    mapPtr = *map;
    mapPtr->DoCalc();
    
    from = mapPtr->iPointsList.begin();
    for (to = iPointsList.begin(); to != iPointsList.end(); to++, from++) {
	*(*to) << *(*from);
    }
}
