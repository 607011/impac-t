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

#define IMPACT_VERSION "1.0.0-BETA4"
#define PARTICLES_WITH_SPRITES

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>

#include "BodyBall.h"
#include "BodyRacket.h"
#include "BodyGround.h"

#ifdef BALL_TRACES
#include "BodyBallTrace.h"
#endif

namespace Impact {
  
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
      LevelCompleted,
      Pausing,
      OptionsScreen,
      PlayerWon,
      GameOver
    } State;


  public:
    static const int Scale;
    static const float32 InvScale;
    static const int DefaultWindowWidth;
    static const int DefaultWindowHeight;
    static const int ColorDepth;
    static const int DefaultLives;
    static const int DefaultPenalty;
    static const int NewLiveAfterSoManyPointsDefault;
    static const int NewLiveAfterSoManyPoints[];
    static const int32 MaxContactPoints = 512;
    static const sf::Time DefaultFadeEffectDuration;
    static const int DefaultKillingsPerKillingSpree = 5;
    static const int DefaultKillingSpreeBonus = 1000;
    static const sf::Time DefaultKillingSpreeInterval;

    Game(void);
    ~Game();
    void enterLoop(void);
    void addBody(Body *body);

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

    inline void setMouseModeEnabled(bool enabled)
    {
      mMouseModeEnabled = enabled;
      mWindow.setVerticalSyncEnabled(enabled);
    }

    inline bool mouseModeEnabled(void) const
    {
      return mMouseModeEnabled;
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
    sf::RenderTexture mRenderTexture;
    sf::Shader mPostFX;
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
    sf::Clock mFadeEffectTimer;
    int mFadeEffectsActive;
    bool mFadeEffectsDarken;
    sf::Time mFadeEffectDuration;
    sf::Text mLevelCompletedMsg;
    sf::Text mGameOverMsg;
    sf::Text mPlayerWonMsg;
    sf::Text mScoreMsg;
    sf::Text mYourScoreMsg;
    sf::Text mTotalScoreMsg;
    sf::Text mTotalScorePointsMsg;
    sf::Text mStatMsg;
    sf::Text mStartMsg;
    sf::Text mProgramInfoMsg;
    sf::Text mLevelMsg;
    sf::SoundBuffer mStartupBuffer;
    sf::Sound mStartupSound;
    sf::SoundBuffer mNewBallBuffer;
    sf::Sound mNewBallSound;
    sf::SoundBuffer mBallOutBuffer;
    sf::Sound mBallOutSound;
    sf::SoundBuffer mBlockHitBuffer;
    sf::Sound mBlockHitSound;
    sf::SoundBuffer mPenaltyBuffer;
    sf::Sound mPenaltySound;
    sf::SoundBuffer mRacketHitBuffer;
    sf::Sound mRacketHitSound;
    sf::SoundBuffer mRacketHitBlockBuffer;
    sf::Sound mRacketHitBlockSound;
    sf::SoundBuffer mExplosionBuffer;
    sf::Sound mExplosionSound;
    sf::SoundBuffer mNewLifeBuffer; 
    sf::Sound mNewLifeSound;
    sf::SoundBuffer mLevelCompleteBuffer; 
    sf::Sound mLevelCompleteSound;
    sf::SoundBuffer mKillingSpreeSoundBuffer; 
    sf::Sound mKillingSpreeSound;

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

    // game logic
    std::vector<sf::Keyboard::Key> mKeyMapping;
    State mState;
    int mScore;
    int mTotalScore;
    int mLives;
    bool mPaused;
    bool mMouseModeEnabled;
    BodyList mBodies;
    int mBlockCount;
    int mWelcomeLevel;
    int mExtraLifeIndex;
    sf::Vector2i mMousePos;
    sf::Vector2i mLastMousePos;
    bool mBallHasBeenLost;
    Ball *mBall;
    Racket *mRacket;
    Level mLevel;
    LevelTimer mLevelTimer;
    b2Vec2 mNewBallPosition;
    std::vector<sf::Time> mLastKillings;
    int mLastKillingsIndex;
#ifdef BALL_TRACES
    BallTrace *mBallTrace;
#endif

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
    void startFadeEffect(bool darken = false, const sf::Time &duration = DefaultFadeEffectDuration);
    void setKillingsPerKillingSpree(int);
    void killingSpree(int bonusPoints);
    void resetKillingSpree(void);
    void checkForKillingSpree(void);

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
  };

}

#endif // __GAME_H_
