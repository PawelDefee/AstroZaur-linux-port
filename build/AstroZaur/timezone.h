/*
    AstroZaur - Universal astrologic processor
    Copyright (C) 2006 Belousov Oleg <belousov.oleg@gmail.com>
    http://www.strijar.ru/astrozaur

    This file based on localtime.c
    "Sources for Time Zone and Daylight Saving Time Data"
    from http://www.twinsun.com/tz/tz-link.htm

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

#include <sys/types.h>	/* for time_t */

#ifndef _TIMEZONE_H_
#define _TIMEZONE_H_

#define TRUE		1
#define FALSE		0
#define R_OK		4

#define TZ_MAX_TIMES	1200
#define TZ_MAX_TYPES	20	/* Maximum number of local time types */
#define TZ_MAX_CHARS	50	/* Maximum number of abbreviation characters */
#define TZ_MAX_LEAPS	50	/* Maximum number of leap second corrections */

#define YEARSPERREPEAT	400	/* years before a Gregorian repeat */
#define AVGSECSPERYEAR	31556952L
#define SECSPERREPEAT	((int_fast64_t) YEARSPERREPEAT * (int_fast64_t) AVGSECSPERYEAR)
#define SECSPERREPEAT_BITS	34
#define SECSPERHOUR	(60*60)
#define SECSPERDAY	(60*60*24)
#define DAYSPERNYEAR	365
#define DAYSPERLYEAR	366
#define	DAYSPERWEEK	7
#define MONSPERYEAR	12
#define	HOURSPERDAY	24
#define MINSPERHOUR	60
#define SECSPERMIN	60

#define MY_TZNAME_MAX	255

#define BIGGEST(a, b)		(((a) > (b)) ? (a) : (b))
#define TYPE_SIGNED(type)	(((type) -1) < 0)
#define TYPE_INTEGRAL(type)	(((type) 0.5) != 0.5)
#define TYPE_BIT(type)		(sizeof (type) * CHAR_BIT)

#define is_digit(c)		((unsigned)(c) - '0' <= 9)
#define isleap(y)		(((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

#define EPOCH_YEAR		1970
#define TM_YEAR_BASE		1900
#define TM_THURSDAY		4
#define EPOCH_WDAY		TM_THURSDAY

#define JULIAN_DAY		0
#define DAY_OF_YEAR		1
#define MONTH_NTH_DAY_OF_WEEK	2

#define	TZ_MAGIC		"TZif"
#define TZDEFAULT		"localtime"
#define TZDEFRULES		"posixrules"
#define TZDEFRULESTRING 	",M4.1.0,M10.5.0"

typedef long long		int_fast64_t;

static const char 		gmt[] = "GMT";

struct tmg {
    int     	tm_sec;         /* seconds */
    int     	tm_min;         /* minutes */
    int     	tm_hour;        /* hours */
    int     	tm_mday;        /* day of the month */
    int     	tm_mon;         /* month */
    int     	tm_year;        /* year */
    int     	tm_wday;        /* day of the week */
    int     	tm_yday;        /* day in the year */
    int     	tm_isdst;       /* daylight saving time */
    long	tm_gmtoff;	/* UTC offset in seconds */
};
																										    
struct ttinfo {			/* time type information */
    long 	tt_gmtoff;	/* UTC offset in seconds */
    int 	tt_isdst;	/* used to set tm_isdst */
    int 	tt_abbrind;	/* abbreviation list index */
    int 	tt_ttisstd;	/* TRUE if transition is std time */
    int 	tt_ttisgmt;	/* TRUE if transition is UTC */
};

struct lsinfo {			/* leap second information */
    time_t 	ls_trans;	/* transition time */
    long 	ls_corr;	/* correction to apply */
};

struct state {
    int 		leapcnt;
    int 		timecnt;
    int 		typecnt;
    int 		charcnt;
    int 		goback;
    int 		goahead;
    time_t 		ats[TZ_MAX_TIMES];
    unsigned char 	types[TZ_MAX_TIMES];
    struct ttinfo 	ttis[TZ_MAX_TYPES];
    char 		chars[BIGGEST (BIGGEST (TZ_MAX_CHARS + 1, sizeof gmt),
		    	    (2 * (MY_TZNAME_MAX + 1)))];
    struct lsinfo 	lsis[TZ_MAX_LEAPS];
};

class CTimeZone {
private:
    struct state	lcl;
    char		*iName;

    long	detzcode(const char *const codep);
    time_t	detzcode64(const char *const codep);
    const char* getzname(const char *strp);
    const char* getqzname(const char *strp, const int delim);
    const char* getsecs(const char *strp, long *const secsp);
    const char* getoffset(const char *strp, long *const offsetp);
    const char* getrule(const char *strp, struct rule *const rulep);
    int 	differ_by_repeat(const time_t t1, const time_t t0);
    int 	increment_overflow(int *number, int delta);
    int 	leaps_thru_end_of(int y);
    
    const char* getnum(const char *strp, 
		    int *const nump, 
		    const int min, 
		    const int max);

    time_t 	transtime(const time_t janfirst, 
		    const int year, 
		    const struct rule *const rulep, 
		    const long offset);

    int 	tzparse(const char *name, 
		    struct state *const sp,
		    const int lastditch);
    
    struct tmg*	timesub(const time_t *const timep,
		    const long offset,
		    const struct state *const sp,
		    struct tmg *const tmp);

    int 	tzload(const char *name, 
		    struct state *const sp,
		    const int doextend);

    struct tmg*	localsub(const time_t *const timep, 
		    const long offset, 
		    struct tmg *tmp);
	
public:
    CTimeZone();
    ~CTimeZone();

    void Name(char* name);
    char* Name();
    
    long Offset(time_t time, int *dst = NULL);
};

#endif
