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
#include "division.h"
#include "utils.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

CDivision::CDivision() {
    iItem = NULL;
    iItemNum = 0;
}

CDivision::~CDivision() {
    for (int i = 0; i < iItemNum; i++) {
	free(iItem[i].iInfo);
	free(iItem[i].iData);
    }

    free(iItem);
}

static int ItemCompare(const void *a, const void *b) {
    DivisionItemType*	A = (DivisionItemType*) a;
    DivisionItemType*	B = (DivisionItemType*) b;

    if (A->iInfoHash < B->iInfoHash) return 1;
    if (A->iInfoHash > B->iInfoHash) return -1;

    if (A->iFrom < B->iFrom && A->iTo < B->iTo) return 1;
    if (A->iFrom > B->iFrom && A->iTo > B->iTo) return -1;

    return 0;
}

bool CDivision::Get(double lon, const char* info, char** data) {
    DivisionItemType	key;
    DivisionItemType	*res;
    
    key.iInfoHash = Hash(info);
    key.iFrom = lon;
    key.iTo = lon;
    
    res = (DivisionItemType*) bsearch(&key, iItem, iItemNum, sizeof(DivisionItemType), ItemCompare);
    
    if (res) {
	*data = res->iData;
	return true;
    } 

    return false;
}

void CDivision::New(const char* info, double from, double to, const char* data) {
    iItemNum++;
    iItem = (DivisionItemType*) realloc(iItem, iItemNum * sizeof(DivisionItemType));

    DivisionItemType*	item = &iItem[iItemNum-1];
		
    item->iInfo = strdup(info);
    item->iInfoHash = Hash(info);
    item->iData = strdup((char*) data);
    item->iFrom = from;
    item->iTo = to;
}

void CDivision::Sort() {
    qsort(iItem, iItemNum, sizeof(DivisionItemType), ItemCompare);
}
