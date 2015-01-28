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



// #define PARTICLES_WITH_SPRITES


namespace Breakout {

  class Game;

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
      NewBall,
      SpecialAction,
      BackAction,
      Restart,
      ExplosionTest,
      ContinueAction,
      LastAction
    } Action;

    typedef enum _State {
      Initialization,
      WelcomeScreen,
      Playing,
      PlayerKilled,
      PauseAfterLevelCompleted,
      LevelCompleted,
      Pausing,
      CreditsScreen,
      OptionsScreen,
      PlayerWon,
      GameOver
    } State;


  public:
    static const int32 MaxContactPoints = 512;
    static const int DefaultWindowWidth = 40*16;
    static const int DefaultWindowHeight = 25*16;
    static const int ColorDepth = 32;
    static const int DefaultLives = 3;
    static const int NewLiveAfterSoManyPointsDefault;
    static const int NewLiveAfterSoManyPoints[];
    static const float Scale;
    static const float InvScale;


    Game(void);
    ~Game();
    void enterLoop(void);
    void addBody(Body *body);

    inline int tileWidth(void) const
    {
      return mLevel.tileWidth();
    }

    inline int tileHeight(void) const
    {
      return mLevel.tileHeight();
    }

    inline b2World *world(void)
    {
      return mWorld;
    }

    inline const Level *level(void) const
    {
      return &mLevel;
    }

    inline Ground *ground(void) const
    {
      return mGround;
    }

  public: // slots
    void onBodyKilled(Body *body);

  private:
    int mGLVersionMajor;
    int mGLVersionMinor;
    const GLubyte *mGLShadingLanguageVersion;

    // SFML
    sf::RenderWindow mWindow;
    sf::View mDefaultView;
    sf::Font mFixedFont;
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;
    sf::Shader mTitleShader;
    sf::Texture mTitleTexture;
    sf::Sprite mTitleSprite;
    sf::Texture mLogoTexture;
    sf::Sprite mLogoSprite;
    sf::Clock mClock;
    sf::Clock mWallClock;
    sf::Clock mScoreClock;
    sf::Clock mBlamClock;
    sf::Clock mCountdownBeforeLevelStarts;
    sf::Clock mPauseAfterLevelCompleted;
    sf::Text mLevelCompletedMsg;
    sf::Text mGameOverMsg;
    sf::Text mPlayerWonMsg;
    sf::Text mScoreMsg;
    sf::Text mTotalScoreMsg;
    sf::Text mStatMsg;
    sf::Text mStartMsg;
    sf::Text mProgramInfoMsg;
    sf::Text mLevelMsg;
    sf::Text mHelpMsg;
    sf::SoundBuffer mStartupBuffer;
    sf::Sound mStartupSound;
    sf::SoundBuffer mNewBallBuffer;
    sf::Sound mNewBallSound;
    sf::SoundBuffer mBallOutBuffer;
    sf::Sound mBallOutSound;
    sf::SoundBuffer mBlockHitBuffer;
    sf::Sound mBlockHitSound;
    sf::SoundBuffer mPadHitBuffer;
    sf::Sound mPadHitSound;
    sf::SoundBuffer mPadHitBlockBuffer;
    sf::Sound mPadHitBlockSound;
    sf::SoundBuffer mExplosionBuffer;
    sf::Sound mExplosionSound;
    sf::SoundBuffer mNewLifeBuffer; 
    sf::Sound mNewLifeSound;
    sf::SoundBuffer mLevelCompleteBuffer; 
    sf::Sound mLevelCompleteSound;
    //sf::Music mWelcomeMusic;
    //sf::Music mBackgroundMusic;
    //sf::Music mLevelCompleteMusic;
    //sf::Music mPlayerKilledMusic;
    //sf::Music mGameOverMusic;

    std::vector<sf::Music*> mMusic;

    // Box2D
    static const int32 VelocityIterations = 4;
    static const int32 PositionIterations = 2;
    b2World *mWorld;
    Ground *mGround;
    ContactPoint mPoints[MaxContactPoints];
    int32 mContactPointCount;

    // b2ContactListener interface
    virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
    {
      B2_NOT_USED(contact);
      B2_NOT_USED(oldManifold);
    }
    virtual void BeginContact(b2Contact *contact)
    {
      B2_NOT_USED(contact);
    }
    virtual void EndContact(b2Contact *contact)
    {
      B2_NOT_USED(contact);
    }
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
    BodyList mBodies;
    int mBlockCount;
    int mWelcomeLevel;
    int mExtraLifeIndex;
    sf::Vector2i mMousePos;
    sf::Vector2i mLastMousePos;

    void showScore(int score, const b2Vec2 &atPos, int factor = 1);
    void addToScore(int);
    void newBall(void);
    void extraBall(void);
    void setState(State state);
    void clearWorld(void);
    void clearWindow(void);
    void drawWorld(const sf::View &view);
    void drawStartMessage(void);
    void drawPlayground(void);
    void stopAllMusic(void);
    void restart(void);
    void resize(void);
    void pause(void);
    void resume(void);
    void buildLevel(void);
    void handlePlayerInteraction(float elapsedSeconds);
    void update(float elapsedSeconds);
    void evaluateCollisions(void);
    void handleEvents(void);

    void gotoWelcomeScreen(void);
    void onWelcomeScreen(void);

    void gotoNextLevel(void);
    void onPlaying(void);

    void gotoLevelCompleted(void);
    void onLevelCompleted(void);

    void gotoGameOver(void);
    void onGameOver(void);

    void gotoPlayerWon(void);
    void onPlayerWon(void);

    void onPausing(void);
    void onCreditsScreen(void);
  };

}

#endif // __GAME_H_
