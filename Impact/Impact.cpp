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

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <zlib.h>

#include <Shlwapi.h>
#include <commdlg.h>
#include <Objbase.h>
#include <Windows.h>

namespace Impact {


  class LeftBoundary : public Body
  {
  public:
    LeftBoundary(Game *game)
      : Body(Body::BodyType::LeftBoundary, game)
    { /* ... */
    }
    BodyType type(void) const { return Body::BodyType::LeftBoundary; }
    virtual void onUpdate(float elapsedSeconds) { UNUSED(elapsedSeconds); }
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const  { UNUSED(target); UNUSED(states); };
  };

  class RightBoundary : public Body
  {
  public:
    RightBoundary(Game *game)
      : Body(Body::BodyType::RightBoundary, game)
    { /* ... */
    }
    BodyType type(void) const { return Body::BodyType::RightBoundary; }
    virtual void onUpdate(float elapsedSeconds) { UNUSED(elapsedSeconds); }
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const  { UNUSED(target); UNUSED(states); };
  };

  class TopBoundary : public Body
  {
  public:
    TopBoundary(Game *game)
      : Body(Body::BodyType::TopBoundary, game)
    { /* ... */
    }
    BodyType type(void) const { return Body::BodyType::TopBoundary; }
    virtual void onUpdate(float elapsedSeconds) { UNUSED(elapsedSeconds); }
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const  { UNUSED(target); UNUSED(states); };
  };

  class BottomBoundary : public Body
  {
  public:
    BottomBoundary(Game *game)
      : Body(Body::BodyType::BottomBoundary, game)
    { /* ... */
    }
    BodyType type(void) const { return Body::BodyType::BottomBoundary; }
    virtual void onUpdate(float elapsedSeconds) { UNUSED(elapsedSeconds); }
    virtual void onDraw(sf::RenderTarget &target, sf::RenderStates states) const  { UNUSED(target); UNUSED(states); };
  };


  const float32 Game::InvScale = 1.f / Game::Scale;
  const b2Vec2 Game::DefaultCenter = b2Vec2(.5f * Game::DefaultTilesHorizontally, .5f * Game::DefaultTilesVertically);
  const unsigned int Game::DefaultLives = 3; //MOD Leben
  const float32 DefaultGravity = 9.81f; //MOD Schwerkraft
  const sf::Time Game::DefaultKillingSpreeInterval = sf::milliseconds(2500); //MOD Multikill
  const unsigned int Game::DefaultKillingsPerKillingSpree = 5; //MOD Multikill
  const unsigned int Game::DefaultKillingSpreeBonus = 1000; //MOD Multikill
  const unsigned int Game::NewLiveAfterSoManyPoints[] = { 10000, 25000, 50000, 100000, -1 }; //MOD Extraball
  const unsigned int Game::NewLiveAfterSoManyPointsDefault = 100000; //MOD Extraball
  const int Game::DefaultForceNewBallPenalty = 500;
  const sf::Time Game::DefaultPenaltyInterval = sf::milliseconds(100); //MOD Strafe

  const sf::Time Game::DefaultFadeEffectDuration = sf::milliseconds(150);
  const sf::Time Game::DefaultAberrationEffectDuration = sf::milliseconds(250);
  const sf::Time Game::DefaultEarthquakeDuration = sf::milliseconds(10 * 1000);
  const sf::Time Game::DefaultOverlayDuration = sf::milliseconds(300);

  const float Game::DefaultWallRestitution = 1.f;

#ifndef NDEBUG
  const char* Game::StateNames[State::LastState] = {
    "NoState",
    "Initialization",
    "WelcomeScreen",
    "CampaignScreen",
    "CreditsScreen",
    "OptionsScreen",
    "AchievementsScreen",
    "SplashScreenBeforePlaying",
    "Playing",
    "LevelCompleted",
    "SelectLevelScreen",
    "Pausing",
    "PlayerWon",
    "GameOver"
  };
#endif

  Game::Game(void)
    : mWorld(nullptr)
    , mDisplayCount(0)
    , mBallHasBeenLost(false)
    , mBall(nullptr)
    , mRacket(nullptr)
    , mGround(nullptr)
    , mContactPointCount(0)
    , mLevelScore(0)
    , mLives(DefaultLives)
    , mMouseButtonDown(false)
    , mPaused(false)
    , mState(State::Initialization)
    , mLastState(State::NoState)
    , mPlaymode(Playmode::Campaign)
    , mKeyMapping(Action::LastAction)
    , mBlockCount(0)
    , mFadeEffectsActive(0)
    , mFadeEffectsDarken(false)
    , mFadeEffectDuration(DefaultFadeEffectDuration)
    , mEarthquakeIntensity(0.f)
    , mEarthquakeDuration(DefaultEarthquakeDuration)
    , mScaleGravityEnabled(false)
    , mScaleBallDensityEnabled(false)
    , mAberrationIntensity(0.f)
    , mBlurPlayground(false)
    , mOverlayDuration(DefaultOverlayDuration)
    , mLastKillingsIndex(0)
    , mMusic(Music::LastMusic)
    , mSoundFX(MaxSoundFX)
    , mSoundIndex(0)
    , mFPSArray(32, 0)
    , mFPS(0)
    , mFPSIndex(0)
    , mMyProcessHandle(0)
    , mNumProcessors(0)
    , mGLVersionMajor(0)
    , mGLVersionMinor(0)
    , mGLSLVersionMajor(0)
    , mGLSLVersionMinor(0)
    , mShadersAvailable(sf::Shader::isAvailable())
  {
    bool ok;

    glewInit();
    glGetIntegerv(GL_MAJOR_VERSION, &mGLVersionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &mGLVersionMinor);

    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    mGLShadingLanguageVersion = reinterpret_cast<const char*>(glslVersion);
    if (mShadersAvailable) {
      const boost::regex re_version("(\\d+)\\.(\\d+)");
      boost::cmatch what;
      boost::regex_search(mGLShadingLanguageVersion.c_str(), what, re_version);
      if (what.size() == 3) {
        std::string glslMajor(what[1].first, what[1].second);
        std::string glslMinor(what[2].first, what[2].second);
        std::istringstream(glslMajor) >> mGLSLVersionMajor;
        std::istringstream(glslMinor) >> mGLSLVersionMinor;
#ifndef NDEBUG
        std::cout << "GLSL version " << mGLSLVersionMajor << "." << mGLSLVersionMinor << " (" << mGLShadingLanguageVersion << ")" << std::endl;
#endif
      }
      if (mGLSLVersionMajor < 1 || (mGLSLVersionMajor == 1 && mGLSLVersionMinor < 10)) {
        mShadersAvailable = false;
        gSettings.useShaders = false;
        gSettings.useShadersForExplosions = false;
      }
    }
    else {
      gSettings.useShaders = false;
      gSettings.useShadersForExplosions = false;
    }

    warmupRNG();

    createMainWindow();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    mWindow.setActive();
    mWindow.setFramerateLimit(DefaultFramerateLimit);
    mWindow.setVerticalSyncEnabled(false);
    resize();

    initSounds();

    sf::Image icon;
    icon.loadFromFile(ImagesDir + "/app-icon.png");
    mWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ok = mFixedFont.loadFromFile(FontsDir + "/04b_03.ttf"); //MOD Font
    if (!ok)
      std::cerr << FontsDir + "/04b_03.ttf failed to load." << std::endl;

    ok = mTitleFont.loadFromFile(FontsDir + "/Dimitri.ttf"); //MOD Font
    if (!ok)
      std::cerr << FontsDir + "/Dimitri.ttf failed to load." << std::endl;

    mParticleTexture.loadFromFile(ImagesDir + "/particle.png"); //MOD Explosionspartikel

    mScrollbarTexture.loadFromFile(ImagesDir + "/white-pixel.png");
    mScrollbarSprite.setTexture(mScrollbarTexture);

    mLevelCompletedMsg.setString(tr("Level complete"));
    mLevelCompletedMsg.setFont(mFixedFont);
    mLevelCompletedMsg.setCharacterSize(64U);

    mGameOverMsg.setString(tr("Game over"));
    mGameOverMsg.setFont(mFixedFont);
    mGameOverMsg.setCharacterSize(64U);

    mPlayerWonMsg.setString(tr("You won"));
    mPlayerWonMsg.setFont(mFixedFont);
    mPlayerWonMsg.setCharacterSize(64U);

    mYourScoreMsg.setString(tr("Your score"));
    mYourScoreMsg.setFont(mFixedFont);
    mYourScoreMsg.setCharacterSize(32U);

    mStartMsg.setFont(mFixedFont);
    mStartMsg.setCharacterSize(16U);
    mStartMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mStartMsg.getLocalBounds().width, 1.4f * mDefaultView.getCenter().y);

    mStatMsg.setFont(mFixedFont);
    mStatMsg.setCharacterSize(8U);
    mStatMsg.setColor(sf::Color(255U, 255U, 63U));

    mScoreMsg.setFont(mFixedFont);
    mScoreMsg.setCharacterSize(16U);

    mCurrentScoreMsg.setFont(mFixedFont);
    mCurrentScoreMsg.setCharacterSize(16U);

    mTotalScoreMsg.setFont(mFixedFont);
    mTotalScoreMsg.setCharacterSize(64U);

    mTotalScorePointsMsg.setFont(mFixedFont);
    mTotalScorePointsMsg.setCharacterSize(64U);

    mLevelMsg.setFont(mFixedFont);
    mLevelMsg.setCharacterSize(16U);
    mLevelMsg.setPosition(4, 4);

    mLevelNameText.setFont(mFixedFont);
    mLevelNameText.setCharacterSize(8U);

    mLevelAuthorText.setFont(mFixedFont);
    mLevelAuthorText.setCharacterSize(8U);

    mFPSText.setFont(mFixedFont);
    mFPSText.setCharacterSize(8U);

    mBackgroundTexture.loadFromFile(ImagesDir + "/welcome-background.jpg");
    mBackgroundSprite.setTexture(mBackgroundTexture);
    mBackgroundSprite.setPosition(0.f, 0.f);
    mBackgroundSprite.setScale(float(mDefaultView.getSize().x) / float(mBackgroundTexture.getSize().x), float(mDefaultView.getSize().y) / float(mBackgroundTexture.getSize().y));

    mLogoTexture.loadFromFile(ImagesDir + "/ct_logo.png");
    mLogoSprite.setTexture(mLogoTexture);
    mLogoSprite.setOrigin(float(mLogoTexture.getSize().x), float(mLogoTexture.getSize().y));
    mLogoSprite.setPosition(mDefaultView.getSize().x - 8.f, mDefaultView.getSize().y - 8.f);

    mTitleText = sf::Text("Impac't", mTitleFont, 120U);
    mTitleText.setPosition(.5f * (mDefaultView.getSize().x - mTitleText.getLocalBounds().width), .11f * (mDefaultView.getSize().y - mTitleText.getLocalBounds().height));

    const float menuTop = std::floor(mDefaultView.getCenter().y - 45.5f);

    // main menu
    mMenuCampaignText = sf::Text(tr("Campaign"), mFixedFont, 32U);
    mMenuCampaignText.setPosition(.5f * (mDefaultView.getSize().x - mMenuCampaignText.getLocalBounds().width), 0 + menuTop);

    mMenuSingleLevel = sf::Text(tr("Single level"), mFixedFont, 32U);
    mMenuSingleLevel.setPosition(.5f * (mDefaultView.getSize().x - mMenuSingleLevel.getLocalBounds().width), 32 + menuTop);

    mMenuLoadLevelText = sf::Text(tr("Load level"), mFixedFont, 32U);
    mMenuLoadLevelText.setPosition(.5f * (mDefaultView.getSize().x - mMenuLoadLevelText.getLocalBounds().width), 64 + menuTop);

    mMenuAchievementsText = sf::Text(tr("Achievements"), mFixedFont, 32U);
    mMenuAchievementsText.setPosition(.5f * (mDefaultView.getSize().x - mMenuAchievementsText.getLocalBounds().width), 96 + menuTop);

    mMenuOptionsText = sf::Text(tr("Options"), mFixedFont, 32U);
    mMenuOptionsText.setPosition(.5f * (mDefaultView.getSize().x - mMenuOptionsText.getLocalBounds().width), 128 + menuTop);

    mMenuCreditsText = sf::Text(tr("Credits"), mFixedFont, 32U);
    mMenuCreditsText.setPosition(.5f * (mDefaultView.getSize().x - mMenuCreditsText.getLocalBounds().width), 160 + menuTop);

    mMenuExitText = sf::Text(tr("Exit"), mFixedFont, 32U);
    mMenuExitText.setPosition(.5f * (mDefaultView.getSize().x - mMenuExitText.getLocalBounds().width), 192 + menuTop);

    mMenuResumeCampaignText = sf::Text(tr("Resume Campaign"), mFixedFont, 32U);

    mMenuRestartCampaignText = sf::Text(tr("Restart Campaign"), mFixedFont, 32U);
    mMenuRestartCampaignText.setPosition(.5f * (mDefaultView.getSize().x - mMenuRestartCampaignText.getLocalBounds().width), 32 + menuTop);

    mMenuBackText = sf::Text(tr("Back"), mFixedFont, 32U);

    mMenuSelectLevelText = sf::Text(tr("Select level"), mFixedFont, 32U);

    mCreditsTitleText = sf::Text(tr("Credits"), mFixedFont, 32U);

    mCreditsText = sf::Text(tr("Impac't: Copyright (c) Oliver Lau, Heise Medien GmbH & Co. KG\n"
      "Music: Hartmut Gieselmann <hag@ct.de>\n"
      "Sounds: Oliver Lau <ola@ct.de> (made with sfxr)\n"
      "SFML: Copyright (c) Laurent Gomila\n"
      "Box2D: Copyright (c) Erin Catto\n"
      "boost: see http://opensource.org/licenses/bsl1.0.html\n"
      "zlib: Copyright (c) Jean-loup Gailly and Mark Adler\n"
      "GLEW: Copyright (c)  Milan Ikits, Marcelo Magallon et al.\n"
      "easings: https://github.com/jesusgollonet/ofpennereasing\n"
      "\n"), mFixedFont, 16U);

    mLevelsRenderTexture.create(600, 170);
    mLevelsRenderView = mLevelsRenderTexture.getDefaultView();

    mKeyMapping[PauseAction] = sf::Keyboard::Escape; //MOD Tasten
    mKeyMapping[RecoverBallAction] = sf::Keyboard::N; //MOD Tasten

    initShaderDependants();

    restart();
  }


  Game::~Game(void)
  {
    gSettings.save();
    clearWorld();
  }


  void Game::initSounds(void)
  {
    bool ok;

    /**/setSoundFXVolume(gSettings.soundfxVolume);/**/
    setMusicVolume(gSettings.musicVolume);

    sf::Listener::setPosition(DefaultCenter.x, DefaultCenter.y, 0.f);

    ok = mMusic[Music::WelcomeMusic].openFromFile(gSettings.musicDir + "/hag5.ogg");
    if (!ok)
      std::cerr << gSettings.musicDir + "/hag1.ogg failed to load." << std::endl;

    mMusic[Music::LevelMusic1].openFromFile(gSettings.musicDir + "/hag2.ogg");
    if (!ok)
      std::cerr << gSettings.musicDir + "/hag2.ogg failed to load." << std::endl;

    mMusic[Music::LevelMusic2].openFromFile(gSettings.musicDir + "/hag3.ogg");
    if (!ok)
      std::cerr << gSettings.musicDir + "/hag3.ogg failed to load." << std::endl;

    mMusic[Music::LevelMusic3].openFromFile(gSettings.musicDir + "/hag4.ogg");
    if (!ok)
      std::cerr << gSettings.musicDir + "/hag4.ogg failed to load." << std::endl;

    mMusic[Music::LevelMusic4].openFromFile(gSettings.musicDir + "/hag5.ogg");
    if (!ok)
      std::cerr << gSettings.musicDir + "/hag5.ogg failed to load." << std::endl;

    mMusic[Music::LevelMusic5].openFromFile(gSettings.musicDir + "/hag1.ogg");
    if (!ok)
      std::cerr << gSettings.musicDir + "/hag1.ogg failed to load." << std::endl;

    for (std::vector<sf::Sound>::iterator sound = mSoundFX.begin(); sound != mSoundFX.end(); ++sound)
      sound->setMinDistance(float(DefaultTilesHorizontally * DefaultTilesVertically));

    ok = mStartupSound.loadFromFile(gSettings.soundFXDir + "/startup.ogg");
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/startup.ogg failed to load." << std::endl;

    ok = mNewBallSound.loadFromFile(gSettings.soundFXDir + "/new-ball.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/new-ball.ogg failed to load." << std::endl;

    ok = mNewLifeSound.loadFromFile(gSettings.soundFXDir + "/new-life.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/new-ball.ogg failed to load." << std::endl;

    ok = mBallOutSound.loadFromFile(gSettings.soundFXDir + "/ball-out.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/ball-out.ogg failed to load." << std::endl;

    ok = mBlockHitSound.loadFromFile(gSettings.soundFXDir + "/block-hit.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/block-hit.ogg failed to load." << std::endl;

    ok = mPenaltySound.loadFromFile(gSettings.soundFXDir + "/penalty.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/penalty.ogg failed to load." << std::endl;

    ok = mRacketHitSound.loadFromFile(gSettings.soundFXDir + "/racket-hit.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/racket-hit.ogg failed to load." << std::endl;

    ok = mRacketHitBlockSound.loadFromFile(gSettings.soundFXDir + "/racket-hit-block.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/racket-hit-block.ogg failed to load." << std::endl;

    ok = mExplosionSound.loadFromFile(gSettings.soundFXDir + "/explosion.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/explosion.ogg failed to load." << std::endl;

    ok = mLevelCompleteSound.loadFromFile(gSettings.soundFXDir + "/level-complete.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/level-complete.ogg failed to load." << std::endl;

    ok = mKillingSpreeSound.loadFromFile(gSettings.soundFXDir + "/killing-spree.ogg"); //MOD Sound
    if (!ok)
      std::cerr << gSettings.soundFXDir + "/killing-spree.ogg failed to load." << std::endl;

  }


  void Game::initShaderDependants(void)
  {
    bool ok = false;

    const float menuTop = std::floor(mDefaultView.getCenter().y - 45.5f);

    mProgramInfoMsg.setString("Impac't v" + std::string(IMPACT_VERSION) + " (" + __TIMESTAMP__ + ")"
      + " - "
      + "Copyright (c) 2015 Oliver Lau <ola@ct.de>"
      + "\n"
      + tr("Built with") + ": SFML " + std::to_string(SFML_VERSION_MAJOR) + "." + std::to_string(SFML_VERSION_MINOR)
      + ", Box2D " + std::to_string(b2_version.major) + "." + std::to_string(b2_version.minor) + "." + std::to_string(b2_version.revision)
      + ", glew " + std::to_string(GLEW_VERSION) + "." + std::to_string(GLEW_VERSION_MAJOR) + "." + std::to_string(GLEW_VERSION_MINOR)
      + ", zlib " + zlibVersion()
      + " - " + "OpenGL " + std::to_string(mGLVersionMajor) + "." + std::to_string(mGLVersionMinor)
      + (mShadersAvailable ? ", GLSL " + mGLShadingLanguageVersion : "")
      );
    mProgramInfoMsg.setFont(mFixedFont);
    mProgramInfoMsg.setCharacterSize(8U);
    mProgramInfoMsg.setPosition(8.f, mDefaultView.getSize().y - mProgramInfoMsg.getLocalBounds().height - 8.f);

    mOptionsTitleText = sf::Text(tr("Options"), mFixedFont, 32U);
    mOptionsTitleText.setPosition(mDefaultView.getCenter().x - .5f * mOptionsTitleText.getLocalBounds().width, menuTop);
    if (mShadersAvailable) {
      mMenuUseShadersText = sf::Text(tr("Use shaders"), mFixedFont, 16U);
      mMenuUseShadersForExplosionsText = sf::Text(tr("Use shaders for explosions"), mFixedFont, 16U);
      mMenuUseShadersForExplosionsText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 96);
    }
    else {
      mMenuUseShadersText = sf::Text(tr("SHADERS ARE NOT AVAILABLE.\nPLEASE UPGRADE YOUR GRAPHICS CARD/DRIVER!"), mFixedFont, 16U);
    }
    mMenuUseShadersText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 80);

    std::string warning;
    mWarningText.setFont(mFixedFont);
    mWarningText.setCharacterSize(16U);
    if (mGLVersionMajor < 3)
      warning += tr(">> Bad OpenGL version (must be >=3.x)\n");
    if (!mShadersAvailable)
      warning += tr(">> Bad GLSL version (must be >=1.10)\n");
    if (!warning.empty())
      mWarningText.setString(warning);
    mWarningText.setPosition(8.f, 4.f);

    if (gSettings.useShaders) {
      const sf::Vector2f &windowSize = sf::Vector2f(float(mWindow.getSize().x), float(mWindow.getSize().y));
      mRenderTexture0.create(DefaultPlaygroundWidth, DefaultPlaygroundHeight);
      mRenderTexture1.create(DefaultPlaygroundWidth, DefaultPlaygroundHeight);
      sf::RenderTexture titleRenderTexture;
      titleRenderTexture.create(unsigned int(mDefaultView.getSize().x), unsigned int(mDefaultView.getSize().y));
      titleRenderTexture.draw(mTitleText);
      mTitleTexture = titleRenderTexture.getTexture();
      mTitleTexture.setSmooth(true);
      mTitleSprite.setTexture(mTitleTexture);
      ok = mAberrationShader.loadFromFile(ShadersDir + "/aberration.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/aberration.fs" << " failed to load/compile." << std::endl;
      mAberrationShader.setParameter("uCenter", sf::Vector2f(.5f, .5f));
      ok = mMixShader.loadFromFile(ShadersDir + "/mix.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/mix.fs" << " failed to load/compile." << std::endl;
      ok = mVBlurShader.loadFromFile(ShadersDir + "/vblur.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/vblur.fs" << " failed to load/compile." << std::endl;
      mVBlurShader.setParameter("uBlur", 4.f);
      mVBlurShader.setParameter("uResolution", windowSize);
      ok = mHBlurShader.loadFromFile(ShadersDir + "/hblur.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/hblur.fs" << " failed to load/compile." << std::endl;
      mHBlurShader.setParameter("uBlur", 4.f);
      mHBlurShader.setParameter("uResolution", windowSize);
      ok = mTitleShader.loadFromFile(ShadersDir + "/title.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/title.fs" << " failed to load/compile." << std::endl;
      mTitleShader.setParameter("uResolution", windowSize);
      ok = mEarthquakeShader.loadFromFile(ShadersDir + "/earthquake.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/earthquake.fs" << " failed to load/compile." << std::endl;
      ok = mOverlayShader.loadFromFile(ShadersDir + "/overlay.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/overlay.fs" << " failed to load/compile." << std::endl;
      mOverlayShader.setParameter("uResolution", windowSize);

      ////MOD Schlüsselloch
      //ok = mKeyholeShader.loadFromFile(ShadersDir + "/keyhole.fs", sf::Shader::Fragment);
      //if (!ok)
      //   std::cerr << ShadersDir + "/keyhole.fs" << " failed to load/compile." << std::endl;
      //mKeyholeShader.setParameter("uStretch", 0.5f); //MOD Stretch
      //mKeyholeShader.setParameter("uSharpness", 2.0f); //MOD Sharpness
      //mKeyholeShader.setParameter("uAspect", mDefaultView.getSize().y / mDefaultView.getSize().x);
      //mKeyholeShader.setParameter("uCenter", sf::Vector2f(.5f, .5f));

      ok = mVignetteShader.loadFromFile(ShadersDir + "/vignette.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/vignette.fs" << " failed to load/compile." << std::endl;
      mVignetteShader.setParameter("uStretch", 1.0f);
      mVignetteShader.setParameter("uHSV", sf::Vector3f(1.1f, 1.0f, 1.0f));
    }

    mMenuParticlesPerExplosionText = sf::Text(tr("Particles per explosion"), mFixedFont, 16U);
    mMenuParticlesPerExplosionText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 112);
    mMenuMusicVolumeText = sf::Text(tr("Music volume"), mFixedFont, 16U);
    mMenuMusicVolumeText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 128);
    mMenuSoundFXVolumeText = sf::Text(tr("Sound fx volume"), mFixedFont, 16U);
    mMenuSoundFXVolumeText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 144);
    mMenuFrameRateLimitText = sf::Text(tr("Frame rate limit"), mFixedFont, 16U);
    mMenuFrameRateLimitText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 160);
    mMenuVelocityIterationsText = sf::Text(tr("Velocity iterations"), mFixedFont, 16U);
    mMenuVelocityIterationsText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 176);
    mMenuPositionIterationsText = sf::Text(tr("Position iterations"), mFixedFont, 16U);
    mMenuPositionIterationsText.setPosition(20.f, -20 + mOptionsTitleText.getPosition().y + 192);
  }


  void Game::initCPULoadMonitor(void)
  {
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;
    GetSystemInfo(&sysInfo);
    mNumProcessors = sysInfo.dwNumberOfProcessors;
    GetSystemTimeAsFileTime(&ftime);
    mLastCPU.LowPart = ftime.dwLowDateTime;
    mLastCPU.HighPart = ftime.dwHighDateTime;
    mMyProcessHandle = GetCurrentProcess();
    GetProcessTimes(mMyProcessHandle, &ftime, &ftime, &fsys, &fuser);
    mLastCPU.LowPart = fsys.dwLowDateTime;
    mLastCPU.HighPart = fsys.dwHighDateTime;
    mLastCPU.LowPart = fuser.dwLowDateTime;
    mLastCPU.HighPart = fuser.dwHighDateTime;
  }


  float Game::getCurrentCPULoadPercentage(void)
  {
    FILETIME ftime, fsys, fuser, fexit;
    ULARGE_INTEGER now, sys, user;
    GetSystemTimeAsFileTime(&ftime);
    now.LowPart = ftime.dwLowDateTime;
    now.HighPart = ftime.dwHighDateTime;
    GetProcessTimes(mMyProcessHandle, &ftime, &fexit, &fsys, &fuser);
    sys.LowPart = fsys.dwLowDateTime;
    sys.HighPart = fsys.dwHighDateTime;
    user.LowPart = fuser.dwLowDateTime;
    user.HighPart = fuser.dwHighDateTime;
    const float percent = 1e2f
      * float(sys.QuadPart - mLastSysCPU.QuadPart + user.QuadPart - mLastUserCPU.QuadPart)
      / float(now.QuadPart - mLastCPU.QuadPart)
      / mNumProcessors;
    mLastCPU.QuadPart = now.QuadPart;
    mLastUserCPU.QuadPart = user.QuadPart;
    mLastSysCPU.QuadPart = sys.QuadPart;
    return percent;
  }


  void Game::createStatsViewRectangle(void)
  {
    mStatsViewRectangle = sf::VertexArray(sf::Quads, 4);
    mStatsViewRectangle[0].position = sf::Vector2f(0.f, 0.f);
    mStatsViewRectangle[0].color = mStatsColor;
    mStatsViewRectangle[1].position = sf::Vector2f(0.f, mStatsView.getSize().y);
    mStatsViewRectangle[1].color = sf::Color::Black;
    mStatsViewRectangle[2].position = sf::Vector2f(mStatsView.getSize().x, mStatsView.getSize().y);
    mStatsViewRectangle[2].color = sf::Color::Black;
    mStatsViewRectangle[3].position = sf::Vector2f(mStatsView.getSize().x, 0.f);
    mStatsViewRectangle[3].color = mStatsColor;
  }


  void Game::createMainWindow(void)
  {
    sf::ContextSettings requestedContextSettings(24U, 0U, 16U, 3U, 0U);
    requestedContextSettings.antialiasingLevel = 0;
    mWindow.create(
      sf::VideoMode(Game::DefaultWindowWidth, Game::DefaultWindowHeight, Game::ColorDepth),
      std::string("Impac't") + " v" + std::string(IMPACT_VERSION),
      sf::Style::Titlebar,
      requestedContextSettings);
#ifndef NDEBUG
    sf::ContextSettings settings = mWindow.getSettings();
    std::cout << "depth bits: " << settings.depthBits << std::endl;
    std::cout << "stencil bits: " << settings.stencilBits << std::endl;
    std::cout << "antialiasing level: " << settings.antialiasingLevel << std::endl;
    std::cout << "OpenGL version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;
#endif
  }


  void Game::stopAllMusic(void)
  {
    for (std::vector<sf::Music>::iterator m = mMusic.begin(); m != mMusic.end(); ++m)
      m->stop();
    if (mLevel.music() != nullptr)
      mLevel.music()->stop();
  }


  void Game::restart(void)
  {
    clearWorld();

    safeRenew(mWorld, new b2World(b2Vec2(0.f, DefaultGravity)));
    mWorld->SetAllowSleeping(true);
    mWorld->SetWarmStarting(true);
    mWorld->SetContinuousPhysics(false);
    mWorld->SetContactListener(this);
    mWorld->SetSubStepping(true);

    mExtraLifeIndex = 0;
    mLives = DefaultLives;
    mLevelScore = 0;
    mBallHasBeenLost = false;
    mLevel.set(0, false);

    mContactPointCount = 0;

    if (gSettings.useShaders) {
      mMixShader.setParameter("uColorMix", sf::Color(255, 255, 255, 255));
      mMixShader.setParameter("uColorAdd", sf::Color(0, 0, 0, 0));
      mMixShader.setParameter("uColorSub", sf::Color(0, 0, 0, 0));
    }

    resume();
    gotoWelcomeScreen();
    resetKillingSpree();
  }


  void Game::clearWorld(void)
  {
    safeDelete(mBall);
    if (mWorld != nullptr) {
      b2Body *node = mWorld->GetBodyList();
      while (node) {
        b2Body *body = node;
        node = node->GetNext();
        mWorld->DestroyBody(body);
      }
    }
    mBodies.clear();
  }


  void Game::resize(void)
  {
    mDefaultView.reset(sf::FloatRect(0.f, 0.f, float(DefaultWindowWidth), float(DefaultWindowHeight)));
    mDefaultView.setCenter(.5f * sf::Vector2f(float(DefaultWindowWidth), float(DefaultWindowHeight)));
    mPlaygroundView.reset(sf::FloatRect(0.f, 0.f, float(DefaultPlaygroundWidth), float(DefaultPlaygroundHeight)));
    mPlaygroundView.setCenter(.5f * sf::Vector2f(float(DefaultPlaygroundWidth), float(DefaultPlaygroundHeight)));
    mPlaygroundView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, float(DefaultPlaygroundHeight) / float(DefaultWindowHeight)));
    mStatsView.reset(sf::FloatRect(0.f, float(DefaultPlaygroundHeight), float(DefaultStatsWidth), float(DefaultStatsHeight)));
    mStatsView.setCenter(sf::Vector2f(.5f * DefaultStatsWidth, .5f * DefaultStatsHeight));
    mStatsView.setViewport(sf::FloatRect(0.f, float(DefaultWindowHeight - DefaultStatsHeight) / float(DefaultWindowHeight), 1.f, float(DefaultStatsHeight) / float(DefaultWindowHeight)));
    if (gSettings.useShaders) {
      mKeyholeShader.setParameter("uAspect", mDefaultView.getSize().y / mDefaultView.getSize().x);
    }
  }


  void Game::setState(State state)
  {
    mLastState = mState;
#ifndef NDEBUG
    std::cout << "Game::setState(" << StateNames[state] << "), lastState = " << StateNames[mLastState] << std::endl;
#endif
    mState = state;
    if (mState == State::Playing)
      mWindow.setMouseCursorVisible(false);
  }


  void Game::setLevelZip(const char *zipFilename)
  {
    mLevelZipFilename = zipFilename;
  }



  void Game::clearEventQueue(void)
  {
    sf::Event event;
    while (mWindow.pollEvent(event))
      /**/;
  }


  void Game::enterLoop(void)
  {
#ifdef CT_VERSION_INTERNAL
    if (!mLevelZipFilename.empty())
      loadLevelFromZip(mLevelZipFilename);
#endif

    while (mWindow.isOpen()) {

      switch (mState) {
      case State::Playing:
        onPlaying();
        break;

      case State::SplashScreenBeforePlaying:
        onSplashScreen();
        break;

      case State::WelcomeScreen:
        onWelcomeScreen();
        break;

      case State::LevelCompleted:
        onLevelCompleted();
        break;

      case State::Pausing:
        onPausing();
        break;

      case State::GameOver:
        onGameOver();
        break;

      case State::PlayerWon:
        onPlayerWon();
        break;

      case State::AchievementsScreen:
        onAchievementsScreen();
        break;

      case State::CreditsScreen:
        onCreditsScreen();
        break;

      case State::OptionsScreen:
        onOptionsScreen();
        break;

      case State::SelectLevelScreen:
        onSelectLevelScreen();
        break;

      case State::CampaignScreen:
        onCampaignScreen();
        break;

      default:
        break;
      }

      mWindow.display();

#ifdef CT_VERSION_INTERNAL
      if (!mLevelZipFilename.empty()) {
        if (mDisplayCount++ > 10) {
          char szPath[MAX_PATH];
          strcpy_s(szPath, MAX_PATH, mLevelZipFilename.c_str());
          PathRemoveFileSpec(szPath);
          const std::string &cwd = std::string(szPath);
          const std::string &fname = cwd + "/" + mLevel.hash();
          mWindow.capture().saveToFile(fname + ".png");
          const std::string &newZipFilename = fname + ".zip";
          MoveFile(mLevelZipFilename.c_str(), newZipFilename.c_str());
          std::stringstream metadata;
          metadata << std::endl << std::endl
            << "[[" << mLevel.hash() << ".jpg" << "]]" << std::endl << std::endl;
          if (!mLevel.name().empty())
            metadata << "**" << mLevel.name() << "**" << std::endl << std::endl;
          if (!mLevel.author().empty())
            metadata << "Autor: " << mLevel.author() << std::endl << std::endl;
          if (!mLevel.info().empty())
            metadata << mLevel.info() << std::endl << std::endl;
          if (!mLevel.credits().empty())
            metadata << "Credits: " << mLevel.credits() << std::endl << std::endl;
          if (!mLevel.copyright().empty())
            metadata << mLevel.copyright() << std::endl << std::endl;
          metadata << "[Level herunterladen](" << mLevel.hash() << ".zip)"
            << std::endl << std::endl
            << "***"
            << std::endl << std::endl;
          std::ofstream mdOut;
          mdOut.open(cwd + "/UserContributedLevels.md", std::ios_base::app);
          mdOut << metadata.str() << std::endl;
          mdOut.close();
          mWindow.close();
        }
      }
#endif
    }
  }


  inline void Game::clearWindow(void)
  {
    mWindow.clear(mLevel.backgroundColor());
  }


  void Game::loadLevelFromZip(const std::string &zipFilename)
  {
#ifndef NDEBUG
    std::cout << "loadLevelFromZip(\"" << zipFilename << "\")" << std::endl;
#endif
    mLevel.loadZip(zipFilename);
    if (mLevel.isAvailable())
      gotoCurrentLevel();
  }


  void Game::openLevelZip(void)
  {
    playSound(mRacketHitSound);
#ifndef NDEBUG
    std::cout << "openLevelZip()" << std::endl;
#endif
#if defined(WIN32)
    char szFile[MAX_PATH];
    ZeroMemory(szFile, sizeof(szFile));
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Zipped level\0*.zip\0";
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = gSettings.lastOpenDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    char szCwd[MAX_PATH];
    ZeroMemory(&szCwd, sizeof(szCwd));
    GetCurrentDirectory(MAX_PATH, szCwd);
    BOOL ok = GetOpenFileName(&ofn);
#endif

    if (ok == TRUE) {
#if defined(WIN32)
      SetCurrentDirectory(szCwd); // GetOpenFileName() changed current directory, so restore it afterwards
#endif
      std::string zipFilename = ofn.lpstrFile;
#if defined(WIN32)
      PathRemoveFileSpec(ofn.lpstrFile);
#endif
      gSettings.lastOpenDir = ofn.lpstrFile;
      loadLevelFromZip(zipFilename);
    }
  }


  void Game::gotoWelcomeScreen(void) 
  {
    clearWorld();
    stopAllMusic();
    playSound(mStartupSound);
    mStartMsg.setString(tr("Click to start"));
    setState(State::WelcomeScreen);
    mWindow.setView(mDefaultView);
    if (gSettings.useShaders) {
      mTitleShader.setParameter("uMaxT", 1.f);
    }
    mWelcomeLevel = 0;
    mWallClock.restart();
    mWindow.setMouseCursorVisible(true);
    mWindow.setFramerateLimit(DefaultFramerateLimit);
    initCPULoadMonitor();
  }


  void Game::onWelcomeScreen(void)
  {
    const sf::Time &elapsed = mClock.restart();
    const sf::Vector2f &mousePos = getCursorPosition();
    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        mWindow.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          if (mMenuSingleLevel.getGlobalBounds().contains(mousePos)) {
            gotoSelectLevelScreen();
          }
          else if (mMenuLoadLevelText.getGlobalBounds().contains(mousePos)) {
            mPlaymode = SingleLevel;
            openLevelZip();
          }
          else if (mMenuCampaignText.getGlobalBounds().contains(mousePos)) {
            gotoCampaignScreen();
          }
          //else if (mMenuAchievementsText.getGlobalBounds().contains(mousePos)) {
          //  gotoAchievementsScreen();
          //}
          else if (mMenuOptionsText.getGlobalBounds().contains(mousePos)) {
            gotoOptionsScreen();
          }
          else if (mMenuCreditsText.getGlobalBounds().contains(mousePos)) {
            gotoCreditsScreen();
          }
          else if (mMenuExitText.getGlobalBounds().contains(mousePos)) {
            mWindow.close();
          }
          return;
        }
      }
    }
    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    update(elapsed);
    drawWorld(mWindow.getDefaultView());

    const sf::Int32 t = mWallClock.getElapsedTime().asMilliseconds();

    if (gSettings.useShaders) {
      sf::RenderStates states;
      states.shader = &mTitleShader;
      mTitleShader.setParameter("uT", 1e-3f * t);
      mWindow.draw(mTitleSprite, states);
    }
    else {
      mWindow.draw(mTitleText);
    }

    if (!mWarningText.getString().isEmpty())
      mWindow.draw(mWarningText);

    if (mWelcomeLevel == 0) {
      ExplosionDef pd(this, b2Vec2(.5f * DefaultTilesHorizontally, .4f * DefaultTilesVertically));
      pd.ballCollisionEnabled = false;
      pd.count = gSettings.particlesPerExplosion;
      pd.texture = mParticleTexture;
      addBody(new Explosion(pd));
      mWelcomeLevel = 1;
    }

    if (t > 350) {
      mMenuSingleLevel.setColor(sf::Color(255, 255, 255, mMenuSingleLevel.getGlobalBounds().contains(mousePos) ? 255 : 192));
      mWindow.draw(mMenuSingleLevel);
      mMenuCampaignText.setColor(sf::Color(255, 255, 255, mMenuCampaignText.getGlobalBounds().contains(mousePos) ? 255 : 192));
      mWindow.draw(mMenuCampaignText);
      mMenuLoadLevelText.setColor(sf::Color(255, 255, 255, mMenuLoadLevelText.getGlobalBounds().contains(mousePos) ? 255 : 192));
      mWindow.draw(mMenuLoadLevelText);
      mMenuAchievementsText.setColor(sf::Color(255, 255, 255, mMenuAchievementsText.getGlobalBounds().contains(mousePos) ? 16 : 16));
      mWindow.draw(mMenuAchievementsText);
      mMenuOptionsText.setColor(sf::Color(255, 255, 255, mMenuOptionsText.getGlobalBounds().contains(mousePos) ? 255 : 192));
      mWindow.draw(mMenuOptionsText);
      mMenuCreditsText.setColor(sf::Color(255, 255, 255, mMenuCreditsText.getGlobalBounds().contains(mousePos) ? 255 : 192));
      mWindow.draw(mMenuCreditsText);
      mMenuExitText.setColor(sf::Color(255, 255, 255, mMenuExitText.getGlobalBounds().contains(mousePos) ? 255 : 192));
      mWindow.draw(mMenuExitText);

      if (mWelcomeLevel == 1) {
        playSound(mExplosionSound, Game::InvScale * b2Vec2(mStartMsg.getPosition().x, mStartMsg.getPosition().y));
        mWelcomeLevel = 2;
        ExplosionDef pd(this, Game::InvScale * b2Vec2(mStartMsg.getPosition().x, mStartMsg.getPosition().y)); //XXX
        pd.count = gSettings.particlesPerExplosion;
        pd.texture = mParticleTexture;
        addBody(new Explosion(pd));
      }
    }
    if (t > 550) {
      mWindow.draw(mLogoSprite);
      if (mWelcomeLevel == 2) {
        playSound(mExplosionSound, Game::InvScale * b2Vec2(mLogoSprite.getPosition().x, mLogoSprite.getPosition().y));
        mWelcomeLevel = 3;
        ExplosionDef pd(this, Game::InvScale * b2Vec2(mLogoSprite.getPosition().x, mLogoSprite.getPosition().y));
        pd.count = gSettings.particlesPerExplosion;
        pd.texture = mParticleTexture;
        addBody(new Explosion(pd));
      }
    }
    if (t > 670) {
      mWindow.draw(mProgramInfoMsg);
      if (mWelcomeLevel == 3) {
        playSound(mExplosionSound, Game::InvScale * b2Vec2(mProgramInfoMsg.getPosition().x, mProgramInfoMsg.getPosition().y));
        mWelcomeLevel = 4;
        ExplosionDef pd(this, Game::InvScale * b2Vec2(mProgramInfoMsg.getPosition().x, mProgramInfoMsg.getPosition().y));
        pd.texture = mParticleTexture;
        pd.count = gSettings.particlesPerExplosion;
        addBody(new Explosion(pd));
      }
    }

    if (mWelcomeLevel == 4) {
      if (mMusic.at(WelcomeMusic).getStatus() != sf::Music::Playing)
        playMusic(WelcomeMusic);
      mWelcomeLevel = 5;
      enumerateAllLevels();
    }
  }


  void Game::gotoLevelCompleted(void)
  {
    mTotalScore = deductPenalty(mLevelScore);
    playSound(mLevelCompleteSound);
    mStartMsg.setString(tr("Click to continue"));
    startBlurEffect();
    setState(State::LevelCompleted);
    if (mLevel.music() != nullptr)
      mLevel.music()->stop();
    mLevelTimer.restart();
    mWindow.setFramerateLimit(DefaultFramerateLimit);
  }


  void Game::onLevelCompleted(void)
  {
    const sf::Time &elapsed = mClock.restart();
    update(elapsed);
    drawPlayground(elapsed);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          gotoNextLevel();
        }
      }
    }

    mWindow.setView(mPlaygroundView);
    mLevelCompletedMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mLevelCompletedMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mLevelCompletedMsg);
    
    drawStartMessage();
  }


  void Game::gotoPlayerWon(void)
  {
    mStartMsg.setString(tr("Click to start over"));
    setState(State::PlayerWon);
    startBlurEffect();
    mTotalScore = deductPenalty(mLevelScore);
    if (mLevel.music() != nullptr)
      mLevel.music()->stop();
    mWindow.setFramerateLimit(DefaultFramerateLimit);
  }


  void Game::onPlayerWon(void)
  {
    const sf::Time &elapsed = mClock.restart();
    update(elapsed);
    drawPlayground(elapsed);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          restart();
        }
      }
    }

    mWindow.setView(mPlaygroundView);

    mPlayerWonMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mPlayerWonMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mPlayerWonMsg);

    mYourScoreMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mYourScoreMsg.getLocalBounds().width, mPlaygroundView.getCenter().y);
    mWindow.draw(mYourScoreMsg);

    mTotalScoreMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mTotalScoreMsg.getLocalBounds().width, mPlaygroundView.getCenter().y - mTotalScoreMsg.getLocalBounds().height - 32);
    mWindow.draw(mTotalScoreMsg);

    mTotalScorePointsMsg.setString(std::to_string(mTotalScore));
    mTotalScorePointsMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mTotalScorePointsMsg.getLocalBounds().width, mPlaygroundView.getCenter().y - mTotalScorePointsMsg.getLocalBounds().height + 64);
    mWindow.draw(mTotalScorePointsMsg);

    drawStartMessage();
  }


  void Game::gotoGameOver(void)
  {
    mStartMsg.setString(tr("Click to continue"));
    setState(State::GameOver);
    startBlurEffect();
    if (gSettings.useShaders) {
      mMixShader.setParameter("uColorMix", sf::Color(255, 255, 255, 220));
    }
    mTotalScore = deductPenalty(mLevelScore);
    mWindow.setFramerateLimit(DefaultFramerateLimit);
  }


  void Game::onGameOver(void)
  {
    const sf::Time &elapsed = mClock.restart();
    update(elapsed);
    drawPlayground(elapsed);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          restart();
        }
      }
    }

    mWindow.setView(mPlaygroundView);
    mGameOverMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mGameOverMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mGameOverMsg);

    mYourScoreMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mYourScoreMsg.getLocalBounds().width, mPlaygroundView.getCenter().y);
    mWindow.draw(mYourScoreMsg);

    mTotalScorePointsMsg.setString(std::to_string(mTotalScore));
    mTotalScorePointsMsg.setPosition(mPlaygroundView.getCenter().x - .5f * mTotalScorePointsMsg.getLocalBounds().width, mPlaygroundView.getCenter().y - mTotalScorePointsMsg.getLocalBounds().height + 64);
    mWindow.draw(mTotalScorePointsMsg);

    drawStartMessage();
  }


  void Game::gotoPausing(void)
  {
    pause();
    mWindow.setFramerateLimit(DefaultFramerateLimit);
  }


  void Game::onPausing(void)
  {
    const sf::Time &elapsed = mClock.restart();
    drawPlayground(elapsed);

    mWindow.setView(mPlaygroundView);
    sf::Text pausingText(tr(">>> Pausing <<<"), mFixedFont, 64U);
    pausingText.setPosition(mPlaygroundView.getCenter().x - .5f * pausingText.getLocalBounds().width, -20 + mPlaygroundView.getCenter().y - pausingText.getLocalBounds().height);
    mWindow.draw(pausingText);

    const sf::Vector2f &mousePos = getCursorPosition();

    sf::Text resumeText(tr("Resume playing"), mFixedFont, 32U);
    resumeText.setPosition(mPlaygroundView.getCenter().x - .5f * resumeText.getLocalBounds().width, 32 + mPlaygroundView.getCenter().y);
    resumeText.setColor(sf::Color(255, 255, 255, resumeText.getGlobalBounds().contains(mousePos) ? 255 : 192));
    mWindow.draw(resumeText);

    sf::Text mainMenuText(tr("Go to main menu"), mFixedFont, 32U);
    mainMenuText.setPosition(mPlaygroundView.getCenter().x - .5f * mainMenuText.getLocalBounds().width, 64 + mPlaygroundView.getCenter().y);
    mainMenuText.setColor(sf::Color(255, 255, 255, mainMenuText.getGlobalBounds().contains(mousePos) ? 255 : 192));
    mWindow.draw(mainMenuText);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        mWindow.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          if (resumeText.getGlobalBounds().contains(mousePos)) {
            resume();
          }
          else if (mainMenuText.getGlobalBounds().contains(mousePos)) {
            playSound(mBlockHitSound);
            gotoWelcomeScreen();
          }
          return;
        }
      }
    }
  }


  void Game::gotoSplashScreen(void)
  {
    setState(State::SplashScreenBeforePlaying);
    mStartMsg.setString(tr("Click to continue"));
    mWindow.setFramerateLimit(DefaultFramerateLimit);
  }


  void Game::onSplashScreen(void)
  {
    const sf::Time &elapsed = mClock.restart();
    drawPlayground(elapsed);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          setState(State::Playing);
          mWallClock.restart();
          mClock.restart();
          mLevelTimer.resume();
        }
      }
    }

    drawStartMessage();
  }


  void Game::gotoCurrentLevel(void)
  {
#ifndef NDEBUG
    std::cout << "Game::gotoCurrentLevel(), level = " << mLevel.num() << std::endl;
#endif
    stopAllMusic();
    clearWorld();
    mBallHasBeenLost = false;
    mWindow.setMouseCursorVisible(false);
    if (gSettings.useShaders) {
      mMixShader.setParameter("uColorMix", sf::Color(255, 255, 255, 255));
    }
    mScaleGravityEnabled = false;
    mScaleBallDensityEnabled = false;
    if (mLevel.isAvailable()) {
      if (mPlaymode == Campaign)
        gSettings.lastCampaignLevel = mLevel.num();
      buildLevel();
      stopBlurEffect();
      mFadeEffectsActive = 0;
      mEarthquakeDuration = sf::Time::Zero;
      mEarthquakeIntensity = 0.f;
      mAberrationDuration = sf::Time::Zero;
      mAberrationIntensity = 0.f;
      mClock.restart();
      if (mLevel.music() != nullptr) {
        mLevel.music()->play();
        mLevel.music()->setVolume(gSettings.musicVolume);
      }
      else {
        playMusic(Game::Music(LevelMusic1 + std::rand() % (LevelMusic5 - LevelMusic1)));
      }
      setState(State::Playing);
      mLevelTimer.restart();
      mStatsClock.restart();
      mPenaltyClock.restart();
      mLevelScore = 0;
      mWindow.setFramerateLimit(gSettings.framerateLimit);
      //MOD KeyholeEnable
    }
    else {
      gotoPlayerWon();
    }
    clearEventQueue();
  }


  void Game::gotoNextLevel(void)
  {
#ifndef NDEBUG
    std::cout << "Game::gotoNextLevel()" << std::endl;
#endif
    if (mPlaymode == Campaign)
      mLevel.gotoNext();
    gotoCurrentLevel();
  }


  void Game::onPlaying(void)
  {
    const sf::Time &elapsed = mClock.restart();

    sf::Vector2i mousePos = sf::Mouse::getPosition(mWindow);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      switch (event.type)
      {
      case sf::Event::Closed:
        mWindow.close();
        break;
      case sf::Event::LostFocus:
        gotoPausing();
        break;
      case sf::Event::GainedFocus:
        resume();
        break;
      case sf::Event::MouseMoved:
        if (mScaleGravityEnabled && mScaleGravityClock.getElapsedTime() < mScaleGravityDuration) {
          const sf::Vector2f &center = sf::Vector2f(float(event.mouseMove.x) / mDefaultView.getSize().x, float(event.mouseMove.y) / mDefaultView.getSize().y);
          mAberrationShader.setParameter("uCenter", center);
        }
        break;
      case sf::Event::MouseButtonPressed:
        if (mBall == nullptr)
          newBall();
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == mKeyMapping[PauseAction]) {
          if (!mPaused)
            gotoPausing();
          else
            resume();
        }
        else if (event.key.code == mKeyMapping[RecoverBallAction] || event.key.code == sf::Keyboard::Space) {
          if (mBall != nullptr) {
            const b2Vec2 &padPos = mRacket->position();
            mBall->setPosition(padPos.x, padPos.y - 3.5f);
            showScore(-DefaultForceNewBallPenalty, mBall->position());
          }
          else {
            newBall();
          }
        }
        break;
      }
    }

    if (mBall != nullptr) { // check if ball has been kicked out of the screen
      const float ballX = mBall->position().x;
      const float ballY = mBall->position().y;
      if (0 > ballX || ballX > float(mLevel.width()) || 0 > ballY) {
        mBall->kill();
      }
      else if (ballY > mLevel.height()) {
        mBall->lethalHit();
        mBall->kill();
      }
    }

    if (mRacket != nullptr) {
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        mRacket->kickLeft();
      }
      else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        mRacket->kickRight();
      }
      else {
        mRacket->stopKick();
      }

      // check if pad has been kicked out of the screen
      const float racketX = mRacket->position().x;
      const float racketY = mRacket->position().y;
      if (racketY > mLevel.height())
        mRacket->setPosition(racketX, mLevel.height() - .5f);
      if (racketX < 0.f)
        mRacket->setPosition(1.5f, racketY);
      else if (racketX > mLevel.width())
        mRacket->setPosition(mLevel.width() - 1.5f, racketY);

      const b2AABB &aabb = mRacket->aabb();
      const float32 w = aabb.upperBound.x - aabb.lowerBound.x;
      const float32 h = aabb.upperBound.y - aabb.lowerBound.y;
      if (mousePos.x < 0) {
        mousePos.x = int(Scale * w);
      }
      if (mousePos.x > int(mWindow.getSize().x)) {
        mousePos.x = int(mWindow.getSize().x) - int(Scale * w);
      }
      sf::Mouse::setPosition(mousePos, mWindow);
      mRacket->moveTo(InvScale * b2Vec2(float32(mousePos.x), float32(mousePos.y)));
    }

    if (mScaleGravityEnabled && mScaleGravityClock.getElapsedTime() > mScaleGravityDuration) {
      mWorld->SetGravity(b2Vec2(0.f, mLevel.gravity()));
      mScaleGravityEnabled = false;
#ifndef NDEBUG
      std::cout << "Gravity back to normal." << std::endl;
#endif
    }

    if (mScaleBallDensityEnabled && mScaleBallDensityClock.getElapsedTime() > mScaleBallDensityDuration) {
      if (mBall && mBall->isAlive()) {
        mBall->setDensity(mBall->tileParam().density.get());
      }
      mScaleBallDensityEnabled = false;
#ifndef NDEBUG
      std::cout << "Ball density back to normal." << std::endl;
#endif
    }

    update(elapsed);
    drawPlayground(elapsed);
  }


  void Game::gotoAchievementsScreen(void)
  {
    mWindow.setFramerateLimit(DefaultFramerateLimit);
    // TODO: implement gotoAchievementsScreen()
  }


  void Game::onAchievementsScreen(void)
  {
    // TODO: implement onAchievementsScreen()
  }


  void Game::gotoCreditsScreen(void)
  {
    clearWorld();
    setState(State::CreditsScreen);
    mWindow.setView(mDefaultView);
    mWindow.setMouseCursorVisible(true);
    mWindow.setFramerateLimit(DefaultFramerateLimit);
    playSound(mRacketHitSound);
    mWallClock.restart();
    mWelcomeLevel = 0;
  }


  void Game::onCreditsScreen(void)
  {
    const sf::Time &elapsed = mClock.restart();
    const sf::Vector2f &mousePos = getCursorPosition();
    const float t = mWallClock.getElapsedTime().asSeconds();

    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    if (gSettings.useShaders) {
      sf::RenderStates states;
      states.shader = &mTitleShader;
      mTitleShader.setParameter("uT", t);
      mWindow.draw(mTitleSprite, states);
    }
    else {
      mWindow.draw(mTitleText);
    }

    const float menuTop = std::floor(mDefaultView.getCenter().y - 10);

    mMenuBackText.setColor(sf::Color(255, 255, 255, mMenuBackText.getGlobalBounds().contains(mousePos) ? 255 : 192));
    mMenuBackText.setPosition(.5f * (mDefaultView.getSize().x - mMenuBackText.getLocalBounds().width), mLevelsRenderTexture.getSize().y + menuTop);
    mWindow.draw(mMenuBackText);

    mCreditsTitleText.setPosition(.5f * (mDefaultView.getSize().x - mCreditsTitleText.getLocalBounds().width), menuTop - 40);
    mWindow.draw(mCreditsTitleText);

    mCreditsText.setPosition(.5f * (mDefaultView.getSize().x - mCreditsText.getLocalBounds().width), menuTop + 10);
    mWindow.draw(mCreditsText);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        mWindow.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          if (mMenuBackText.getGlobalBounds().contains(mousePos)) {
            playSound(mBlockHitSound);
            gotoWelcomeScreen();
            return;
          }
        }
      }
    }

    if (mWelcomeLevel == 0) {
      ExplosionDef pd(this, b2Vec2(.5f * DefaultTilesHorizontally, .4f * DefaultTilesVertically));
      pd.ballCollisionEnabled = false;
      pd.count = gSettings.particlesPerExplosion;
      pd.texture = mParticleTexture;
      addBody(new Explosion(pd));
      mWelcomeLevel = 1;
    }

    update(elapsed);
    drawWorld(mWindow.getDefaultView());
  }


  void Game::gotoOptionsScreen(void)
  {
    mWelcomeLevel = 0;
    mWallClock.restart();
    playSound(mRacketHitSound);
    setState(State::OptionsScreen);
    mWindow.setFramerateLimit(gSettings.framerateLimit);
    mMusic[0].play();
  }


  void Game::onOptionsScreen(void)
  {
    const sf::Time &elapsed = mClock.restart();
    const sf::Vector2f &mousePos = getCursorPosition();
    const float t = mWallClock.getElapsedTime().asSeconds();

    mWindow.setView(mDefaultView);
    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    if (gSettings.useShaders) {
      sf::RenderStates states;
      states.shader = &mTitleShader;
      mTitleShader.setParameter("uT", t);
      mWindow.draw(mTitleSprite, states);
    }
    else {
      mWindow.draw(mTitleText);
    }

    if (mWelcomeLevel == 0) {
      ExplosionDef pd(this, b2Vec2(.5f * DefaultTilesHorizontally, .4f * DefaultTilesVertically));
      pd.ballCollisionEnabled = false;
      pd.count = gSettings.particlesPerExplosion;
      pd.texture = mParticleTexture;
      addBody(new Explosion(pd));
      mWelcomeLevel = 1;
    }

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        mWindow.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          if (mMenuBackText.getGlobalBounds().contains(mousePos)) {
            playSound(mBlockHitSound);
            gotoWelcomeScreen();
            return;
          }
          else if (mShadersAvailable && mMenuUseShadersText.getGlobalBounds().contains(mousePos)) {
            gSettings.useShaders = !gSettings.useShaders;
            if (gSettings.useShaders)
              initShaderDependants();
            createMainWindow();
            gSettings.save();
          }
          else if (mShadersAvailable && gSettings.useShaders && mMenuUseShadersForExplosionsText.getGlobalBounds().contains(mousePos)) {
            gSettings.useShadersForExplosions = !gSettings.useShadersForExplosions;
            ExplosionDef pd(this, InvScale * b2Vec2(mousePos.x, mousePos.y));
            pd.count = gSettings.particlesPerExplosion;
            pd.texture = mParticleTexture;
            addBody(new Explosion(pd));
            gSettings.save();
          }
          else if (mMenuParticlesPerExplosionText.getGlobalBounds().contains(mousePos)) {
            gSettings.particlesPerExplosion += 10U;
            if (gSettings.particlesPerExplosion > 200U)
              gSettings.particlesPerExplosion = 10U;
            ExplosionDef pd(this, InvScale * b2Vec2(mousePos.x, mousePos.y));
            pd.count = gSettings.particlesPerExplosion;
            pd.texture = mParticleTexture;
            addBody(new Explosion(pd));
            gSettings.save();
          }
          else if (mMenuMusicVolumeText.getGlobalBounds().contains(mousePos)) {
            gSettings.musicVolume += 5.f;
            if (gSettings.musicVolume > 100.f)
              gSettings.musicVolume = 0.f;
            gSettings.save();
            setMusicVolume(gSettings.musicVolume);
          }
          else if (mMenuSoundFXVolumeText.getGlobalBounds().contains(mousePos)) {
            gSettings.soundfxVolume += 5.f;
            if (gSettings.soundfxVolume > 100.f)
              gSettings.soundfxVolume = 0.f;
            gSettings.save();
            setSoundFXVolume(gSettings.soundfxVolume);
            playSound(mRacketHitBlockSound);
          }
          else if (mMenuFrameRateLimitText.getGlobalBounds().contains(mousePos)) {
            if (gSettings.framerateLimit == 0)
              gSettings.framerateLimit = 60;
            else
              gSettings.framerateLimit *= 2;
            if (gSettings.framerateLimit > 480)
              gSettings.framerateLimit = 0;
            mWindow.setFramerateLimit(gSettings.framerateLimit);
            gSettings.save();
          }
          else if (mMenuPositionIterationsText.getGlobalBounds().contains(mousePos)) {
            if (gSettings.positionIterations > 256)
              gSettings.positionIterations = 16;
            else
              gSettings.positionIterations *= 2;
            gSettings.save();
          }
          else if (mMenuVelocityIterationsText.getGlobalBounds().contains(mousePos)) {
            if (gSettings.velocityIterations > 256)
              gSettings.velocityIterations = 16;
            else
              gSettings.velocityIterations *= 2;
            gSettings.save();
          }
        }
      }
    }

    update(elapsed);
    drawWorld(mWindow.getDefaultView());

    mWindow.draw(mOptionsTitleText);

    if (mShadersAvailable) {
      mMenuUseShadersText.setColor(sf::Color(255U, 255U, 255U, mMenuUseShadersText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
      sf::Text useShadersText(gSettings.useShaders ? tr("on") : tr("off"), mFixedFont, 16U);
      useShadersText.setPosition(mDefaultView.getCenter().x + 160, mMenuUseShadersText.getPosition().y);
      mWindow.draw(useShadersText);
    }
    mWindow.draw(mMenuUseShadersText);

    mMenuParticlesPerExplosionText.setColor(sf::Color(255U, 255U, 255U, mMenuParticlesPerExplosionText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
    mWindow.draw(mMenuParticlesPerExplosionText);

    mMenuMusicVolumeText.setColor(sf::Color(255U, 255U, 255U, mMenuMusicVolumeText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
    mWindow.draw(mMenuMusicVolumeText);

    mMenuSoundFXVolumeText.setColor(sf::Color(255U, 255U, 255U, mMenuSoundFXVolumeText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
    mWindow.draw(mMenuSoundFXVolumeText);

    mMenuFrameRateLimitText.setColor(sf::Color(255U, 255U, 255U, mMenuFrameRateLimitText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
    mWindow.draw(mMenuFrameRateLimitText);

    mMenuVelocityIterationsText.setColor(sf::Color(255U, 255U, 255U, mMenuVelocityIterationsText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
    mWindow.draw(mMenuVelocityIterationsText);

    mMenuPositionIterationsText.setColor(sf::Color(255U, 255U, 255U, mMenuPositionIterationsText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
    mWindow.draw(mMenuPositionIterationsText);

    if (mShadersAvailable && gSettings.useShaders) {
      mMenuUseShadersForExplosionsText.setColor(sf::Color(255U, 255U, 255U, mMenuUseShadersForExplosionsText.getGlobalBounds().contains(mousePos) ? 255U : 192U));
      sf::Text useShadersForExplosionsText(gSettings.useShadersForExplosions ? tr("on") : tr("off"), mFixedFont, 16U);
      useShadersForExplosionsText.setPosition(mDefaultView.getCenter().x + 160, mMenuUseShadersForExplosionsText.getPosition().y);
      mWindow.draw(mMenuUseShadersForExplosionsText);
      mWindow.draw(useShadersForExplosionsText);
    }

    sf::Text particlesPerExplosionText(std::to_string(gSettings.particlesPerExplosion), mFixedFont, 16U);
    particlesPerExplosionText.setPosition(mDefaultView.getCenter().x + 160, mMenuParticlesPerExplosionText.getPosition().y);
    mWindow.draw(particlesPerExplosionText);

    sf::Text musicVolumeText(gSettings.musicVolume == 0 ? tr("off") : std::to_string(int(gSettings.musicVolume)) + "%", mFixedFont, 16U);
    musicVolumeText.setPosition(mDefaultView.getCenter().x + 160, mMenuMusicVolumeText.getPosition().y);
    mWindow.draw(musicVolumeText);

    sf::Text soundfxVolumeText(gSettings.soundfxVolume == 0 ? tr("off") : std::to_string(int(gSettings.soundfxVolume)) + "%", mFixedFont, 16U);
    soundfxVolumeText.setPosition(mDefaultView.getCenter().x + 160, mMenuSoundFXVolumeText.getPosition().y);
    mWindow.draw(soundfxVolumeText);

    sf::Text frameRateLimitText(gSettings.framerateLimit == 0 ? tr("off") : std::to_string(int(gSettings.framerateLimit)) + "fps", mFixedFont, 16U);
    frameRateLimitText.setPosition(mDefaultView.getCenter().x + 160, mMenuFrameRateLimitText.getPosition().y);
    mWindow.draw(frameRateLimitText);

    sf::Text velocityIterationsText(std::to_string(int(gSettings.velocityIterations)), mFixedFont, 16U);
    velocityIterationsText.setPosition(mDefaultView.getCenter().x + 160, mMenuVelocityIterationsText.getPosition().y);
    mWindow.draw(velocityIterationsText);

    sf::Text positionIterationsText(std::to_string(int(gSettings.positionIterations)), mFixedFont, 16U);
    positionIterationsText.setPosition(mDefaultView.getCenter().x + 160, mMenuPositionIterationsText.getPosition().y);
    mWindow.draw(positionIterationsText);

    const float menuTop = std::floor(mDefaultView.getCenter().y - 10);
    mMenuBackText.setColor(sf::Color(255, 255, 255, mMenuBackText.getGlobalBounds().contains(mousePos) ? 255 : 192));
    mMenuBackText.setPosition(.5f * (mDefaultView.getSize().x - mMenuBackText.getLocalBounds().width), mLevelsRenderTexture.getSize().y + menuTop);
    mWindow.draw(mMenuBackText);

    updateStats();
    mWindow.setView(mStatsView);
    mWindow.draw(mFPSText);
  }


  void Game::gotoSelectLevelScreen(void)
  {
    clearWorld();
    setState(State::SelectLevelScreen);
    mWindow.setView(mDefaultView);
    mWindow.setMouseCursorVisible(true);
    mWindow.setFramerateLimit(DefaultFramerateLimit);
    playSound(mRacketHitSound);
    mWallClock.restart();
    mWelcomeLevel = 0;
  }


  void Game::onSelectLevelScreen(void)
  {
    const sf::Time &elapsed = mClock.restart();
    const sf::Vector2f &mousePos = getCursorPosition();
    const float t = mWallClock.getElapsedTime().asSeconds();


    static const int marginTop = 10;
    static const int marginBottom = 10;
    static const int lineHeight = 20;
    static const float scrollSpeed = 64.f;

    const float menuTop = std::floor(mDefaultView.getCenter().y - 10);

    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    if (gSettings.useShaders) {
      sf::RenderStates states;
      states.shader = &mTitleShader;
      mTitleShader.setParameter("uT", t);
      mWindow.draw(mTitleSprite, states);
    }
    else {
      mWindow.draw(mTitleText);
    }

    sf::Sprite levelSprite;
    levelSprite.setTexture(mLevelsRenderTexture.getTexture());
    levelSprite.setScale(1.f, -1.f);
    levelSprite.setPosition(20.f, menuTop + mLevelsRenderTexture.getSize().y);

    sf::FloatRect topSection = levelSprite.getGlobalBounds();
    topSection.height *= .1f;
    topSection.width -= 10.f;
    sf::FloatRect bottomSection = levelSprite.getGlobalBounds();
    bottomSection.height *= .1f;
    bottomSection.width -= 10.f;
    bottomSection.top += .9f * levelSprite.getGlobalBounds().height;

    const float totalHeight = float(marginTop + marginBottom + lineHeight * mLevels.size());
    const float scrollAreaHeight = mLevelsRenderView.getSize().y;

    if (topSection.contains(mousePos)) {
      if (mLevelsRenderView.getCenter().y - .5f * mLevelsRenderView.getSize().y > 0.f)
        mLevelsRenderView.move(0.f, -scrollSpeed * elapsed.asSeconds());
    }
    else if (bottomSection.contains(mousePos)) {
      if (mLevelsRenderView.getCenter().y + .5f * mLevelsRenderView.getSize().y < totalHeight)
        mLevelsRenderView.move(0.f, +scrollSpeed * elapsed.asSeconds());
    }

    const float scrollTop = mLevelsRenderView.getCenter().y - .5f * mLevelsRenderView.getSize().y;
    const float scrollRatio = 1.f - (totalHeight - scrollTop - scrollAreaHeight) / (totalHeight - scrollAreaHeight);
    const float scrollbarWidth = 8.f;
    const float scrollbarLeft = mLevelsRenderView.getCenter().x + .5f * mLevelsRenderView.getSize().x - scrollbarWidth;
    const float scrollbarHeight = scrollAreaHeight * scrollAreaHeight / totalHeight;
    const float scrollbarTop = scrollTop + (scrollAreaHeight - scrollbarHeight) * scrollRatio;

    sf::FloatRect scrollbarRect(scrollbarLeft + levelSprite.getPosition().x, scrollbarTop + menuTop, scrollbarWidth, scrollbarHeight);

    mScrollbarSprite.setPosition(scrollbarLeft, scrollbarTop);
    mScrollbarSprite.setScale(scrollbarWidth, scrollbarHeight);
    mScrollbarSprite.setColor(sf::Color(255, 255, 255, scrollbarRect.contains(mousePos) ? 255 : 192));

    if (mEnumerateFuture.valid()) {
      std::future_status status = mEnumerateFuture.wait_for(std::chrono::milliseconds(1));
      if (status != std::future_status::deferred) {
        mLevelsRenderTexture.setView(mLevelsRenderView);
        mLevelsRenderTexture.clear(sf::Color(0, 0, 0, 40));
        mLevelsRenderTexture.draw(mScrollbarSprite);
        mEnumerateMutex.lock();
        // TODO: optimize performance by drawing only visible lines
        for (std::vector<Level>::size_type i = 0; i < mLevels.size(); ++i) {
          const std::string &levelName = mLevels.at(i).name();
          sf::Text levelText("Level " + std::to_string(i + 1) + ": " + (levelName.empty() ? "<unnamed>" : levelName), mFixedFont, 16U);
          const float levelTextTop = float(marginTop + lineHeight * i);
          levelText.setPosition(10.f, levelTextTop);
          sf::FloatRect levelTextRect(10.f, menuTop + levelTextTop - scrollTop, levelText.getLocalBounds().width, float(lineHeight));
          const bool mouseOver = levelTextRect.contains(mousePos);
          levelText.setColor(sf::Color(255, 255, 255, mouseOver ? 255 : 160));
          if (mouseOver && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            mLevel.set(i + 1, true);
            gotoCurrentLevel();
          }
          mLevelsRenderTexture.draw(levelText);
        }
        mEnumerateMutex.unlock();
      }
    }

    mMenuBackText.setColor(sf::Color(255, 255, 255, mMenuBackText.getGlobalBounds().contains(mousePos) ? 255 : 192));
    mMenuBackText.setPosition(.5f * (mDefaultView.getSize().x - mMenuBackText.getLocalBounds().width), mLevelsRenderTexture.getSize().y + menuTop);
    mWindow.draw(mMenuBackText);

    mMenuSelectLevelText.setPosition(.5f * (mDefaultView.getSize().x - mMenuSelectLevelText.getLocalBounds().width), menuTop - mMenuBackText.getLocalBounds().height - 30);
    mWindow.draw(mMenuSelectLevelText);

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        mWindow.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          if (mMenuBackText.getGlobalBounds().contains(mousePos)) {
            playSound(mBlockHitSound);
            gotoWelcomeScreen();
            return;
          }
          else if (scrollbarRect.contains(mousePos)) {
            mMouseButtonDown = true;
            mLastMousePos = mousePos;
          }
        }
      }
      else if (event.type == sf::Event::MouseButtonReleased) {
        mMouseButtonDown = false;
      }
      else if (event.type == sf::Event::MouseMoved) {
        if (mMouseButtonDown) {
#ifndef NDEBUG
          const sf::Vector2f &mousePos = sf::Vector2f(float(event.mouseMove.x), float(event.mouseMove.y));
          const sf::Vector2f &d = mousePos - mLastMousePos;
          mLevelsRenderView.move(0.f, d.y * (scrollAreaHeight / totalHeight));
          mLastMousePos = mousePos;
#endif
        }
      }
      else if (event.type == sf::Event::MouseWheelMoved) {
        if ((event.mouseWheel.delta < 0 && mLevelsRenderView.getCenter().y - .5f * mLevelsRenderView.getSize().y > 0.f) || (event.mouseWheel.delta > 0 && mLevelsRenderView.getCenter().y + .5f * mLevelsRenderView.getSize().y < float(marginTop + marginBottom + lineHeight * mLevels.size())))
          mLevelsRenderView.move(0.f, 62.f * (event.mouseWheel.delta));
      }
    }

    levelSprite.setTexture(mLevelsRenderTexture.getTexture());
    mWindow.draw(levelSprite);

    if (mWelcomeLevel == 0) {
      ExplosionDef pd(this, b2Vec2(.5f * DefaultTilesHorizontally, .4f * DefaultTilesVertically));
      pd.ballCollisionEnabled = false;
      pd.count = gSettings.particlesPerExplosion;
      pd.texture = mParticleTexture;
      addBody(new Explosion(pd));
      mWelcomeLevel = 1;
    }

    update(elapsed);
    drawWorld(mWindow.getDefaultView());
  }


  void Game::gotoCampaignScreen(void)
  {
    clearWorld();
    setState(State::CampaignScreen);
    mWindow.setView(mDefaultView);
    mWindow.setMouseCursorVisible(true);
    mWindow.setFramerateLimit(DefaultFramerateLimit);
    playSound(mRacketHitSound);
    mWelcomeLevel = 0;
    mWallClock.restart();
  }


  void Game::onCampaignScreen(void)
  {
    sf::Time elapsed = mClock.restart();

	const sf::Vector2f &mousePos = getCursorPosition();

    mMenuResumeCampaignText.setString(gSettings.lastCampaignLevel > 1 ? tr("Resume Campaign") : tr("Start Campaign"));

    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        mWindow.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          if (mMenuResumeCampaignText.getGlobalBounds().contains(mousePos)) {
            mPlaymode = Campaign;
            mLevel.set(gSettings.lastCampaignLevel - 1, false);
            gotoNextLevel();
          }
          else if (mMenuRestartCampaignText.getGlobalBounds().contains(mousePos) && gSettings.lastCampaignLevel > 1) {
            mPlaymode = Campaign;
            mLevelScore = 0;
            mTotalScore = 0;
            mLevel.set(0, false);
            gotoNextLevel();
          }
          else if (mMenuBackText.getGlobalBounds().contains(mousePos)) {
            playSound(mBlockHitSound);
            gotoWelcomeScreen();
          }
          return;
        }
      }
    }
    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    const float t = mWallClock.getElapsedTime().asSeconds();

    if (gSettings.useShaders) {
      sf::RenderStates states;
      states.shader = &mTitleShader;
      mTitleShader.setParameter("uT", t);
      mWindow.draw(mTitleSprite, states);
    }
    else {
      mWindow.draw(mTitleText);
    }

    const float menuTop = std::floor(mDefaultView.getCenter().y - 45.5f);

    sf::Text campaignLevelText = sf::Text(tr(">>> Campaign @ Level ") + std::to_string(gSettings.lastCampaignLevel) + " <<<", mFixedFont, 32U);
    campaignLevelText.setPosition(.5f * (mDefaultView.getSize().x - campaignLevelText.getLocalBounds().width), 0 + menuTop);
    mWindow.draw(campaignLevelText);

    mMenuResumeCampaignText.setColor(sf::Color(255, 255, 255, mMenuResumeCampaignText.getGlobalBounds().contains(mousePos) ? 255 : 192));
    mMenuResumeCampaignText.setPosition(.5f * (mDefaultView.getSize().x - mMenuResumeCampaignText.getLocalBounds().width), 64 + menuTop);
    mWindow.draw(mMenuResumeCampaignText);

    if (gSettings.lastCampaignLevel > 1) {
      mMenuRestartCampaignText.setColor(sf::Color(255, 255, 255, mMenuRestartCampaignText.getGlobalBounds().contains(mousePos) ? 255 : 192));
      mMenuRestartCampaignText.setPosition(.5f * (mDefaultView.getSize().x - mMenuRestartCampaignText.getLocalBounds().width), 96 + menuTop);
      mWindow.draw(mMenuRestartCampaignText);
    }

    mMenuBackText.setColor(sf::Color(255, 255, 255, mMenuBackText.getGlobalBounds().contains(mousePos) ? 255 : 192));
    mMenuBackText.setPosition(.5f * (mDefaultView.getSize().x - mMenuBackText.getLocalBounds().width), 224 + menuTop);
    mWindow.draw(mMenuBackText);

    if (mWelcomeLevel == 0) {
      ExplosionDef pd(this, InvScale * b2Vec2(mousePos.x, mousePos.y));
      pd.ballCollisionEnabled = false;
      pd.count = gSettings.particlesPerExplosion;
      pd.texture = mParticleTexture;
      addBody(new Explosion(pd));
      mWelcomeLevel = 1;
    }

    update(elapsed);
    drawWorld(mWindow.getDefaultView());
  }


  inline void Game::executeVignette(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack)
  {
    if (gSettings.useShaders) {
      if (mRacket != nullptr && mBall != nullptr) {
        // mVignetteShader.setParameter("uHSV", sf::Vector3f(1.f, 1.f, 1.f));
        sf::RenderStates states;
        sf::Sprite sprite(in.getTexture());
        states.shader = &mVignetteShader;
        out.draw(sprite, states);
        if (copyBack)
          executeCopy(in, out);
      }
    }
  }


  inline void Game::executeKeyhole(sf::RenderTexture &out, sf::RenderTexture &in, const b2Vec2 &center, bool copyBack)
  {
    if (gSettings.useShaders) {
      sf::RenderStates states;
      sf::Sprite sprite(in.getTexture());
      states.shader = &mKeyholeShader;
      const sf::Vector2f &pos = sf::Vector2f(center.x / DefaultTilesHorizontally, center.y / DefaultTilesVertically);
      mKeyholeShader.setParameter("uCenter", pos);
      out.draw(sprite, states);
      if (copyBack)
        executeCopy(in, out);
    }
  }


  inline void Game::executeAberration(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack)
  {
    if (gSettings.useShaders) {
      sf::RenderStates states;
      sf::Sprite sprite(in.getTexture());
      states.shader = &mAberrationShader;
      mAberrationShader.setParameter("uT", mAberrationClock.getElapsedTime().asSeconds());
      out.draw(sprite, states);
      if (copyBack)
        executeCopy(in, out);
    }
  }


  void Game::startAberrationEffect(float32 gravityScale, const sf::Time &duration, const sf::Vector2f &center)
  {
#ifndef NDEBUG
    std::cout << "startAberrationEffect(" << gravityScale << ", " << duration.asSeconds() << ")" << std::endl;
#endif
    if (!gSettings.useShaders)
      return;
    const sf::Time &elapsed = mAberrationClock.restart();
    if (mAberrationDuration > sf::Time::Zero) {
      mAberrationDuration += duration - elapsed;
    }
    else {
      mAberrationDuration += duration;
    }
    mAberrationIntensity += .02f * gravityScale;
    mAberrationShader.setParameter("uMaxT", mAberrationDuration.asSeconds());
    mAberrationShader.setParameter("uDistort", mAberrationIntensity);
    mAberrationShader.setParameter("uCenter", center);
  }


  inline void Game::executeBlur(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack)
  {
    if (gSettings.useShaders) {
      sf::RenderStates states0;
      states0.shader = &mHBlurShader;
      sf::Sprite sprite0;
      sf::RenderStates states1;
      states1.shader = &mVBlurShader;
      sf::Sprite sprite1;
      const float blur = b2Min(1.f, 8.f * mBlurClock.getElapsedTime().asSeconds());
      for (int i = 1; i < 5; ++i) {
        mVBlurShader.setParameter("uBlur", 3 * i * blur);
        mHBlurShader.setParameter("uBlur", 3 * i * blur);
        sprite1.setTexture(in.getTexture());
        out.draw(sprite1, states1);
        sprite0.setTexture(out.getTexture());
        in.draw(sprite0, states0);
      }
      executeCopy(in, out);
    }
  }


  void Game::startBlurEffect(void)
  {
    mBlurClock.restart();
    mBlurPlayground = true;
  }


  void Game::stopBlurEffect(void)
  {
    mBlurPlayground = false;
  }


  inline void Game::executeEarthquake(sf::RenderTexture &out, sf::RenderTexture &in, bool copyBack)
  {
    if (gSettings.useShaders) {
      sf::Sprite sprite(in.getTexture());
      sf::RenderStates states;
      states.shader = &mEarthquakeShader;
      const float32 maxIntensity = mEarthquakeIntensity * InvScale;
      boost::random::uniform_real_distribution<float32> randomShift(-maxIntensity, maxIntensity);
      mEarthquakeShader.setParameter("uT", mEarthquakeClock.getElapsedTime().asSeconds());
      mEarthquakeShader.setParameter("uRShift", sf::Vector2f(randomShift(gRNG), randomShift(gRNG)));
      mEarthquakeShader.setParameter("uGShift", sf::Vector2f(randomShift(gRNG), randomShift(gRNG)));
      mEarthquakeShader.setParameter("uBShift", sf::Vector2f(randomShift(gRNG), randomShift(gRNG)));
      out.draw(sprite, states);
      if (copyBack)
        executeCopy(in, out);
    }
  }


  void Game::startEarthquake(float32 intensity, const sf::Time &duration)
  {
    if (!gSettings.useShaders)
      return;
    if (mEarthquakeIntensity > 0.f) {
      mEarthquakeDuration += duration;
      mEarthquakeIntensity += intensity;
    }
    else {
      mEarthquakeDuration = duration;
      mEarthquakeIntensity = intensity;
      mEarthquakeClock.restart();
    }
    mEarthquakeShader.setParameter("uMaxT", mEarthquakeDuration.asSeconds());
    OverlayDef od;
    od.line1 = std::string("Shake ") + std::to_string(int(10 * mEarthquakeIntensity));
    od.line2 = std::string("for ") + std::to_string(mEarthquakeDuration.asMilliseconds() / 1000) + "s";
    startOverlay(od);
  }


  void Game::startOverlay(const OverlayDef &od)
  {
    mOverlayDuration = od.duration;
    mOverlayText1 = sf::Text(od.line1, mTitleFont, 80U);
    mOverlayText1.setPosition(.5f * (mDefaultView.getSize().x - mOverlayText1.getLocalBounds().width), .16f * (mDefaultView.getSize().y - mOverlayText1.getLocalBounds().height));
    mOverlayText2 = sf::Text(od.line2, mTitleFont, 80U);
    mOverlayText2.setPosition(.5f * (mDefaultView.getSize().x - mOverlayText2.getLocalBounds().width), .32f * (mDefaultView.getSize().y - mOverlayText2.getLocalBounds().height));
    if (gSettings.useShaders) {
      mOverlayShader.setParameter("uMinScale", od.minScale);
      mOverlayShader.setParameter("uMaxScale", od.maxScale);
      mOverlayShader.setParameter("uMaxT", od.duration.asSeconds());
      sf::RenderTexture overlayRenderTexture;
      overlayRenderTexture.create(unsigned int(mDefaultView.getSize().x), unsigned int(mDefaultView.getSize().y));
      overlayRenderTexture.draw(mOverlayText1);
      overlayRenderTexture.draw(mOverlayText2);
      mOverlayTexture = overlayRenderTexture.getTexture();
      mOverlayTexture.setSmooth(true);
      mOverlaySprite.setTexture(mOverlayTexture);
    }
    else {
      mOverlayText1.setColor(sf::Color(255, 255, 255, 128));
      mOverlayText2.setColor(sf::Color(255, 255, 255, 128));
    }
    mOverlayClock.restart();
  }



  inline void Game::executeCopy(sf::RenderTexture &out, sf::RenderTexture &in)
  {
	  sf::Sprite sprite(in.getTexture());
    out.draw(sprite);
  }


  void Game::drawPlayground(const sf::Time &elapsed)
  {
    mWindow.setView(mPlaygroundView);
    clearWindow();

    if (gSettings.useShaders) {
      mRenderTexture0.clear(mLevel.backgroundColor());
      mRenderTexture0.draw(mLevel.backgroundSprite());

      for (BodyList::const_iterator b = mBodies.cbegin(); b != mBodies.cend(); ++b) {
        const Body *body = *b;
        if (body->isAlive())
          mRenderTexture0.draw(*body);
      }

      //MOD Keyhole
      //if (mBall != nullptr && gSettings.useShaders) {
      //  executeKeyhole(mRenderTexture1, mRenderTexture0, mBall->position(), true);
      //}

      if (mBlurPlayground) {
        executeBlur(mRenderTexture1, mRenderTexture0, true);
      }

      if (mAberrationDuration > sf::Time::Zero) {
        if (mAberrationClock.getElapsedTime() < mAberrationDuration) {
          executeAberration(mRenderTexture1, mRenderTexture0, true);
        }
        else {
          mAberrationDuration = sf::Time::Zero;
          mAberrationIntensity = 0.f;
        }
      }

      if (mEarthquakeIntensity > 0.f && mEarthquakeClock.getElapsedTime() < mEarthquakeDuration) {
        executeEarthquake(mRenderTexture1, mRenderTexture0, true);
      }
      else {
        if (mEarthquakeClock.getElapsedTime() > mEarthquakeDuration)
          mEarthquakeIntensity = 0.f;
      }

      if (mFadeEffectsActive > 0) {
        sf::Uint8 c;
        if (mFadeEffectTimer.getElapsedTime() < mFadeEffectDuration) {
          c = sf::Uint8(Easing<float>::quadEaseInForthAndBack(mFadeEffectTimer.getElapsedTime().asSeconds(), 0.f, 255.f, mFadeEffectDuration.asSeconds()));
        }
        else {
          c = 0;
          mFadeEffectsActive = 0;
        }
        if (mFadeEffectsDarken)
          mMixShader.setParameter("uColorSub", sf::Color(c, c, c, 0));
        else
          mMixShader.setParameter("uColorAdd", sf::Color(c, c, c, 0));
      }
      else {
        mMixShader.setParameter("uColorSub", sf::Color(0, 0, 0, 0));
        mMixShader.setParameter("uColorAdd", sf::Color(0, 0, 0, 0));
      }
      sf::Sprite sprite(mRenderTexture0.getTexture());
      sf::RenderStates states;
      states.shader = &mMixShader;
      mWindow.draw(sprite, states);

    }
    else { // !gSettings.useShaders
      mWindow.clear(mLevel.backgroundColor());
      mWindow.draw(mLevel.backgroundSprite());
      for (BodyList::const_iterator b = mBodies.cbegin(); b != mBodies.cend(); ++b) {
        const Body *body = *b;
        if (body->isAlive())
          mWindow.draw(*body);
      }
    }

    if (mOverlayDuration > sf::Time::Zero) {
      if (mOverlayClock.getElapsedTime() < mOverlayDuration) {
        mWindow.setView(mDefaultView);
        if (gSettings.useShaders) {
          sf::RenderStates states;
          states.shader = &mOverlayShader;
          mOverlayShader.setParameter("uT", mOverlayClock.getElapsedTime().asSeconds());
          mWindow.draw(mOverlaySprite, states);
        }
        else {
          mWindow.draw(mOverlayText1);
        }
      }
      else {
        mOverlayDuration = sf::Time::Zero;
      }
    }

    updateStats();

    mWindow.setView(mStatsView);
    mWindow.draw(mStatsViewRectangle);
    mWindow.draw(mLevelMsg);
    mWindow.draw(mFPSText);
    mWindow.draw(mLevelNameText);
    mWindow.draw(mLevelAuthorText);

    if (mState == State::Playing) {
      mWindow.draw(mScoreMsg);
      mWindow.draw(mCurrentScoreMsg);
      for (unsigned int life = 0; life < mLives; ++life) {
        const sf::Texture &ballTexture = mLevel.texture(Ball::Name);
        sf::Sprite lifeSprite(ballTexture);
        lifeSprite.setOrigin(0.f, 0.f);
        lifeSprite.setPosition(4 + (ballTexture.getSize().x * 1.5f) * life, 26.f);
        mWindow.draw(lifeSprite);
      }
    }

    // draw special effect hints
    std::vector<std::vector<SpecialEffect>::iterator> expiredEffects;
    sf::Vector2f pos(mStatsView.getSize().x - 4, mStatsView.getSize().y - 16);
    for (std::vector<SpecialEffect>::iterator i = mSpecialEffects.begin(); i != mSpecialEffects.end(); ++i) {
      if (i->isActive()) {
        const sf::Uint8 alpha = 255U - sf::Uint8(255U * i->clock->getElapsedTime().asMilliseconds() / i->duration.asMilliseconds());
        i->sprite.setPosition(pos);
        i->sprite.setColor(sf::Color(255U, 255U, 255U, alpha));
        mWindow.draw(i->sprite);
        pos.x -= i->texture.getSize().x;
      }
      else {
        expiredEffects.push_back(i);
      }
    }
    for (std::vector<std::vector<SpecialEffect>::iterator>::const_iterator i = expiredEffects.cbegin(); i != expiredEffects.cend(); ++i) {
#ifndef NDEBUG
      std::cout << "Expired effect 0x" << std::hex << std::setfill('0') << std::setw(8) << (*i)->clock << std::endl;
#endif
      mSpecialEffects.erase(*i);
    }

  }


  void Game::updateStats(void)
  {
    if (mStatsClock.getElapsedTime() > sf::milliseconds(33)) {
      mLevelMsg.setString(tr("Level") + " " + std::to_string(mLevel.num()));
      mFPSText.setString(std::to_string(mFPS) + " fps\nCPU: " + std::to_string(int(getCurrentCPULoadPercentage())) + "%");
      mFPSText.setPosition(mStatsView.getSize().x - b2Max(mFPSText.getGlobalBounds().width - 4, 60.f), mStatsView.getSize().y - 8 - mFPSText.getGlobalBounds().height);
      if (mState == State::Playing) {
        const int penalty = calcPenalty();
        mScoreMsg.setString(std::to_string(mLevelScore) + (penalty > 0 ? " " + std::to_string(-penalty) : ""));
        mScoreMsg.setPosition(mStatsView.getSize().x - mScoreMsg.getLocalBounds().width - 4, 4);
        mCurrentScoreMsg.setString("total: " + std::to_string(b2Max(0, mTotalScore + mLevelScore - penalty)));
        mCurrentScoreMsg.setPosition(mStatsView.getSize().x - mCurrentScoreMsg.getLocalBounds().width - 4, 20);
      }
      mStatsClock.restart();
    }
  }


  void Game::drawStartMessage(void)
  {
    mStartMsg.setColor(sf::Color(255, 255, 255, 192 + sf::Uint8(63 * std::sin(14 * mWallClock.getElapsedTime().asSeconds()))));
    mStartMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mStartMsg.getLocalBounds().width, mPlaygroundView.getCenter().y + mPlaygroundView.getSize().y / 4);
    mWindow.draw(mStartMsg);
  }


  inline void Game::drawWorld(const sf::View &view)
  {
    mWindow.setView(view);
    for (BodyList::const_iterator b = mBodies.cbegin(); b != mBodies.cend(); ++b) {
      const Body *body = *b;
      if (body->isAlive()) {
        mWindow.draw(*body);
      }
    }
  }


  void Game::evaluateCollisions(void)
  {
    std::list<Body*> killedBodies;

    auto isAlive = [&killedBodies](Body *body) {
      return std::find(killedBodies.cbegin(), killedBodies.cend(), body) == killedBodies.cend();
    }; // Obscure syntax? Google "c++ lambda functions closures" ;-)

    for (int i = 0; i < mContactPointCount; ++i) {
      ContactPoint &cp = mPoints[i];
      b2Body *bodyA = cp.fixtureA->GetBody();
      b2Body *bodyB = cp.fixtureB->GetBody();
      Body *a = reinterpret_cast<Body *>(cp.fixtureA->GetUserData());
      Body *b = reinterpret_cast<Body *>(cp.fixtureB->GetUserData());
      if (a == nullptr || b == nullptr)
        continue;
      if (a->type() == Body::BodyType::Racket || b->type() == Body::BodyType::Racket) {
        if (a->type() == Body::BodyType::LeftBoundary || b->type() == Body::BodyType::LeftBoundary) {
          Racket *racket = reinterpret_cast<Racket*>(a->type() == Body::BodyType::Racket ? a : b);
          const b2AABB &aabb = racket->aabb();
          b2Vec2 pos = racket->position();
          if (cp.normal.x < 0) {
#ifndef NDEBUG
            std::cout << "racket stuck LEFT" << std::endl;
#endif
            pos.x -= aabb.lowerBound.x + InvScale * .1f;
            racket->setPosition(pos);
            setCursorOnRacket();
          }
        }
        else if (a->type() == Body::BodyType::RightBoundary || b->type() == Body::BodyType::RightBoundary) {
          Racket *racket = reinterpret_cast<Racket*>(a->type() == Body::BodyType::Racket ? a : b);
          const b2AABB &aabb = racket->aabb();
          b2Vec2 pos = racket->position();
          if (cp.normal.x > 0) {
#ifndef NDEBUG
            std::cout << "racket stuck RIGHT" << std::endl;
#endif
            pos.x -= aabb.upperBound.x + InvScale * .1f;
            racket->setPosition(pos);
            setCursorOnRacket();
          }
        }
        else if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Racket *racket = reinterpret_cast<Racket*>(a->type() == Body::BodyType::Racket ? a : b);
          const b2AABB &aabb = racket->aabb();
          b2Vec2 pos = racket->position();
          if (cp.normal.y > 0) {
#ifndef NDEBUG
            std::cout << "racket stuck to GROUND: " << cp.normal.y << std::endl;
#endif
            pos.y -= aabb.upperBound.y + InvScale * .1f;
            racket->setPosition(pos);
            setCursorOnRacket();
          }
        }
      }
      if (a->type() == Body::BodyType::Block || b->type() == Body::BodyType::Block) {
        if (a->type() == Body::BodyType::Ball || b->type() == Body::BodyType::Ball) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          if (isAlive(block)) {
            bool destroyed = block->hit(cp.normalImpulse);
            if (destroyed) {
              block->kill();
              showScore(block->getScore(), block->position());
              killedBodies.push_back(block);
            }
            else if (cp.normalImpulse > 20)
              playSound(mBlockHitSound, block->position());
          }
        }
        else if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          if (isAlive(block)) {
            block->kill();
            killedBodies.push_back(block);
          }
        }
        else if (a->type() == Body::BodyType::Racket || b->type() == Body::BodyType::Racket) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          if (block->body()->GetGravityScale() > 0.f) {
            if (isAlive(block)) {
              showScore(block->getScore(), block->position(), 2);
              block->kill();
              playSound(mRacketHitBlockSound, block->position());
              killedBodies.push_back(block);
            }
          }
          else {
            if (mPenaltyClock.getElapsedTime() > DefaultPenaltyInterval) {
              showScore(-block->getScore(), block->position());
              playSound(mPenaltySound, block->position());
              startFadeEffect();
              mPenaltyClock.restart();
            }
          }
        }
      }
      else if (a->type() == Body::BodyType::Ball || b->type() == Body::BodyType::Ball) {
        Ball *ball = reinterpret_cast<Ball*>(a->type() == Body::BodyType::Ball ? a : b);
        if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          if (isAlive(ball)) {
            ball->lethalHit();
            ball->kill();
            killedBodies.push_back(ball);
            startFadeEffect(true, sf::milliseconds(350));
          }
        }
        else if (a->type() == Body::BodyType::Racket || b->type() == Body::BodyType::Racket) {
          if (cp.normalImpulse > 20)
            playSound(mRacketHitSound, ball->position());
        }
      }
    }
  }


  inline void Game::update(const sf::Time &elapsed)
  {
    if (elapsed == sf::Time::Zero)
      return;

    float elapsedSeconds = 1e-6f * elapsed.asMicroseconds();

    BodyList remainingBodies;
    for (BodyList::iterator b = mBodies.begin(); b != mBodies.end(); ++b) {
      Body *body = *b;
      if (body != nullptr) {
        if (body->isAlive()) {
          body->update(elapsedSeconds);
          remainingBodies.push_back(body);
        }
        else {
          if (body->type() == Body::BodyType::Ball)
            mBall = nullptr;
          delete body;
        }
      }
    }
    mBodies = remainingBodies;

    mContactPointCount = 0;
    mWorld->Step(elapsedSeconds, gSettings.velocityIterations, gSettings.positionIterations);
    /* Note from the Box2D manual: You should always process the
     * contact points [collected in PostSolve()] immediately after
     * the time step; otherwise some other client code might
     * alter the physics world, invalidating the contact buffer.
     */
    if (mState == State::Playing)
      evaluateCollisions();
    mWorld->ClearForces();

    mFPSArray[mFPSIndex++] = int(1.f / elapsed.asSeconds());
    if (mFPSIndex >= mFPSArray.size())
      mFPSIndex = 0;
    mFPS = std::accumulate(mFPSArray.begin(), mFPSArray.end(), 0) / mFPSArray.size();
  }


  void Game::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
  {
    if (mContactPointCount < MaxContactPoints) {
      ContactPoint &cp = mPoints[mContactPointCount];
      cp.fixtureA = contact->GetFixtureA();
      cp.fixtureB = contact->GetFixtureB();
      Body *bodyA = reinterpret_cast<Body*>(cp.fixtureA->GetUserData());
      Body *bodyB = reinterpret_cast<Body*>(cp.fixtureB->GetUserData());
      if (bodyA != nullptr && bodyB != nullptr) {
        b2Manifold *manifold = contact->GetManifold();
        cp.normal = manifold->localNormal;
        cp.normalImpulse = impulse->normalImpulses[0];
        ++mContactPointCount;
      }
    }
  }


  void Game::startFadeEffect(bool darken, const sf::Time &duration)
  {
    mFadeEffectsDarken = darken;
    mFadeEffectDuration = duration;
    if (mFadeEffectsActive == 0)
      mFadeEffectTimer.restart();
    ++mFadeEffectsActive;
  }


  void Game::buildLevel(void)
  {
#ifndef NDEBUG
    std::cout << "Game::buildLevel()" << std::endl;
#endif

    mLastKillings = std::vector<sf::Time>(mLevel.killingsPerKillingSpree(), sf::milliseconds(INT_MIN));

    const float32 g = mLevel.gravity();
    mWorld->SetGravity(b2Vec2(0.f, g));

    const float32 W = mLevel.size().x;
    const float32 H = mLevel.size().y;

    // create level boundaries
    b2BodyDef bd;
    b2Body *boundaries = mWorld->CreateBody(&bd);
    b2EdgeShape rightShape;
    rightShape.Set(b2Vec2(W, 0), b2Vec2(W, H));
    b2FixtureDef fdRight;
    fdRight.restitution = DefaultWallRestitution; //TODO: parametrize wall restitution
    fdRight.shape = &rightShape;
    fdRight.userData = new RightBoundary(this);
    boundaries->CreateFixture(&fdRight);
    b2EdgeShape leftShape;
    leftShape.Set(b2Vec2(0, 0), b2Vec2(0, H));
    b2FixtureDef fdLeft;
    fdLeft.restitution = DefaultWallRestitution; //TODO: parametrize wall restitution
    fdLeft.shape = &leftShape;
    fdLeft.userData = new LeftBoundary(this);
    boundaries->CreateFixture(&fdLeft);
    b2EdgeShape topShape;
    topShape.Set(b2Vec2(0, g > 0.f ? 0.f : float32(mLevel.height())), b2Vec2(W, g > 0.f ? 0.f : float32(mLevel.height())));
    b2FixtureDef fdTop;
    fdTop.restitution = DefaultWallRestitution; //TODO: parametrize wall restitution
    fdTop.shape = &topShape;
    boundaries->CreateFixture(&fdTop);

    mGround = new Ground(this, W);
    mGround->setPosition(0, g < 0.f ? 0 : mLevel.height());
    addBody(mGround);


    const sf::Texture *bgTex = mLevel.backgroundSprite().getTexture();
    if (bgTex != nullptr) {
      const sf::Image &bg = bgTex->copyToImage();
      unsigned int nPixels = bg.getSize().x * bg.getSize().y;
      if (nPixels > 0) {
        const sf::Uint8 *pixels = bg.getPixelsPtr();
        const sf::Uint8 *pixelsEnd = pixels + (4 * nPixels);
        sf::Vector3i color;
        while (pixels < pixelsEnd) {
          color.x += *(pixels + 0);
          color.y += *(pixels + 1);
          color.z += *(pixels + 2);
          pixels += 4;
        }
        mStatsColor = sf::Color(sf::Uint8(color.x / nPixels), sf::Uint8(color.y / nPixels), sf::Uint8(color.z / nPixels), 255U);
      }
      else {
        mStatsColor = sf::Color::Black;
      }
    }

    createStatsViewRectangle();

    // create level elements
    mBlockCount = 0;
    for (int y = 0; y < mLevel.height(); ++y) {
      const uint32_t *mapRow = mLevel.mapDataScanLine(y);
      for (int x = 0; x < mLevel.width(); ++x) {
        const b2Vec2 &pos = b2Vec2(float32(x), float32(y));
        const uint32_t tileId = mapRow[x];
        if (tileId == 0)
          continue;
        const TileParam &tileParam = mLevel.tileParam(tileId);
        if (tileId >= mLevel.firstGID()) {
          if (tileParam.textureName == Ball::Name) {
            newBall(pos);
            mBall->setDensity(tileParam.density.get());
            mBall->setRestitution(tileParam.restitution.get());
            mBall->setFriction(tileParam.friction.get());
            mBall->setSmooth(tileParam.smooth);
            mBall->setTileParam(tileParam);
          }
          else if (tileParam.textureName == Racket::Name) {
            mRacket = new Racket(this, pos, mGround->body());
            mRacket->setSmooth(tileParam.smooth);
            // mRacket->setXAxisConstraint(mLevel.height() - .5f);
            addBody(mRacket);
          }
          else if (tileParam.fixed.get()) {
            Wall *wall = new Wall(tileId, this);
            wall->setPosition(pos);
            wall->setRestitution(tileParam.restitution.get());
            wall->setFriction(tileParam.friction.get());
            wall->setSmooth(tileParam.smooth);
            wall->setTileParam(tileParam);
            addBody(wall);
          }
          else {
            Block *block = new Block(tileId, this);
            block->setPosition(pos);
            block->setScore(tileParam.score);
            block->setGravityScale(tileParam.gravityScale);
            block->setDensity(tileParam.density.get());
            block->setRestitution(tileParam.restitution.get());
            block->setFriction(tileParam.friction.get());
            block->setSmooth(tileParam.smooth);
            block->setEnergy(tileParam.minimumKillImpulse);
            block->setMinimumHitImpulse(tileParam.minimumHitImpulse);
            block->setTileParam(tileParam);
            addBody(block);
            ++mBlockCount;
          }
        }
      }
    }

    mLevelNameText.setString(">> " + mLevel.name() + " <<");
    mLevelNameText.setPosition(4, 52);
    mLevelAuthorText.setString(mLevel.author());
    mLevelAuthorText.setPosition(4, 62);

    setCursorOnRacket();
  }


  void Game::showScore(int score, const b2Vec2 &atPos, int factor)
  {
    addToScore(score * factor);
    const std::string &text = (factor > 1 ? (std::to_string(factor) + "*") : "") + std::to_string(score);
    TextBodyDef td(this, text, mFixedFont, atPos);
    TextBody *scoreText = new TextBody(td);
    addBody(scoreText);
  }


  void Game::addToScore(int points)
  {
    int newScore = mLevelScore + points;
    if (points > 0) {
      int threshold = NewLiveAfterSoManyPoints[mExtraLifeIndex];
      if (threshold > 0 && newScore > threshold) {
        ++mExtraLifeIndex;
        extraBall();
      }
      else if ((mLevelScore % NewLiveAfterSoManyPointsDefault) > (newScore % NewLiveAfterSoManyPointsDefault)) {
        extraBall();
      }
    }
    mLevelScore = b2Max(0, newScore);
  }


  inline sf::Vector2f Game::getCursorPosition(void) const
  {
	  const sf::Vector2i &mousePos = sf::Mouse::getPosition(mWindow);
	  return sf::Vector2f(float(mousePos.x), float(mousePos.y));
  }


  void Game::setCursorOnRacket(void)
  {
    if (mRacket != nullptr) {
      const b2Vec2 &racketPos = float32(Game::Scale) * mRacket->position();
      sf::Mouse::setPosition(sf::Vector2i(int(racketPos.x), int(racketPos.y)), mWindow);
    }
  }


  void Game::extraBall(void)
  {
    ++mLives;
    playSound(mNewLifeSound);
  }


  void Game::newBall(const b2Vec2 &pos)
  {
    playSound(mNewBallSound);
    safeRenew(mBall, new Ball(this));
    if (mBallHasBeenLost) {
      const b2Vec2 &padPos = mRacket->position();
      mBall->setPosition(padPos.x, padPos.y - 1.2f * sign(mLevel.gravity()));
    }
    else {
      mBall->setPosition(pos);
    }
    addBody(mBall);
  }


  void Game::pause(void)
  {
#ifndef NDEBUG
    std::cout << "pause()" << std::endl;
#endif
    mPaused = true;
    setState(State::Pausing);
    mLevelTimer.pause();
    startBlurEffect();
    mWindow.setMouseCursorVisible(true);
    if (mLevel.music() != nullptr)
      mLevel.music()->pause();
  }


  void Game::resume(void)
  {
#ifndef NDEBUG
    std::cout << "resume()" << std::endl;
#endif
    mPaused = false;
    mLevelTimer.resume();
    stopBlurEffect();
    setCursorOnRacket();
    if (mState == State::Pausing)
      setState(State::Playing);
    if (mLevel.music() != nullptr)
      mLevel.music()->play();
  }


  inline void Game::addBody(Body *body)
  {
    mBodies.push_back(body);
  }


  void Game::resetKillingSpree(void)
  {
    for (std::vector<sf::Time>::iterator t = mLastKillings.begin(); t != mLastKillings.end(); ++t)
      *t = sf::milliseconds(INT_MIN);
    mLastKillingsIndex = 0;
  }


  void Game::addSpecialEffect(const SpecialEffect &effect)
  {
    std::vector<SpecialEffect>::iterator i;
    for (i = mSpecialEffects.begin(); i != mSpecialEffects.end(); ++i) {
      if (i->clock == effect.clock) {
        i->duration = effect.duration;
        break;
      }
    }
    if (i == mSpecialEffects.cend())
      mSpecialEffects.push_back(effect);
  }


  void Game::setSoundFXVolume(float volume)
  {
    for (std::vector<sf::Sound>::iterator sound = mSoundFX.begin(); sound != mSoundFX.end(); ++sound)
      sound->setVolume(volume);
  }


  void Game::setMusicVolume(float volume)
  {
    for (std::vector<sf::Music>::iterator m = mMusic.begin(); m != mMusic.end(); ++m)
      m->setVolume(volume);
    if (mLevel.music() != nullptr)
      mLevel.music()->setVolume(volume);
  }


  void Game::playSound(const sf::SoundBuffer &buffer, const b2Vec2 &pos)
  {
    sf::Sound &sound = mSoundFX[mSoundIndex];
    sound.setBuffer(buffer);
    sound.setPosition(pos.x, 0, 0);
    sound.play();
    if (++mSoundIndex >= mSoundFX.size())
      mSoundIndex = 0;
  }


  void Game::playMusic(Game::Music music, bool loop)
  {
    stopAllMusic();
    mMusic[music].play();
    mMusic[music].setLoop(loop);
  }


  int Game::calcPenalty(void) const
  {
    return 5 * mLevelTimer.accumulatedMilliseconds() / 1000; //MOD PenaltyPerSecond
  }


  int Game::deductPenalty(int score) const
  {
    return b2Max(0, score - calcPenalty());
  }


  void Game::onBodyKilled(Body *killedBody)
  {
    if (killedBody->type() == Body::BodyType::Block) {
      playSound(mExplosionSound, killedBody->position());
      ExplosionDef pd(this, killedBody->position());
      pd.ballCollisionEnabled = mLevel.explosionParticlesCollideWithBall();
      pd.count = gSettings.particlesPerExplosion;
      pd.texture = mParticleTexture;
      addBody(new Explosion(pd));
      {
        // check for killing spree
        mLastKillings[mLastKillingsIndex] = mWallClock.getElapsedTime();
        int i = (mLastKillingsIndex - mLastKillings.size()) % int(mLastKillings.size());
        const sf::Time &dt = mLastKillings.at(mLastKillingsIndex) - mLastKillings.at(i);
        mLastKillingsIndex = (mLastKillingsIndex + 1) % mLastKillings.size();
        if (dt < mLevel.killingSpreeInterval()) {
          playSound(mKillingSpreeSound, killedBody->position());
          showScore((mLevel.killingSpreeInterval() - dt).asMilliseconds() + mLevel.killingSpreeBonus(), killedBody->position() + b2Vec2(0.f, 1.35f));
          resetKillingSpree();
        }
      }
      const TileParam &tileParam = killedBody->tileParam();
      if (tileParam.earthquakeDuration > sf::Time::Zero && tileParam.earthquakeIntensity > 0.f) {
        startEarthquake(tileParam.earthquakeIntensity, tileParam.earthquakeDuration);
        addSpecialEffect(SpecialEffect(mEarthquakeDuration, &mEarthquakeClock, killedBody->texture()));
      }
      //MOD Tileparam
      if (tileParam.scaleGravityDuration > sf::Time::Zero) {
        mWorld->SetGravity(tileParam.scaleGravityBy * mWorld->GetGravity());
        mScaleGravityEnabled = true;
        mScaleGravityClock.restart();
        mScaleGravityDuration = tileParam.scaleGravityDuration;
        startAberrationEffect(tileParam.scaleGravityBy, tileParam.scaleGravityDuration);
        OverlayDef od;
        od.line1 = std::string("G*") + std::to_string(int(tileParam.scaleGravityBy));
        od.line2 = std::string("for ") + std::to_string(tileParam.scaleGravityDuration.asMilliseconds() / 1000) + "s";
        startOverlay(od);
        addSpecialEffect(SpecialEffect(mScaleGravityDuration, &mScaleGravityClock, killedBody->texture()));
      }
      if (tileParam.scaleBallDensityDuration > sf::Time::Zero) {
        mBall->setDensity(tileParam.scaleBallDensityBy * mBall->tileParam().density.get());
        mScaleBallDensityEnabled = true;
        mScaleBallDensityClock.restart();
        mScaleBallDensityDuration = tileParam.scaleBallDensityDuration;
      }
      if (--mBlockCount == 0)
        gotoLevelCompleted();
    }
    else if (killedBody->type() == Body::BodyType::Ball) {
      if (mState == State::Playing) {
        playSound(mBallOutSound, killedBody->position());
        mBallHasBeenLost = true;
        if (killedBody->energy() == 0) {
          if (mLives-- == 0) {
            gotoGameOver();
          }
        }
      }
    }
  }


  void Game::enumerateAllLevels(void)
  {
#ifndef NDEBUG
    std::cout << std::endl << "Game::enumerateAllLevels()" << std::endl << std::endl;
#endif
    std::packaged_task<bool()> task([this]{
#if defined(WIN32)
      const int prio = GetThreadPriority(GetCurrentThread());
      SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
#endif
      if (mLevels.empty()) {
        Level level;
        int l = 1;
        do {
          level = Level(l);
          if (level.isAvailable()) {
            mEnumerateMutex.lock();
            mLevels.push_back(level);
            mEnumerateMutex.unlock();
            ++l;
          }
        } while (level.isAvailable());
      }
#if defined(WIN32)
      SetThreadPriority(GetCurrentThread(), prio);
#endif
      return true;
    });
    mEnumerateFuture = task.get_future();
    std::thread(std::move(task)).detach();
  }
}
