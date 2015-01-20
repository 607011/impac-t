// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {


  const sf::Time ParticleSystem::sMaxAge = sf::milliseconds(1000);


  ParticleSystem::ParticleSystem(Game *game, int count)
    : Body(Body::BodyType::Ground, game)
    , mColor(sf::Color::White)
    , mParticles(count)
    , mVertices(sf::Quads, 4 * count)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("ParticleSystem");
    setLifetime(sMaxAge);
  }

  
  ParticleSystem::~ParticleSystem()
  {
    // ...
  }


  void ParticleSystem::setPosition(float x, float y)
  {
    b2Vec2 pos(x, y);
    const int N = mParticles.size();
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
      SimpleParticle &p = mParticles[i];
      p.lifeTime = sf::milliseconds(500);

      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.position.Set(x, y);
      bd.fixedRotation = true;
      bd.bullet = true;
      bd.userData = this;
      p.body = mGame->world()->CreateBody(&bd);

      b2CircleShape circleShape;
      circleShape.m_radius = 0.01f;

      b2FixtureDef fd;
      fd.density = 1;
      fd.filter.categoryBits = 2;
      fd.filter.maskBits = 1;
      fd.shape = &circleShape;
      p.body->CreateFixture(&fd);

      b2Body *body = p.body;
      b2Fixture* f = body->GetFixtureList();
      float angle = std::rand() * _PI / RAND_MAX;
      float speed = 10.f * (std::rand() % 250);
      const b2Vec2 &v = float32(speed) * b2Vec2(std::cos(angle), std::sin(angle));
      const float a = 0.5f * (3 + std::rand() % 4);
      const int j = 4 * i;
      mVertices[j+0].color = mColor;
      mVertices[j+1].color = mColor;
      mVertices[j+2].color = mColor;
      mVertices[j+3].color = mColor;
      body->SetTransform(pos, 0.f);
      body->SetLinearVelocity(v);
      body->SetGravityScale(0.1f);
      body->SetLinearDamping(1.2f);
      f->SetRestitution(0.8f);
      f->SetFriction(2.f);
    }
  }


  void ParticleSystem::setColor(const sf::Color &color)
  {
    mColor = color;
  }


  void ParticleSystem::onUpdate(float elapsedSeconds)
  {
    const int N = mParticles.size();
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
      SimpleParticle &p = mParticles[i];
      const b2Vec2 &pos = p.body->GetPosition();
      const sf::Uint8 alpha = 0xffU - sf::Uint8(Easing<float>::quadEaseIn(age().asSeconds(), 0.0f, 255.0f, mMaxAge.asSeconds()));
      sf::Vector2f offset(pos.x, pos.y);
      static const float a = 2.f;
      const int j = 4 * i;
      mVertices[j+0].position = sf::Vector2f(-a, -a) + offset;
      mVertices[j+1].position = sf::Vector2f(a, -a) + offset;
      mVertices[j+2].position = sf::Vector2f(a, a) + offset;
      mVertices[j+3].position = sf::Vector2f(-a, a) + offset;
      mVertices[j+0].color.a = alpha;
      mVertices[j+1].color.a = alpha;
      mVertices[j+2].color.a = alpha;
      mVertices[j+3].color.a = alpha;
    }

    if (overAge())
      this->kill();
  }


  void ParticleSystem::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    states.texture = nullptr;
    target.draw(mVertices, states);

  }


}
