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
#include <intrin.h>
#include <mmintrin.h>


namespace Impact {

  const float32 Game::InvScale = 1.f / Game::Scale;
  const int Game::DefaultLives = 3;
  const int Game::DefaultPenalty = 100;
  const float32 DefaultGravity = 9.81f;
  const int Game::NewLiveAfterSoManyPoints[] = { 10000, 25000, 50000, 100000, -1 };
  const int Game::NewLiveAfterSoManyPointsDefault = 100000;
  const sf::Time Game::DefaultKillingSpreeInterval = sf::milliseconds(2500);
  const sf::Time Game::DefaultFadeEffectDuration = sf::milliseconds(150);
  const sf::Time Game::DefaultAberrationEffectDuration = sf::milliseconds(250);
  const sf::Time Game::DefaultEarthquakeDuration = sf::milliseconds(10 * 1000);
  const sf::Time Game::DefaultOverlayDuration = sf::milliseconds(300);

  Game::Game(void)
    : mWindow(sf::VideoMode(Game::DefaultWindowWidth, Game::DefaultWindowHeight, Game::ColorDepth), "Impac't", sf::Style::Titlebar | sf::Style::Close)
    , mWorld(nullptr)
    , mBallHasBeenLost(false)
    , mBall(nullptr)
    , mGround(nullptr)
    , mContactPointCount(0)
    , mScore(0)
    , mLives(3)
    , mPaused(false)
    , mState(State::Initialization)
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
    , mFPSArray(20, 0)
    , mFPS(0)
    , mFPSIndex(0)
  {
    bool ok;

    glewInit();
    glGetIntegerv(GL_MAJOR_VERSION, &mGLVersionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &mGLVersionMinor);
    mGLShadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    mWindow.setActive();
    mWindow.setVerticalSyncEnabled(false);
    resize();

    sf::Image icon;
    icon.loadFromFile(ImagesDir + "/app-icon.png");
    mWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ok = mFixedFont.loadFromFile(FontsDir + "/04b_03.ttf");
    if (!ok)
      std::cerr << FontsDir + "/04b_03.ttf failed to load." << std::endl;

    ok = mTitleFont.loadFromFile(FontsDir + "/Dimitri.ttf");
    if (!ok)
      std::cerr << FontsDir + "/Dimitri.ttf failed to load." << std::endl;

    ok = mStartupBuffer.loadFromFile(SoundFXDir + "/startup.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/startup.ogg failed to load." << std::endl;
    mStartupSound.setBuffer(mStartupBuffer);
    mStartupSound.setVolume(100);
    mStartupSound.setLoop(false);

    ok = mNewBallBuffer.loadFromFile(SoundFXDir + "/new-ball.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/new-ball.ogg failed to load." << std::endl;
    mNewBallSound.setBuffer(mNewBallBuffer);
    mNewBallSound.setVolume(100);
    mNewBallSound.setLoop(false);

    ok = mNewLifeBuffer.loadFromFile(SoundFXDir + "/new-life.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/new-ball.ogg failed to load." << std::endl;
    mNewLifeSound.setBuffer(mNewLifeBuffer);
    mNewLifeSound.setVolume(100);
    mNewLifeSound.setLoop(false);

    ok = mBallOutBuffer.loadFromFile(SoundFXDir + "/ball-out.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/ball-out.ogg failed to load." << std::endl;
    mBallOutSound.setBuffer(mBallOutBuffer);
    mBallOutSound.setVolume(100);
    mBallOutSound.setLoop(false);

    ok = mBlockHitBuffer.loadFromFile(SoundFXDir + "/block-hit.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/block-hit.ogg failed to load." << std::endl;
    mBlockHitSound.setBuffer(mBlockHitBuffer);
    mBlockHitSound.setVolume(100);
    mBlockHitSound.setLoop(false);

    ok = mPenaltyBuffer.loadFromFile(SoundFXDir + "/penalty.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/penalty.ogg failed to load." << std::endl;
    mPenaltySound.setBuffer(mPenaltyBuffer);
    mPenaltySound.setVolume(100);
    mPenaltySound.setLoop(false);

    ok = mRacketHitBuffer.loadFromFile(SoundFXDir + "/racket-hit.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/racket-hit.ogg failed to load." << std::endl;
    mRacketHitSound.setBuffer(mRacketHitBuffer);
    mRacketHitSound.setVolume(100);
    mRacketHitSound.setLoop(false);

    ok = mRacketHitBlockBuffer.loadFromFile(SoundFXDir + "/racket-hit-block.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/racket-hit-block.ogg failed to load." << std::endl;
    mRacketHitBlockSound.setBuffer(mRacketHitBlockBuffer);
    mRacketHitBlockSound.setVolume(100);
    mRacketHitBlockSound.setLoop(false);

    ok = mExplosionBuffer.loadFromFile(SoundFXDir + "/explosion.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/explosion.ogg failed to load." << std::endl;
    mExplosionSound.setBuffer(mExplosionBuffer);
    mExplosionSound.setVolume(100);
    mExplosionSound.setLoop(false);

    ok = mLevelCompleteBuffer.loadFromFile(SoundFXDir + "/level-complete.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/level-complete.ogg failed to load." << std::endl;
    mLevelCompleteSound.setBuffer(mLevelCompleteBuffer);
    mLevelCompleteSound.setVolume(100);
    mLevelCompleteSound.setLoop(false);

    ok = mKillingSpreeSoundBuffer.loadFromFile(SoundFXDir + "/killing-spree.ogg");
    if (!ok)
      std::cerr << SoundFXDir + "/killing-spree.ogg failed to load." << std::endl;
    mKillingSpreeSound.setBuffer(mKillingSpreeSoundBuffer);
    mKillingSpreeSound.setVolume(100);
    mKillingSpreeSound.setLoop(false);

    mParticleTexture.loadFromFile(ImagesDir + "/particle.png");
    mSoftParticleTexture.loadFromFile(ImagesDir + "/smooth-dot-12x12.png");

    mLevelCompletedMsg.setString("Level complete");
    mLevelCompletedMsg.setFont(mFixedFont);
    mLevelCompletedMsg.setCharacterSize(64U);
    mLevelCompletedMsg.setColor(sf::Color(255, 255, 255));

    mGameOverMsg.setString("Game over");
    mGameOverMsg.setFont(mFixedFont);
    mGameOverMsg.setCharacterSize(64U);
    mGameOverMsg.setColor(sf::Color(255, 255, 255));

    mPlayerWonMsg.setString("You won");
    mPlayerWonMsg.setFont(mFixedFont);
    mPlayerWonMsg.setCharacterSize(64U);
    mPlayerWonMsg.setColor(sf::Color(255, 255, 255));

    mYourScoreMsg.setString("Your score");
    mYourScoreMsg.setFont(mFixedFont);
    mYourScoreMsg.setCharacterSize(32U);
    mYourScoreMsg.setColor(sf::Color(255, 255, 255));

    mStartMsg.setFont(mFixedFont);
    mStartMsg.setCharacterSize(16U);
    mStartMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mStartMsg.getLocalBounds().width, 1.4f * mDefaultView.getCenter().y);

    mStatMsg.setFont(mFixedFont);
    mStatMsg.setCharacterSize(8U);
    mStatMsg.setColor(sf::Color(255, 255, 63));

    mScoreMsg.setFont(mFixedFont);
    mScoreMsg.setCharacterSize(16U);
    mScoreMsg.setColor(sf::Color(255, 255, 255, 200));

    mTotalScoreMsg.setFont(mFixedFont);
    mTotalScoreMsg.setCharacterSize(64U);
    mTotalScoreMsg.setColor(sf::Color(255, 255, 255));

    mTotalScorePointsMsg.setFont(mFixedFont);
    mTotalScorePointsMsg.setCharacterSize(64U);
    mTotalScorePointsMsg.setColor(sf::Color(255, 255, 255));

    mLevelMsg.setFont(mFixedFont);
    mLevelMsg.setCharacterSize(16U);
    mLevelMsg.setColor(sf::Color(255, 255, 255, 200));

    mProgramInfoMsg.setString("Impac't v" + std::string(IMPACT_VERSION) + " (" + __TIMESTAMP__ + ")" 
      + " - "
      + "Copyright (c) 2015 Oliver Lau <ola@ct.de>"
      + "\n"
      + tr("Built with") + ": SFML " + std::to_string(SFML_VERSION_MAJOR) + "." + std::to_string(SFML_VERSION_MINOR) + ", "
      + "Box2D " + std::to_string(b2_version.major) + "." + std::to_string(b2_version.minor) + "." + std::to_string(b2_version.revision) + ", "
      + "glew " + std::to_string(GLEW_VERSION) + "." + std::to_string(GLEW_VERSION_MAJOR) + "." + std::to_string(GLEW_VERSION_MINOR)
      + " - "
      + "OpenGL " + std::to_string(mGLVersionMajor) + "." + std::to_string(mGLVersionMinor) + ", "
      + "GLSL " + std::string(reinterpret_cast<const char*>(mGLShadingLanguageVersion))
      );
    mProgramInfoMsg.setFont(mFixedFont);
    mProgramInfoMsg.setColor(sf::Color::White);
    mProgramInfoMsg.setCharacterSize(8U);
    mProgramInfoMsg.setPosition(8.f, mDefaultView.getSize().y - mProgramInfoMsg.getLocalBounds().height - 8.f);

    mBackgroundTexture.loadFromFile(BackgroundsDir + "/abstract05.jpg");
    mBackgroundSprite.setTexture(mBackgroundTexture);
    mBackgroundSprite.setPosition(0.f, 0.f);
    mBackgroundSprite.setScale(static_cast<float>(mDefaultView.getSize().x) / static_cast<float>(mBackgroundTexture.getSize().x), static_cast<float>(mDefaultView.getSize().y) / static_cast<float>(mBackgroundTexture.getSize().y));

    mLogoTexture.loadFromFile(ImagesDir + "/ct_logo.png");
    mLogoSprite.setTexture(mLogoTexture);
    mLogoSprite.setOrigin(static_cast<float>(mLogoTexture.getSize().x), static_cast<float>(mLogoTexture.getSize().y));
    mLogoSprite.setPosition(mDefaultView.getSize().x - 8.f, mDefaultView.getSize().y - 8.f);

    mTitleText = sf::Text("Impac't", mTitleFont, 120U);
    mTitleText.setPosition(.5f * (mDefaultView.getSize().x - mTitleText.getLocalBounds().width), .25f * (mDefaultView.getSize().y - mTitleText.getLocalBounds().height));


    if (gSettings.useShaders) {
      mRenderTexture0.create(DefaultPlaygroundWidth, DefaultPlaygroundHeight);
      mRenderTexture1.create(DefaultPlaygroundWidth, DefaultPlaygroundHeight);
    }

    if (gSettings.useShaders) {
      sf::RenderTexture titleRenderTexture;
      titleRenderTexture.create(static_cast<unsigned int>(mDefaultView.getSize().x), static_cast<unsigned int>(mDefaultView.getSize().y));
      titleRenderTexture.draw(mTitleText);
      mTitleTexture = titleRenderTexture.getTexture();
      mTitleTexture.setSmooth(true);
      mTitleSprite.setTexture(mTitleTexture);
    }

    if (gSettings.useShaders) {
      ok = mAberrationShader.loadFromFile(ShadersDir + "/aberration.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/aberration.fs" << " failed to load/compile." << std::endl;

      mMixShader.loadFromFile(ShadersDir + "/mix.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/mix.fs" << " failed to load/compile." << std::endl;

      mVBlurShader.loadFromFile(ShadersDir + "/vblur.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/vblur.fs" << " failed to load/compile." << std::endl;
      mVBlurShader.setParameter("uBlur", 4.f);
      mVBlurShader.setParameter("uResolution", sf::Vector2f(static_cast<float>(mWindow.getSize().x), static_cast<float>(mWindow.getSize().y)));

      mHBlurShader.loadFromFile(ShadersDir + "/hblur.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/hblur.fs" << " failed to load/compile." << std::endl;
      mHBlurShader.setParameter("uBlur", 4.f);
      mHBlurShader.setParameter("uResolution", sf::Vector2f(static_cast<float>(mWindow.getSize().x), static_cast<float>(mWindow.getSize().y)));

      mTitleShader.loadFromFile(ShadersDir + "/title.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/title.fs" << " failed to load/compile." << std::endl;
      mTitleShader.setParameter("uResolution", sf::Vector2f(static_cast<float>(mWindow.getSize().x), static_cast<float>(mWindow.getSize().y)));

      mEarthquakeShader.loadFromFile(ShadersDir + "/earthquake.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/earthquake.fs" << " failed to load/compile." << std::endl;

      mOverlayShader.loadFromFile(ShadersDir + "/approachingoverlay.fs", sf::Shader::Fragment);
      if (!ok)
        std::cerr << ShadersDir + "/approachingoverlay.fs" << " failed to load/compile." << std::endl;
    }

    mKeyMapping[Action::PauseAction] = sf::Keyboard::Pause;
    mKeyMapping[Action::BackAction] = sf::Keyboard::Escape;
    mKeyMapping[Action::Restart] = sf::Keyboard::Delete;
    mKeyMapping[Action::ContinueAction] = sf::Keyboard::Space;

    restart();
  }


  Game::~Game(void)
  {
    clearWorld();
  }


  void Game::stopAllMusic(void)
  {
    for (std::vector<sf::Music*>::iterator m = mMusic.begin(); m != mMusic.end(); ++m)
      (*m)->stop();
  }


  void Game::restart(void)
  {

    pause();
    clearWorld();

    safeRenew(mWorld, new b2World(b2Vec2(0.f, DefaultGravity)));
    mWorld->SetContactListener(this);
    mWorld->SetAllowSleeping(false);
    mWorld->SetWarmStarting(true);
    mWorld->SetContinuousPhysics(false);
    mWorld->SetSubStepping(true);

    mExtraLifeIndex = 0;
    mLives = DefaultLives;
    mScore = 0;
    mBallHasBeenLost = false;
    mLevel.set(0);

    mContactPointCount = 0;

    if (gSettings.useShaders) {
      mMixShader.setParameter("uColorMix", sf::Color(255, 255, 255, 255));
      mMixShader.setParameter("uColorAdd", sf::Color(0, 0, 0, 0));
      mMixShader.setParameter("uColorSub", sf::Color(0, 0, 0, 0));
    }

    gotoWelcomeScreen();

    resetKillingSpree();

    resume();
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
    mDefaultView.reset(sf::FloatRect(0.f, 0.f, static_cast<float>(DefaultWindowWidth), static_cast<float>(DefaultWindowHeight)));
    mDefaultView.setCenter(.5f * sf::Vector2f(static_cast<float>(DefaultWindowWidth), static_cast<float>(DefaultWindowHeight)));
    mPlaygroundView.reset(sf::FloatRect(0.f, 0.f, static_cast<float>(DefaultPlaygroundWidth), static_cast<float>(DefaultPlaygroundHeight)));
    mPlaygroundView.setCenter(.5f * sf::Vector2f(static_cast<float>(DefaultPlaygroundWidth), static_cast<float>(DefaultPlaygroundHeight)));
    mPlaygroundView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, static_cast<float>(DefaultPlaygroundHeight) / static_cast<float>(DefaultWindowHeight)));
    mStatsView.reset(sf::FloatRect(0.f, static_cast<float>(DefaultPlaygroundHeight), static_cast<float>(DefaultStatsWidth), static_cast<float>(DefaultStatsHeight)));
    mStatsView.setCenter(sf::Vector2f(.5f * DefaultStatsWidth, .5f * DefaultStatsHeight));
    mStatsView.setViewport(sf::FloatRect(0.f, static_cast<float>(DefaultWindowHeight - DefaultStatsHeight) / static_cast<float>(DefaultWindowHeight), 1.f, static_cast<float>(DefaultStatsHeight) / static_cast<float>(DefaultWindowHeight)));
  }


  void Game::setState(State state)
  {
#ifndef NDEBUG
    std::cout  << "Game::setState(" << int(state) << ")" << std::endl;
#endif
    mState = state;
  }


  void Game::enterLoop(void)
  {
    while (mWindow.isOpen()) {

      switch (mState) {
      case State::Playing:
        onPlaying();
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

      default:
        break;
      }

      mWindow.display();
    }
  }


  inline void Game::clearWindow(void)
  {
    mWindow.clear(sf::Color(42, 52, 54, 255));
  }


  void Game::handleEvents(void)
  {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
      switch (event.type)
      {
      case sf::Event::Closed:
        mWindow.close();
        break;
      case sf::Event::LostFocus:
        pause();
        break;
      case sf::Event::GainedFocus:
        resume();
        break;
      case sf::Event::MouseButtonPressed:
        if (mState == State::Playing) {
          if (mBall == nullptr)
            newBall();
        }
        else if (mState == State::LevelCompleted) {
          gotoNextLevel();
        }
        else if (mState == State::GameOver) {
          restart();
        }
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == mKeyMapping[Action::BackAction]) {
          mWindow.close();
        }
        else if (event.key.code == mKeyMapping[Action::PauseAction]) {
          if (mPaused)
            resume();
          else
            pause();
        }
        else if (event.key.code == mKeyMapping[Action::NewBall] || event.key.code == sf::Keyboard::Space) {
          if (mState == State::Playing) {
            if (mBall) {
              const b2Vec2 &padPos = mRacket->position();
              mBall->setPosition(padPos.x, padPos.y - 3.5f);
              showScore(-500, mBall->position());
            }
            else {
              newBall();
            }
          }
          else if (mState == State::LevelCompleted) {
            gotoNextLevel();
          }
          else if (mState == State::GameOver) {
            restart();
          }
        }
        break;
      }
    }
  }


  inline void Game::handlePlayerInteraction(const sf::Time &elapsed)
  {
    if (mRacket != nullptr) {
      const sf::Vector2i &mousePos = sf::Mouse::getPosition(mWindow);
      mRacket->moveTo(InvScale * b2Vec2(float32(mousePos.x), float32(mousePos.y)));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        mRacket->kickLeft();
      }
      else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        mRacket->kickRight();
      }
      else {
        mRacket->stopKick();
      }
    }
  }


  void Game::gotoWelcomeScreen(void) 
  {
    clearWorld();
    stopAllMusic();
    mStartupSound.play();
    mStartMsg.setString("Click to start");
    setState(State::WelcomeScreen);
    mWindow.setView(mDefaultView);
    if (gSettings.useShaders) {
      mTitleShader.setParameter("uMaxT", 1.f);
    }
    mWelcomeLevel = 0;
    mWallClock.restart();
    mWindow.setMouseCursorVisible(true);
  }


  void Game::onWelcomeScreen(void)
  {
    sf::Time elapsed = mClock.restart();
    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        mWindow.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          gotoNextLevel();
          return;
        }
      }
      else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == mKeyMapping[Action::BackAction]) {
          mWindow.close();
          return;
        }
      }
    }
    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    update(elapsed);
    drawWorld(mWindow.getDefaultView());

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

    if (mWelcomeLevel == 0) {
      ExplosionDef pd(this, b2Vec2(0.5f * 40.f, 0.4f * 25.f));
      pd.ballCollisionEnabled = false;
      pd.count = gSettings.particlesPerExplosion;
      pd.texture = mParticleTexture;
      addBody(new Explosion(pd));
      mWelcomeLevel = 1;
    }

    if (t > 0.5f) {
      drawStartMessage();
      if (mWelcomeLevel == 1) {
        mExplosionSound.play();
        mWelcomeLevel = 2;
        ExplosionDef pd(this, Game::InvScale * b2Vec2(mStartMsg.getPosition().x, mStartMsg.getPosition().y));
        pd.count = gSettings.particlesPerExplosion;
        pd.texture = mParticleTexture;
        addBody(new Explosion(pd));
      }
    }
    if (t > 0.6f) {
      mWindow.draw(mLogoSprite);
      if (mWelcomeLevel == 2) {
        mExplosionSound.play();
        mWelcomeLevel = 3;
        ExplosionDef pd(this, Game::InvScale * b2Vec2(mLogoSprite.getPosition().x, mLogoSprite.getPosition().y));
        pd.count = gSettings.particlesPerExplosion;
        pd.texture = mParticleTexture;
        addBody(new Explosion(pd));
      }
    }
    if (t > 0.7f) {
      mWindow.draw(mProgramInfoMsg);
      if (mWelcomeLevel == 4) {
        mExplosionSound.play();
        mWelcomeLevel = 5;
        ExplosionDef pd(this, Game::InvScale * b2Vec2(mProgramInfoMsg.getPosition().x, mProgramInfoMsg.getPosition().y));
        pd.texture = mParticleTexture;
        pd.count = gSettings.particlesPerExplosion;
        addBody(new Explosion(pd));
      }
    }
  }


  void Game::gotoLevelCompleted(void)
  {
    mLevelTimer.pause();
    mLevelCompleteSound.play();
    mStartMsg.setString(tr("Click to continue"));
    mBlurClock.restart();
    mBlurPlayground = true;
    setState(State::LevelCompleted);
  }


  void Game::onLevelCompleted(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed);

    drawPlayground(elapsed);

    mWindow.setView(mPlaygroundView);
    mLevelCompletedMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mLevelCompletedMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mLevelCompletedMsg);
    
    drawStartMessage();
  }


  void Game::gotoPlayerWon(void)
  {
    mStartMsg.setString(tr("Click to start over"));
    setState(State::PlayerWon);
    mBlurClock.restart();
    mBlurPlayground = true;
    mTotalScore = mScore - mLevelTimer.accumulatedSeconds();
  }


  void Game::onPlayerWon(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed);

    drawPlayground(elapsed);

    mWindow.setView(mPlaygroundView);

    mPlayerWonMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mGameOverMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mPlayerWonMsg);

    mYourScoreMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mYourScoreMsg.getLocalBounds().width, mPlaygroundView.getCenter().y);
    mWindow.draw(mYourScoreMsg);

    mTotalScoreMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mTotalScoreMsg.getLocalBounds().width, mPlaygroundView.getCenter().y - mTotalScoreMsg.getLocalBounds().height - 32);
    mWindow.draw(mTotalScoreMsg);

    mTotalScorePointsMsg.setString(std::to_string(mTotalScore));
    mTotalScorePointsMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mTotalScorePointsMsg.getLocalBounds().width, mPlaygroundView.getCenter().y - mTotalScorePointsMsg.getLocalBounds().height + 64);
    mWindow.draw(mTotalScorePointsMsg);

    drawStartMessage();
  }


  void Game::gotoGameOver(void)
  {
    mStartMsg.setString(tr("Click to continue"));
    setState(State::GameOver);
    mBlurClock.restart();
    mBlurPlayground = true;
    if (gSettings.useShaders) {
      mMixShader.setParameter("uColorMix", sf::Color(255, 255, 255, 220));
    }
    mTotalScore = b2Max(0, mScore - mLevelTimer.accumulatedSeconds());
  }


  void Game::onGameOver(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed);

    drawPlayground(elapsed);

    mWindow.setView(mPlaygroundView);
    mGameOverMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mGameOverMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mGameOverMsg);

    mYourScoreMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mYourScoreMsg.getLocalBounds().width, mPlaygroundView.getCenter().y);
    mWindow.draw(mYourScoreMsg);

    mTotalScorePointsMsg.setString(std::to_string(mTotalScore));
    mTotalScorePointsMsg.setPosition(mPlaygroundView.getCenter().x - 0.5f * mTotalScorePointsMsg.getLocalBounds().width, mPlaygroundView.getCenter().y - mTotalScorePointsMsg.getLocalBounds().height + 64);
    mWindow.draw(mTotalScorePointsMsg);

    drawStartMessage();
  }


  void Game::onPausing(void)
  {
    onPlaying();
  }


  void Game::gotoNextLevel(void)
  {
#ifndef NDEBUG
    std::cout << "Game::gotoNextLevel()" << std::endl;
#endif
    stopAllMusic();
    clearWorld();
    mBallHasBeenLost = false;
    // mWindow.setMouseCursorVisible(false);
    if (gSettings.useShaders) {
      mMixShader.setParameter("uColorMix", sf::Color(255, 255, 255, 255));
    }
    mScaleGravityEnabled = false;
    mScaleBallDensityEnabled = false;
    bool levelAvailable = mPlaymode == Playmode::Campaign ? mLevel.gotoNext() : mLevel.isAvailable();
    if (levelAvailable) {
      buildLevel();
      mClock.restart();
      mBlurPlayground = false;
      mFadeEffectsActive = 0;
      mEarthquakeDuration = sf::Time::Zero;
      mEarthquakeIntensity = 0.f;
      mAberrationDuration = sf::Time::Zero;
      mAberrationIntensity = 0.f;
      mLevelTimer.resume();
      setState(State::Playing);
    }
    else {
      gotoPlayerWon();
    }
  }


  void Game::onPlaying(void)
  {
    const sf::Time &elapsed = mClock.restart();
    if (!mPaused) {
      update(elapsed);
      if (mState == State::Playing) {
        if (mBall != nullptr) { // check if ball has been kicked out of the screen
          const float ballX = mBall->position().x;
          const float ballY = mBall->position().y;
          if (0 > ballX || ballX > static_cast<float>(mLevel.width()) || 0 > ballY) {
            mBall->kill();
          }
          else if (ballY > mLevel.height()) {
            mBall->lethalHit();
            mBall->kill();
          }
        }

        if (mRacket != nullptr) { // check if pad has been kicked out of the screen
          const float racketX = mRacket->position().x;
          const float racketY = mRacket->position().y;
          if (racketY > mLevel.height())
            mRacket->setPosition(racketX, mLevel.height() - .5f);
          if (racketX < 0.f)
            mRacket->setPosition(1.5f, racketY);
          else if (racketX > mLevel.width())
            mRacket->setPosition(mLevel.width() - 1.5f, racketY);
        }
      }
      handlePlayerInteraction(elapsed);
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
    drawPlayground(elapsed);
  }


  void Game::gotoAchievementsScreen(void)
  {
    // TODO
  }


  void Game::onAchievementsScreen(void)
  {
    // TODO
  }


  void Game::gotoCreditsScreen(void)
  {
    // TODO
  }


  void Game::onCreditsScreen(void)
  {
    // TODO
  }


  void Game::gotoOptionsScreen(void)
  {
    // TODO
  }


  void Game::onOptionsScreen(void)
  {
    // TODO
  }



  inline void Game::executeAberration(sf::RenderTexture &out, sf::RenderTexture &in)
  {
    sf::RenderStates states;
	  sf::Sprite sprite(in.getTexture());
    states.shader = &mAberrationShader;
    mAberrationShader.setParameter("uT", mAberrationClock.getElapsedTime().asSeconds());
    out.draw(sprite, states);
  }


  void Game::startAberrationEffect(float32 gravityScale, const sf::Time &duration)
  {
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
  }


  inline void Game::executeBlur(sf::RenderTexture &out, sf::RenderTexture &in)
  {
    sf::RenderStates states0;
    sf::Sprite sprite0;
    sf::RenderStates states1;
    sf::Sprite sprite1;
    if (gSettings.useShaders) {
      states0.shader = &mHBlurShader;
      states1.shader = &mVBlurShader;
    }
    const float blur = b2Min(4.f, 1.f + mBlurClock.getElapsedTime().asSeconds());
    for (int i = 1; i < 4; ++i) {
      sprite1.setTexture(in.getTexture());
      mVBlurShader.setParameter("uBlur", blur * i);
      out.draw(sprite1, states1);
      sprite0.setTexture(out.getTexture());
      mHBlurShader.setParameter("uBlur", blur * i);
      in.draw(sprite0, states0);
    }
    executeCopy(out, in);
  }


  inline void Game::executeEarthquake(sf::RenderTexture &out, sf::RenderTexture &in)
  {
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
    od.line1 = std::string("Shake ") + std::to_string(static_cast<int>(10 * mEarthquakeIntensity));
    od.line2 = std::string("for ") + std::to_string(mEarthquakeDuration.asMilliseconds() / 1000) + "s";
    startOverlay(od);
  }


  void Game::startOverlay(const OverlayDef &od)
  {
    mOverlayDuration = od.duration;

    if (gSettings.useShaders) {
      mOverlayShader.setParameter("uMinScale", od.minScale);
      mOverlayShader.setParameter("uMaxScale", od.maxScale);
      mOverlayShader.setParameter("uMaxT", od.duration.asSeconds());
      mOverlayShader.setParameter("uResolution", sf::Vector2f(static_cast<float>(DefaultWindowWidth), static_cast<float>(DefaultWindowHeight)));
    }

    sf::RenderTexture overlayRenderTexture;
    overlayRenderTexture.create(static_cast<unsigned int>(mDefaultView.getSize().x), static_cast<unsigned int>(mDefaultView.getSize().y));

    sf::Text overlayText1(od.line1, mTitleFont, 80U);
    overlayText1.setPosition(.5f * (mDefaultView.getSize().x - overlayText1.getLocalBounds().width), .16f * (mDefaultView.getSize().y - overlayText1.getLocalBounds().height));
    overlayRenderTexture.draw(overlayText1);

    sf::Text overlayText2(od.line2, mTitleFont, 80U);
    overlayText2.setPosition(.5f * (mDefaultView.getSize().x - overlayText2.getLocalBounds().width), .32f * (mDefaultView.getSize().y - overlayText2.getLocalBounds().height));
    overlayRenderTexture.draw(overlayText2);

    mOverlayTexture = overlayRenderTexture.getTexture();
    mOverlayTexture.setSmooth(true);
    mOverlaySprite.setTexture(mOverlayTexture);

    mOverlayClock.restart();
  }



  inline void Game::executeCopy(sf::RenderTexture &out, sf::RenderTexture &in)
  {
	  sf::Sprite sprite(in.getTexture());
	  out.draw(sprite);
  }


  void Game::drawPlayground(const sf::Time &elapsed)
  {
    handleEvents();

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

      if (mBlurPlayground && gSettings.useShaders) {
        executeBlur(mRenderTexture1, mRenderTexture0);
        executeCopy(mRenderTexture0, mRenderTexture1);
      }

      if (mAberrationDuration > sf::Time::Zero && gSettings.useShaders) {
        if (mAberrationClock.getElapsedTime() < mAberrationDuration) {
          executeAberration(mRenderTexture1, mRenderTexture0);
          executeCopy(mRenderTexture0, mRenderTexture1);
        }
        else {
          mAberrationDuration = sf::Time::Zero;
          mAberrationIntensity = 0.f;
        }
      }

      if (mEarthquakeIntensity > 0.f && mEarthquakeClock.getElapsedTime() < mEarthquakeDuration && gSettings.useShaders) {
        // TODO: display hint what's going on ...
        executeEarthquake(mRenderTexture1, mRenderTexture0);
        executeCopy(mRenderTexture0, mRenderTexture1);
      }
      else {
        if (mEarthquakeClock.getElapsedTime() > mEarthquakeDuration)
          mEarthquakeIntensity = 0.f;
      }

      if (gSettings.useShaders) {
        if (mFadeEffectsActive > 0 && gSettings.useShaders) {
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
      else {
        sf::Sprite sprite(mRenderTexture0.getTexture());
        mWindow.draw(sprite);
      }

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
          mWindow.draw(mOverlaySprite);
        }
      }
      else {
        mOverlayDuration = sf::Time::Zero;
      }
    }

    mWindow.setView(mStatsView);
    mLevelMsg.setString(tr("Level") + " " + std::to_string(mLevel.num()));
    mLevelMsg.setPosition(4, 4);
    mWindow.draw(mLevelMsg);

    if (mState == State::Playing) {
      int penalty = 5 * mLevelTimer.accumulatedMilliseconds() / 1000;
      mScoreMsg.setString(std::to_string(b2Max(0, mScore - penalty)));
      mScoreMsg.setPosition(mStatsView.getSize().x - mScoreMsg.getLocalBounds().width - 4, 4);
      mWindow.draw(mScoreMsg);
      for (int life = 0; life < mLives; ++life) {
        const sf::Texture &ballTexture = mLevel.texture(Ball::Name);
        sf::Sprite lifeSprite(ballTexture);
        lifeSprite.setOrigin(0.f, 0.f);
        lifeSprite.setPosition(4 + (ballTexture.getSize().x * 1.5f) * life, 26.f);
        mWindow.draw(lifeSprite);
      }
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
    for (int i = 0; i < mContactPointCount; ++i) {
      ContactPoint &cp = mPoints[i];
      b2Body *bodyA = cp.fixtureA->GetBody();
      b2Body *bodyB = cp.fixtureB->GetBody();
      Body *a = reinterpret_cast<Body *>(cp.fixtureA->GetUserData());
      Body *b = reinterpret_cast<Body *>(cp.fixtureB->GetUserData());
      if (a == nullptr || b == nullptr)
        return;
      if (a->type() == Body::BodyType::Block || b->type() == Body::BodyType::Block) {
        if (a->type() == Body::BodyType::Ball || b->type() == Body::BodyType::Ball) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          if (std::find(killedBodies.cbegin(), killedBodies.cend(), block) == killedBodies.cend()) {
            bool destroyed = block->hit(cp.normalImpulse);
            if (destroyed) {
              block->kill();
              showScore(block->getScore(), block->position());
              killedBodies.push_back(block);
            }
            else if (cp.normalImpulse > 20)
              mBlockHitSound.play();
          }
        }
        else if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          if (std::find(killedBodies.cbegin(), killedBodies.cend(), block) == killedBodies.cend()) {
            block->kill();
            killedBodies.push_back(block);
          }
        }
        else if (a->type() == Body::BodyType::Racket || b->type() == Body::BodyType::Racket) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          if (block->body()->GetGravityScale() > 0.f) {
            if (std::find(killedBodies.cbegin(), killedBodies.cend(), block) == killedBodies.cend()) {
              showScore(block->getScore(), block->position(), 2);
              block->kill();
              mRacketHitBlockSound.play();
              killedBodies.push_back(block);
            }
          }
          else {
            showScore(-block->getScore(), block->position());
            mPenaltySound.play();
            startFadeEffect();
          }
        }
      }
      else if (a->type() == Body::BodyType::Ball || b->type() == Body::BodyType::Ball) {
        if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Ball *ball = reinterpret_cast<Ball*>(a->type() == Body::BodyType::Ball ? a : b);
          if (std::find(killedBodies.cbegin(), killedBodies.cend(), ball) == killedBodies.cend()) {
            ball->lethalHit();
            ball->kill();
            killedBodies.push_back(ball);
            startFadeEffect(true, sf::milliseconds(350));
          }
        }
        else if (a->type() == Body::BodyType::Racket || b->type() == Body::BodyType::Racket) {
          if (cp.normalImpulse > 20)
            mRacketHitSound.play();
        }
      }
    }
  }


  inline void Game::update(const sf::Time &elapsed)
  {
    float elapsedSeconds = 1e-6f * elapsed.asMicroseconds();
    
    if (mState == State::Playing)
      evaluateCollisions();

    BodyList remainingBodies;
    for (BodyList::iterator b = mBodies.begin(); b != mBodies.end(); ++b) {
      Body *body = *b;
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
    mBodies = remainingBodies;

    mContactPointCount = 0;
    mWorld->Step(elapsedSeconds, VelocityIterations, PositionIterations);

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
        cp.position = contact->GetManifold()->points[0].localPoint;
        cp.normal = b2Vec2_zero;
        cp.normalImpulse = impulse->normalImpulses[0];
        cp.tangentImpulse = impulse->tangentImpulses[0];
        cp.separation = 0.f;
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
    fdRight.restitution = 0.9f;
    fdRight.density = 0.f;
    fdRight.shape = &rightShape;
    boundaries->CreateFixture(&fdRight);
    b2EdgeShape leftShape;
    leftShape.Set(b2Vec2(0, 0), b2Vec2(0, H));
    b2FixtureDef fdLeft;
    fdLeft.restitution = 0.9f;
    fdLeft.density = 0.f;
    fdLeft.shape = &leftShape;
    boundaries->CreateFixture(&fdLeft);
    b2EdgeShape topShape;
    topShape.Set(b2Vec2(0, g > 0.f ? 0.f : static_cast<float32>(mLevel.height())), b2Vec2(W, g > 0.f ? 0.f : static_cast<float32>(mLevel.height())));
    b2FixtureDef fdTop;
    fdTop.restitution = 0.9f;
    fdTop.density = 0.f;
    fdTop.shape = &topShape;
    boundaries->CreateFixture(&fdTop);

    mGround = new Ground(this, W);
    mGround->setPosition(0, g < 0.f ? 0 : mLevel.height());
    addBody(mGround);

#ifdef BALL_TRACE
    safeRenew(mBallTrace, new BallTrace(this));
    addBody(mBallTrace);
#endif

    // create level elements
    mBlockCount = 0;
    for (int y = 0; y < mLevel.height(); ++y) {
      const uint32_t *mapRow = mLevel.mapDataScanLine(y);
      for (int x = 0; x < mLevel.width(); ++x) {
        const b2Vec2 &pos = b2Vec2(static_cast<float32>(x), static_cast<float32>(y));
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

    // place mouse cursor on racket position
    const b2Vec2 &racketPos = static_cast<float32>(Game::Scale) * mRacket->position();
    sf::Mouse::setPosition(sf::Vector2i(int(racketPos.x), int(racketPos.y)), mWindow);
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
    int newScore = mScore + points;
    if (points > 0) {
      int threshold = NewLiveAfterSoManyPoints[mExtraLifeIndex];
      if (threshold > 0 && newScore > threshold) {
        ++mExtraLifeIndex;
        extraBall();
      }
      else if ((mScore % NewLiveAfterSoManyPointsDefault) > (newScore % NewLiveAfterSoManyPointsDefault)) {
        extraBall();
      }
    }
    mScore = b2Max(newScore, 0);
  }


  void Game::extraBall(void)
  {
    ++mLives;
    mNewLifeSound.play();
  }


  void Game::newBall(const b2Vec2 &pos)
  {
    mNewBallSound.play();
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
    mPaused = true;
    mLevelTimer.pause();
  }


  void Game::resume(void)
  {
    mPaused = false;
    mLevelTimer.resume();
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


  void Game::onBodyKilled(Body *killedBody)
  {
    if (killedBody->type() == Body::BodyType::Block) {
      mExplosionSound.play();
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
          mKillingSpreeSound.play();
          showScore((mLevel.killingSpreeInterval() - dt).asMilliseconds() + mLevel.killingSpreeBonus(), killedBody->position() + b2Vec2(0.f, 1.35f));
          resetKillingSpree();
        }
      }
      const TileParam &tileParam = killedBody->tileParam();
      if (tileParam.earthquakeDuration > sf::Time::Zero && tileParam.earthquakeIntensity > 0.f) {
        startEarthquake(tileParam.earthquakeIntensity, tileParam.earthquakeDuration);
      }
      if (tileParam.scaleGravityDuration > sf::Time::Zero) {
        mWorld->SetGravity(tileParam.scaleGravityBy * mWorld->GetGravity());
        mScaleGravityEnabled = true;
        mScaleGravityClock.restart();
        mScaleGravityDuration = tileParam.scaleGravityDuration;
        startAberrationEffect(tileParam.scaleGravityBy, tileParam.scaleGravityDuration);
        OverlayDef od;
        od.line1 = std::string("G*") + std::to_string(static_cast<int>(tileParam.scaleGravityBy));
        od.line2 = std::string("for ") + std::to_string(tileParam.scaleGravityDuration.asMilliseconds() / 1000) + "s";
        startOverlay(od);
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
        mBallOutSound.play();
        mBallHasBeenLost = true;
        if (killedBody->energy() == 0) {
          if (mLives-- == 0) {
            gotoGameOver();
          }
        }
      }
    }
  }

}

