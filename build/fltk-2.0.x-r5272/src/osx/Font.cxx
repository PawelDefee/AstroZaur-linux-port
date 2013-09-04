// "$Id: Font.cxx 4886 2006-03-30 09:55:32Z fabien $"
//
// Copyright 1998-2006 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".

// It appears that "ATSUI" is the correct interface to use to get
// Unicode. Unfortunatly the API is complex...

#include <fltk/x.h>
#include <fltk/Font.h>
#include <ctype.h>
#include <stdlib.h>
#include <fltk/utf.h>
#include <fltk/math.h>
#include <fltk/string.h>
#include <fltk/Window.h>

#include <Carbon/Carbon.h>

using namespace fltk;

// The public-visible fltk::Font structures are actually imbedded in
// this larger structure which points at the the above list
struct IFont {
  fltk::Font f;
  int attribute_mask; // all attributes that can be turned on
  const char* name;
  float ascent, descent;
};

// We store the attributes in neat blocks of 2^n:
fltk::Font* fltk::Font::plus(int x) {
  IFont* font = (IFont*)this;
  x &= font->attribute_mask & ~attributes_;
  return &((font+x)->f);
}

const char* fltk::Font::system_name() {
  IFont* ifont = (IFont*)this;
  if (!ifont->name) ifont->name = newstring(name());
  return ifont->name;
}

const char* fltk::xfont() { return current_font_->system_name(); }

const char* fltk::Font::current_name() { return current_font_->name_; }

////////////////////////////////////////////////////////////////

// The predefined fonts that fltk has:
static IFont fonts [] = {
  {{"Arial",	0},	3,	0},
  {{"Arial",	1},	3,	0},
  {{"Arial",	2},	3,	0},
  {{"Arial",	3},	3,	0},
  {{"Courier New",0},	3,	0},
  {{"Courier New",1},	3,	0},
  {{"Courier New",2},	3,	0},
  {{"Courier New",3},	3,	0},
  {{"Times New Roman",	0},	3,	0},
  {{"Times New Roman",	1},	3,	0},
  {{"Times New Roman",	2},	3,	0},
  {{"Times New Roman",	3},	3,	0},
  {{"Symbol",	0},	0,	0},
  {{"Monaco",	0},	1,	0},
  {{"Monaco",	1},	1,	0},
  {{"Webdings",	0},	0,	0}
};

fltk::Font* const fltk::HELVETICA 		= &(fonts[0].f);
fltk::Font* const fltk::HELVETICA_BOLD		= &(fonts[1].f);
fltk::Font* const fltk::HELVETICA_ITALIC	= &(fonts[2].f);
fltk::Font* const fltk::HELVETICA_BOLD_ITALIC	= &(fonts[3].f);
fltk::Font* const fltk::COURIER 		= &(fonts[4].f);
fltk::Font* const fltk::COURIER_BOLD		= &(fonts[5].f);
fltk::Font* const fltk::COURIER_ITALIC		= &(fonts[6].f);
fltk::Font* const fltk::COURIER_BOLD_ITALIC	= &(fonts[7].f);
fltk::Font* const fltk::TIMES 			= &(fonts[8].f);
fltk::Font* const fltk::TIMES_BOLD		= &(fonts[9].f);
fltk::Font* const fltk::TIMES_ITALIC		= &(fonts[10].f);
fltk::Font* const fltk::TIMES_BOLD_ITALIC	= &(fonts[11].f);
fltk::Font* const fltk::SYMBOL_FONT		= &(fonts[12].f);
fltk::Font* const fltk::SCREEN_FONT		= &(fonts[13].f);
fltk::Font* const fltk::SCREEN_BOLD_FONT	= &(fonts[14].f);
fltk::Font* const fltk::ZAPF_DINGBATS		= &(fonts[15].f);

// Turn an old integer into a predefined font:
fltk::Font* fltk::font(int i) {return &(fonts[i%16].f);}

////////////////////////////////////////////////////////////////

// For fltk::list_fonts(), make a new font, and optionally the bold and
// italic subfonts:
Font* fl_make_font(const char* name, int attrib) {
  // see if it is one of our built-in fonts and return it:
  int j; for (j = 0; j < 16; j++) {
    if (fonts[j].f.attributes_ == attrib &&
	!strcasecmp(fonts[j].f.name_, name)) return &(fonts[j].f);
  }
  // no, lets create some fonts:
  IFont* newfont = new IFont[4];
  newfont[0].f.name_ = newstring(name);
  for (j = 0; j < 4; j++) {
    newfont[j].f.name_ = newfont[0].f.name_;
    newfont[j].f.attributes_ = attrib|j;
    newfont[j].attribute_mask = 3;
    newfont[j].name = 0;
    newfont[j].ascent = 0;
  }
  return &(newfont[0].f);
}

////////////////////////////////////////////////////////////////
// Public interface:

void fltk::setfont(Font* font, float psize) {
  psize = rint(psize*10)/10.0f;

  IFont* ifont = (IFont*)font;

  if (!ifont->ascent) {
    // Using ATS to get the general Glyph size information
    ifont->ascent = .75;
    ifont->descent = .25;
    CFStringRef cfname = CFStringCreateWithCString(0L, font->system_name(), kCFStringEncodingASCII);
    ATSFontRef afont = ATSFontFindFromName(cfname, kATSOptionFlagsDefault);
    CFRelease(cfname);
    if (!afont) return; // we lose...
    ATSFontMetrics m = { 0 };
    ATSFontGetHorizontalMetrics(afont, kATSOptionFlagsDefault, &m);
    //if (m.avgAdvanceWidth) q_width = int(m.avgAdvanceWidth*size);
    if (m.ascent) {
      ifont->ascent = m.ascent+m.descent;
      ifont->descent = -m.descent;
    }
    // does the ATSFontRef need to be freed somehow?
  }

  current_font_ = font; current_size_ = psize;

  if (quartz_gc) {
    CGContextSelectFont(quartz_gc, ((IFont*)current_font_)->name,
			current_size_, kCGEncodingMacRoman);
  }
}

float fltk::getascent()  { return ((IFont*)current_font_)->ascent*current_size_; }
float fltk::getdescent() { return ((IFont*)current_font_)->descent*current_size_; }

#include "utf8tomac.cxx"

#define WCBUFLEN 256

float fltk::getwidth(const char* text, int n) {
  if (!quartz_gc) {
    Window *w = Window::first();
    if (w) w->make_current();
    if (!quartz_gc || !current_font_) return -1;
    CGContextSelectFont(quartz_gc, ((IFont*)current_font_)->name,
			current_size_, kCGEncodingMacRoman);
  }
  char localbuffer[WCBUFLEN];
  char* buffer = localbuffer;
  char* mallocbuffer = 0;
  int count = utf8tomac(text, n, buffer, WCBUFLEN);
  if (count >= WCBUFLEN) {
    buffer = mallocbuffer = new char[count+1];
    count = utf8toa(text, n, buffer, count+1);
  }
  CGContextSetTextDrawingMode(quartz_gc, kCGTextInvisible);
  CGContextShowTextAtPoint(quartz_gc, 0, 0, buffer, count);
  CGContextSetTextDrawingMode(quartz_gc, kCGTextFill);
  CGPoint p = CGContextGetTextPosition(quartz_gc);
  delete[] mallocbuffer;
  return p.x;
}

void fltk::drawtext_transformed(const char *text, int n, float x, float y) {
  char localbuffer[WCBUFLEN];
  char* buffer = localbuffer;
  char* mallocbuffer = 0;
  int count = utf8tomac(text, n, buffer, WCBUFLEN);
  if (count >= WCBUFLEN) {
    buffer = mallocbuffer = new char[count+1];
    count = utf8toa(text, n, buffer, count+1);
  }
  CGContextShowTextAtPoint(quartz_gc, x, y, buffer, count);
  delete[] mallocbuffer;
}

//
// End of "$Id: Font.cxx 4886 2006-03-30 09:55:32Z fabien $".
//
