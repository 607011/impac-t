// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.


#ifndef __GAME_H_
#define __GAME_H_

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>

#include "BodyBall.h"
#include "BodyPad.h"
#include "BodyGround.h"


namespace Breakout {

  class Game;

  //class DestructionListener : public b2DestructionListener {
  //public:
  //  void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
  //  void SayGoodbye(b2Joint* joint) { B2_NOT_USED(joint); }
  //  Game *test;
  //};


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
      KickLeft,
      KickRight,
      SpecialAction,
      BackAction,
      Restart,
      ExplosionTest,
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
    static const int32 MaxContactPoints = 2048;
    static const int DefaultWindowWidth = 640;
    static const int DefaultWindowHeight = 400;
    static const int ColorDepth = 32;
    static const int DefaultLives = 3;
    static const int NewLiveAfterSoManyPoints = 2500;
    static const float ShotSpeed;
    static const float Scale;

    Game(void);
    ~Game();
    void enterLoop(void);
    void addBody(Body *body);
    void onBodyKilled(Body *body);
    int tileWidth(void) const;
    int tileHeight(void) const;
    b2World *world(void);
    const Level *level(void) const;
    Ground *ground(void) const;

  private:
    // SFML
    sf::RenderWindow mWindow;
    sf::View mDefaultView;
    sf::View mPlayView;
    sf::Font mFixedFont;
    sf::Clock mClock;
    sf::Clock mWallClock;
    sf::Text mScoreMsg;
    sf::SoundBuffer mNewBallBuffer;
    sf::Sound mNewBallSound;
    sf::SoundBuffer mBallOutBuffer;
    sf::Sound mBallOutSound;
    sf::SoundBuffer mBlockHitBuffer;
    sf::Sound mBlockHitSound;
    sf::SoundBuffer mPadHitBuffer;
    sf::Sound mPadHitSound;
    sf::SoundBuffer mExplosionBuffer;
    sf::Sound mExplosionSound;

    // Box2D
    static const int32 VelocityIterations = 4;
    static const int32 PositionIterations = 2;
    b2World *mWorld;
    Ground *mGround;
    ContactPoint mPoints[MaxContactPoints];
    //DestructionListener mDestructionListener;
    int32 mPointCount;

    // b2ContactListener interface
    virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
    virtual void BeginContact(b2Contact *contact);
    virtual void EndContact(b2Contact *contact);
    virtual void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);

    // level data
    Ball *mBall;
    Pad *mPad;
    Level mLevel;

    // game logic
    std::vector<sf::Keyboard::Key> mKeyMapping;
    State mState;
    int mTotalScore;
    int mScore;
    int mLives;
    bool mPaused;
    bool mRestartRequested;
    BodyList mBodies;
    unsigned int mCurrentBodyId;

    void showScore(int score, const b2Vec2 &atPos, int factor = 1);
    void addToScore(int);
    void gameOver(void);
    void newBall(void);
    void setState(State state);
    void clearWorld(void);
    void clearWindow(void);
    void drawWorld(const sf::View &view);
    void restart(void);
    void resize(void);
    void pause(void);
    void resume(void);
    void buildLevel(void);
    void onPlaying(void);
    void onWelcomeScreen(void);
    void update(float elapsedSeconds);
    void evaluateCollisions(void);
    void handleEvents(void);
    void handlePlayerInteraction(void);

  };

}

#endif // __GAME_H_
