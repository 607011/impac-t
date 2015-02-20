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


#ifdef WIN32
#pragma once
#endif

#ifndef __STDAFX_H_
#define __STDAFX_H_

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 4503)
#include <boost/signals2.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
// #include <boost/thread/mutex.hpp>
#pragma warning(pop)

#include <limits>
#include <algorithm>
#include <numeric>
#include <memory>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <typeinfo>
#include <future>
#include <functional>
#include <sys/stat.h>

#include <GL/glew.h>
#include <GL/GLU.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>

#include <Box2D/Box2D.h>

#include <zlib.h>

#include "util.h"
#include "globals.h"
#include "Easings.h"
#include "LevelTimer.h"
#include "TileParam.h"
#include "Level.h"
#include "Destructible.h"
#include "Body.h"
#include "BodyText.h"
#include "BodyBlock.h"
#include "BodyBall.h"
#include "BodyRacket.h"
#include "BodyGround.h"
#include "BodyWall.h"
#include "Explosion.h"
#include "Impact.h"


#endif // __STDAFX_H_
