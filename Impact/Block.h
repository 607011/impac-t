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


#ifndef __BODYBLOCK_H_
#define __BODYBLOCK_H_

#include "Body.h"
#include "Impact.h"

namespace Impact {

  class Block : public Body
  {
  public:
    Block(int index, Game *game, const TileParam &tileParam);

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Block; }

    virtual bool hit(float impulse);

    void setGravityScale(float32);

    static const std::string Name;
    static const float32 DefaultDensity;
    static const float32 DefaultFriction;
    static const float32 DefaultRestitution;
    static const float32 DefaultLinearDamping;
    static const float32 DefaultAngularDamping;

  protected:
    static const int TextureMargin = 8;


  private:
    float32 mGravityScale;
    int mMinimumHitImpulse;
  };

}

#endif // __BODYBLOCK_H_

