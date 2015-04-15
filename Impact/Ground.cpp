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


#include "stdafx.h"

namespace Impact {

  const std::string Ground::Name = "Ground";

  Ground::Ground(Game *game, float32 width)
    : Body(Body::BodyType::Ground, game)
  {
    mName = Name;

    b2BodyDef bd;
    bd.userData = this;
    mBody = mGame->world()->CreateBody(&bd);

    b2EdgeShape bottomBoundary;
    bottomBoundary.Set(b2Vec2_zero, b2Vec2(width, 0.f));
    b2Fixture *f = mBody->CreateFixture(&bottomBoundary, 0.f);
    f->SetUserData(this);
  }


  void Ground::setPosition(int x, int y)
  {
    setPosition(b2Vec2(float32(x), float32(y)));
  }


  void Ground::setPosition(float32 x, float32 y)
  {
    setPosition(b2Vec2(x, y));
  }


  void Ground::setPosition(const b2Vec2 &pos)
  {
    mBody->SetTransform(pos, 0.f);
  }
}
