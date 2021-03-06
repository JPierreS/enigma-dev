// 
// Copyright (C) 2014 Seth N. Hetu
// 
// This file is a part of the ENIGMA Development Environment.
// 
// ENIGMA is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, version 3 of the license or any later version.
// 
// This application and its source code is distributed AS-IS, WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along
// with this code. If not, see <http://www.gnu.org/licenses/>
//

#include "GM5Drawing.h"

#include <cmath>
#include <list>
#include <algorithm>
#include <cstdio>

#include "Graphics_Systems/General/GSstdraw.h"
#include "Graphics_Systems/General/GScolors.h"
#include "Graphics_Systems/General/GSsprite.h"
#include "Graphics_Systems/General/GSprimitives.h"

namespace {


template <typename T>
void swap(T& val1, T& val2){
  T temp = val1;
  val1 = val2;
  val2 = temp;
}
}


namespace enigma_user
{

//GM5 draw state.
gs_scalar pen_size = 1;
int brush_style = enigma_user::bs_solid; 
int pen_color = enigma_user::c_black;
int brush_color = enigma_user::c_black;


void draw_rectangle(gs_scalar x1, gs_scalar y1, gs_scalar x2, gs_scalar y2)
{
  //lwid/hwid are the width and half-width, respectively. shft is used to nudge odd-width penned shapes to the right.
  int lwid = std::max(1,(int)round(pen_size));
  int hwid = lwid/2;
  int shft = (lwid%2==0)?0:1;

  //Ensure (x1,y1) are the smallest components, and (x2,y2) are the largest.
  if (x2<x1) { swap(x1,x2); }
  if (y2<y1) { swap(y1,y2); }

  //Nudge the shape into position.
  x1 = int(x1) + shft;
  x2 = int(x2) + shft - 1 ;
  y1 = int(y1);
  y2 = int(y2) - 1 ;

  //Fill the shape, if we have the correct brush style.
  //Note: At the moment, we treat all unsupported brush styles as "solid"; only "hollow" avoids drawing.
  if (brush_style != bs_hollow) {
    draw_set_color(brush_color);
    draw_rectangle(x1,y1,x2,y2,false);
  }

  //Draw the line (this always happens; a pen_size of 0 still draws a 1px line).
  //Note: So many off-by-one errors worries me; will have to confirm on Windows that this
  //      is not a bug in the OpenGL code.
  draw_set_color(pen_color);
  draw_line_width(x1,           y1-hwid, x1,      y2+hwid, lwid);
  draw_line_width(x1-hwid-shft, y2,      x2+hwid, y2,      lwid);
  draw_line_width(x2,           y2+hwid, x2,      y1-hwid, lwid);
  draw_line_width(x2+hwid,      y1,      x1-hwid, y1,      lwid);
}


void draw_triangle(gs_scalar x1, gs_scalar y1, gs_scalar x2, gs_scalar y2, gs_scalar x3, gs_scalar y3)
{
  const int lwid = std::max(1,(int)round(pen_size));

  //Fill the shape, if we have the correct brush style.
  //Note: At the moment, we treat all unsupported brush styles as "solid"; only "hollow" avoids drawing.
  if (brush_style != bs_hollow) {
    draw_set_color(brush_color);
    draw_triangle(x1,y1,x2,y2,x3,y3,false);
  }

  //Draw the outline.
  draw_set_color(pen_color);
  draw_line_width(x1,y1, x2,y2, lwid);
  draw_line_width(x2,y2, x3,y3, lwid);
  draw_line_width(x3,y3, x1,y1, lwid);

  //We fake triangle joints with circles.
  //TODO: Triangle coords are still off slightly, not just on the joints.
  draw_circle(x1,y1,lwid/2.0, false);
  draw_circle(x2,y2,lwid/2.0, false);
  draw_circle(x3,y3,lwid/2.0, false);
}


void draw_ellipse(gs_scalar x1, gs_scalar y1, gs_scalar x2, gs_scalar y2)
{
  const int lwid = std::max(1,(int)round(pen_size));

  //Ensure (x1,y1) are the smallest components, and (x2,y2) are the largest.
  if (x2<x1) { swap(x1,x2); }
  if (y2<y1) { swap(y1,y2); }

  //Nudge the shape into position.
  gs_scalar shft = (lwid%2 == 0) ? 1 : 0.5;
  x1 += shft;
  x2 -= shft;
  y1 += shft;
  y2 -= shft;

  //Fill the shape, if we have the correct brush style.
  //Note: At the moment, we treat all unsupported brush styles as "solid"; only "hollow" avoids drawing.
  if (brush_style != bs_hollow) {
    draw_set_color(brush_color);
    draw_ellipse(x1,y1,x2,y2,false);
  }

  //This is borrowed from General drawing code.
  draw_set_color(pen_color);
  gs_scalar x = (x1+x2)/2,y=(y1+y2)/2;
  gs_scalar hr = fabs(x2-x),vr=fabs(y2-y);
  gs_scalar pr = 2*M_PI/draw_get_circle_precision();
  for(gs_scalar i=pr;i<M_PI;i+=pr) {
    gs_scalar xc1 = cos(i)*hr;
    gs_scalar yc1 = sin(i)*vr;
    i += pr;
    gs_scalar xc2=cos(i)*hr;
    gs_scalar yc2=sin(i)*vr;
    draw_line_width(x+xc1,y+yc1  ,  x+xc2,y+yc2  , lwid);
    draw_line_width(x-xc1,y+yc1  ,  x-xc2,y+yc2  , lwid);
    draw_line_width(x+xc1,y-yc1  ,  x+xc2,y-yc2  , lwid);
    draw_line_width(x-xc1,y-yc1  ,  x-xc2,y-yc2  , lwid);
  }

  //These are needed to prevent the top half-width of the line from overlapping for wide lines.
  //This is clearly massively inefficient; currently it rotates the entire shape through each half-width
  //  in an attempt to cover blank spots. 
  for(gs_scalar i=pr;i<M_PI;i+=pr) {
    gs_scalar xc1 = cos(i+pr/2)*hr;
    gs_scalar yc1 = sin(i+pr/2)*vr;
    i += pr;
    gs_scalar xc2=cos(i+pr/2)*hr;
    gs_scalar yc2=sin(i+pr/2)*vr;
    draw_line_width(x+xc1,y+yc1  ,  x+xc2,y+yc2  , lwid);
    draw_line_width(x-xc1,y+yc1  ,  x-xc2,y+yc2  , lwid);
    draw_line_width(x+xc1,y-yc1  ,  x+xc2,y-yc2  , lwid);
    draw_line_width(x-xc1,y-yc1  ,  x-xc2,y-yc2  , lwid);
  }
  for(gs_scalar i=pr;i<M_PI;i+=pr) {
    gs_scalar xc1 = cos(i-pr/2)*hr;
    gs_scalar yc1 = sin(i-pr/2)*vr;
    i += pr;
    gs_scalar xc2=cos(i-pr/2)*hr;
    gs_scalar yc2=sin(i-pr/2)*vr;
    draw_line_width(x+xc1,y+yc1  ,  x+xc2,y+yc2  , lwid);
    draw_line_width(x-xc1,y+yc1  ,  x-xc2,y+yc2  , lwid);
    draw_line_width(x+xc1,y-yc1  ,  x+xc2,y-yc2  , lwid);
    draw_line_width(x-xc1,y-yc1  ,  x-xc2,y-yc2  , lwid);
  }
}

void draw_circle(gs_scalar x, gs_scalar y, float radius)
{
  const int lwid = std::max(1,(int)round(pen_size));

  //Shuffle the radius a bit.
  if (lwid%2 == 0) {
    radius -= 1;
  }

  //Fill the shape, if we have the correct brush style.
  //Note: At the moment, we treat all unsupported brush styles as "solid"; only "hollow" avoids drawing.
  if (brush_style != bs_hollow) {
    draw_set_color(brush_color);
    draw_circle(x,y,radius,false);
  }

  //This is borrowed from General drawing code.
  draw_set_color(pen_color);
  double pr = 2 * M_PI / draw_get_circle_precision();
  double oldX = 0.0;
  double oldY = 0.0;
  for (double i = 0; i <= 2*M_PI; i += pr) {
    double xc1=cos(i)*radius;
    double yc1=sin(i)*radius;
    double newX = x+xc1;
    double newY = y+yc1;
    if (i>0) {
      draw_line_width(oldX,oldY, newX, newY, lwid);
    }
    oldX = newX;
    oldY = newY;
  }

  //Similar to the ellipse drawing code, this overlays patches to the circle's shape.
  //It is also inefficient, but necessary for large pen widths.
  oldX = oldY = 0.0;
  for (double i = 0; i <= 2*M_PI; i += pr) {
    double xc1=cos(i+pr/2)*radius;
    double yc1=sin(i+pr/2)*radius;
    double newX = x+xc1;
    double newY = y+yc1;
    if (i>0) {
      draw_line_width(oldX,oldY, newX, newY, lwid);
    }
    oldX = newX;
    oldY = newY;
  }
}


void draw_polygon_end()
{
  const int lwid = std::max(1,(int)round(pen_size));

  //We need a copy of currPoly since draw_polygon_end() will clear it.
  std::list<enigma::PolyVertex> cachedPoly = enigma::currComplexPoly;

  //Fill the shape, if we have the correct brush style.
  //Note: At the moment, we treat all unsupported brush styles as "solid"; only "hollow" avoids drawing.
  //TODO: Test if GM5 actually respects brush_style for polygons.
  if (brush_style != bs_hollow) {
    draw_set_color(brush_color);
    draw_polygon_end(false); //Dispatch to GSstdraw
  } else {
    enigma::currComplexPoly.clear(); //Just clear it; we're only drawing a line.
  }

  //Draw the line around it.
  draw_set_color(pen_color);
  enigma::PolyVertex lastPt = *(--cachedPoly.end());
  for (std::list<enigma::PolyVertex>::iterator it = cachedPoly.begin(); it!=cachedPoly.end(); it++) {
    enigma::PolyVertex currPt = *it;
    draw_line_width(lastPt.x,lastPt.y, currPt.x,currPt.y , lwid);
    lastPt = currPt;
  }
}



}

