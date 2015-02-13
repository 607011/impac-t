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

  const std::string Block::Name = "Block";
  const float32 Block::DefaultDensity = 19.32f;
  const float32 Block::DefaultFriction = .71f;
  const float32 Block::DefaultRestitution = 0.04f;

  Block::Block(int index, Game *game)
    : Body(Body::BodyType::Block, game)
    , mGravityScale(2.f)
    , mMinimumHitImpulse(0)
  {
    mName = Name;

    const sf::Texture &texture = mGame->level()->tileParam(index).texture;
    sf::Image img;
    img.create(texture.getSize().x + 2 * TextureBorderWidth, texture.getSize().y + 2 * TextureBorderWidth, sf::Color(255, 255, 255, 0));
    img.copy(texture.copyToImage(), TextureBorderWidth, TextureBorderWidth, sf::IntRect(0, 0, 0, 0), true);
    mTexture.loadFromImage(img);

    mShader.loadFromFile(gShadersDir + "/fallingblock.frag", sf::Shader::Fragment);
    mShader.setParameter("uAge", .0f);
    mShader.setParameter("uBlur", .0f);
    mShader.setParameter("uColor", sf::Color(255, 255, 255, 255));
    mShader.setParameter("uResolution", float(texture.getSize().x), float(texture.getSize().y));

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(.5f * mTexture.getSize().x, .5f * mTexture.getSize().y);

    const int W = texture.getSize().x;
    const int H = texture.getSize().y;

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.angle = .0f;
    bd.linearDamping = 5.f;
    bd.angularDamping = .5f;
    bd.gravityScale = .0f;
    bd.allowSleep = true;
    bd.awake = false;
    bd.fixedRotation = false;
    bd.bullet = false;
    bd.active = true;
    bd.userData = this;
    mBody = game->world()->CreateBody(&bd);

    b2PolygonShape polygon;
    const float32 hs = .5f * Game::InvScale;
    const float32 hh = hs * H;
    const float32 xoff = hs * (W - H);
    polygon.SetAsBox(xoff, hh);

    b2FixtureDef fdBox;
    fdBox.shape = &polygon;
    fdBox.density = DefaultDensity;
    fdBox.friction = DefaultFriction;
    fdBox.restitution = DefaultRestitution;
    fdBox.userData = this;
    mBody->CreateFixture(&fdBox);

    b2CircleShape circleL;
    circleL.m_p.Set(-xoff, 0.f);
    circleL.m_radius = hh;

    b2FixtureDef fdCircleL;
    fdCircleL.shape = &circleL;
    fdCircleL.density = DefaultDensity;
    fdCircleL.friction = DefaultFriction;
    fdCircleL.restitution = DefaultRestitution;
    fdCircleL.userData = this;
    mBody->CreateFixture(&fdCircleL);

    b2CircleShape circleR;
    circleR.m_p.Set(xoff, 0.f);
    circleR.m_radius = hh;

    b2FixtureDef fdCircleR;
    fdCircleR.shape = &circleR;
    fdCircleR.density = DefaultDensity;
    fdCircleR.friction = DefaultFriction;
    fdCircleR.restitution = DefaultRestitution;
    fdCircleR.userData = this;
    mBody->CreateFixture(&fdCircleR);
  }


  void Block::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    mSprite.setPosition(Game::Scale * mBody->GetPosition().x, Game::Scale * mBody->GetPosition().y);
    mSprite.setRotation(rad2deg(mBody->GetAngle()));
    mShader.setParameter("uAge", age().asSeconds());
  }


  void Block::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    states.shader = &mShader;
    target.draw(mSprite, states);
  }


  bool Block::hit(float impulse)
  {
    const int v = int(impulse);
    bool destroyed = Body::hit(v);
    if (!destroyed && v > mMinimumHitImpulse) {
      mBody->SetLinearDamping(0.f);
      mBody->SetGravityScale(mGravityScale);
      mShader.setParameter("uColor", sf::Color(sf::Color(255, 255, 255, 0xbf)));
      mShader.setParameter("uBlur", 1.28f);
    }
    return destroyed;
  }


  void Block::setMinimumHitImpulse(int v)
  {
    mMinimumHitImpulse = v;
  }


  void Block::setGravityScale(float32 gravityScale)
  {
    mGravityScale = gravityScale;
  }


}
