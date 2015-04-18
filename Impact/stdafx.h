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

#ifdef LINUX_AMD64
#include "linux_amd64.h"
#endif

#ifndef __STDAFX_H_
#define __STDAFX_H_

#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/signals2.hpp>
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
#include <thread>
#include <future>
#include <chrono>
#include <sys/stat.h>

#include <GL/glew.h>
#include <GL/glu.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>

#include <Box2D/Box2D.h>

#include "util.h"
#include "LocalSettings.h"
#include "globals.h"
#include "Easings.h"
#include "Timer.h"
#include "TileParam.h"
#include "Level.h"
#include "Destructible.h"
#include "Body.h"
#include "Text.h"
#include "Block.h"
#include "Bumper.h"
#include "Ball.h"
#include "Racket.h"
#include "Ground.h"
#include "Wall.h"
#include "Explosion.h"
#include "Impact.h"


#endif // __STDAFX_H_
