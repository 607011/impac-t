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

  const std::string Ball::Name = "Ball"; //DO NOT CHANGE UNDER ANY CIRCUMSTANCES!

  const float32 Ball::DefaultDensity = 2.f; //MOD Ballmasse
  const float32 Ball::DefaultFriction = .7f; //MOD Ballreibung
  const float32 Ball::DefaultRestitution = .5f; //MOD Ballelastizit�t
  const float32 Ball::DefaultLinearDamping = .5f; //MOD Geschwindigkeitsd�mpfung
  const float32 Ball::DefaultAngularDamping = .21f; //MOD Rotationsgeschwindigkeitsd�mpfung

  Ball::Ball(Game *game, BodyShapeType shapeType)
    : Body(Body::BodyType::Ball, game)
  {
    mName = Name;
    setEnergy(1);
    const sf::Texture &texture = mGame->level()->texture(mName);
    sf::Image img;
    img.create(texture.getSize().x + 2 * TextureMargin, texture.getSize().y + 2 * TextureMargin, sf::Color(0, 0, 0, 0));
    img.copy(texture.copyToImage(), TextureMargin, TextureMargin, sf::IntRect(0, 0, 0, 0), true);
    mTexture.loadFromImage(img);

    setHalfTextureSize(texture);

    const float32 halfW = .5f * mTexture.getSize().x;
    const float32 halfH = .5f * mTexture.getSize().y;

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(halfW, halfH);

    if (gLocalSettings().useShaders()) {
      mShader.loadFromFile(ShadersDir + "/motionblur.vs", ShadersDir + "/motionblur.fs");
      mShader.setParameter("uBlur", 2.f);
      mShader.setParameter("uResolution", float(mTexture.getSize().x), float(mTexture.getSize().y));
    }

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.linearDamping = DefaultLinearDamping;
    bd.angularDamping = DefaultAngularDamping;
    bd.bullet = true;
    bd.userData = this;
    mBody = game->world()->CreateBody(&bd);

    b2FixtureDef fd;
    fd.density = DefaultDensity;
    fd.friction = DefaultFriction;
    fd.restitution = DefaultRestitution;
    fd.userData = this;
    fd.filter.categoryBits = Body::BallMask;

    switch (shapeType) {
    case BodyShapeType::CircleShape:
    {
      b2CircleShape circle;
      circle.m_radius = .5f * texture.getSize().x * Game::InvScale;
      fd.shape = &circle;
      mBody->CreateFixture(&fd);
      break;
    }
    case BodyShapeType::PolygonShape:
    {
      b2PolygonShape square;
      const float edge = .5f * Game::InvScale * texture.getSize().x;
      square.SetAsBox(edge, edge);
      fd.shape = &square;
      mBody->CreateFixture(&fd);
      break;
    }
    default:
      throw "Unknown BodyShapeType:" + std::to_string((int)shapeType);
    }

  }


  void Ball::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    if (gLocalSettings().useShaders()) {
      mShader.setParameter("uV", mBody->GetLinearVelocity().x, mBody->GetLinearVelocity().y);
      mShader.setParameter("uRot", mBody->GetAngle());
    }
    else {
      mSprite.setRotation(rad2deg(mBody->GetAngle()));
    }
    mSprite.setPosition(Game::Scale * mBody->GetPosition().x, Game::Scale * mBody->GetPosition().y);
  }


  void Ball::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    if (gLocalSettings().useShaders()) {
      states.shader = &mShader;
    }
    target.draw(mSprite, states);
  }


  void Ball::setPosition(const b2Vec2 &p)
  {
    Body::setPosition(p);
    onUpdate(0);
  }


}
