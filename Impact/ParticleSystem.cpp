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

#ifndef PARTICLES_WITH_SPRITES
  const float ParticleSystem::sHalfSize = 2.f;
#endif

  const sf::Time ParticleSystem::sMaxAge = sf::milliseconds(1000);
  const sf::Color ParticleSystem::sColor = sf::Color::White;

  ParticleSystem::ParticleSystem(Game *game, const b2Vec2 &pos, bool ballCollisionEnabled, int count)
    : Body(Body::BodyType::Particle, game)
    , mParticles(count)
#ifndef PARTICLES_WITH_SPRITES
    , mVertices(sf::Quads, 4 * count)
#endif
  {
    setZIndex(Body::ZIndex::Foreground + 0);
    mName = std::string("ParticleSystem");
    setLifetime(sMaxAge);
#ifdef PARTICLES_WITH_SPRITES
    mTexture.loadFromFile("resources/images/particle.png");
    mShader.loadFromFile("resources/shaders/particlesystem.frag", sf::Shader::Fragment);
    mShader.setParameter("uTexture", sf::Shader::CurrentTexture);
    mShader.setParameter("uMaxAge", sMaxAge.asSeconds());
#endif

    b2World *world = mGame->world();
    const int N = mParticles.size();
    for (int i = 0; i < N; ++i) {
      SimpleParticle &p = mParticles[i];
      p.dead = false;
      p.lifeTime = sf::milliseconds(500 + std::rand() % 500);
      b2Rot angle(_2PI * std::rand() / RAND_MAX);
      float speed = float(Game::Scale * (2 + 5 * float(std::rand()) / RAND_MAX));
      const b2Vec2 &v = speed * b2Vec2(angle.c, angle.s);
#ifdef PARTICLES_WITH_SPRITES
      p.sprite.setTexture(mTexture);
      mTexture.setRepeated(false);
      mTexture.setSmooth(false);
      p.sprite.setOrigin(.5f * mTexture.getSize().x, .5f * mTexture.getSize().y);
#else
      const int j = 4 * i;
      mVertices[j+0].color = sColor;
      mVertices[j+1].color = sColor;
      mVertices[j+2].color = sColor;
      mVertices[j+3].color = sColor;
#endif

      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.position.Set(pos.x, pos.y);
      bd.fixedRotation = true;
      bd.bullet = false;
      bd.userData = this;
      bd.gravityScale = 5.f;
      bd.linearDamping = 1.f;
      bd.linearVelocity = v;
      p.body = world->CreateBody(&bd);

      b2CircleShape circleShape;
      circleShape.m_radius = 1e-4f * Game::InvScale;

      b2FixtureDef fd;
      fd.density = 0.f;
      fd.restitution = .1f;
      fd.friction = 1.f;
      fd.filter.categoryBits = Body::ParticleMask;
      fd.filter.maskBits = 0xffffU ^ Body::ParticleMask ^ Body::RacketMask;
      if (!ballCollisionEnabled)
        fd.filter.maskBits ^= Body::BallMask;
      fd.shape = &circleShape;
      p.body->CreateFixture(&fd);
    }
  }


  ParticleSystem::~ParticleSystem()
  {
    b2World *world = mGame->world();
    for (std::vector<SimpleParticle>::const_iterator p = mParticles.cbegin(); p != mParticles.cend(); ++p) {
      if (!p->dead)
        world->DestroyBody(p->body);
    }
  }


  void ParticleSystem::setBallCollisionEnabled(bool ballCollisionEnabled)
  {
    for (std::vector<SimpleParticle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p) {
      b2Fixture *fixture = p->body->GetFixtureList();
      b2Filter filter = fixture->GetFilterData();
      if (ballCollisionEnabled)
        filter.maskBits |= Body::BallMask;
      else
        filter.maskBits &= ~Body::BallMask;
      fixture->SetFilterData(filter);
    }
  }


  void ParticleSystem::onUpdate(float)
  {
#ifndef PARTICLES_WITH_SPRITES
    static const sf::Vector2f topLeft(-sHalfSize, -sHalfSize);
    static const sf::Vector2f topRight(sHalfSize, -sHalfSize);
    static const sf::Vector2f bottomRight(sHalfSize, sHalfSize);
    static const sf::Vector2f bottomLeft(-sHalfSize, sHalfSize);
#endif
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
#ifdef PARTICLES_WITH_SPRITES
        p.sprite.setPosition(pos.x * Game::Scale, pos.y * Game::Scale);
#else
        const float lifetime = p.lifeTime.asSeconds();
        const sf::Uint8 alpha = 0xffU - sf::Uint8(Easing<float>::quadEaseIn(b2Clamp(age().asSeconds(), 0.f, lifetime), 0.0f, 255.0f, lifetime));
        sf::Vector2f offset(pos.x * Game::Scale, pos.y * Game::Scale);
        const int j = 4 * i;
        mVertices[j+0].position = offset + topLeft;
        mVertices[j+1].position = offset + topRight;
        mVertices[j+2].position = offset + bottomRight;
        mVertices[j+3].position = offset + bottomLeft;
        mVertices[j+0].color.a = alpha;
        mVertices[j+1].color.a = alpha;
        mVertices[j+2].color.a = alpha;
        mVertices[j+3].color.a = alpha;
#endif
      }
      allDead &= p.dead;
    }
#ifdef PARTICLES_WITH_SPRITES
    mShader.setParameter("uAge", age().asSeconds());
#endif
    if (allDead || overAge())
      this->kill();
  }


  void ParticleSystem::onDraw(sf::RenderTarget &target, sf::RenderStates states) const
  {
#ifdef PARTICLES_WITH_SPRITES
    states.shader = &mShader;
    for (std::vector<SimpleParticle>::const_iterator p = mParticles.cbegin(); p != mParticles.cend(); ++p)
      if (!p->dead)
        target.draw(p->sprite, states);
#else
    states.texture = nullptr;
    target.draw(mVertices, states);
#endif
  }

}
