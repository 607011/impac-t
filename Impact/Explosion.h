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


#ifndef __PARTICLESYSTEM_H_
#define __PARTICLESYSTEM_H_

#include <Box2D/Box2D.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <vector>

#include "Body.h"
#include "Impact.h"

namespace Impact {
  
  struct SimpleParticle 
  {
    b2Body *body;
    sf::Time lifeTime;
    bool dead;
    sf::Sprite sprite;
  };


  struct ExplosionDef
  {
    ExplosionDef(Game *game, const b2Vec2 &pos)
      : game(game)
      , pos(pos)
      , ballCollisionEnabled(false)
      , count(50)
      , minLifetime(sf::milliseconds(500))
      , maxLifetime(sf::milliseconds(1000))
      , minSpeed(2.f * Game::Scale)
      , maxSpeed(5.f * Game::Scale)
      , gravityScale(5.f)
      , linearDamping(.2f)
      , radius(1e-6f)
      , density(1.f)
      , friction(0.f)
      , restitution(.8f)
    { /* ... */ }
    Game *game;
    b2Vec2 pos;
    bool ballCollisionEnabled;
    int count;
    sf::Time minLifetime;
    sf::Time maxLifetime;
    float32 minSpeed;
    float32 maxSpeed;
    float32 gravityScale;
    float32 linearDamping;
    float32 radius;
    float32 density;
    float32 friction;
    float32 restitution;
    sf::Texture texture;
  };


  class Explosion : public Body
  {
  public:
    Explosion(const ExplosionDef &);
    virtual ~Explosion();

    // Body implementation
    virtual void onUpdate(float elapsedSeconds);
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual BodyType type(void) const { return Body::BodyType::Particle; }

  private:
    std::vector<SimpleParticle> mParticles;

    sf::Shader *mShader;

    static std::vector<sf::Shader*> sShaders;
    static std::vector<sf::Shader*>::size_type sCurrentShaderIndex;

    struct ShaderPool {
      static const std::vector<sf::Shader*>::size_type N = 8; // maximum number of concurrent explosions
      ShaderPool(void)
      {
#ifndef NDEBUG
        std::cout << "ShaderPool() ..." << std::endl;
#endif
        if (gLocalSettings().useShaders())
          init();
      }
      static void init(void)
      {
#ifndef NDEBUG
        std::cout << "ShaderPool::init() ..." << std::endl;
#endif
        std::ifstream inFile;
        inFile.open(ShadersDir + "/explosion.fs");
        std::stringstream strStream;
        strStream << inFile.rdbuf();
        std::string fragmentShaderCode = strStream.str();
        for (std::vector<sf::Shader*>::size_type i = 0; i < N; ++i) {
          sf::Shader *shader = new sf::Shader;
          shader->loadFromMemory(fragmentShaderCode, sf::Shader::Fragment);
          sShaders.push_back(shader);
        }
      }
      static sf::Shader *getNext(void)
      {
        if (!gLocalSettings().useShaders())
          return nullptr;
        if (sShaders.size() == 0)
          ShaderPool::init();
        sf::Shader *next = sShaders.at(sCurrentShaderIndex);
        if (++sCurrentShaderIndex >= sShaders.size())
          sCurrentShaderIndex = 0;
        return next;
      }
    };
    static ShaderPool sShaderPool;
  };

}

#endif // __PARTICLESYSTEM_H_
