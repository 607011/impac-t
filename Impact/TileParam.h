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

#ifndef __TILEPARAM_H_
#define __TILEPARAM_H_

#include <Box2D/Box2D.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <string>

#include "util.h"


namespace Impact {

  struct TileParam {
    TileParam(void)
      : score(0)
      , smooth(true)
      , gravityScale(1.f)
      , scaleBallDensityBy(1.f)
      , scaleGravityBy(1.f)
      , minimumHitImpulse(5)
      , minimumKillImpulse(50)
      , earthquakeIntensity(.1f)
      //MOD Keyhole2
    { /* ... */ }
    TileParam(const TileParam &other)
      : score(other.score)
      , fixed(other.fixed)
      , friction(other.friction)
      , restitution(other.restitution)
      , density(other.density)
      , gravityScale(other.gravityScale)
      , smooth(other.smooth)
      , minimumHitImpulse(other.minimumHitImpulse)
      , minimumKillImpulse(other.minimumKillImpulse)
      , scaleGravityDuration(other.scaleGravityDuration)
      , scaleGravityBy(other.scaleGravityBy)
      , scaleBallDensityDuration(other.scaleBallDensityDuration)
      , scaleBallDensityBy(other.scaleBallDensityBy)
      , earthquakeDuration(other.earthquakeDuration)
      , earthquakeIntensity(other.earthquakeIntensity)
    { /* ... */ }
    int score;
    std::string textureName;
    sf::Texture texture;
    DynamicValue<bool> fixed;
    DynamicValue<float32> friction;
    DynamicValue<float32> restitution;
    DynamicValue<float32> density;
    float32 gravityScale;
    bool smooth;
    int minimumHitImpulse;
    int minimumKillImpulse;
    sf::Time scaleGravityDuration;
    float32 scaleGravityBy;
    sf::Time scaleBallDensityDuration;
    float32 scaleBallDensityBy;
    sf::Time earthquakeDuration;
    float32 earthquakeIntensity;
    //MOD Keyhole1
  };


}

#endif // __TILEPARAM_H_
