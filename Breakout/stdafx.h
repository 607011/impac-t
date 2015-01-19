// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

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
#include <boost/foreach.hpp>
#pragma warning(pop)

#include <limits>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include <map>
#include <random>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cassert>
#include <sstream>

#include <glew.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>

#include <Box2D/Box2D.h>

#include <zlib.h>


#include "main.h"
#include "util.h"
#include "globals.h"
#include "Level.h"
#include "Destructible.h"
#include "Body.h"
#include "BodyBlock.h"
#include "BodyBall.h"
#include "BodyPad.h"
#include "BodyGround.h"
#include "Breakout.h"


#endif // __STDAFX_H_
