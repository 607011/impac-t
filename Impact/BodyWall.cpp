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


  Wall::Wall(int index, Game *game)
    : Body(Body::BodyType::Wall, game)
  {
    setZIndex(Body::ZIndex::Intermediate + 0);
    mTexture = mGame->level()->tile(index).texture;
    mName = std::string("Wall");

    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(0.5f * W, 0.5f * H);

    b2BodyDef bd;
    bd.type = b2_staticBody;
    mBody = game->world()->CreateBody(&bd);

    b2PolygonShape polygon;
    polygon.SetAsBox(0.5f * W * Game::InvScale, 0.5f * H * Game::InvScale);

    b2FixtureDef fd;
    fd.density = 0.f;
    fd.restitution = .5f;
    fd.shape = &polygon;
    mBody->CreateFixture(&fd);
  }


  void Wall::setPosition(int x, int y)
  {
    setPosition(b2Vec2(float32(x), float32(y)));
  }


  void Wall::setPosition(const b2Vec2 &pos)
  {
    Body::setPosition(pos);
    const b2Transform &tx = mBody->GetTransform();
    mSprite.setPosition(Game::Scale * tx.p.x, Game::Scale * tx.p.y);
    mSprite.setRotation(rad2deg(tx.q.GetAngle()));
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
