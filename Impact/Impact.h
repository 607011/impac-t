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

#include "globals.h"
#include "Settings.h"
#include "Level.h"
#include "Ball.h"
#include "Racket.h"
#include "Ground.h"

#ifndef NO_RECORDER
#include "Recorder.h"
#endif

#include <future>



namespace Impact {

  class Game;

  struct SpecialEffect {
    SpecialEffect(void)
      : clock(nullptr)
    { /* ... */ }
    SpecialEffect(const sf::Time &d, sf::Clock *clk, const sf::Texture &tex)
      : duration(d)
      , clock(clk)
      , texture(tex)
    {
      sprite.setTexture(texture);
      sprite.setOrigin(float(texture.getSize().x), float(texture.getSize().y));
    }
    SpecialEffect(const SpecialEffect &other)
      : SpecialEffect(other.duration, other.clock, other.texture)
    { /* ... */ }
    inline bool isActive(void) const
    {
      return clock->getElapsedTime() < duration;
    }
    sf::Time duration;
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Clock *clock;
  };

  struct ContactPoint {
    b2Fixture *fixtureA;
    b2Fixture *fixtureB;
    b2Vec2 normal;
    float32 normalImpulse;
  };


  struct OverlayDef {
    OverlayDef(void)
      : duration(sf::milliseconds(1000))
      , minScale(.2f)
      , maxScale(2.5f)
    { /* ... */ }
    OverlayDef(const OverlayDef &other)
      : duration(other.duration)
      , minScale(other.minScale)
      , maxScale(other.maxScale)
      , line1(other.line1)
      , line2(other.line2)
    { /* ... */
    }
    sf::Time duration;
    float minScale;
    float maxScale;
    std::string line1;
    std::string line2;
  };


  class Game : public b2ContactListener {

    typedef enum _Playmode {
      Campaign,
      SingleLevel,
      LastPlaymode
    } Playmode;

    typedef enum _Actions {
      NoAction,
      PauseAction,
      RecoverBallAction,
      LastAction
    } Action;

    typedef enum _State {
      /* !!! DO NOT FORGET TO CHANGE Game::StateNames WHEN MAKING CHANGES HERE !!! */
      NoState,
      Initialization,
      WelcomeScreen,
      CampaignScreen,
      CreditsScreen,
      OptionsScreen,
      AchievementsScreen,
      Playing,
      LevelCompleted,
      SelectLevelScreen,
      Pausing,
      PlayerWon,
      GameOver,
      LastState
    } State;

    typedef enum _Music {
      WelcomeMusic,
      LevelMusic1,
      LevelMusic2,
      LevelMusic3,
      LevelMusic4,
      LevelMusic5,
      LastMusic
    } Music;

#ifndef NDEBUG
    static const char* StateNames[State::LastState];
#endif


  public:
    static const int Scale = 16;
    static const float32 InvScale;
    static const unsigned int DefaultTilesHorizontally = 40U;
    static const unsigned int DefaultTilesVertically = 25U;
    static const b2Vec2 DefaultCenter;
    static const unsigned int DefaultPlaygroundWidth = 640U;
    static const unsigned int DefaultPlaygroundHeight = 400U;
    static const unsigned int DefaultStatsWidth = DefaultPlaygroundWidth;
    static const unsigned int DefaultStatsHeight = 80U;
    static const unsigned int DefaultWindowWidth = DefaultPlaygroundWidth;
    static const unsigned int DefaultWindowHeight = DefaultPlaygroundHeight + DefaultStatsHeight;
    static const unsigned int ColorDepth = 32U;
    static const unsigned int DefaultFramerateLimit = 0U;
    static const unsigned int DefaultLives;
    static const unsigned int NewLiveAfterSoManyPointsDefault;
    static const unsigned int NewLiveAfterSoManyPoints[];
    static const int MaxSoundFX = 8;
    static const int DefaultForceNewBallPenalty;
    static const int32 MaxContactPoints = 512;
    static const sf::Time DefaultFadeEffectDuration;
    static const sf::Time DefaultAberrationEffectDuration;
    static const sf::Time DefaultEarthquakeDuration;
    static const sf::Time DefaultOverlayDuration;
    static const sf::Time DefaultPenaltyInterval;
    static const unsigned int DefaultKillingsPerKillingSpree;
    static const unsigned int DefaultKillingSpreeBonus;
    static const sf::Time DefaultKillingSpreeInterval;
    static const float DefaultWallRestitution;

    Game(void);
    ~Game();
    void setLevelZip(const char *zipFilename);
    void loop(void);
    void addBody(Body *body);
    void initSounds(void);
    void initShaderDependants(void);
    void clearEventQueue(void);

#ifndef NO_RECORDER
    Recorder *mRec;
#endif

    inline b2World *world(void)
    {
      return mWorld;
    }

    inline const Level *level(void) const
    {
      return &mLevel;
    }

    inline const Ground *ground(void) const
    {
      return mGround;
    }

  public: // slots
    void onBodyKilled(Body *body);

  private:
    DWORD mNumProcessors;
    HANDLE mMyProcessHandle;
    ULARGE_INTEGER mLastCPU;
    ULARGE_INTEGER mLastSysCPU;
    ULARGE_INTEGER mLastUserCPU;
    void initCPULoadMonitor(void);
    float getCurrentCPULoadPercentage(void);

    int mGLVersionMajor;
    int mGLVersionMinor;
    int mGLSLVersionMajor;
    int mGLSLVersionMinor;
    std::string mGLShadingLanguageVersion;
    bool mShadersAvailable;

    // SFML
    sf::RenderWindow mWindow;
    bool mRecorderEnabled;
    sf::Clock mRecorderClock;
    sf::Clock mRecorderWallClock;
    sf::View mDefaultView;
    sf::View mPlaygroundView;
    sf::View mStatsView;
    sf::Color mStatsColor;
    sf::VertexArray mStatsViewRectangle;
    sf::RenderTexture mRenderTexture0;
    sf::RenderTexture mRenderTexture1;
    sf::Shader mMixShader;
    int mFadeEffectsActive;
    bool mFadeEffectsDarken;
    sf::Time mFadeEffectDuration;
    sf::Shader mHBlurShader;
    sf::Shader mVBlurShader;
    bool mBlurPlayground;
    sf::Shader mKeyholeShader;
    bool mVignettizePlayground;
    sf::Vector3f mHSVShift;
    sf::Shader mVignetteShader;
    sf::Font mFixedFont;
    sf::Font mTitleFont;
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;
    sf::Shader mTitleShader;
    sf::Text mWarningText;
    sf::Text mTitleText;
    sf::Texture mTitleTexture;
    sf::Sprite mTitleSprite;
    sf::Text mMenuSingleLevel;
    sf::Text mMenuLoadLevelText;
    sf::Text mMenuExitText;
    sf::Text mMenuBackText;
    sf::Text mMenuSelectLevelText;
    sf::Text mMenuCampaignText;
    sf::Text mMenuRestartCampaignText;
    sf::Text mMenuResumeCampaignText;
    sf::Text mMenuAchievementsText;
    sf::Text mMenuOptionsText;
    sf::Text mMenuCreditsText;
    sf::Text mMenuUseShadersText;
    sf::Text mMenuUseShadersForExplosionsText;
    sf::Text mMenuParticlesPerExplosionText;
    sf::Text mMenuMusicVolumeText;
    sf::Text mMenuSoundFXVolumeText;
    sf::Text mMenuFrameRateLimitText;
    sf::Text mMenuVelocityIterationsText;
    sf::Text mMenuPositionIterationsText;
    sf::Text mOptionsTitleText;
    sf::Text mCreditsTitleText;
    sf::Text mCreditsText;
    sf::Text mLevelNameText;
    sf::Text mLevelAuthorText;
    sf::Text mFPSText;
    sf::Texture mLogoTexture;
    sf::Sprite mLogoSprite;
    sf::Text mOverlayText1;
    sf::Text mOverlayText2;
    sf::Texture mOverlayTexture;
    sf::Sprite mOverlaySprite;
    sf::Shader mOverlayShader;
    sf::Time mOverlayDuration;
    sf::Clock mOverlayClock;
    std::vector<OverlayDef> mOverlayQueue;
    sf::Texture mParticleTexture;
    std::string mFadeShaderCode;
    sf::Shader mEarthquakeShader;
    float32 mEarthquakeIntensity;
    sf::Clock mEarthquakeClock;
    sf::Time mEarthquakeDuration;
    sf::Shader mAberrationShader;
    sf::Clock mAberrationClock;
    sf::Time mAberrationDuration;
    float32 mAberrationIntensity;
    sf::RenderTexture mLevelsRenderTexture;
    sf::View mLevelsRenderView;
    sf::Texture mScrollbarTexture;
    sf::Sprite mScrollbarSprite;
    sf::Vector2f mLastMousePos;
    bool mMouseButtonDown;
    sf::Time mElapsed;
    sf::Clock mClock;
    sf::Clock mWallClock;
    sf::Clock mScoreClock;
    sf::Clock mBlurClock;
    sf::Clock mFadeEffectTimer;
    sf::Clock mScaleGravityClock;
    sf::Time mScaleGravityDuration;
    bool mScaleGravityEnabled;
    sf::Clock mScaleBallDensityClock;
    sf::Time mScaleBallDensityDuration;
    bool mScaleBallDensityEnabled;
    sf::Text mLevelCompletedMsg;
    sf::Text mGameOverMsg;
    sf::Text mPlayerWonMsg;
    sf::Text mScoreMsg;
    sf::Text mCurrentScoreMsg;
    sf::Text mYourScoreMsg;
    sf::Text mTotalScoreMsg;
    sf::Text mTotalScorePointsMsg;
    sf::Text mStatMsg;
    sf::Text mStartMsg;
    sf::Text mProgramInfoMsg;
    sf::Text mLevelMsg;
    sf::SoundBuffer mStartupSound;
    sf::SoundBuffer mNewBallSound;
    sf::SoundBuffer mBallOutSound;
    sf::SoundBuffer mBlockHitSound;
    sf::SoundBuffer mPenaltySound;
    sf::SoundBuffer mRacketHitSound;
    sf::SoundBuffer mRacketHitBlockSound;
    sf::SoundBuffer mExplosionSound;
    sf::SoundBuffer mNewLifeSound;
    sf::SoundBuffer mLevelCompleteSound;
    sf::SoundBuffer mKillingSpreeSound;

    std::vector<sf::Music> mMusic;
    std::vector<int> mFPSArray;
    std::vector<int>::size_type mFPSIndex;
    int mFPS;

    // Box2D
    b2World *mWorld;
    Ground *mGround;
    ContactPoint mPoints[MaxContactPoints];
    int32 mContactPointCount;

    // b2ContactListener interface
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);

    // game logic
    std::vector<sf::Keyboard::Key> mKeyMapping;
    bool mPaused;
    State mState;
    State mLastState;
    Playmode mPlaymode;
    int mLevelScore;
    int mTotalScore;
    unsigned int mLives;
    BodyList mBodies;
    int mBlockCount;
    int mWelcomeLevel;
    int mExtraLifeIndex;
    bool mBallHasBeenLost;
    Ball *mBall;
    Racket *mRacket;
    Level mLevel;
    Timer mLevelTimer;
    sf::Clock mStatsClock;
    sf::Clock mPenaltyClock;
    std::vector<sf::Time> mLastKillings;
    int mLastKillingsIndex;
    std::vector<SpecialEffect> mSpecialEffects;

    std::string mLevelZipFilename;
    int mDisplayCount;

    std::vector<Level> mLevels;
    std::mutex mEnumerateMutex;
    bool mQuitEnumeration;
    void enumerateAllLevels(void);
    std::packaged_task<bool()> mEnumerateTask;
    std::future<bool> mEnumerateFuture;
    std::vector<sf::Sound> mSoundFX;
    std::vector<sf::Sound>::size_type mSoundIndex;
    void setSoundFXVolume(float volume);
    void playSound(const sf::SoundBuffer &buffer, const b2Vec2 &pos = DefaultCenter);
    void setMusicVolume(float volume);
    void playMusic(Music music, bool loop = true);
    int calcPenalty(void) const;
    int deductPenalty(int score) const;
    void createStatsViewRectangle(void);
    void addSpecialEffect(const SpecialEffect &);
    void createMainWindow(void);
    void showScore(int score, const b2Vec2 &atPos, int factor = 1);
    void addToScore(int);
    void newBall(const b2Vec2 &pos = b2Vec2_zero);
	  sf::Vector2f getCursorPosition(void) const;
    void setCursorOnRacket(void);
    void extraBall(void);
    void setState(State state);
    void clearWorld(void);
    void clearWindow(void);
    void updateStats(void);
    void drawWorld(const sf::View &view);
    void drawStartMessage(void);
    void drawPlayground(void);
    void resumeAllMusic(void);
    void stopAllMusic(void);
    void pauseAllMusic(void);
    void restart(void);
    void resize(void);
    void pause(void);
    void resume(void);
    void buildLevel(void);
    void update(void);
    void evaluateCollisions(void);
    void startOverlay(const OverlayDef &);
    void startBlurEffect(void);
    void stopBlurEffect(void);
    void startEarthquake(float32 intensity, const sf::Time &duration);
    void startFadeEffect(bool darken = false, const sf::Time &duration = DefaultFadeEffectDuration);
    void startAberrationEffect(float32 gravityScale, const sf::Time &duration = DefaultAberrationEffectDuration, const sf::Vector2f &pos = sf::Vector2f(.5f, .5f));
    void setKillingsPerKillingSpree(int);
    void executeCopy(sf::RenderTexture &out, sf::RenderTexture &in);
    void executeAberration(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack);
    void executeBlur(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack);
    void executeEarthquake(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack);
    void executeKeyhole(sf::RenderTexture &out, sf::RenderTexture &in, const b2Vec2 &center, bool copyBack);
    void executeVignette(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack);
    void resetKillingSpree(void);

    void gotoWelcomeScreen(void);
    void onWelcomeScreen(void);

    void gotoCurrentLevel(void);

    void gotoNextLevel(void);
    void onPlaying(void);

    void gotoLevelCompleted(void);
    void onLevelCompleted(void);

    void gotoGameOver(void);
    void onGameOver(void);

    void gotoPlayerWon(void);
    void onPlayerWon(void);

    void gotoAchievementsScreen(void);
    void onAchievementsScreen(void);

    void gotoCreditsScreen(void);
    void onCreditsScreen(void);
    
    void gotoOptionsScreen(void);
    void onOptionsScreen(void);

    void gotoSelectLevelScreen(void);
    void onSelectLevelScreen(void);

    void gotoCampaignScreen(void);
    void onCampaignScreen(void);

    void gotoPausing(void);
    void onPausing(void);

    void openLevelZip(void);
    void loadLevelFromZip(const std::string &zipFilename);
  };

}

#endif // __GAME_H_
