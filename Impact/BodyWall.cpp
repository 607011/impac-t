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

  const std::string Wall::Name = "Wall";
  const float32 Wall::DefaultDensity = 0.f;
  const float32 Wall::DefaultFriction = .2f;
  const float32 Wall::DefaultRestitution = 0.5f;


  Wall::Wall(int index, Game *game)
    : Body(Body::BodyType::Wall, game)
  {
    mName = Name;
    mTexture = mGame->level()->tileParam(index).texture;

    const float halfW = .5f * mTexture.getSize().x;
    const float halfH = .5f * mTexture.getSize().y;

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(halfW, halfH);

    b2BodyDef bd;
    bd.type = b2_staticBody;
    mBody = game->world()->CreateBody(&bd);

    b2PolygonShape polygon;
    polygon.SetAsBox(halfW * Game::InvScale, halfH * Game::InvScale);

    b2FixtureDef fd;
    fd.density = DefaultDensity;
    fd.restitution = DefaultRestitution;
    fd.friction = DefaultFriction;
    fd.shape = &polygon;
    mBody->CreateFixture(&fd);
  }


  void Wall::setPosition(int x, int y)
  {
    setPosition(b2Vec2(static_cast<float32>(x), static_cast<float32>(y)));
  }


  void Wall::setPosition(const b2Vec2 &pos)
  {
    Body::setPosition(pos);
    const b2Vec2 &p = mBody->GetPosition();
    mSprite.setPosition(Game::Scale * p.x, Game::Scale * p.y);
  }


  void Wall::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
  }


  void Wall::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


}
