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
#include "utils.h"
#include "lua.h"
#include "date.h"
#include "program.h"
#include "static_holder.h"
#define lua CStaticHolder::GetLua()

#define	JD1970 2440587.5

extern "C" {
    #include <sweph/swephexp.h>
}

CDate::CDate() {
    Date("now");
    Time("now");
    TimeZone("GMT");

    PushTable();
    lua.SetField(".ptr_date", this);
    lua.Pop(1);
}

void CDate::Date(const char *str) {
    int out, i, data;
    char line[16];

    Ready(false);

    if (!strcasecmp(str,"now")) {
	time_t now = time(NULL);
	tm *now_tm = localtime(&now);

	iDay = now_tm->tm_mday;
	iMon = now_tm->tm_mon + 1;
	iYear = now_tm->tm_year + 1900;
	
	program.Event(this, eventChangeDate);
	return;
    }
    
    out = 0;
    i = 0;
    while (1) {
	if (*str == '.' || *str == '/' || *str == 0) {
	    line[i++] = 0;
	    data = atoi(line);
	    
	    switch (out) {
		case 0:
		    iDay = data;
		    break;
		case 1:
		    iMon = data;
		    break;
		case 2:
		    iYear = data;
		    break;
	    }
	    
	    if (*str == 0) break;
	    i = 0;
	    out++;
	    str++;
	}
	line[i++] = *(str++);
    }
    program.Event(this, eventChangeDate);
}

void CDate::XML(xmlNodePtr node) {
    if (!strcmp((char*)node->name,"date")) {
	if (has_content(node)) 
	    Date((char *) node->children->content);
    }
    if (!strcmp((char*)node->name,"time")) {
    	xmlChar *zone = xmlGetProp(node,(xmlChar *) "zone");;
	
	if (zone) {
	    TimeZone((char *) zone);
	    xmlFree(zone);
	}

	if (has_content(node))
	    Time((char *) node->children->content);
    }
}

xmlNodePtr CDate::XML(xmlNodePtr parent, int level) {
    xmlNodePtr node, subNode;

    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level)));
    node = xmlNewTextChild(parent, NULL, (xmlChar*)"date", (xmlChar*)Date());

    xmlAddChild(parent, xmlNewText((xmlChar*)xml_level(level)));
    subNode = xmlNewTextChild(parent, NULL, (xmlChar*)"time", (xmlChar*)Time());
    xmlNewProp(subNode, (xmlChar*)"zone", (xmlChar*)iTimeZone.Name());

    return node;
}

void CDate::Time(const char *str) {
    Ready(false);
    if (!strcasecmp(str,"now")) {
	time_t now = time(NULL);
	tm *now_tm = localtime(&now);

	iTime = now_tm->tm_sec + now_tm->tm_min*60 + now_tm->tm_hour*3600;
    } else {
	iTime = TimeFromStr(str);
    }
    program.Event(this, eventChangeTime);
}

void CDate::TimeZone(const char *str) {
    time_t	sec;
    
    iTimeZone.Name((char *) str);
    
    iJD = swe_julday(iYear, iMon, iDay, 0, SE_GREG_CAL);
    sec = (iJD - JD1970) * (3600.0*24.0) + iTime;
    iOffset = iTimeZone.Offset(sec);
	
    sec -= iOffset;
    iOffset = iTimeZone.Offset(sec);
	
    iJD += (iTime - iOffset) / (3600.0*24);
    Ready(true);

    program.Event(this, eventChangeTimeZone);
}

char* CDate::TimeZone() {
    return iTimeZone.Name();
}

char* CDate::TimeOffset() {
    char 	buf[32];

    TimeToStr(iOffset, buf);
    return strdup(buf);
}

char* CDate::Date() {
    char buf[32];
    
    sprintf(buf, "%s%i/%s%i/%i", 
	iDay < 10 ? "0":"", iDay, 
	iMon < 10 ? "0":"", iMon, 
	iYear);
    return strdup(buf);
}

char* CDate::Formated(char* format) {
    static char buf[64];
    char str[16];
    char *to, *from;
    int	 t, h, m, s, sign = 1;
    
    if (iTime < 0) sign = -1;
    t = abs(iTime);
    
    s = t % 60;
    m = (t / 60) % 60;
    h = (t / 3600) % 60;
    
    to = (char*) &buf;
    from = format;
    
    while (*from) {
	if (*from == '%') {
	    from++;
	    switch (*from) {
		case 'Y':
		    sprintf(str,"%i",iYear);
		    memcpy(to, str, strlen(str));
		    to += strlen(str);
		    break;
		case 'm':
		    sprintf(str,"%s%i", iMon < 10 ? "0":"", iMon);
		    memcpy(to, str, strlen(str));
		    to += strlen(str);
		    break;
		case 'd':
		    sprintf(str,"%s%i", iDay < 10 ? "0":"", iDay);
		    memcpy(to, str, strlen(str));
		    to += strlen(str);
		    break;
		case 'H':
		    sprintf(str,"%s%i", h < 10 ? "0":"", h);
		    memcpy(to, str, strlen(str));
		    to += strlen(str);
		    break;
		case 'M':
		    sprintf(str,"%s%i", m < 10 ? "0":"", m);
		    memcpy(to, str, strlen(str));
		    to += strlen(str);
		    break;
		case 'S':
		    sprintf(str,"%s%i", s < 10 ? "0":"", s);
		    memcpy(to, str, strlen(str));
		    to += strlen(str);
		    break;
		default:
		    break;
	    }
	    from++;
	} else {
	    *to = *from;
	    to++;
	    from++;
	}
    }
    
    *to = 0;
    return buf;
}

char* CDate::Time() {
    char buf[32];
    
    TimeToStr(iTime,buf);
    return strdup(buf);
}

double CDate::JD() {
    if (! Ready()) {
	time_t	sec;
    
	iJD = swe_julday(iYear, iMon, iDay, 0, SE_GREG_CAL);
	sec = (iJD - JD1970) * (3600.0*24.0) + iTime;
	iOffset = iTimeZone.Offset(sec);
	
	sec -= iOffset;
	iOffset = iTimeZone.Offset(sec);
	
	iJD += (iTime - iOffset) / (3600.0*24);
	Ready(true);
    }
    return iJD;
}

void CDate::JD(double data) {
    double 	time, local;
    time_t	sec;

    iJD = data;

    sec = (data - JD1970) * (3600.0*24.0);
    iOffset = iTimeZone.Offset(sec);
    local = data + iOffset/(3600.0*24.0);
    swe_revjul(local, SE_GREG_CAL, &iYear, &iMon, &iDay, &time);
    iTime = time * 3600.0;

    Ready(true);
    program.Event(this, eventChangeDate);
    program.Event(this, eventChangeTime);
    program.Event(this, eventChangeTimeZone);
}
    
void CDate::Rotate(long int sec) {
    double tjd_ut, time;

    tjd_ut = swe_julday(iYear, iMon, iDay, 0, SE_GREG_CAL);
    iTime += sec;

    if (iTime < 0 || iTime > 24*3600) {
	long int days = iTime / (24*3600);
    
	if (iTime < 0) days--;
	tjd_ut += days;
	iTime -= days * 24*3600;
	swe_revjul(tjd_ut, SE_GREG_CAL, &iYear, &iMon, &iDay, &time);
    }
    Ready(false);
    program.Event(this, eventChangeDate);
    program.Event(this, eventChangeTime);
}

void CDate::TimeToStr(const int time, char *str) {
    int t, h, m, s, sign = 1;
    
    if (time < 0) sign = -1;
    t = abs(time);
    
    s = t % 60;
    m = (t / 60) % 60;
    h = (t / 3600) % 60;
    sprintf(str, "%s%i:%s%i:%s%i",
	sign < 0 ? "-":"",
	h,
	m < 10 ? "0":"", m,
	s < 10 ? "0":"", s);
}

int CDate::TimeFromStr(const char *str) {
    int		scale, i, sign = 1, data, time = 0;
    char	line[16];
    
    scale = 60*60;
    i = 0;
    while (1) {
	if (*str == '-') {
	    sign = -1;
	    str++;
	}
	if (*str == ':' || *str == 0) {
	    line[i++] = 0;
	    data = atoi(line);
	    time += data * scale;
	    if (*str == 0) break;
	    scale /= 60;
	    i = 0;
	    str++;
	}
	line[i++] = *(str++);
    }
    return time*sign;
}

bool CDate::LuaSet(const char *var) {
    if (strcmp(var, "jd") == 0) {
        JD(lua.ToNumber(-1));
	return true;
    } else if (strcmp(var, "zone") == 0) {
	TimeZone(lua.ToString(-1));
	return true;
    }
    
    return false;
}

bool CDate::LuaGet(const char* var) {
    if (strcmp(var, "day") == 0) {
	lua.PushNumber(iDay);
	return true;
    } else if (strcmp(var, "month") == 0) {
	lua.PushNumber(iMon);
	return true;
    } else if (strcmp(var, "year") == 0) {
	lua.PushNumber(iYear);
	return true;
    } else if (strcmp(var, "offset") == 0) {
	lua.PushNumber(iOffset/3600.0);
	return true;
    } else if (strcmp(var, "hour") == 0) {
	lua.PushNumber((iTime/3600) % 60);
	return true;
    } else if (strcmp(var, "min") == 0) {
	lua.PushNumber((iTime/60) % 60);
	return true;
    } else if (strcmp(var, "sec") == 0) {
	lua.PushNumber(iTime % 60);
	return true;
    } else if (strcmp(var, "jd") == 0) {
	lua.PushNumber(JD());
	return true;
    } else if (strcmp(var, "zone") == 0) {
	lua.PushString(TimeZone());
	return true;
    }

    return false;
}
