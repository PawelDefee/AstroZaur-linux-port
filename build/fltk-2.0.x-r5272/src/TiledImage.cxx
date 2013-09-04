//
// "$Id: TiledImage.cxx 5073 2006-05-03 03:47:38Z spitzak $"
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

/*! \class fltk::TiledImage

  Draws the image supplied to it's constructor repeatedly to cover
  the area provided to draw(). Clips the images as necessary to
  fit them in the rectangle.
*/

#include <fltk/TiledImage.h>
#include <fltk/SharedImage.h>
#include <fltk/draw.h>

using namespace fltk;

/*! \fn TiledImage::TiledImage(Symbol*)

  A pointer to the image must be provided to the constructor. If it
  is null then the TiledImage draws nothing.
*/

TiledImage::TiledImage(const char * name): Symbol(0) {
    if (name) image(SharedImage::get(name));
}

/*! Returns w and h unchanged, indicating that it can draw any size
  of rectangle, with no preference. */
void TiledImage::_measure(int& w, int& h) const {
  // Do nothing to indicate any rectangle is allowed.
  // I think what I intended with this old code was to return a useful
  // size for some (nyi) automatic resize of widgets. But it won't draw
  // right unless ALIGN_CLIP was on. Also I suspect this is being used
  // for backgrounds and probably should have no effect on resize anyway.
#if 0
  if (!image_) return;
  int iw = w;
  int ih = h;
  image_->measure(iw,ih);
  if (iw > w) w = iw;
  if (ih > h) h = ih;
#endif
}

/*! Repeatedly draws the image to fill the area, putting the top-left
  corner at \a x,y. This checks the current clip region and does
  minimal drawing of only the visible portions of the image.
*/
void TiledImage::_draw(const Rectangle& r) const
{
  if (!image_) return;
  int iw = r.w();
  int ih = r.h();
  image_->measure(iw,ih); if (iw <= 0 || ih <= 0) return;
  int cx = 0;
  int cy = 0;
  // Perhaps this should use the Align flags to set cx, cy.

  // Figure out the smallest rectangle enclosing this and the clip region:
  Rectangle cr(r); intersect_with_clip(cr);
  if (cr.empty()) return;
  cx += cr.x()-r.x(); cy += cr.y()-r.y();
  push_clip(cr);

  int temp = -cx % iw;
  cx = (temp>0 ? iw : 0) - temp;
  temp = -cy % ih;
  cy = (temp>0 ? ih : 0) - temp;

  int ccx=cx;
  Rectangle ir(iw,ih);

  while (-cy < cr.h()) {
    ir.y(cr.x()-cy);
    while (-cx < cr.w()) {
      ir.x(cr.x()-cx);
      image_->draw(ir);
      cx -= iw;
    }
    cy -= ih;
    cx = ccx;
  }
  pop_clip();
}

void TiledImage::inset(Rectangle& r) const {
  if (image_) image_->inset(r);
}

//
// End of "$Id: TiledImage.cxx 5073 2006-05-03 03:47:38Z spitzak $".
//
