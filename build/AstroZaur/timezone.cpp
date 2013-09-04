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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>		/* for CHAR_BIT et al. */
#include <sys/param.h>
#include <fcntl.h>
#include <string.h>

#include "program.h"
#include "timezone.h"

struct rule
{
    int r_type;			/* type of rule--see below */
    int r_day;			/* day number of rule */
    int r_week;			/* week number of rule */
    int r_mon;			/* month number of rule */
    long r_time;		/* transition time of rule */
};

struct tzhead
{
    char tzh_magic[4];		/* TZ_MAGIC */
    char tzh_version[1];	/* '\0' or '2' as of 2005 */
    char tzh_reserved[15];	/* reserved--must be zero */
    char tzh_ttisgmtcnt[4];	/* coded number of trans. time flags */
    char tzh_ttisstdcnt[4];	/* coded number of trans. time flags */
    char tzh_leapcnt[4];	/* coded number of leap seconds */
    char tzh_timecnt[4];	/* coded number of transition times */
    char tzh_typecnt[4];	/* coded number of local time types */
    char tzh_charcnt[4];	/* coded number of abbr. chars */
};

static const int year_lengths[2] = {
    DAYSPERNYEAR, DAYSPERLYEAR
};

static const int mon_lengths[2][MONSPERYEAR] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

long
CTimeZone::detzcode (const char *const codep)
{
    long result;
    int i;

    result = (codep[0] & 0x80) ? ~0L : 0;

    for (i = 0; i < 4; ++i)
	result = (result << 8) | (codep[i] & 0xff);
    return result;
}

time_t
CTimeZone::detzcode64 (const char *const codep)
{
    time_t result;
    int i;

    result = (codep[0] & 0x80) ? (~(int_fast64_t) 0) : 0;

    for (i = 0; i < 8; ++i)
	result = result * 256 + (codep[i] & 0xff);
    return result;
}

int
CTimeZone::differ_by_repeat (const time_t t1, const time_t t0)
{
    if (TYPE_INTEGRAL (time_t) &&
	TYPE_BIT (time_t) - TYPE_SIGNED (time_t) < SECSPERREPEAT_BITS)
	return 0;

    return t1 - t0 == SECSPERREPEAT;
}

const char *
CTimeZone::getzname (const char *strp)
{
    char c;

    while ((c = *strp) != '\0' && !is_digit (c) && c != ',' && c != '-'
	   && c != '+')
	++strp;
    return strp;
}

const char *
CTimeZone::getqzname (const char *strp, const int delim)
{
    int c;

    while ((c = *strp) != '\0' && c != delim)
	++strp;
    return strp;
}

const char *
CTimeZone::getnum (const char *strp, int *const nump, const int min,
		   const int max)
{
    char c;
    int num;

    if (strp == NULL || !is_digit (c = *strp))
	return NULL;
    num = 0;

    do {
	num = num * 10 + (c - '0');
	if (num > max)
	    return NULL;	/* illegal value */
	c = *++strp;
    }

    while (is_digit (c));
    if (num < min)
	return NULL;		/* illegal value */
    *nump = num;
    return strp;
}

const char *
CTimeZone::getsecs (const char *strp, long *const secsp)
{
    int num;

    strp = getnum (strp, &num, 0, HOURSPERDAY * DAYSPERWEEK - 1);
    if (strp == NULL)
	return NULL;

    *secsp = num * (long) SECSPERHOUR;
    if (*strp == ':') {
	++strp;
	strp = getnum (strp, &num, 0, MINSPERHOUR - 1);

	if (strp == NULL)
	    return NULL;
	*secsp += num * SECSPERMIN;
	if (*strp == ':') {
	    ++strp;
	    strp = getnum (strp, &num, 0, SECSPERMIN);
	    if (strp == NULL)
		return NULL;
	    *secsp += num;
	}
    }
    return strp;
}

const char *
CTimeZone::getoffset (const char *strp, long *const offsetp)
{
    int neg = 0;

    if (*strp == '-') {
	neg = 1;
	++strp;
    } else if (*strp == '+')
	++strp;

    strp = getsecs (strp, offsetp);
    if (strp == NULL)
	return NULL;		/* illegal time */

    if (neg)
	*offsetp = -*offsetp;

    return strp;
}

const char *
CTimeZone::getrule (const char *strp, struct rule *const rulep)
{
    if (*strp == 'J') {
	rulep->r_type = JULIAN_DAY;
	++strp;
	strp = getnum (strp, &rulep->r_day, 1, DAYSPERNYEAR);
    } else if (*strp == 'M') {
	rulep->r_type = MONTH_NTH_DAY_OF_WEEK;
	++strp;
	strp = getnum (strp, &rulep->r_mon, 1, MONSPERYEAR);

	if (strp == NULL)
	    return NULL;
	if (*strp++ != '.')
	    return NULL;

	strp = getnum (strp, &rulep->r_week, 1, 5);
	if (strp == NULL)
	    return NULL;
	if (*strp++ != '.')
	    return NULL;

	strp = getnum (strp, &rulep->r_day, 0, DAYSPERWEEK - 1);
    } else if (is_digit (*strp)) {
	rulep->r_type = DAY_OF_YEAR;
	strp = getnum (strp, &rulep->r_day, 0, DAYSPERLYEAR - 1);
    } else
	return NULL;		/* invalid format */

    if (strp == NULL)
	return NULL;
    if (*strp == '/') {
	++strp;
	strp = getsecs (strp, &rulep->r_time);
    } else
	rulep->r_time = 2 * SECSPERHOUR;	/* default = 2:00:00 */

    return strp;
}

time_t
CTimeZone::transtime (const time_t janfirst, const int year,
		      const struct rule * const rulep, const long offset)
{
    int leapyear;
    time_t value;
    int i;
    int d, m1, yy0, yy1, yy2, dow;

    value = 0;
    leapyear = isleap (year);

    switch (rulep->r_type) {
    case JULIAN_DAY:
	value = janfirst + (rulep->r_day - 1) * SECSPERDAY;
	if (leapyear && rulep->r_day >= 60)
	    value += SECSPERDAY;
	break;

    case DAY_OF_YEAR:
	value = janfirst + rulep->r_day * SECSPERDAY;
	break;

    case MONTH_NTH_DAY_OF_WEEK:
	value = janfirst;

	for (i = 0; i < rulep->r_mon - 1; ++i)
	    value += mon_lengths[leapyear][i] * SECSPERDAY;

	m1 = (rulep->r_mon + 9) % 12 + 1;
	yy0 = (rulep->r_mon <= 2) ? (year - 1) : year;
	yy1 = yy0 / 100;
	yy2 = yy0 % 100;
	dow =
	    ((26 * m1 - 2) / 10 + 1 + yy2 + yy2 / 4 + yy1 / 4 - 2 * yy1) % 7;
	if (dow < 0)
	    dow += DAYSPERWEEK;

	d = rulep->r_day - dow;
	if (d < 0)
	    d += DAYSPERWEEK;

	for (i = 1; i < rulep->r_week; ++i) {
	    if (d + DAYSPERWEEK >= mon_lengths[leapyear][rulep->r_mon - 1])
		break;

	    d += DAYSPERWEEK;
	}

	value += d * SECSPERDAY;
	break;
    }

    return value + rulep->r_time + offset;
}

int
CTimeZone::tzparse (const char *name, struct state *const sp,
		    const int lastditch)
{
    const char *stdname;
    const char *dstname;
    size_t stdlen;
    size_t dstlen;
    long stdoffset;
    long dstoffset;
    time_t *atp;
    unsigned char *typep;
    char *cp;
    int load_result;

    dstname = 0;
    stdname = name;

    if (lastditch) {
	stdlen = strlen (name);	/* length of standard zone name */
	name += stdlen;

	if (stdlen >= sizeof sp->chars)
	    stdlen = (sizeof sp->chars) - 1;
	stdoffset = 0;
    } else {
	if (*name == '<') {
	    name++;
	    stdname = name;
	    name = getqzname (name, '>');
	    if (*name != '>')
		return (-1);
	    stdlen = name - stdname;
	    name++;
	} else {
	    name = getzname (name);
	    stdlen = name - stdname;
	}

	if (*name == '\0')
	    return -1;

	name = getoffset (name, &stdoffset);
	if (name == NULL)
	    return -1;
    }

    load_result = tzload (TZDEFRULES, sp, FALSE);

    if (load_result != 0)
	sp->leapcnt = 0;	/* so, we're off a little */

    sp->timecnt = 0;
    if (*name != '\0') {
	if (*name == '<') {
	    dstname = ++name;
	    name = getqzname (name, '>');
	    if (*name != '>')
		return -1;
	    dstlen = name - dstname;
	    name++;
	} else {
	    dstname = name;
	    name = getzname (name);
	    dstlen = name - dstname;	/* length of DST zone name */
	}

	if (*name != '\0' && *name != ',' && *name != ';') {
	    name = getoffset (name, &dstoffset);
	    if (name == NULL)
		return -1;
	} else
	    dstoffset = stdoffset - SECSPERHOUR;

	if (*name == '\0' && load_result != 0)
	    name = TZDEFRULESTRING;

	if (*name == ',' || *name == ';') {
	    struct rule start;
	    struct rule end;
	    int year;
	    time_t janfirst;
	    time_t starttime;
	    time_t endtime;

	    ++name;
	    if ((name = getrule (name, &start)) == NULL)
		return -1;
	    if (*name++ != ',')
		return -1;
	    if ((name = getrule (name, &end)) == NULL)
		return -1;
	    if (*name != '\0')
		return -1;
	    sp->typecnt = 2;	/* standard time and DST */

	    sp->ttis[0].tt_gmtoff = -dstoffset;
	    sp->ttis[0].tt_isdst = 1;
	    sp->ttis[0].tt_abbrind = stdlen + 1;
	    sp->ttis[1].tt_gmtoff = -stdoffset;
	    sp->ttis[1].tt_isdst = 0;
	    sp->ttis[1].tt_abbrind = 0;
	    atp = sp->ats;
	    typep = sp->types;
	    janfirst = 0;

	    for (year = EPOCH_YEAR; sp->timecnt + 2 <= TZ_MAX_TIMES; ++year) {
		time_t newfirst;

		starttime = transtime (janfirst, year, &start, stdoffset);
		endtime = transtime (janfirst, year, &end, dstoffset);
		if (starttime > endtime) {
		    *atp++ = endtime;
		    *typep++ = 1;	/* DST ends */
		    *atp++ = starttime;
		    *typep++ = 0;	/* DST begins */
		} else {
		    *atp++ = starttime;
		    *typep++ = 0;	/* DST begins */
		    *atp++ = endtime;
		    *typep++ = 1;	/* DST ends */
		}

		sp->timecnt += 2;
		newfirst = janfirst;
		newfirst += year_lengths[isleap (year)] * SECSPERDAY;
		if (newfirst <= janfirst)
		    break;
		janfirst = newfirst;
	    }
	} else {
	    long theirstdoffset;
	    long theirdstoffset;
	    long theiroffset;
	    int isdst;
	    int i;
	    int j;

	    if (*name != '\0')
		return -1;

	    theirstdoffset = 0;
	    for (i = 0; i < sp->timecnt; ++i) {
		j = sp->types[i];
		if (!sp->ttis[j].tt_isdst) {
		    theirstdoffset = -sp->ttis[j].tt_gmtoff;
		    break;
		}
	    }

	    theirdstoffset = 0;
	    for (i = 0; i < sp->timecnt; ++i) {
		j = sp->types[i];
		if (sp->ttis[j].tt_isdst) {
		    theirdstoffset = -sp->ttis[j].tt_gmtoff;
		    break;
		}
	    }

	    isdst = FALSE;
	    theiroffset = theirstdoffset;

	    for (i = 0; i < sp->timecnt; ++i) {
		j = sp->types[i];
		sp->types[i] = sp->ttis[j].tt_isdst;
		if (sp->ttis[j].tt_ttisgmt) {
		    /* No adjustment to transition time */
		} else {
		    if (isdst && !sp->ttis[j].tt_ttisstd) {
			sp->ats[i] += dstoffset - theirdstoffset;
		    } else {
			sp->ats[i] += stdoffset - theirstdoffset;
		    }
		}

		theiroffset = -sp->ttis[j].tt_gmtoff;
		if (sp->ttis[j].tt_isdst)
		    theirdstoffset = theiroffset;
		else
		    theirstdoffset = theiroffset;
	    }

	    sp->ttis[0].tt_gmtoff = -stdoffset;
	    sp->ttis[0].tt_isdst = FALSE;
	    sp->ttis[0].tt_abbrind = 0;
	    sp->ttis[1].tt_gmtoff = -dstoffset;
	    sp->ttis[1].tt_isdst = TRUE;
	    sp->ttis[1].tt_abbrind = stdlen + 1;
	    sp->typecnt = 2;
	}
    } else {
	dstlen = 0;
	sp->typecnt = 1;	/* only standard time */
	sp->timecnt = 0;
	sp->ttis[0].tt_gmtoff = -stdoffset;
	sp->ttis[0].tt_isdst = 0;
	sp->ttis[0].tt_abbrind = 0;
    }

    sp->charcnt = stdlen + 1;
    if (dstlen != 0)
	sp->charcnt += dstlen + 1;

    if ((size_t) sp->charcnt > sizeof sp->chars)
	return -1;
    cp = sp->chars;

    (void) strncpy (cp, stdname, stdlen);
    cp += stdlen;
    *cp++ = '\0';

    if (dstlen != 0) {
	(void) strncpy (cp, dstname, dstlen);
	*(cp + dstlen) = '\0';
    }
    return 0;
}

int
CTimeZone::tzload (const char *name, struct state *const sp,
		   const int doextend)
{
    const char *p;
    int i;
    FILE *fid;
    int stored;
    int nread, size;

    union
    {
	struct tzhead tzhead;
	char buf[2 * sizeof (struct tzhead) + 2 * sizeof *sp +
		 4 * TZ_MAX_TIMES];
    }
    u;

    if (name == NULL && (name = TZDEFAULT) == NULL)
	return -1;

    {
	int doaccess;
	char fullname[FILENAME_MAX + 1];

	if (name[0] == ':')
	    ++name;
	doaccess = name[0] == '/';

	if (!doaccess) {
	    if ((p = program.TimeZones()) == NULL)
		return -1;

	    if ((strlen (p) + strlen (name) + 1) >= sizeof fullname)
		return -1;
	    (void) strcpy (fullname, p);
	    (void) strcat (fullname, "/");
	    (void) strcat (fullname, name);

	    if (strchr (name, '.') != NULL)
		doaccess = TRUE;
	    name = fullname;
	}

	if (doaccess && access (name, R_OK) != 0)
	    return -1;
	if ((fid = fopen (name, "rb")) == NULL)
	    return -1;

	if (fseek (fid, 0, SEEK_END) || (size = ftell (fid)) == EOF || fseek (fid, 0, SEEK_SET)) {
	    fclose (fid);
	    return -1;
	}
    }

    nread = fread (u.buf, 1, size, fid);
    if (fclose (fid) < 0 || nread <= 0)
	return -1;

    for (stored = 4; stored <= 8; stored *= 2) {
	int ttisstdcnt;
	int ttisgmtcnt;

	ttisstdcnt = (int) detzcode (u.tzhead.tzh_ttisstdcnt);
	ttisgmtcnt = (int) detzcode (u.tzhead.tzh_ttisgmtcnt);
	sp->leapcnt = (int) detzcode (u.tzhead.tzh_leapcnt);
	sp->timecnt = (int) detzcode (u.tzhead.tzh_timecnt);
	sp->typecnt = (int) detzcode (u.tzhead.tzh_typecnt);
	sp->charcnt = (int) detzcode (u.tzhead.tzh_charcnt);
	p = u.tzhead.tzh_charcnt + sizeof u.tzhead.tzh_charcnt;

	if (sp->leapcnt < 0 || sp->leapcnt > TZ_MAX_LEAPS ||
	    sp->typecnt <= 0 || sp->typecnt > TZ_MAX_TYPES ||
	    sp->timecnt < 0 || sp->timecnt > TZ_MAX_TIMES ||
	    sp->charcnt < 0 || sp->charcnt > TZ_MAX_CHARS ||
	    (ttisstdcnt != sp->typecnt && ttisstdcnt != 0) ||
	    (ttisgmtcnt != sp->typecnt && ttisgmtcnt != 0))
	    return -1;

	if (nread - (p - u.buf) < sp->timecnt * stored +	/* ats */
	    sp->timecnt +	/* types */
	    sp->typecnt * 6 +	/* ttinfos */
	    sp->charcnt +	/* chars */
	    sp->leapcnt * (stored + 4) +	/* lsinfos */
	    ttisstdcnt +	/* ttisstds */
	    ttisgmtcnt)		/* ttisgmts */
	    return -1;

	for (i = 0; i < sp->timecnt; ++i) {
	    sp->ats[i] = (stored == 4) ? detzcode (p) : detzcode64 (p);
	    p += stored;
	}

	for (i = 0; i < sp->timecnt; ++i) {
	    sp->types[i] = (unsigned char) *p++;
	    if (sp->types[i] >= sp->typecnt)
		return -1;
	}

	for (i = 0; i < sp->typecnt; ++i) {
	    struct ttinfo *ttisp;

	    ttisp = &sp->ttis[i];
	    ttisp->tt_gmtoff = detzcode (p);
	    p += 4;
	    ttisp->tt_isdst = (unsigned char) *p++;

	    if (ttisp->tt_isdst != 0 && ttisp->tt_isdst != 1)
		return -1;

	    ttisp->tt_abbrind = (unsigned char) *p++;
	    if (ttisp->tt_abbrind < 0 || ttisp->tt_abbrind > sp->charcnt)
		return -1;
	}

	for (i = 0; i < sp->charcnt; ++i)
	    sp->chars[i] = *p++;

	sp->chars[i] = '\0';	/* ensure '\0' at end */

	for (i = 0; i < sp->leapcnt; ++i) {
	    struct lsinfo *lsisp;

	    lsisp = &sp->lsis[i];
	    lsisp->ls_trans = (stored == 4) ? detzcode (p) : detzcode64 (p);
	    p += stored;
	    lsisp->ls_corr = detzcode (p);
	    p += 4;
	}

	for (i = 0; i < sp->typecnt; ++i) {
	    struct ttinfo *ttisp;

	    ttisp = &sp->ttis[i];
	    if (ttisstdcnt == 0)
		ttisp->tt_ttisstd = FALSE;
	    else {
		ttisp->tt_ttisstd = *p++;
		if (ttisp->tt_ttisstd != TRUE && ttisp->tt_ttisstd != FALSE)
		    return -1;
	    }
	}

	for (i = 0; i < sp->typecnt; ++i) {
	    struct ttinfo *ttisp;

	    ttisp = &sp->ttis[i];
	    if (ttisgmtcnt == 0)
		ttisp->tt_ttisgmt = FALSE;
	    else {
		ttisp->tt_ttisgmt = *p++;
		if (ttisp->tt_ttisgmt != TRUE && ttisp->tt_ttisgmt != FALSE)
		    return -1;
	    }
	}

	for (i = 0; i < sp->timecnt - 2; ++i)
	    if (sp->ats[i] > sp->ats[i + 1]) {
		++i;
		if (TYPE_SIGNED (time_t)) {
		    sp->timecnt = i;
		} else {
		    int j;

		    for (j = 0; j + i < sp->timecnt; ++j) {
			sp->ats[j] = sp->ats[j + i];
			sp->types[j] = sp->types[j + i];
		    }
		    sp->timecnt = j;
		}
		break;
	    }

	if (u.tzhead.tzh_version[0] == '\0')
	    break;
	nread -= p - u.buf;

	for (i = 0; i < nread; ++i)
	    u.buf[i] = p[i];
	if (stored >= (int) sizeof (time_t) && TYPE_INTEGRAL (time_t))
	    break;
    }

    if (doextend && nread > 2 &&
	u.buf[0] == '\n' && u.buf[nread - 1] == '\n' &&
	sp->typecnt + 2 <= TZ_MAX_TYPES) {
	struct state ts;
	int result;

	u.buf[nread - 1] = '\0';
	result = tzparse (&u.buf[1], &ts, FALSE);

	if (result == 0 && ts.typecnt == 2 &&
	    sp->charcnt + ts.charcnt <= TZ_MAX_CHARS) {
	    for (i = 0; i < 2; ++i)
		ts.ttis[i].tt_abbrind += sp->charcnt;

	    for (i = 0; i < ts.charcnt; ++i)
		sp->chars[sp->charcnt++] = ts.chars[i];

	    i = 0;
	    while (i < ts.timecnt && ts.ats[i] <= sp->ats[sp->timecnt - 1])
		++i;

	    while (i < ts.timecnt && sp->timecnt < TZ_MAX_TIMES) {
		sp->ats[sp->timecnt] = ts.ats[i];
		sp->types[sp->timecnt] = sp->typecnt + ts.types[i];
		++sp->timecnt;
		++i;
	    }

	    sp->ttis[sp->typecnt++] = ts.ttis[0];
	    sp->ttis[sp->typecnt++] = ts.ttis[1];
	}
    }

    i = 2 * YEARSPERREPEAT;
    sp->goback = sp->goahead = sp->timecnt > i;
    sp->goback &= sp->types[i] == sp->types[0] &&
	differ_by_repeat (sp->ats[i], sp->ats[0]);

    sp->goahead &=
	sp->types[sp->timecnt - 1] == sp->types[sp->timecnt - 1 - i] &&
	differ_by_repeat (sp->ats[sp->timecnt - 1],
			  sp->ats[sp->timecnt - 1 - i]);

    return 0;
}

int
CTimeZone::increment_overflow (int *number, int delta)
{
    int number0;

    number0 = *number;
    *number += delta;
    return (*number < number0) != (delta < 0);
}

int
CTimeZone::leaps_thru_end_of (int y)
{
    return (y >= 0) ? (y / 4 - y / 100 + y / 400) :
	-(leaps_thru_end_of (-(y + 1)) + 1);
}

struct tmg *
CTimeZone::timesub (const time_t * const timep,
		    const long offset,
		    const struct state *const sp, struct tmg *const tmp)
{
    const struct lsinfo *lp;
    time_t tdays;
    int idays;			/* unsigned would be so 2003 */
    long rem;
    int y, hit, i;
    const int *ip;
    long corr;

    corr = 0;
    hit = 0;
    i = sp->leapcnt;

    while (--i >= 0) {
	lp = &sp->lsis[i];
	if (*timep >= lp->ls_trans) {
	    if (*timep == lp->ls_trans) {
		hit = ((i == 0 && lp->ls_corr > 0) ||
		       lp->ls_corr > sp->lsis[i - 1].ls_corr);

		if (hit)
		    while (i > 0 &&
			   sp->lsis[i].ls_trans ==
			   sp->lsis[i - 1].ls_trans + 1 &&
			   sp->lsis[i].ls_corr == sp->lsis[i - 1].ls_corr + 1)
		    {
			++hit;
			--i;
		    }
	    }
	    corr = lp->ls_corr;
	    break;
	}
    }

    y = EPOCH_YEAR;
    tdays = *timep / SECSPERDAY;
    rem = *timep - tdays * SECSPERDAY;
    while (tdays < 0 || tdays >= year_lengths[isleap (y)]) {
	int newy;
	time_t tdelta;
	int idelta;
	int leapdays;

	tdelta = tdays / DAYSPERLYEAR;
	idelta = tdelta;

	if (tdelta - idelta >= 1 || idelta - tdelta >= 1)
	    return NULL;
	if (idelta == 0)
	    idelta = (tdays < 0) ? -1 : 1;

	newy = y;

	if (increment_overflow (&newy, idelta))
	    return NULL;

	leapdays = leaps_thru_end_of (newy - 1) - leaps_thru_end_of (y - 1);
	tdays -= ((time_t) newy - y) * DAYSPERNYEAR;
	tdays -= leapdays;
	y = newy;
    }

    {
	long seconds;

	seconds = tdays * SECSPERDAY + 0.5;
	tdays = seconds / SECSPERDAY;
	rem += seconds - tdays * SECSPERDAY;
    }

    idays = tdays;
    rem += offset - corr;

    while (rem < 0) {
	rem += SECSPERDAY;
	--idays;
    }

    while (rem >= SECSPERDAY) {
	rem -= SECSPERDAY;
	++idays;
    }

    while (idays < 0) {
	if (increment_overflow (&y, -1))
	    return NULL;
	idays += year_lengths[isleap (y)];
    }

    while (idays >= year_lengths[isleap (y)]) {
	idays -= year_lengths[isleap (y)];
	if (increment_overflow (&y, 1))
	    return NULL;
    }

    tmp->tm_year = y;

    if (increment_overflow (&tmp->tm_year, -TM_YEAR_BASE))
	return NULL;
    tmp->tm_yday = idays;

    tmp->tm_wday = EPOCH_WDAY +
	((y - EPOCH_YEAR) % DAYSPERWEEK) *
	(DAYSPERNYEAR % DAYSPERWEEK) +
	leaps_thru_end_of (y - 1) - leaps_thru_end_of (EPOCH_YEAR - 1) +
	idays;

    tmp->tm_wday %= DAYSPERWEEK;

    if (tmp->tm_wday < 0)
	tmp->tm_wday += DAYSPERWEEK;
    tmp->tm_hour = (int) (rem / SECSPERHOUR);
    rem %= SECSPERHOUR;
    tmp->tm_min = (int) (rem / SECSPERMIN);

    tmp->tm_sec = (int) (rem % SECSPERMIN) + hit;
    ip = mon_lengths[isleap (y)];

    for (tmp->tm_mon = 0; idays >= ip[tmp->tm_mon]; ++(tmp->tm_mon))
	idays -= ip[tmp->tm_mon];

    tmp->tm_mday = (int) (idays + 1);
    tmp->tm_isdst = 0;

    return tmp;
}

struct tmg *
CTimeZone::localsub (const time_t * const timep, const long offset,
		     struct tmg *tmp)
{
    struct state *sp = &lcl;
    const struct ttinfo *ttisp;
    int i;
    struct tmg *result;
    const time_t t = *timep;

    if ((sp->goback && t < sp->ats[0]) ||
	(sp->goahead && t > sp->ats[sp->timecnt - 1])) {
	time_t newt = t;
	time_t seconds;
	time_t tcycles;
	int_fast64_t icycles;

	if (t < sp->ats[0])
	    seconds = sp->ats[0] - t;
	else
	    seconds = t - sp->ats[sp->timecnt - 1];

	--seconds;
	tcycles = seconds / YEARSPERREPEAT / AVGSECSPERYEAR;
	++tcycles;
	icycles = tcycles;

	if (tcycles - icycles >= 1 || icycles - tcycles >= 1)
	    return NULL;

	seconds = icycles;
	seconds *= YEARSPERREPEAT;
	seconds *= AVGSECSPERYEAR;

	if (t < sp->ats[0])
	    newt += seconds;
	else
	    newt -= seconds;

	if (newt < sp->ats[0] || newt > sp->ats[sp->timecnt - 1])
	    return NULL;

	result = localsub (&newt, offset, tmp);

	if (result == tmp) {
	    time_t newy;

	    newy = tmp->tm_year;
	    if (t < sp->ats[0])
		newy -= icycles * YEARSPERREPEAT;
	    else
		newy += icycles * YEARSPERREPEAT;

	    tmp->tm_year = newy;
	    if (tmp->tm_year != newy)
		return NULL;
	}
	return result;
    }

    if (sp->timecnt == 0 || t < sp->ats[0]) {
	i = 0;
	while (sp->ttis[i].tt_isdst)
	    if (++i >= sp->typecnt) {
		i = 0;
		break;
	    }
    } else {
	int lo = 1;
	int hi = sp->timecnt;

	while (lo < hi) {
	    int mid = (lo + hi) >> 1;

	    if (t < sp->ats[mid])
		hi = mid;
	    else
		lo = mid + 1;
	}
	i = (int) sp->types[lo - 1];
    }

    ttisp = &sp->ttis[i];

    result = timesub (&t, ttisp->tt_gmtoff, sp, tmp);
    tmp->tm_isdst = ttisp->tt_isdst;
    tmp->tm_gmtoff = ttisp->tt_gmtoff;

    return result;
}

CTimeZone::CTimeZone ()
{
    iName = NULL;
}

CTimeZone::~CTimeZone ()
{
    if (iName)
	free (iName);
}

void
CTimeZone::Name (char *name)
{
    if (iName)
	free (iName);
    iName = strdup (name);

    tzload (name, &lcl, TRUE);
}

char *
CTimeZone::Name ()
{
    return iName;
}

long
CTimeZone::Offset (time_t time, int *dst)
{
    struct tmg tmp, *tm;

    tm = localsub (&time, 0L, &tmp);
    if (dst)
	*dst = tm->tm_isdst;

    return tm->tm_gmtoff;
}
