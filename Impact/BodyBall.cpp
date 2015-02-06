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

  Ball::Ball(Game *game)
    : Body(Body::BodyType::Ball, game)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    setEnergy(1);
    mName = std::string("Ball");
    mTexture = mGame->level()->texture(mName);

    const int W = mTexture.getSize().x;
    const int H = mTexture.getSize().y;

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(.5f * W, .5f * H);

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.angle = 0.f;
    bd.linearDamping = 0.5f;
    bd.angularDamping = 0.21f;
    bd.gravityScale = 1.f;
    bd.linearVelocity = b2Vec2(0.f, -1.f);
    bd.bullet = true;
    bd.userData = this;
    mBody = game->world()->CreateBody(&bd);

    b2CircleShape circle;
    circle.m_p.SetZero();
    circle.m_radius = .5f * W * Game::InvScale;

    b2FixtureDef fd;
    fd.shape = &circle;
    fd.density = 2.f;
    fd.friction = 0.7f;
    fd.restitution = 0.1f;
    fd.userData = this;
    fd.filter.categoryBits = Body::BallMask;
    mBody->CreateFixture(&fd);
  }


  void Ball::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    mSprite.setPosition(Game::Scale * mBody->GetPosition().x, Game::Scale * mBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mBody->GetAngle()));
  }


  void Ball::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }

}
