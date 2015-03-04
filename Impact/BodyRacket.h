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


#ifndef __BODYRACKET_H_
#define __BODYRACKET_H_

#include <Box2D/Box2D.h>
#include "Body.h"
#include "Impact.h"

#include <string>

namespace Impact {

  class Racket : public Body
  {
  public:
    Racket(Game *game, const b2Vec2 &pos, b2Body *ground);

    void kickLeft(void);
    void kickRight(void);
    void stopKick(void);

    virtual void setRestitution(float32);
    virtual void setFriction(float32);
    virtual void setDensity(float32);
    virtual void setPosition(float32 x, float32 y);
    virtual void setPosition(const b2Vec2 &pos);
    virtual const b2Vec2 &position(void) const;
    virtual void applyLinearVelocity(const b2Vec2 &v);
    virtual void moveTo(const b2Vec2 &pos);
    virtual void stopMotion(void);
    void setXAxisConstraint(float32 y);
    virtual b2Body *body(void);

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Racket; }

    static const std::string Name;
    static const float32 DefaultDensity;
    static const float32 DefaultFriction;
    static const float32 DefaultRestitution;

  private:
    b2RevoluteJoint *mJoint;
    b2Body *mTeetingBody;
    b2MouseJoint *mMouseJoint;
    b2Body *mGround;
  };

}

#endif // __BODYRACKET_H_
