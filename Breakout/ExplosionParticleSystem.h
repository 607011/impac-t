// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __EXPLOSIONPARTICLESYSTEM_H_
#define __EXPLOSIONPARTICLESYSTEM_H_

#include <Box2D/Box2D.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Body.h"
#include "Breakout.h"

namespace Breakout {
  
  struct ExplosionParticle 
  {
    b2Body *body;
    sf::Time lifeTime;
    bool dead;
  };


  class ExplosionParticleSystem : public Body
  {
  public:
    static ExplosionParticleSystem *instance(void);

  protected:
    ExplosionParticleSystem(void);
    virtual ~ExplosionParticleSystem();

  public:
    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Particle; }

    virtual void setPosition(float x, float y);
    virtual void setColor(const sf::Color &);

    virtual void setGame(Game *);

    void spawn(float x, float y, const int particleCount = 100);

  protected:
    static const float sHalfSize;
    static const sf::Time sMaxAge;
    std::vector<ExplosionParticle> mParticles;
    sf::Color mColor;
    sf::VertexArray mVertices;
    int mCurrentParticleIndex;

    static const int MaxParticleCount = 4096;

  private:
    static ExplosionParticleSystem *mInstance;
    ExplosionParticleSystem(const ExplosionParticleSystem&);

  };

}

#endif // __EXPLOSIONPARTICLESYSTEM_H_
