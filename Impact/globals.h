/*  

    Copyright (c) 2015 Oliver Lau <ola@ct.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#include <string>
#include <boost/random/mersenne_twister.hpp>
#include "Settings.h"

namespace Impact {

#define IMPACT_VERSION "1.0.0-BETA14"

#define ResourcesDir std::string("resources")
#define LevelsDir ResourcesDir + "/levels"
#define SoundFXDir ResourcesDir + "/soundfx"
#define ImagesDir ResourcesDir + "/images"
#define BackgroundsDir ResourcesDir + "/backgrounds"
#define FontsDir ResourcesDir + "/fonts"
#define ShadersDir ResourcesDir + "/shaders"

  extern boost::random::mt19937 gRNG;

  extern Settings gSettings;
}


#endif // __GLOBALS_H_
