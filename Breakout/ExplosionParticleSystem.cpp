// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"

namespace Breakout {

  const float ExplosionParticleSystem::sHalfSize = 2.f;
  const sf::Time ExplosionParticleSystem::sMaxAge = sf::milliseconds(1000);
  ExplosionParticleSystem *ExplosionParticleSystem::mInstance = nullptr;

  ExplosionParticleSystem::ExplosionParticleSystem(void)
    : Body(Body::BodyType::Particle, nullptr)
    , mColor(sf::Color::White)
    , mParticles(MaxParticleCount)
    , mVertices(sf::Quads, 4 * MaxParticleCount)
    , mCurrentParticleIndex(0)
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("ParticleSystem");
    setLifetime(sMaxAge);
  }


  ExplosionParticleSystem::~ExplosionParticleSystem()
  {
#ifndef NDEBUG
    std::cout << "~dtor of " << typeid(this).name() << std::endl;
#endif
    b2World *world = mGame->world();
    for (std::vector<ExplosionParticle>::const_iterator p = mParticles.cbegin(); p != mParticles.cend(); ++p) {
      if (!p->dead)
        world->DestroyBody(p->body);
    }
  }


  ExplosionParticleSystem *ExplosionParticleSystem::instance(void)
  {
    if (mInstance == nullptr)
      mInstance = new ExplosionParticleSystem;
    return mInstance;
  }


  void ExplosionParticleSystem::setGame(Game *game)
  {
    mGame = game;
  }


  void ExplosionParticleSystem::spawn(float x, float y, const int particleCount)
  {
    b2World *world = mGame->world();
#pragma omp parallel for
    for (int i = 0; i < particleCount; ++i) {
      ExplosionParticle &p = mParticles[(mCurrentParticleIndex + i) % MaxParticleCount];
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
      bd.position.Set(x, y - 1.f);
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
    mCurrentParticleIndex += particleCount;
  }


  void ExplosionParticleSystem::setPosition(float x, float y)
  {
    spawn(x, y);
  }


  void ExplosionParticleSystem::setColor(const sf::Color &color)
  {
    mColor = color;
  }


  void ExplosionParticleSystem::onUpdate(float elapsedSeconds)
  {
    UNUSED(elapsedSeconds);
    const int N = mParticles.size();
    const float sx = float(mGame->tileWidth());
    const float sy = float(mGame->tileHeight());
    static const sf::Vector2f topLeft(-sHalfSize * sx, -sHalfSize * sy);
    static const sf::Vector2f topRight(sHalfSize * sx, -sHalfSize * sy);
    static const sf::Vector2f bottomRight(sHalfSize * sx, sHalfSize * sy);
    static const sf::Vector2f bottomLeft(-sHalfSize * sx, sHalfSize * sy);
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
      ExplosionParticle &p = mParticles[i];
      if (age() > p.lifeTime && !p.dead) {
        p.dead = true;
        mGame->world()->DestroyBody(p.body);
      }
      else {
        const b2Vec2 &pos = p.body->GetPosition();
        sf::Vector2f offset(pos.x * sx, pos.y * sy);
        const sf::Uint8 alpha = 0xffU - sf::Uint8(Easing<float>::quadEaseIn(age().asSeconds(), 0.0f, 255.0f, p.lifeTime.asSeconds()));
        const int j = 4 * i;
        mVertices[j+0].color.a = alpha;
        mVertices[j+1].color.a = alpha;
        mVertices[j+2].color.a = alpha;
        mVertices[j+3].color.a = alpha;
      }
    }
  }


  void ExplosionParticleSystem::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    states.texture = nullptr;
    glPushAttrib(0);
    glPointSize(sHalfSize * 2);
    glBegin(GL_POINTS);
    for (std::vector<ExplosionParticle>::const_iterator p = mParticles.cbegin(); p != mParticles.cend(); ++p) {
      b2Vec2 pos = p->body->GetPosition();
      const float alpha = Easing<float>::quadEaseIn(age().asSeconds(), 0.f, 1.f, p->lifeTime.asSeconds());
      glColor4f(mColor.r, mColor.g, mColor.b, alpha);
      glVertex2f(pos.x, pos.y);
    }
    glEnd();
    glPopAttrib();
  }

}
