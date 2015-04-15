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

  const std::string Bumper::Name = "Bumper";

  Bumper::Bumper(int index, Game *game)
    : Body(Body::BodyType::Bumper, game)
    , mActivated(false)
  {
    mName = Name;
    mTexture = mGame->level()->tileParam(index).texture;
    mSprite.setTexture(mTexture);
    mSprite.setOrigin(.5f * mTexture.getSize().x, .5f * mTexture.getSize().y);

    setHalfTextureSize(mTexture);

    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.userData = this;
    mBody = game->world()->CreateBody(&bd);

    b2CircleShape circle;
    circle.m_radius = .5f * mTexture.getSize().x * Game::InvScale;

    b2FixtureDef fd;
    fd.shape = &circle;
    fd.userData = this;
    mBody->CreateFixture(&fd);
  }


  void Bumper::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    static const float MaxT = 0.05f;
    if (mActivated) {
      auto forthAndBack = [](float t, float b, float c) {
        return std::sin(b2_pi * t / MaxT) * (c - b) + b;
      };
      const float t = mActivationTimer.getElapsedTime().asSeconds();
      const float scale = forthAndBack(t, 1.f, 1.2f);
      mSprite.setScale(scale, scale);
      if (t > MaxT)
        mActivated = false;
    }
  }


  void Bumper::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(mSprite, states);
  }


  void Bumper::setPosition(float32 x, float32 y)
  {
    setPosition(b2Vec2(x, y));
  }


  void Bumper::setPosition(const b2Vec2 &pos)
  {
    const sf::Vector2u &txSz = mTexture.getSize();
    mBody->SetTransform(pos + b2Vec2(mHalfTextureSize.x, 1 - mHalfTextureSize.y), 0.f);
    const b2Vec2 &p = mBody->GetPosition();
    mSprite.setPosition(Game::Scale * p.x, Game::Scale * p.y);
  }


  void Bumper::activate(void)
  {
    mActivationTimer.restart();
    mActivated = true;
  }
}
