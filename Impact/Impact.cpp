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

  const int Game::Scale = 16;
  const float32 Game::InvScale = 1.f / Game::Scale;
  const int Game::DefaultWindowWidth = 40 * int(Game::Scale);
  const int Game::DefaultWindowHeight = 25 * int(Game::Scale);
  const int Game::ColorDepth = 32;
  const int Game::DefaultLives = 3;
  const int Game::DefaultPenalty = 100;
  const float32 DefaultGravity = 9.81f;
  const int Game::NewLiveAfterSoManyPoints[] = { 10000, 25000, 50000, 100000, -1 };
  const int Game::NewLiveAfterSoManyPointsDefault = 100000;
  const sf::Time Game::DefaultKillingSpreeInterval = sf::milliseconds(2500);
  const sf::Time Game::DefaultGlareEffectDuration = sf::milliseconds(150);

  Game::Game(void)
    : mWindow(sf::VideoMode(Game::DefaultWindowWidth, Game::DefaultWindowHeight, Game::ColorDepth), "Impac't", sf::Style::Titlebar | sf::Style::Close)
    , mWorld(nullptr)
    , mBallHasBeenLost(false)
    , mBall(nullptr)
    , mGround(nullptr)
#ifdef BALL_TRACES
    , mBallTrace(nullptr)
#endif
    , mContactPointCount(0)
    , mScore(0)
    , mLives(3)
    , mPaused(false)
    , mState(State::Initialization)
    , mKeyMapping(Action::LastAction)
    , mBlockCount(0)
    , mMouseModeEnabled(false)
    , mGlareEffectsActive(0)
    , mGlareEffectsDarken(false)
    , mGlareEffectDuration(DefaultGlareEffectDuration)
    , mLastKillingsIndex(0)
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
    resize();

    mRenderTexture.create(Game::DefaultWindowWidth, Game::DefaultWindowHeight, false);

    sf::Image icon;
    icon.loadFromFile(gImagesDir + "/app-icon.png");
    mWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ok = mFixedFont.loadFromFile(gFontsDir + "/04b_03.ttf");
    if (!ok)
      std::cerr << gFontsDir + "/04b_03.ttf failed to load." << std::endl;

    ok = mStartupBuffer.loadFromFile(gSoundFXDir + "/startup.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/startup.ogg failed to load." << std::endl;
    mStartupSound.setBuffer(mStartupBuffer);
    mStartupSound.setVolume(100);
    mStartupSound.setLoop(false);

    ok = mNewBallBuffer.loadFromFile(gSoundFXDir + "/new-ball.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/new-ball.ogg failed to load." << std::endl;
    mNewBallSound.setBuffer(mNewBallBuffer);
    mNewBallSound.setVolume(100);
    mNewBallSound.setLoop(false);

    ok = mNewLifeBuffer.loadFromFile(gSoundFXDir + "/new-life.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/new-ball.ogg failed to load." << std::endl;
    mNewLifeSound.setBuffer(mNewLifeBuffer);
    mNewLifeSound.setVolume(100);
    mNewLifeSound.setLoop(false);

    ok = mBallOutBuffer.loadFromFile(gSoundFXDir + "/ball-out.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/ball-out.ogg failed to load." << std::endl;
    mBallOutSound.setBuffer(mBallOutBuffer);
    mBallOutSound.setVolume(100);
    mBallOutSound.setLoop(false);

    ok = mBlockHitBuffer.loadFromFile(gSoundFXDir + "/block-hit.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/block-hit.ogg failed to load." << std::endl;
    mBlockHitSound.setBuffer(mBlockHitBuffer);
    mBlockHitSound.setVolume(100);
    mBlockHitSound.setLoop(false);

    ok = mPenaltyBuffer.loadFromFile(gSoundFXDir + "/penalty.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/penalty.ogg failed to load." << std::endl;
    mPenaltySound.setBuffer(mPenaltyBuffer);
    mPenaltySound.setVolume(100);
    mPenaltySound.setLoop(false);

    ok = mRacketHitBuffer.loadFromFile(gSoundFXDir + "/racket-hit.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/racket-hit.ogg failed to load." << std::endl;
    mRacketHitSound.setBuffer(mRacketHitBuffer);
    mRacketHitSound.setVolume(100);
    mRacketHitSound.setLoop(false);

    ok = mRacketHitBlockBuffer.loadFromFile(gSoundFXDir + "/racket-hit-block.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/racket-hit-block.ogg failed to load." << std::endl;
    mRacketHitBlockSound.setBuffer(mRacketHitBlockBuffer);
    mRacketHitBlockSound.setVolume(100);
    mRacketHitBlockSound.setLoop(false);

    ok = mExplosionBuffer.loadFromFile(gSoundFXDir + "/explosion.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/explosion.ogg failed to load." << std::endl;
    mExplosionSound.setBuffer(mExplosionBuffer);
    mExplosionSound.setVolume(100);
    mExplosionSound.setLoop(false);

    ok = mLevelCompleteBuffer.loadFromFile(gSoundFXDir + "/level-complete.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/level-complete.ogg failed to load." << std::endl;
    mLevelCompleteSound.setBuffer(mLevelCompleteBuffer);
    mLevelCompleteSound.setVolume(100);
    mLevelCompleteSound.setLoop(false);

    ok = mKillingSpreeSoundBuffer.loadFromFile(gSoundFXDir + "/killing-spree.ogg");
    if (!ok)
      std::cerr << gSoundFXDir + "/killing-spree.ogg failed to load." << std::endl;
    mKillingSpreeSound.setBuffer(mKillingSpreeSoundBuffer);
    mKillingSpreeSound.setVolume(100);
    mKillingSpreeSound.setLoop(false);

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
    mProgramInfoMsg.setPosition(8.f, mDefaultView.getCenter().y + 0.5f * mDefaultView.getSize().y - mProgramInfoMsg.getLocalBounds().height - 8.f);

    mBackgroundTexture.loadFromFile(gBackgroundsDir + "/abstract05.jpg");
    mBackgroundSprite.setTexture(mBackgroundTexture);
    mBackgroundSprite.setPosition(0.f, 0.f);

    mLogoTexture.loadFromFile(gImagesDir + "/ct_logo.png");
    mLogoSprite.setTexture(mLogoTexture);
    mLogoSprite.setOrigin(float(mLogoTexture.getSize().x), float(mLogoTexture.getSize().y));
    mLogoSprite.setPosition(mDefaultView.getSize().x - 8.f, mDefaultView.getSize().y - 8.f);

    mTitleTexture.loadFromFile(gImagesDir + "/title.png");
    mTitleTexture.setSmooth(true);
    mTitleSprite.setTexture(mTitleTexture);
    mTitleSprite.setPosition(0.f, 0.f);

    mPostFX.loadFromFile(gShadersDir + "/postfx.frag", sf::Shader::Fragment);

    mTitleShader.loadFromFile(gShadersDir + "/title.frag", sf::Shader::Fragment);

    mKeyMapping[Action::MoveLeft] = sf::Keyboard::Left;
    mKeyMapping[Action::MoveRight] = sf::Keyboard::Right;
    mKeyMapping[Action::SpecialAction] = sf::Keyboard::Space;
    mKeyMapping[Action::NewBall] = sf::Keyboard::N;
    mKeyMapping[Action::BackAction] = sf::Keyboard::Escape;
    mKeyMapping[Action::KickLeft] = sf::Keyboard::X;
    mKeyMapping[Action::KickRight] = sf::Keyboard::Y;
    mKeyMapping[Action::Restart] = sf::Keyboard::Delete;
    mKeyMapping[Action::ExplosionTest] = sf::Keyboard::P;
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

    mPostFX.setParameter("uColorMix", sf::Color(255, 255, 255, 255));
    mPostFX.setParameter("uColorAdd", sf::Color(0, 0, 0, 0));
    mPostFX.setParameter("uColorSub", sf::Color(0, 0, 0, 0));
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
    mDefaultView = sf::View(sf::FloatRect(0.f, 0.f, float(mWindow.getSize().x), float(mWindow.getSize().y)));
    mDefaultView.setCenter(0.5f * sf::Vector2f(mWindow.getSize()));
  }


  void Game::setState(State state)
  {
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

      default:
        break;
      }

      mWindow.display();
    }
  }


  inline void Game::clearWindow(void)
  {
    mWindow.clear(mLevel.backgroundColor());
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
        else if (event.key.code == mKeyMapping[Action::NewBall] || event.key.code == sf::Keyboard::Space) {
          if (mState == State::Playing) {
            if (mBall == nullptr)
              newBall();
            else
              startGlareEffect();
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


  void Game::handlePlayerInteraction(float elapsedSeconds)
  {
    if (mRacket != nullptr) {
      if (mMouseModeEnabled) {
        mMousePos = sf::Mouse::getPosition(mWindow);
        if (mMousePos.x < 0 || mMousePos.x > int(mWindow.getSize().x) || mMousePos.y < 0 || mMousePos.y > int(mWindow.getSize().y)) {
          mMousePos = sf::Vector2i(int(Game::Scale * mRacket->position().x), int(Game::Scale * mRacket->position().y));
          mLastMousePos = mMousePos;
          sf::Mouse::setPosition(mMousePos, mWindow);
        }
        const sf::Vector2i &d = mMousePos - mLastMousePos;
        const b2Vec2 &v = Game::InvScale / elapsedSeconds * b2Vec2(float32(d.x), float32(d.y));
        mRacket->body()->SetLinearVelocity(v);
        mLastMousePos = mMousePos;
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
      else {
        if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::KickLeft])) {
          mRacket->kickLeft();
        }
        else if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::KickRight])) {
          mRacket->kickRight();
        }
        else {
          mRacket->stopKick();
        }
        if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::MoveLeft])) {
          mRacket->moveLeft();
        }
        else if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::MoveRight])) {
          mRacket->moveRight();
        }
        else {
          mRacket->stopMotion();
        }
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
    mTitleShader.setParameter("uMaxT", 1.f);
    mWelcomeLevel = 0;
    mWallClock.restart();
    mWindow.setMouseCursorVisible(true);
    mWindow.setVerticalSyncEnabled(false);
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
          setMouseModeEnabled(true);
          gotoNextLevel();
        }
      }
      else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == mKeyMapping[Action::BackAction]) {
          mWindow.close();
        }
        else if (event.key.code == mKeyMapping[Action::ContinueAction]) {
          setMouseModeEnabled(false);
          gotoNextLevel();
        }
      }
    }
    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    update(elapsed.asSeconds());
    drawWorld(mDefaultView);

    const float t = mWallClock.getElapsedTime().asSeconds();
    sf::RenderStates states;
    states.shader = &mTitleShader;
    mTitleShader.setParameter("uT", t);
    mWindow.draw(mTitleSprite, states);

    if (mWelcomeLevel == 0) {
      addBody(new ParticleSystem(this, b2Vec2(0.5f * 40.f, 0.4f * 25.f), false, 122U));
      mWelcomeLevel = 1;
    }

    if (t > 0.5f) {
      drawStartMessage();
      if (mWelcomeLevel == 1) {
        mExplosionSound.play();
        mWelcomeLevel = 2;
        addBody(new ParticleSystem(this, Game::InvScale * b2Vec2(mStartMsg.getPosition().x, mStartMsg.getPosition().y), false, 100U));
      }
    }
    if (t > 0.6f) {
      mWindow.draw(mLogoSprite);
      if (mWelcomeLevel == 2) {
        mExplosionSound.play();
        mWelcomeLevel = 3;
        addBody(new ParticleSystem(this, Game::InvScale * b2Vec2(mLogoSprite.getPosition().x, mLogoSprite.getPosition().y), false, 100U));
      }
    }
    if (t > 0.7f) {
      mWindow.draw(mProgramInfoMsg);
      if (mWelcomeLevel == 4) {
        mExplosionSound.play();
        mWelcomeLevel = 5;
        addBody(new ParticleSystem(this, Game::InvScale * b2Vec2(mProgramInfoMsg.getPosition().x, mProgramInfoMsg.getPosition().y), false, 100U));
      }
    }
  }


  void Game::gotoLevelCompleted(void)
  {
    mLevelTimer.pause();
    mLevelCompleteSound.play();
    mStartMsg.setString(tr("Click to continue"));
    mBlamClock.restart();
    mRacket->body()->SetLinearVelocity(b2Vec2_zero);
    setState(State::LevelCompleted);
  }


  void Game::onLevelCompleted(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed.asSeconds());

    mPostFX.setParameter("uColorMix", sf::Color(255, 255, 255, 0x7f));
    drawPlayground();

    mLevelCompletedMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mLevelCompletedMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mLevelCompletedMsg);
    
    drawStartMessage();
  }


  void Game::gotoPlayerWon(void)
  {
    mStartMsg.setString(tr("Click to start over"));
    setState(State::PlayerWon);
    mTotalScore = mScore - mLevelTimer.accumulatedSeconds();
  }


  void Game::onPlayerWon(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed.asSeconds());

    drawPlayground();

    mPlayerWonMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mGameOverMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mPlayerWonMsg);

    mYourScoreMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mYourScoreMsg.getLocalBounds().width, mDefaultView.getCenter().y);
    mWindow.draw(mYourScoreMsg);

    mTotalScoreMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mTotalScoreMsg.getLocalBounds().width, mDefaultView.getCenter().y - mTotalScoreMsg.getLocalBounds().height - 32);
    mWindow.draw(mTotalScoreMsg);

    mTotalScorePointsMsg.setString(std::to_string(mTotalScore));
    mTotalScorePointsMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mTotalScorePointsMsg.getLocalBounds().width, mDefaultView.getCenter().y - mTotalScorePointsMsg.getLocalBounds().height + 64);
    mWindow.draw(mTotalScorePointsMsg);

    drawStartMessage();
  }


  void Game::gotoGameOver(void)
  {
    mStartMsg.setString(tr("Click to continue"));
    setState(State::GameOver);
    mTotalScore = mScore - mLevelTimer.accumulatedSeconds();
  }


  void Game::onGameOver(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed.asSeconds());

    mPostFX.setParameter("uColorMix", sf::Color(255, 255, 255, 0x7f));
    drawPlayground();

    mGameOverMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mGameOverMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mGameOverMsg);

    mYourScoreMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mYourScoreMsg.getLocalBounds().width, mDefaultView.getCenter().y);
    mWindow.draw(mYourScoreMsg);

    mTotalScorePointsMsg.setString(std::to_string(mTotalScore));
    mTotalScorePointsMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mTotalScorePointsMsg.getLocalBounds().width, mDefaultView.getCenter().y - mTotalScorePointsMsg.getLocalBounds().height + 64);
    mWindow.draw(mTotalScorePointsMsg);

    drawStartMessage();
  }


  void Game::onPausing(void)
  {
    onPlaying();
  }


  void Game::gotoNextLevel(void)
  {
    stopAllMusic();
    clearWorld();
    mBallHasBeenLost = false;
    mWindow.setMouseCursorVisible(false);
    mPostFX.setParameter("uColorMix", sf::Color(255, 255, 255, 255));
    if (mLevel.gotoNext()) {
      buildLevel();
      setState(State::Playing);
      mClock.restart();
      mLevelTimer.resume();
    }
    else {
      gotoPlayerWon();
    }
  }


  void Game::onPlaying(void)
  {
    const sf::Time &elapsed = mClock.restart();
    if (!mPaused) {
      handlePlayerInteraction(elapsed.asSeconds());
      update(elapsed.asSeconds());
      if (mState == State::Playing) {
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

        if (mRacket != nullptr) { // check if pad has been kicked out of the screen
          const float racketX = mRacket->position().x;
          const float racketY = mRacket->position().y;
          if (racketY > mLevel.height())
            mRacket->setPosition(racketX, mLevel.height() - 0.5f);
          if (racketX < 0.f)
            mRacket->setPosition(1., racketY);
          else if (racketX > mLevel.width())
            mRacket->setPosition(mLevel.width() - 1.f, racketY);
        }
      }
    }
    drawPlayground();
  }


  auto quadEaseIn = [](float t, float b, float c, float d) {
    float dt = t / d;
    dt = dt < 0.5f ? dt * 2 : 1 - 2 * (dt - 0.5f);
    float v = c * ((t = dt - 1) * t * t + 1) + b;
    return v;
  };


  void Game::drawPlayground(void)
  {
    handleEvents();

    clearWindow();

    mRenderTexture.clear(mLevel.backgroundColor());
    mRenderTexture.draw(mLevel.backgroundSprite());
    mRenderTexture.setView(mDefaultView);
    for (BodyList::const_iterator b = mBodies.cbegin(); b != mBodies.cend(); ++b) {
      const Body *body = *b;
      if (body->isAlive()) {
        mRenderTexture.draw(*body);
      }
    }

    mLevelMsg.setString("Level " + std::to_string(mLevel.num()));
    mLevelMsg.setPosition(4, 4);
    mRenderTexture.draw(mLevelMsg);

    if (mState == State::Playing) {
      int penalty = mLevelTimer.accumulatedSeconds();
      mScoreMsg.setString(std::to_string(b2Max(0, mScore - penalty)));
      mScoreMsg.setPosition(mDefaultView.getCenter().x + mDefaultView.getSize().x / 2 - mScoreMsg.getLocalBounds().width - 4, 4);
      mRenderTexture.draw(mScoreMsg);
      for (int life = 0; life < mLives; ++life) {
        const sf::Texture &ballTexture = mLevel.texture(std::string("Ball"));
        sf::Sprite lifeSprite(ballTexture);
        lifeSprite.setOrigin(0.f, 0.f);
        lifeSprite.setColor(sf::Color(255, 255, 255, 0xa0));
        lifeSprite.setPosition(4 + (ballTexture.getSize().x * 1.5f) * life, mDefaultView.getCenter().y - 0.5f * mDefaultView.getSize().y + 26);
        mRenderTexture.draw(lifeSprite);
      }
    }

    sf::Sprite sprite(mRenderTexture.getTexture());
    sf::RenderStates states;
    if (mGlareEffectsActive > 0) {
      sf::Uint8 c = 0;
      if (mGlareEffectTimer.getElapsedTime() < mGlareEffectDuration) {
        c = sf::Uint8(quadEaseIn(mGlareEffectTimer.getElapsedTime().asSeconds(), 0.f, 255.f, mGlareEffectDuration.asSeconds()));
      }
      else {
        mGlareEffectsActive = 0;
      }
      if (mGlareEffectsDarken)
        mPostFX.setParameter("uColorSub", sf::Color(c, c, c, 0));
      else
        mPostFX.setParameter("uColorAdd", sf::Color(c, c, c, 0));
    }
    else {
      if (mGlareEffectsDarken)
        mPostFX.setParameter("uColorSub", sf::Color(0, 0, 0, 0));
      else
        mPostFX.setParameter("uColorAdd", sf::Color(0, 0, 0, 0));
    }
    states.shader = &mPostFX;
    mWindow.draw(sprite, states);
  }


  void Game::drawStartMessage(void)
  {
    mStartMsg.setColor(sf::Color(255, 255, 255, 192 + sf::Uint8(63 * std::sin(14 * mWallClock.getElapsedTime().asSeconds()))));
    mStartMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mStartMsg.getLocalBounds().width, mDefaultView.getCenter().y + mDefaultView.getSize().y / 4);
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
            else {
              mBlockHitSound.play();
            }
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
            startGlareEffect();
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
            startGlareEffect(true, sf::milliseconds(350));
          }
        }
        else if (a->type() == Body::BodyType::Racket || b->type() == Body::BodyType::Racket) {
          if (cp.normalImpulse > 20)
            mRacketHitSound.play();
        }
      }
    }
  }


  inline void Game::update(float elapsedSeconds)
  {
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

#ifdef BALL_TRACES
    if (mBall && mBallTrace)
      mBallTrace->addMarker(mBall->position(), mBall->body()->GetTransform().q.GetAngle());
#endif

    mContactPointCount = 0;
    mWorld->Step(elapsedSeconds, VelocityIterations, PositionIterations);
  }


  void Game::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
  {    if (mContactPointCount < MaxContactPoints) {      ContactPoint &cp = mPoints[mContactPointCount];      cp.fixtureA = contact->GetFixtureA();      cp.fixtureB = contact->GetFixtureB();      Body *bodyA = reinterpret_cast<Body*>(cp.fixtureA->GetUserData());      Body *bodyB = reinterpret_cast<Body*>(cp.fixtureB->GetUserData());      if (bodyA != nullptr && bodyB != nullptr) {        cp.position = contact->GetManifold()->points[0].localPoint;        cp.normal = b2Vec2_zero;        cp.normalImpulse = impulse->normalImpulses[0];        cp.tangentImpulse = impulse->tangentImpulses[0];        cp.separation = 0.f;        ++mContactPointCount;      }    }  }


  void Game::startGlareEffect(bool darken, const sf::Time &duration)
  {
    mGlareEffectsDarken = darken;
    mGlareEffectDuration = duration;
    if (mGlareEffectsActive == 0) {
      mGlareEffectTimer.restart();
    }
    ++mGlareEffectsActive;
  }


  void Game::buildLevel(void)
  {
    mLastKillings = std::vector<sf::Time>(mLevel.killingsPerKillingSpree(), sf::milliseconds(INT_MIN));

    mWorld->SetGravity(b2Vec2(0.f, mLevel.gravity()));

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
    topShape.Set(b2Vec2(0, 0), b2Vec2(W, 0));
    b2FixtureDef fdTop;
    fdTop.restitution = 0.9f;
    fdTop.density = 0.f;
    fdTop.shape = &topShape;
    boundaries->CreateFixture(&fdTop);

    safeRenew(mGround, new Ground(this, W));
    mGround->setPosition(0, mLevel.height());
    addBody(mGround);

    //safeRenew(mBallTrace, new BallTrace(this));
    //addBody(mBallTrace);

    // create level elements
    mBlockCount = 0;
    for (int y = 0; y < mLevel.height(); ++y) {
      const uint32_t *mapRow = mLevel.mapDataScanLine(y);
      for (int x = 0; x < mLevel.width(); ++x) {
        const uint32_t tileId = mapRow[x];
        if (tileId == 0)
          continue;
        const Tile &tile = mLevel.tile(tileId);
        if (tileId >= mLevel.firstGID()) {
          if (tile.textureName == "Ball") {
            mNewBallPosition.Set(float32(x), float32(y));
            newBall();
          }
          else if (tile.textureName == "Racket") {
            mRacket = new Racket(this);
            mRacket->setPosition(float32(x), float32(y));
            addBody(mRacket);
          }
          else if (tile.textureName == "Wall" || tile.fixed) {
            Wall *wall = new Wall(tileId, this);
            wall->setPosition(x, y);
            wall->setRestitution(tile.restitution);
            addBody(wall);
          }
          else {
            Block *block = new Block(tileId, this);
            block->setPosition(x, y);
            block->setScore(tile.score);
            block->setGravityScale(tile.gravityScale);
            block->setDensity(tile.density);
            block->setRestitution(tile.restitution);
            addBody(block);
            ++mBlockCount;
          }
        }
      }
    }

    // place mouse cursor on racket position
    const b2Vec2 &racketPos = float32(Game::Scale) * mRacket->position();
    mMousePos = sf::Vector2i(int(racketPos.x), int(racketPos.y));
    mLastMousePos = mMousePos;
    sf::Mouse::setPosition(mMousePos, mWindow);


  }


  void Game::showScore(int score, const b2Vec2 &atPos, int factor)
  {
    addToScore(score * factor);
    std::string text = ((factor > 1) ? (std::to_string(factor) + "*") : "") + std::to_string(score);
    TextBody *scoreText = new TextBody(this, text, 24U);
    scoreText->setFont(mFixedFont);
    scoreText->setPosition(atPos.x, atPos.y);
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


  void Game::newBall(void)
  {
    mNewBallSound.play();
    safeRenew(mBall, new Ball(this));
    if (mBallHasBeenLost) {
      const b2Vec2 &padPos = mRacket->position();
      mBall->setPosition(padPos.x, padPos.y - 3.5f);
    }
    else {
      mBall->setPosition(mNewBallPosition);
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


  void Game::killingSpree(int bonusPoints)
  {
    mKillingSpreeSound.play();
    showScore(bonusPoints + mLevel.killingSpreeBonus(), mRacket->position());
    resetKillingSpree();
  }


  void Game::resetKillingSpree(void)
  {
    for (std::vector<sf::Time>::iterator t = mLastKillings.begin(); t != mLastKillings.end(); ++t)
      *t = sf::milliseconds(INT_MIN);
    mLastKillingsIndex = 0;
  }


  inline void Game::checkForKillingSpree(void)
  {
    mLastKillings[mLastKillingsIndex] = mWallClock.getElapsedTime();
    const int i = (mLastKillingsIndex - mLastKillings.size()) % int(mLastKillings.size());
    const sf::Time &dt = mLastKillings.at(mLastKillingsIndex) - mLastKillings.at(i);
    mLastKillingsIndex = (mLastKillingsIndex + 1) % mLastKillings.size();
    if (dt < mLevel.killingSpreeInterval())
      killingSpree((mLevel.killingSpreeInterval() - dt).asMilliseconds());
  }


  void Game::onBodyKilled(Body *killedBody)
  {
    if (killedBody->type() == Body::BodyType::Block) {
      mExplosionSound.play();
      addBody(new ParticleSystem(this, killedBody->position(), mLevel.explosionParticlesCollideWithBall()));
      checkForKillingSpree();
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

