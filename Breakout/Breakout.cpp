// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"


namespace Breakout {

  const float Game::Scale = 16.f;

  Game::Game(void)
    : mWindow(sf::VideoMode(Game::DefaultWindowWidth, Game::DefaultWindowHeight, Game::ColorDepth), "Bangout")
    , mWorld(nullptr)
    , mBall(nullptr)
    , mGround(nullptr)
    , mContactPointCount(0)
    , mScore(0)
    , mTotalScore(0)
    , mLives(3)
    , mPaused(false)
    , mState(State::Initialization)
    , mKeyMapping(Action::LastAction)
    , mBlockCount(0)
  {
    bool ok;
    mWindow.setVerticalSyncEnabled(false);
    // glewInit();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);
    mWindow.setActive();
    resize();

    //sf::Image icon;
    //icon.loadFromFile("resources/gui/app-icon.png");
    //mWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ok = mFixedFont.loadFromFile("resources/fonts/emulogic.ttf");
    if (!ok)
      sf::err() << "resources/fonts/emulogic.ttf failed to load." << std::endl;
    ok = mDecorationFont.loadFromFile("resources/fonts/gunmetl.ttf");
    if (!ok)
      sf::err() << "resources/fonts/gunmetl.ttf failed to load." << std::endl;

    ok = mNewBallBuffer.loadFromFile("resources/soundfx/new-ball.ogg");
    if (!ok)
      sf::err() << "resources/sounds/new-ball.ogg failed to load." << std::endl;
    mNewBallSound.setBuffer(mNewBallBuffer);
    mNewBallSound.setVolume(100);
    mNewBallSound.setLoop(false);

    ok = mBallOutBuffer.loadFromFile("resources/soundfx/ball-out.ogg");
    if (!ok)
      sf::err() << "resources/soundfx/ball-out.ogg failed to load." << std::endl;
    mBallOutSound.setBuffer(mBallOutBuffer);
    mBallOutSound.setVolume(100);
    mBallOutSound.setLoop(false);

    ok = mBlockHitBuffer.loadFromFile("resources/soundfx/block-hit.ogg");
    if (!ok)
      sf::err() << "resources/soundfx/block-hit.ogg failed to load." << std::endl;
    mBlockHitSound.setBuffer(mBlockHitBuffer);
    mBlockHitSound.setVolume(100);
    mBlockHitSound.setLoop(false);

    ok = mPadHitBuffer.loadFromFile("resources/soundfx/pad-hit.ogg");
    if (!ok)
      sf::err() << "resources/soundfx/pad-hit.ogg failed to load." << std::endl;
    mPadHitSound.setBuffer(mPadHitBuffer);
    mPadHitSound.setVolume(100);
    mPadHitSound.setLoop(false);

    ok = mPadHitBlockBuffer.loadFromFile("resources/soundfx/pad-hit-block.ogg");
    if (!ok)
      sf::err() << "resources/soundfx/pad-hit-block.ogg failed to load." << std::endl;
    mPadHitBlockSound.setBuffer(mPadHitBlockBuffer);
    mPadHitBlockSound.setVolume(100);
    mPadHitBlockSound.setLoop(false);

    ok = mExplosionBuffer.loadFromFile("resources/soundfx/explosion.ogg");
    if (!ok)
      sf::err() << "resources/soundfx/explosion.ogg failed to load." << std::endl;
    mExplosionSound.setBuffer(mExplosionBuffer);
    mExplosionSound.setVolume(100);
    mExplosionSound.setLoop(false);

    mWelcomeMsg.setString("c't Breakout");
    mWelcomeMsg.setFont(mDecorationFont);
    mWelcomeMsg.setCharacterSize(113U);
    mWelcomeMsg.setColor(sf::Color(255, 255, 255));

    mLevelCompletedMsg.setString("Level complete");
    mLevelCompletedMsg.setFont(mDecorationFont);
    mLevelCompletedMsg.setCharacterSize(89U);
    mLevelCompletedMsg.setColor(sf::Color(255, 255, 255));

    mGameOverMsg.setString("Game over");
    mGameOverMsg.setFont(mDecorationFont);
    mGameOverMsg.setCharacterSize(89U);
    mGameOverMsg.setColor(sf::Color(255, 255, 255));

    mStartMsg.setFont(mFixedFont);
    mStartMsg.setCharacterSize(16U);
    mStartMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mStartMsg.getLocalBounds().width, 1.4f * mDefaultView.getCenter().y);

    mStatMsg.setFont(mFixedFont);
    mStatMsg.setCharacterSize(8U);
    mStatMsg.setColor(sf::Color(255, 255, 63));

    mScoreMsg.setFont(mFixedFont);
    mScoreMsg.setCharacterSize(16U);
    mScoreMsg.setColor(sf::Color(255, 255, 255, 200));

    mLevelMsg.setFont(mFixedFont);
    mLevelMsg.setCharacterSize(16U);
    mLevelMsg.setColor(sf::Color(255, 255, 255, 200));

    mProgramInfoMsg.setString("c't Breakout v" + std::string(__APP_VERSION) + " " + __TIMESTAMP__ + "\n"
      + "Copyright (c) 2015 Oliver Lau. All rights reserved.");
    mProgramInfoMsg.setFont(mFixedFont);
    mProgramInfoMsg.setColor(sf::Color::White);
    mProgramInfoMsg.setCharacterSize(8U);
    mProgramInfoMsg.setPosition(8.f, mDefaultView.getCenter().y + 0.5f * mDefaultView.getSize().y - mProgramInfoMsg.getLocalBounds().height - 8.f);

    mBackgroundTexture.loadFromFile("resources/backgrounds/abstract05.jpg");
    mBackgroundSprite.setTexture(mBackgroundTexture);
    mBackgroundSprite.setPosition(0.f, 0.f);

    mKeyMapping[Action::MoveLeft] = sf::Keyboard::Left;
    mKeyMapping[Action::MoveRight] = sf::Keyboard::Right;
    mKeyMapping[Action::SpecialAction] = sf::Keyboard::Space;
    mKeyMapping[Action::BackAction] = sf::Keyboard::Escape;
    mKeyMapping[Action::KickLeft] = sf::Keyboard::X;
    mKeyMapping[Action::KickRight] = sf::Keyboard::Y;
    mKeyMapping[Action::Restart] = sf::Keyboard::Delete;
    mKeyMapping[Action::ExplosionTest] = sf::Keyboard::P;
    mKeyMapping[Action::ContinueAction] = sf::Keyboard::Space;

    //ExplosionParticleSystem::instance()->setGame(this);
    //addBody(ExplosionParticleSystem::instance());

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

    safeRenew(mWorld, new b2World(b2Vec2(0.f, 9.81f)));
    mWorld->SetContactListener(this);
    mWorld->SetAllowSleeping(false);
    mWorld->SetWarmStarting(true);
    mWorld->SetContinuousPhysics(false);
    mWorld->SetSubStepping(true);

    mLives = DefaultLives;
    mTotalScore = 0;
    mLevel.set(0);

    mContactPointCount = 0;

    gotoWelcomeScreen();

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
#ifndef NDEBUG
    std::cout << "Game::setState(" << mState << ")\n";
#endif
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

      case State::PlayerKilled:
        //onPlayerKilled();
        break;

      case State::PauseAfterLevelCompleted:
        //onPauseAfterLevelCompleted();
        break;

      case State::LevelCompleted:
        onLevelCompleted();
        break;

      case State::Pausing:
        onPausing();
        break;

      case State::CreditsScreen:
        onCreditsScreen();
        break;

      case State::GameOver:
        onGameOver();
        break;

      default:
        break;
      }

      mWindow.display();
    }
  }


  void Game::clearWindow(void)
  {
    mWindow.clear();
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
      case sf::Event::Resized:
        resize();
        break;
      case sf::Event::LostFocus:
        pause();
        break;
      case sf::Event::GainedFocus:
        resume();
        break;
      case sf::Event::KeyReleased:
        //...
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == mKeyMapping[Action::BackAction]) {
          mWindow.close();
        }
        else if (event.key.code == mKeyMapping[Action::SpecialAction]) {
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
        }
#ifndef NDEBUG
        else if (event.key.code == mKeyMapping[Action::Restart]) {
          gotoLevelCompleted();
        }
#endif
        break;
      }
    }
  }


  void Game::handlePlayerInteraction(void)
  {
    if (mPad != nullptr) {
      mPad->stopMotion();
      mPad->stopKick();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::KickLeft]))
        mPad->kickLeft();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::KickRight]))
        mPad->kickRight();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::MoveLeft]))
        mPad->moveLeft();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::MoveRight]))
        mPad->moveRight();
    }
  }


  void Game::gotoWelcomeScreen(void) 
  {
    clearWorld();
    stopAllMusic();
    // mWelcomeMusic.play();
    mStartMsg.setString("Press SPACE to start");
    mBlamClock.restart();
    setState(State::WelcomeScreen);
    mWindow.setView(mDefaultView);
  }


  void Game::onWelcomeScreen(void)
  {
    sf::Time elapsed = mClock.restart();
    sf::Event event;
    while (mWindow.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        mWindow.close();
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == mKeyMapping[Action::BackAction]) {
          mWindow.close();
        }
        else if (event.key.code == mKeyMapping[Action::ContinueAction]) {
          gotoNextLevel();
        }
      }
    }
    mWindow.clear(sf::Color(31, 31, 47));
    mWindow.draw(mBackgroundSprite);

    update(elapsed.asSeconds());
    drawWorld(mDefaultView);
    
    mWelcomeMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mWelcomeMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mWelcomeMsg);

    drawStartMessage();

    mWindow.draw(mProgramInfoMsg);

    if (mBlamClock.getElapsedTime() > sf::milliseconds(999)) {
      mBlamClock.restart();
      ParticleSystem *ps = new ParticleSystem(
        this,
        b2Vec2(40.f * std::rand() / RAND_MAX, 25.f * std::rand() / RAND_MAX),
        111U);
      addBody(ps);
    }

  }


  void Game::gotoLevelCompleted(void)
  {
    mStartMsg.setString("Press SPACE to continue");
    mBlamClock.restart();
    mScoreClock.restart();
    setState(State::LevelCompleted);
  }


  void Game::onLevelCompleted(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed.asSeconds());

    if (mBlockCount > 0 && mScoreClock.getElapsedTime() > sf::milliseconds(50)) {
      mScoreClock.restart();
      BodyList::iterator b = std::find_if(mBodies.begin(), mBodies.end(), [](const Body *body)  {
        return body->type() == Body::BodyType::Block;
      });
      if (b != mBodies.end())
        (*b)->kill();
    }

    drawPlayground();

    mLevelCompletedMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mLevelCompletedMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mLevelCompletedMsg);
    
    drawStartMessage();
  }


  void Game::onCreditsScreen(void)
  {
    // ...
  }


  void Game::gotoGameOver(void)
  {
    mStartMsg.setString("Press SPACE to continue");
    setState(State::GameOver);
  }


  void Game::onGameOver(void)
  {
    const sf::Time &elapsed = mClock.restart();

    update(elapsed.asSeconds());

    drawPlayground();

    mGameOverMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mGameOverMsg.getLocalBounds().width, 20.f);
    mWindow.draw(mGameOverMsg);
    
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
    if (mLevel.gotoNext()) {
      buildLevel();
      // mBackgroundMusic.play();
      setState(State::Playing);
      mClock.restart();
    }
    else {
      std::cout << "You won!" << std::endl;
    }
  }


  void Game::onPlaying(void)
  {
    const sf::Time &elapsed = mClock.restart();
    if (!mPaused) {
      handlePlayerInteraction();
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

        if (mPad != nullptr) { // check if pad has been kicked out of the screen
          const float padX = mPad->position().x;
          const float padY = mPad->position().y;
          if (padY > mLevel.height())
            mPad->setPosition(padX, mLevel.height() - 0.5f);
          if (padX < 0.f)
            mPad->setPosition(1., padY);
          else if (padX > mLevel.width())
            mPad->setPosition(mLevel.width() - 1.f, padY);
        }
      }
    }
    drawPlayground();
  }


  void Game::drawPlayground(void)
  {
    handleEvents();

    clearWindow();
    mWindow.draw(mLevel.backgroundSprite());
    drawWorld(mDefaultView);

    mLevelMsg.setString("Level " + std::to_string(mLevel.num()));
    mLevelMsg.setPosition(4, 4);
    mWindow.draw(mLevelMsg);

    mScoreMsg.setString(std::to_string(mScore));
    mScoreMsg.setPosition(mDefaultView.getCenter().x + mDefaultView.getSize().x / 2 - mScoreMsg.getLocalBounds().width - 4, 4);
    mWindow.draw(mScoreMsg);

    for (int life = 0; life < mLives; ++life) {
      const sf::Texture &ballTexture = mLevel.texture(std::string("Ball"));
      sf::Sprite lifeSprite(ballTexture);
      lifeSprite.setOrigin(0.f, 0.f);
      lifeSprite.setColor(sf::Color(255, 255, 255, 0xa0));
      lifeSprite.setPosition(4 + (ballTexture.getSize().x * 1.5f) * life, mDefaultView.getCenter().y - 0.5f * mDefaultView.getSize().y + 26);
      mWindow.draw(lifeSprite);
    }
  }


  void Game::drawStartMessage(void)
  {
    mStartMsg.setColor(sf::Color(200, 200, 230, sf::Uint8(192.0f + 64.0f * std::sin(14.0f * mWallClock.getElapsedTime().asSeconds()))));
    mStartMsg.setPosition(mDefaultView.getCenter().x - 0.5f * mStartMsg.getLocalBounds().width, mDefaultView.getCenter().y + mDefaultView.getSize().y / 4);
    mWindow.draw(mStartMsg);
  }


  void Game::drawWorld(const sf::View &view)
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
          Ball *ball = reinterpret_cast<Ball*>(a->type() == Body::BodyType::Ball ? a : b);
          bool destroyed = block->hit(cp.normalImpulse);
          if (destroyed) {
            showScore(block->getScore(), block->position());
            block->kill();
          }
          else {
            mBlockHitSound.play();
          }
        }
        else if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          block->kill();
        }
        else if (a->type() == Body::BodyType::Pad || b->type() == Body::BodyType::Pad) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          showScore(block->getScore(), block->position(), 2);
          block->kill();
          mPadHitBlockSound.play();
        }
      }
      else if (a->type() == Body::BodyType::Ball || b->type() == Body::BodyType::Ball) {
        if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Ball *ball = reinterpret_cast<Ball*>(a->type() == Body::BodyType::Ball ? a : b);
          ball->lethalHit();
          ball->kill();
        }
        else if (a->type() == Body::BodyType::Pad || b->type() == Body::BodyType::Pad) {
          if (cp.normalImpulse > 20)
            mPadHitSound.play();
        }
      }
    }
  }


  void Game::update(float elapsedSeconds)
  {
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
  }


  void Game::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
  {    if (mContactPointCount < MaxContactPoints) {      ContactPoint &cp = mPoints[mContactPointCount];      cp.fixtureA = contact->GetFixtureA();      cp.fixtureB = contact->GetFixtureB();      Body *bodyA = reinterpret_cast<Body*>(cp.fixtureA->GetUserData());      Body *bodyB = reinterpret_cast<Body*>(cp.fixtureB->GetUserData());      if (bodyA != nullptr && bodyB != nullptr) {        cp.position = contact->GetManifold()->points[0].localPoint;        cp.normal = b2Vec2_zero;        cp.normalImpulse = impulse->normalImpulses[0];        cp.tangentImpulse = impulse->tangentImpulses[0];        cp.separation = 0.f;        ++mContactPointCount;      }    }  }


  void Game::buildLevel(void)
  {
    // create boundaries
    { 
      float32 W = float32(mLevel.width());
      float32 H = float32(mLevel.height());

      b2BodyDef bd;
      b2Body *boundaries = mWorld->CreateBody(&bd);

      b2EdgeShape topBoundary;
      topBoundary.Set(b2Vec2(0, 0), b2Vec2(W, 0));
      boundaries->CreateFixture(&topBoundary, 0.f);

      b2EdgeShape rightBoundary;
      rightBoundary.Set(b2Vec2(W, 0), b2Vec2(W, H));
      boundaries->CreateFixture(&rightBoundary, 0.f);

      b2EdgeShape leftBoundary;
      leftBoundary.Set(b2Vec2(0, 0), b2Vec2(0, H));
      boundaries->CreateFixture(&leftBoundary, 0.f);

      mGround = new Ground(this, W);
      mGround->setPosition(0.f, float(mLevel.height()));
      addBody(mGround);
    }

    // create pad
    {
      mPad = new Pad(this);
      mPad->setPosition(0.5f * mLevel.width(), mLevel.height() - 0.5f);
      addBody(mPad);
    }

    // create blocks
    mBlockCount = 0;
    for (int y = 0; y < mLevel.height(); ++y) {
      const uint32_t *mapRow = mLevel.mapDataScanLine(y);
      for (int x = 0; x < mLevel.width(); ++x) {
        const uint32_t tileId = mapRow[x];
        if (tileId >= mLevel.firstGID()) {
          Block *block = new Block(tileId, this);
          block->setScore(mLevel.score(tileId));
          block->setPosition(float(x), float(y));
          addBody(block);
          ++mBlockCount;
        }
      }
    }

    newBall();
  }


  void Game::showScore(int score, const b2Vec2 &atPos, int factor)
  {
    addToScore(score * factor);
    std::string text = ((factor > 1) ? (std::to_string(factor) + "*") : "") + std::to_string(score);
    TextBody *scoreText = new TextBody(this, text, 14U);
    scoreText->setFont(mFixedFont);
    scoreText->setPosition(atPos.x - 0.5f, atPos.y - 0.5f);
    addBody(scoreText);
  }


  void Game::addToScore(int points)
  {
    mScore += points;
    if ((mScore % NewLiveAfterSoManyPoints) > ((mScore + points) % NewLiveAfterSoManyPoints))
      ++mLives;
  }


  void Game::newBall(void)
  {
    mNewBallSound.play();
    safeRenew(mBall, new Ball(this));
    const b2Vec2 &padPos = mPad->position();
    mBall->setPosition(padPos.x, padPos.y - 3.5f);
    addBody(mBall);
  }


  void Game::pause(void)
  {
    mPaused = true;
  }


  void Game::resume(void)
  {
    mPaused = false;
  }


  void Game::addBody(Body *body)
  {
    mBodies.push_back(body);
  }


  void Game::onBodyKilled(Body *killedBody)
  {
    if (killedBody->type() == Body::BodyType::Ball) {
      if (mState == State::Playing) {
        mBallOutSound.play();
        if (killedBody->energy() == 0) {
          if (mLives-- == 0) {
            gotoGameOver();
          }
        }
      }
    }
    else if (killedBody->type() == Body::BodyType::Block) {
      --mBlockCount;
      mExplosionSound.play();
      ParticleSystem *ps = new ParticleSystem(this, killedBody->position());
      addBody(ps);
      if (mBlockCount == 0)
        gotoLevelCompleted();
    }
  }

}

