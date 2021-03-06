/** Copyright (C) 2008 Josh Ventura
***
*** This file is a part of the ENIGMA Development Environment.
***
*** ENIGMA is free software: you can redistribute it and/or modify it under the
*** terms of the GNU General Public License as published by the Free Software
*** Foundation, version 3 of the license or any later version.
***
*** This application and its source code is distributed AS-IS, WITHOUT ANY
*** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*** FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
*** details.
***
*** You should have received a copy of the GNU General Public License along
*** with this code. If not, see <http://www.gnu.org/licenses/>
**/

/**
  @file  graphics_object.h
  @brief Adds a planar tier following the main tier.
*/

#ifndef _planar_object_h
#define _planar_object_h

#include "object.h"
#include "var4.h"
#include "scalar.h"
#include "reflexive_types.h"

#define ISLOCAL_persistent true

namespace enigma
{
  struct object_planar: object_basic
  {
    //Position
      cs_scalar x, y;
      cs_scalar  xprevious, yprevious;
      cs_scalar  xstart, ystart;

    //Persistence
    #ifdef ISLOCAL_persistent
      bool  persistent;
    #endif

    //Motion
      directionv direction;
      speedv     speed;
      hspeedv    hspeed;
      vspeedv    vspeed;

    //Accelerators
      cs_scalar  gravity;
      cs_scalar  gravity_direction;
      cs_scalar  friction;

    //Constructors
      object_planar();
      object_planar(unsigned, int);
      virtual ~object_planar();
  };

  void propagate_locals(object_planar*);
}

#endif //_planar_object_h
