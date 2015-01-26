// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {

  const float ParticleSystem::sHalfSize = .1333f;
  const sf::Time ParticleSystem::sMaxAge = sf::milliseconds(1000);


  ParticleSystem::ParticleSystem(Game *game, const b2Vec2 &pos, int count)
    : Body(Body::BodyType::Particle, game)
    , mColor(sf::Color::White)
    , mParticles(count)
    , mVertices(sf::Quads, 4 * count)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("ParticleSystem");
    setLifetime(sMaxAge);
    setPosition(pos.x, pos.y);
  }


  ParticleSystem::~ParticleSystem()
  {
#ifndef NDEBUG
    std::cout << "~dtor of " << typeid(this).name() << std::endl;
#endif
    b2World *world = mGame->world();
    for (std::vector<SimpleParticle>::const_iterator p = mParticles.cbegin(); p != mParticles.cend(); ++p) {
      if (!p->dead)
        world->DestroyBody(p->body);
    }
  }


  void ParticleSystem::setPosition(float x, float y)
  {
#ifndef NDEBUG
    std::cout << "ParticleSystem::setPosition(" << x << ", " << y << ")" << std::endl;
#endif
    b2World *world = mGame->world();
    const int N = mParticles.size();
    for (int i = 0; i < N; ++i) {
      SimpleParticle &p = mParticles[i];
      p.dead = false;
      p.lifeTime = sf::milliseconds(500 + std::rand() % 500);

      float angle = 2.f * _PI * std::rand() / RAND_MAX;
      float speed = float(mGame->tileWidth() * (2 + std::rand() % 5));
      const b2Vec2 &v = speed * b2Vec2(std::cos(angle), std::sin(angle));
      const int j = 4 * i;
      mVertices[j+0].color = mColor;
      mVertices[j+1].color = mColor;
      mVertices[j+2].color = mColor;
      mVertices[j+3].color = mColor;

      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.position.Set(x, y);
      bd.fixedRotation = true;
      bd.bullet = false;
      bd.userData = this;
      bd.gravityScale = 5.f;
      bd.linearDamping = 1.f;
      bd.linearVelocity = v;
      p.body = world->CreateBody(&bd);

      b2CircleShape circleShape;
      circleShape.m_radius = sHalfSize;

      b2FixtureDef fd;
      fd.density = 0.f;
      fd.restitution = .1f;
      fd.friction = 1.f;
      fd.filter.categoryBits = Body::BodyMask::ParticleMask;
      fd.filter.maskBits = 0xffff & ~Body::BodyMask::ParticleMask & ~Body::BodyMask::BallMask & ~Body::BodyMask::PadMask;
      fd.shape = &circleShape;
      p.body->CreateFixture(&fd);
    }
  }


  void ParticleSystem::setColor(const sf::Color &color)
  {
    mColor = color;
  }


  void ParticleSystem::onUpdate(float)
  {
    const float sx = float(mGame->tileWidth());
    const float sy = float(mGame->tileHeight());
    static const sf::Vector2f topLeft(-sHalfSize * sx, -sHalfSize * sy);
    static const sf::Vector2f topRight(sHalfSize * sx, -sHalfSize * sy);
    static const sf::Vector2f bottomRight(sHalfSize * sx, sHalfSize * sy);
    static const sf::Vector2f bottomLeft(-sHalfSize * sx, sHalfSize * sy);
    bool allDead = true;
    const int N = mParticles.size();
#pragma omp parallel for reduction(&:allDead)
    for (int i = 0; i < N; ++i) {
      SimpleParticle &p = mParticles[i];
      if (age() > p.lifeTime && !p.dead) {
        p.dead = true;
        mGame->world()->DestroyBody(p.body);
      }
      else {
        const b2Vec2 &pos = p.body->GetPosition();
        sf::Vector2f offset(pos.x * sx, pos.y * sy);
        const int j = 4 * i;
        mVertices[j+0].position = offset + topLeft;
        mVertices[j+1].position = offset + topRight;
        mVertices[j+2].position = offset + bottomRight;
        mVertices[j+3].position = offset + bottomLeft;
        const sf::Uint8 alpha = 0xffU - sf::Uint8(Easing<float>::quadEaseIn(age().asSeconds(), 0.0f, 255.0f, p.lifeTime.asSeconds()));
        mVertices[j+0].color.a = alpha;
        mVertices[j+1].color.a = alpha;
        mVertices[j+2].color.a = alpha;
        mVertices[j+3].color.a = alpha;
      }
      allDead &= p.dead;
    }
    if (allDead || overAge())
      this->kill();
  }


  void ParticleSystem::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    states.texture = nullptr;
    target.draw(mVertices, states);
  }

}
