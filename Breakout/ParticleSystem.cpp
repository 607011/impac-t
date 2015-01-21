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
    const int N = mParticles.size();
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
      SimpleParticle &p = mParticles[i];
      p.lifeTime = sf::milliseconds(500);

      float angle = 2.f * _PI * std::rand() / RAND_MAX;
      float speed = float(mGame->tileWidth() * (20 + std::rand() % 250));
      const b2Vec2 &v = speed * b2Vec2(std::cos(angle), std::sin(angle));
      std::cout << v.x << "/" << v.y << std::endl;
      const float a = 0.5f * (3 + std::rand() % 4);
      const int j = 4 * i;
      mVertices[j+0].color = mColor;
      mVertices[j+1].color = mColor;
      mVertices[j+2].color = mColor;
      mVertices[j+3].color = mColor;

      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.position.Set(x, y - 1.f);
      bd.fixedRotation = true;
      bd.bullet = true;
      bd.userData = this;
      bd.gravityScale = 0.1f;
      bd.linearDamping = 0.2f;
      bd.linearVelocity = v;
      p.body = mGame->world()->CreateBody(&bd);

      b2CircleShape circleShape;
      circleShape.m_radius = 0.01f;

      b2FixtureDef fd;
      fd.density = 0.1f;
      fd.filter.categoryBits = 2;
      fd.filter.maskBits = 1;
      fd.shape = &circleShape;
      p.body->CreateFixture(&fd);

      b2Fixture* f = p.body->GetFixtureList();
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
    const float sx = float(mGame->tileWidth());
    const float sy = float(mGame->tileHeight());
    static const float a = 2.f;
    static const sf::Vector2f topLeft(-a, -a);
    static const sf::Vector2f topRight(a, -a);
    static const sf::Vector2f bottomRight(a, a);
    static const sf::Vector2f bottomLeft(-a, a);
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
      SimpleParticle &p = mParticles[i];
      const b2Vec2 &pos = p.body->GetPosition();
      sf::Vector2f offset(pos.x * sx, pos.y * sy);
      const int j = 4 * i;
      mVertices[j+0].position = offset + topLeft;
      mVertices[j+1].position = offset + topRight;
      mVertices[j+2].position = offset + bottomRight;
      mVertices[j+3].position = offset + bottomLeft;
      const sf::Uint8 alpha = 0xffU - sf::Uint8(Easing<float>::quadEaseIn(age().asSeconds(), 0.0f, 255.0f, mMaxAge.asSeconds()));
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
