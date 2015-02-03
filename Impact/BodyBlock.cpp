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


#define BANGOUT_BLOCKS_ARE_ROUNDED_RECTANGLES

  Block::Block(int index, Game *game)
    : Body(Body::BodyType::Block, game)
    , mGravityScale(2.f)
  {
    setZIndex(Body::ZIndex::Intermediate + 0);
    setEnergy(100);
    mTexture = mGame->level()->tile(index).texture;
    mName = std::string("Block");

    const float W = float(mTexture.getSize().x);
    const float H = float(mTexture.getSize().y);

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(0.5f * W, 0.5f * H);

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.angle = 0.f;
    bd.linearDamping = 0.5f;
    bd.angularDamping = 0.5f;
    bd.gravityScale = 0.f;
    bd.allowSleep = true;
    bd.awake = false;
    bd.fixedRotation = false;
    bd.bullet = false;
    bd.active = true;
    bd.userData = this;
    mBody = game->world()->CreateBody(&bd);

#ifdef BANGOUT_BLOCKS_ARE_ROUNDED_RECTANGLES
    b2PolygonShape polygon;
    polygon.SetAsBox(0.5f * (W - 8) * Game::InvScale, 0.5f * H * Game::InvScale);

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = 800.f;
    fdBox.friction = mGame->world()->GetGravity().y;
    fdBox.restitution = 0.04f;
    fdBox.userData = this;
    mBody->CreateFixture(&fdBox);

    b2CircleShape circleL;
    circleL.m_p.Set(-8.f * Game::InvScale, 0.f);
    circleL.m_radius = 8.f * Game::InvScale;

    b2FixtureDef fdCircleL;
    fdCircleL.shape = &circleL;
    fdCircleL.density = 800.f;
    fdCircleL.friction = mGame->world()->GetGravity().y;
    fdCircleL.restitution = 0.04f;
    fdCircleL.userData = this;
    mBody->CreateFixture(&fdCircleL);

    b2CircleShape circleR;
    circleR.m_p.Set(+8.f * Game::InvScale, 0.f);
    circleR.m_radius = 8.f * Game::InvScale;

    b2FixtureDef fdCircleR;
    fdCircleR.shape = &circleR;
    fdCircleR.density = 800.f;
    fdCircleR.friction = mGame->world()->GetGravity().y;
    fdCircleR.restitution = 0.04f;
    fdCircleR.userData = this;
    mBody->CreateFixture(&fdCircleR);
#else
    b2PolygonShape polygon;
    polygon.SetAsBox(0.5f * W * Game::InvScale, 0.5f * H * Game::InvScale);

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = 800.f;
    fdBox.friction = mGame->world()->GetGravity().y;
    fdBox.restitution = 0.04f;
    fdBox.userData = this;
    mBody->CreateFixture(&fdBox);
#endif
  }


  void Block::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    const b2Transform &tx = mBody->GetTransform();
    mSprite.setPosition(Game::Scale * tx.p.x, Game::Scale * tx.p.y);
    mSprite.setRotation(rad2deg(tx.q.GetAngle()));
  }


  void Block::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


  bool Block::hit(float impulse)
  {
    bool destroyed = Body::hit(int(impulse));
    if (!destroyed) {
      mBody->SetGravityScale(mGravityScale);
      mSprite.setColor(sf::Color(255, 255, 255, 192));
    }
    return destroyed;
  }


  void Block::setGravityScale(float32 gravityScale)
  {
    mGravityScale = gravityScale;
  }

}
