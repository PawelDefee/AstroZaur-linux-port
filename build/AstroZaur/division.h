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

#ifndef _DIVISION_H_
#define _DIVISION_H_

#include "astro.h"
#include <list>

using namespace std;

// Part item

typedef struct {
    char*		iData;
    char*		iInfo;
    unsigned int	iInfoHash;
    double		iFrom;
    double		iTo;
} DivisionItemType;

class CDivision : public CAstro {
private:
    DivisionItemType*	iItem;
    unsigned int	iItemNum;
    
public:
    CDivision();
    ~CDivision();

    bool Get(double lon, const char* info, char** data);
    void New(const char* info, double from, double to, const char* data);
    void Sort();
};

#include "static_holder.h"
#define division CStaticHolder::GetDivision()

#endif
