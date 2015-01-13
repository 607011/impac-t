// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.


#ifndef __GAME_H_
#define __GAME_H_

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>

namespace Breakout {

  class Game;

  class DestructionListener : public b2DestructionListener {
  public:
    void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
    void SayGoodbye(b2Joint* joint) { B2_NOT_USED(joint); }

    Game* test;
  };


  struct ContactPoint {
    b2Fixture *fixtureA;
    b2Fixture *fixtureB;
    b2Vec2 normal;
    b2Vec2 position;
    b2PointState state;
    float32 normalImpulse;
    float32 tangentImpulse;
    float32 separation;
  };


  class Game : public b2ContactListener {

    typedef enum _Actions {
      NoAction,
      MoveLeft,
      MoveRight,
      SpecialAction,
      BackAction,
      LastAction
    } Action;

    typedef enum _State {
      Initialization,
      WelcomeScreen,
      Playing,
      PlayerKilled,
      CountdownBeforeLevelStarts,
      PauseAfterLevelCompleted,
      LevelCompleted,
      Pausing,
      CreditsScreen,
      OptionsScreen
    } State;


  public:
    // friend class DestructionListener;

    static const int32 MaxContactPoints = 2048;
    static const int DefaultWindowWidth = 640;
    static const int DefaultWindowHeight = 400;
    static const int ColorDepth = 32;
    static const int DefaultLives = 3;
    static const int NewLiveAfterSoManyPoints = 100000;
    static const float ShotSpeed;

    Game(void);
    ~Game();
    void enterLoop(void);

    // SFML
    sf::RenderWindow mWindow;
    sf::View mDefaultView;
    sf::View mPlayView;
    sf::Font mDecorationFont;
    sf::Font mFixedFont;
    sf::Clock mClock;
    sf::Clock mWallClock;

    // Box2D
    static const int32 VelocityIterations = 10;
    static const int32 PositionIterations = 10;

    b2World* mWorld;
    ContactPoint mPoints[MaxContactPoints];
    DestructionListener mDestructionListener;
    int mPointCount;

    // b2ContactListener interface
    virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
    virtual void BeginContact(b2Contact *contact);
    virtual void EndContact(b2Contact *contact);
    virtual void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);

    // game logic
    std::vector<sf::Keyboard::Key> mKeyMapping;
    Level mLevel;
    State mState;
    int mTotalScore;
    int mScore;
    int  mLives;

    void setState(State state);
    void clearWindow(void);
    void drawWorld(const sf::View &view);
    void restart(void);
    void loadLevel(void);
    void buildLevel(void);
    void onPlaying(void);
    void onWelcomeScreen(void);
    void update(float elapsedSeconds);
    void handleEvents(void);

  };

}

#endif // __GAME_H_
