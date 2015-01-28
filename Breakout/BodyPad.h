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


#ifndef __BODYPAD_H_
#define __BODYPAD_H_

#include <Box2D/Box2D.h>
#include "Body.h"
#include "Breakout.h"

namespace Breakout {

  class Pad : public Body
  {
  public:
    Pad(Game *game);
#ifndef NDEBUG
    ~Pad() {
      std::cout << "~dtor of " << typeid(this).name() << std::endl;
    }
#endif

    void moveLeft(void);
    void moveRight(void);
    void stopMotion(void);
    void kickLeft(void);
    void kickRight(void);
    void stopKick(void);

    virtual void setPosition(float x, float y);
    virtual const b2Vec2 &position(void) const;

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Pad; }

  private:
    b2RevoluteJoint* mJoint;
    b2Body *mTeetingBody;
  };

}

#endif // __PAD_H_
