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


#ifndef __BALLTRACE_H_
#define __BALLTRACE_H_

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Body.h"
#include "Impact.h"

namespace Impact {
  
  struct SimpleBallTrace 
  {
    sf::Clock age;
    sf::Sprite sprite;
  };


  class BallTrace : public Body
  {
  public:
    BallTrace(Game *game);

    
    void addMarker(float x, float y, float32 angle);
    void addMarker(const b2Vec2 &pos, float32 angle);

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Ball; }

  private:
    static const sf::Time sMaxAge;
    static const sf::Color sColor;
    std::vector<SimpleBallTrace> mTraces;
    int mCurrentTraceIndex;

  };

}

#endif // __BALLTRACE_H_
