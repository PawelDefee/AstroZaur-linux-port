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

#ifndef _DATE_H_
#define _DATE_H_

#include "astro.h"
#include "timezone.h"

// DateTime class

class CDate : public CAstro { 
private:
    int 	iDay;
    int 	iMon;
    int 	iYear;
    long int 	iTime;		// In seconds
    CTimeZone 	iTimeZone;
    long int	iOffset;	// from GMT, in seconds
    
    double	iJD;
    
    void TimeToStr(const int time, char *str);
    int TimeFromStr(const char *str);
    
public:
    CDate();
    ~CDate() {};

    void XML(xmlNodePtr node);
    xmlNodePtr XML(xmlNodePtr parent, int level);

    void Date(const char *str);
    void Time(const char *str);
    void TimeZone(const char *str);
    char* Date();
    char* Time();
    char* TimeZone();
    char* TimeOffset();
    char* Formated(char* format);
    
    double JD();
    void JD(double data);
    
    void Rotate(long int sec);

    bool LuaSet(const char* var);
    bool LuaGet(const char* var);
};

#endif
