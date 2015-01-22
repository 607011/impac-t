// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"


namespace Breakout {

  const float Game::ShotSpeed = 9.f;
  const float Game::Scale = 16.f;

  Game::Game(void)
    : mWindow(sf::VideoMode(Game::DefaultWindowWidth, Game::DefaultWindowHeight, Game::ColorDepth), "Bangout")
    , mWorld(nullptr)
    , mBall(nullptr)
    , mGround(nullptr)
    , mPointCount(0)
    , mScore(0)
    , mTotalScore(0)
    , mLives(3)
    , mPaused(false)
    , mRestartRequested(false)
    , mState(State::Initialization)
    , mKeyMapping(Action::LastAction)
  {
    bool ok;


    mWindow.setVerticalSyncEnabled(true);
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

    ok = mNewBallBuffer.loadFromFile("resources/soundfx/new-ball.wav");
    if (!ok)
      sf::err() << "resources/sounds/new-ball.wav failed to load." << std::endl;
    mNewBallSound.setBuffer(mNewBallBuffer);
    mNewBallSound.setVolume(100);
    mNewBallSound.setLoop(false);

    ok = mBallOutBuffer.loadFromFile("resources/soundfx/ball-out.wav");
    if (!ok)
      sf::err() << "resources/sounds/ball-out.wav failed to load." << std::endl;
    mBallOutSound.setBuffer(mBallOutBuffer);
    mBallOutSound.setVolume(100);
    mBallOutSound.setLoop(false);

    ok = mBlockHitBuffer.loadFromFile("resources/soundfx/block-hit.wav");
    if (!ok)
      sf::err() << "resources/sounds/block-hit.wav failed to load." << std::endl;
    mBlockHitSound.setBuffer(mBlockHitBuffer);
    mBlockHitSound.setVolume(100);
    mBlockHitSound.setLoop(false);

    ok = mPadHitBuffer.loadFromFile("resources/soundfx/pad-hit.wav");
    if (!ok)
      sf::err() << "resources/sounds/pad-hit.wav failed to load." << std::endl;
    mPadHitSound.setBuffer(mPadHitBuffer);
    mPadHitSound.setVolume(100);
    mPadHitSound.setLoop(false);

    ok = mExplosionBuffer.loadFromFile("resources/soundfx/explosion.wav");
    if (!ok)
      sf::err() << "resources/sounds/pad-hit.wav failed to load." << std::endl;
    mExplosionSound.setBuffer(mExplosionBuffer);
    mExplosionSound.setVolume(100);
    mExplosionSound.setLoop(false);

    mScoreMsg.setFont(mFixedFont);
    mScoreMsg.setCharacterSize(22);
    mScoreMsg.setColor(sf::Color(255, 255, 255));

    mKeyMapping[Action::MoveLeft] = sf::Keyboard::Left;
    mKeyMapping[Action::MoveRight] = sf::Keyboard::Right;
    mKeyMapping[Action::SpecialAction] = sf::Keyboard::Space;
    mKeyMapping[Action::BackAction] = sf::Keyboard::Escape;
    mKeyMapping[Action::KickLeft] = sf::Keyboard::X;
    mKeyMapping[Action::KickRight] = sf::Keyboard::Y;
    mKeyMapping[Action::Restart] = sf::Keyboard::Delete;
    mKeyMapping[Action::ExplosionTest] = sf::Keyboard::P;

    restart();
  }


  Game::~Game(void)
  {
    // ...
  }


  void Game::restart(void)
  {
    pause();

    safeRenew(mWorld, new b2World(b2Vec2(0.f, 9.81f)));
    // mWorld->SetDestructionListener(&mDestructionListener);
    mWorld->SetContactListener(this);
    mWorld->SetAllowSleeping(false);
    mWorld->SetWarmStarting(true);
    mWorld->SetContinuousPhysics(false);
    mWorld->SetSubStepping(true);

    mLives = DefaultLives;
    mTotalScore = 0;
    mLevel.set(1);
    buildLevel();
    setState(State::Playing);

    resume();
  }


  void Game::resize(void)
  {
    mDefaultView = sf::View(sf::FloatRect(0.f, 0.f, float(mWindow.getSize().x), float(mWindow.getSize().y)));
    mDefaultView.setCenter(0.5f * sf::Vector2f(mWindow.getSize()));
    mPlayView = sf::View(sf::FloatRect(0.f, 0.f, float(mWindow.getSize().x), float(mWindow.getSize().y)));
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

      case State::CountdownBeforeLevelStarts:
        //onCountdownBeforeLevelStarts();
        break;

      case State::PlayerKilled:
        //onPlayerKilled();
        break;

      case State::PauseAfterLevelCompleted:
        //onPauseAfterLevelCompleted();
        break;

      case State::LevelCompleted:
        //onLevelCompleted();
        break;

      case State::Pausing:
        //onPausing();
        break;

      case State::CreditsScreen:
        //onCreditsScreen();
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


  void Game::clearWorld(void)
  {
    b2Body* node = mWorld->GetBodyList();
    while (node) {
      b2Body* body = node;
      node = node->GetNext();
      mWorld->DestroyBody(body);
    }
    safeDelete(mBall);
    mBodies.clear();
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
          newBall();
        }
        else if (event.key.code == mKeyMapping[Action::Restart]) {
          mRestartRequested = true;
        }
        else if (event.key.code == mKeyMapping[Action::ExplosionTest]) {
          TextBody *text = new TextBody(this, "BLAH!", 12U, sf::milliseconds(250));
          text->setPosition(mPad->position().x, mPad->position().y);
          text->setFont(mFixedFont);
          addBody(text);
        }
        break;
      }
    }
  }


  void Game::handlePlayerInteraction(void)
  {
    if (!mPaused) {
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


  void Game::onPlaying(void)
  {
    if (mRestartRequested) {
      mRestartRequested = false;
      restart(); // XXX
      std::cout << "mRestartRequested!" << std::endl;
      return;
    }
    const sf::Time &elapsed = mClock.restart();
    clearWindow();
    mWindow.draw(mLevel.backgroundSprite());
    handleEvents();
    handlePlayerInteraction();
    if (!mPaused)
      update(elapsed.asSeconds());
    drawWorld(mPlayView);
    mScoreMsg.setString(std::to_string(mScore));
    mScoreMsg.setPosition(mPlayView.getCenter().x + mPlayView.getSize().x / 2 - mScoreMsg.getLocalBounds().width - 20, 20);
    mWindow.draw(mScoreMsg);

    for (int life = 0; life < mLives; ++life) {
      const sf::Texture &ballTexture = mLevel.texture(std::string("Ball"));
      sf::Sprite lifeSprite(ballTexture);
      lifeSprite.setOrigin(8.f, 8.f);
      lifeSprite.setPosition((ballTexture.getSize().x * 1.5f) * (1 + life), mPlayView.getCenter().y - 0.5f * mPlayView.getSize().y + 32);
      mWindow.draw(lifeSprite);
    }

    if (mBall != nullptr) { // check if ball has been kicked out of the screen accidentally
      const float ballX = mBall->position().x;
      const float ballY = mBall->position().y;
      if (0 > ballX || ballX > float(mLevel.width()) || 0 > ballY || ballY > float(mLevel.height()))
        mBall->kill();
    }

    if (mPad != nullptr && mPad->position().y > mLevel.height())
        mPad->setPosition(mPad->position().x, mLevel.height() - 0.5f);
  }


  void Game::onWelcomeScreen(void)
  {
    const sf::Time &elapsed = mClock.restart();
    clearWindow();
    drawWorld(mDefaultView);
  }


  void Game::drawWorld(const sf::View &view)
  {
    mWindow.setView(view);
    const BodyList &bodies = mBodies;
    for (BodyList::const_iterator b = bodies.cbegin(); b != bodies.cend(); ++b) {
      const Body *body = *b;
      if (body->isAlive()) {
        mWindow.draw(*body);
      }
    }
  }


  void Game::evaluateCollisions(void)
  {
    for (int i = 0; i < mPointCount; ++i) {
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
          block->hit(cp.normalImpulse);
          mBlockHitSound.play();
        }
        else if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          showScore(2 * block->getScore(), block->position());
          block->kill();
          mExplosionSound.play();
          ParticleSystem *ps = new ParticleSystem(this);
          ps->setPosition(block->position().x, block->position().y);
          addBody(ps);
        }
        else if (a->type() == Body::BodyType::Pad || b->type() == Body::BodyType::Pad) {
          Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
          showScore(block->getScore(), block->position(), 2);
          block->kill();
          mExplosionSound.play();
          ParticleSystem *ps = new ParticleSystem(this);
          ps->setPosition(block->position().x, block->position().y);
          addBody(ps);
        }
      }
      else if (a->type() == Body::BodyType::Ball || b->type() == Body::BodyType::Ball) {
        if (a->type() == Body::BodyType::Ground || b->type() == Body::BodyType::Ground) {
          Ball *ball = reinterpret_cast<Ball*>(a->type() == Body::BodyType::Ball ? a : b);
          ball->kill();
        }
        else if (a->type() == Body::BodyType::Pad || b->type() == Body::BodyType::Pad) {
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

    mPointCount = 0;
    mWorld->Step(elapsedSeconds, VelocityIterations, PositionIterations);
  }


  void Game::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
  {
    B2_NOT_USED(contact);
    B2_NOT_USED(oldManifold);
  }


  void Game::BeginContact(b2Contact *contact)
  {
    B2_NOT_USED(contact);
  }


  void Game::EndContact(b2Contact *contact)
  {
    B2_NOT_USED(contact);
  }


  void Game::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
  {    if (mPointCount < MaxContactPoints) {      ContactPoint &cp = mPoints[mPointCount];      cp.fixtureA = contact->GetFixtureA();      cp.fixtureB = contact->GetFixtureB();      Body *bodyA = reinterpret_cast<Body*>(cp.fixtureA->GetUserData());      Body *bodyB = reinterpret_cast<Body*>(cp.fixtureB->GetUserData());      if (bodyA != nullptr && bodyB != nullptr) {        cp.position = contact->GetManifold()->points[0].localPoint;        cp.normal = b2Vec2_zero;        cp.normalImpulse = impulse->normalImpulses[0];        cp.tangentImpulse = impulse->tangentImpulses[0];        cp.separation = 0.f;        ++mPointCount;      }    }  }


  void Game::buildLevel(void)
  {
    mCurrentBodyId = 0;
    clearWorld();

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
    for (int y = 0; y < mLevel.height(); ++y) {
      const uint32_t *mapRow = mLevel.mapDataScanLine(y);
      for (int x = 0; x < mLevel.width(); ++x) {
        const uint32_t tileId = mapRow[x];
        if (tileId >= mLevel.firstGID()) {
          Block *block = new Block(tileId, this);
          block->setScore(mLevel.score(tileId));
          block->setPosition(float(x), float(y));
          addBody(block);
        }
      }
    }

    newBall();
  }


  void Game::gameOver(void)
  {
    std::cout << "gameOver()" << std::endl << std::endl;
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
    body->setId(mCurrentBodyId++);
    mBodies.push_back(body);
  }


  void Game::onBodyKilled(Body *killedBody)
  {
    if (killedBody->type() == Body::BodyType::Ball) {
      mBallOutSound.play();
      if (mLives-- == 0)
        gameOver();
    }
  }


  const Level *Game::level(void) const
  {
    return &mLevel;
  }


  b2World *Game::world(void)
  {
    return mWorld;
  }


  Ground *Game::ground(void) const
  {
    return mGround;
  }


  int Game::tileWidth(void) const
  {
    return mLevel.tileWidth();
  }


  int Game::tileHeight(void) const
  {
    return mLevel.tileHeight();
  }


}

